#include "Logger.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>

// kvar att göra add threadid, blocking dev fix message before shutdown?

typedef struct {
    time_t timestamp;
    pid_t pid;
    char level[16];
    char module[32];
    char message[256];
} LogMessage;

static int write_fd = -1;
static pid_t logger_pid = -1;
static LogLevel log_level = LOG_LEVEL_INFO;

static FILE* log_file = NULL;
//static char log_path[256] = "log.txt";
static char log_path[PATH_MAX];

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static void log_ProcessLoop(int read_fd);
static void log_ToFile(const LogMessage* log_msg);
const char* log_GetLevelString(LogLevel level);


static int file_exists(const char *path) {
    return access(path, F_OK) == 0;
}

// Hitta projektets root via .project_rootså vi kan spara loggerna i samma directory oavsett vilken maskin eler struktur vi kör på
static void find_project_root(char *root_path, size_t size)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        exit(1);
    }

    strncpy(root_path, cwd, size-1);
    root_path[size-1] = '\0';

    while (1)
    {
        char marker[PATH_MAX];
        snprintf(marker, sizeof(marker), "%s/.project_root", root_path);

        if (file_exists(marker))
            break;  // hittade root

        char *last_slash = strrchr(root_path, '/');
        if (!last_slash)
        {
            fprintf(stderr, "Project root not found!\n");
            exit(1);
        }
        *last_slash = '\0';
    }
}


int log_Init(const char* custom_path)
{
    // 1️⃣ Determine log file path
    if (custom_path != NULL) {
        strncpy(log_path, custom_path, sizeof(log_path) - 1);
        log_path[sizeof(log_path) - 1] = '\0';
    } else {
        char project_root[PATH_MAX];
        find_project_root(project_root, sizeof(project_root));

        char logs_dir[PATH_MAX];
        snprintf(logs_dir, sizeof(logs_dir), "%s/Logs", project_root);

        snprintf(log_path, sizeof(log_path), "%s/log.txt", logs_dir);

        printf("DEBUG: Logs directory = %s\n", logs_dir);
        printf("DEBUG: Log file path = %s\n", log_path);
        printf("DEBUG project_root = '%s'\n", project_root);
        printf("DEBUG logs_dir = '%s'\n", logs_dir);
        printf("DEBUG log_path = '%s'\n", log_path);
        printf("DEBUG strlen(log_path) = %zu\n", strlen(log_path));

        // 2️⃣ Ensure parent directory exists
        char *last_slash = strrchr(log_path, '/');
        if (last_slash) {
            char parent_dir[PATH_MAX];
            size_t len = last_slash - log_path;
            strncpy(parent_dir, log_path, len);
            parent_dir[len] = '\0';

            struct stat st = {0};
            if (stat(parent_dir, &st) == -1) {
                printf("Parent directory missing, creating: %s\n", parent_dir);
                if (mkdir(parent_dir, 0755) < 0) {
                    perror("mkdir parent_dir failed");
                    return -1;
                }
            } else {
                printf("Parent directory exists: %s\n", parent_dir);
            }
        }

        // 3️⃣ Create log.txt if missing
        FILE* tmp = fopen(log_path, "a");
        if (!tmp) {
            perror("fopen log file failed");
            return -1;
        }
        fclose(tmp);
        printf("Log file ready: %s\n", log_path);
    }

    // 4️⃣ Setup logger pipe and fork child
    int pipe_fds[2];
    if (pipe(pipe_fds) < 0) {
        perror("pipe");
        return -1;
    }

    logger_pid = fork();
    if (logger_pid < 0) {
        perror("fork");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return -1;
    }

    if (logger_pid == 0) {
        // Child process
        close(pipe_fds[1]);
        log_ProcessLoop(pipe_fds[0]); // infinite read loop
        close(pipe_fds[0]);
        exit(0);
    } else {
        // Parent process
        close(pipe_fds[0]);
        write_fd = pipe_fds[1];

        int flags = fcntl(write_fd, F_GETFL, 0);
        if (flags != -1) {
            fcntl(write_fd, F_SETFL, flags | O_NONBLOCK);
        }

        printf("Logger initialized (PID: %d)\n", logger_pid);
    }

    return 0;
}


void log_Message(LogLevel level, const char* module, const char* msg)
{
    if(write_fd < 0)
    {
        fprintf(stderr, "Logger not initialized!\n");
        return;
    }

    if(level < log_level)
        return;
    
    pthread_mutex_lock(&log_mutex);
    
    LogMessage log_msg = {0};
    log_msg.timestamp = time(NULL);
    log_msg.pid = getpid();
    strncpy(log_msg.level, log_GetLevelString(level), sizeof(log_msg.level) - 1);
    strncpy(log_msg.module, module, sizeof(log_msg.module) - 1);
    strncpy(log_msg.message, msg, sizeof(log_msg.message) - 1);
    
    ssize_t written = write(write_fd, &log_msg, sizeof(LogMessage));

    if (written == sizeof(LogMessage))
    {
        pthread_mutex_unlock(&log_mutex);
        return;
    }
    
    // Handle write errors
    if (written < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
    {
        static unsigned long dropped = 0;
        dropped++;
        
        if (dropped % 1000 == 0)
            fprintf(stderr, "WARNING: Log buffer full, %lu messages dropped\n", dropped);
        
        if (level >= LOG_LEVEL_ERROR)
            fprintf(stderr, "[DROPPED ERROR] [%s] %s\n", module, msg);
    }
    else if (written < 0)
    {
        perror("log_Message write");
    }
    else
    {
        fprintf(stderr, "WARNING: Partial log write (%zd/%zu bytes)\n", 
                written, sizeof(LogMessage));
    }
    
    pthread_mutex_unlock(&log_mutex);
}

static void log_ProcessLoop(int read_fd)
{
    LogMessage log_msg;
    ssize_t bytes_read;

    printf("DEBUG: child opening log file: '%s'\n", log_path);

    log_file = fopen(log_path, "a");
    if (!log_file)
    {
        perror("log_ProcessLoop fopen");
        return;
    }

    //line buffering
    setvbuf(log_file, NULL, _IOLBF, 8192);

    while (1) 
    {
        bytes_read = read(read_fd, &log_msg, sizeof(LogMessage));

        if (bytes_read == 0)
        {
            printf("Logger: pipe closed\n");
            break;
        }

        if (bytes_read != sizeof(LogMessage))
        {
            fprintf(stderr, "Logger: incomplete read (%zd/%zu bytes)\n", bytes_read, sizeof(LogMessage));
            continue;
        }

        log_ToFile(&log_msg);    
    }
    if(log_file)
    {
        fflush(log_file);
        fclose(log_file);
        log_file = NULL;
    }
}

static void log_ToFile(const LogMessage* log_msg)
{
    if (!log_file)
        return;

    char time_buf[64];
    struct tm* tm_info = localtime(&log_msg->timestamp);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(log_file, "[%s] [PID:%d] [%s] [%s]: %s\n", time_buf, log_msg->pid, log_msg->level, log_msg->module, log_msg->message);

    #ifdef DEBUG
    printf("[%s] [%s] [%s]: %s\n", time_buf, log_msg->level, log_msg->module, log_msg->message);
    #endif
}

const char* log_GetLevelString(LogLevel level)
{
    switch(level)
    {
        case LOG_LEVEL_DEBUG:   return "DEBUG";
        case LOG_LEVEL_INFO:    return "INFO";
        case LOG_LEVEL_WARN:    return "WARNING";
        case LOG_LEVEL_ERROR:   return "ERROR";
        default: return "INFO";
    }
}

void log_SetLevel(LogLevel level)
{
    log_level = level;
}

void log_CloseWrite(void)
{
    if (write_fd >= 0)
    {
        close(write_fd);
        write_fd = -1;
    }
}

void log_Cleanup(void)
{
    if (write_fd >= 0)
    {
        close(write_fd);
        write_fd = -1;
    }
    if (logger_pid > 0)
    {
        printf("Waiting for logger to exit...\n");
        waitpid(logger_pid, NULL, 0);
        printf("Logger exited.\n");
        logger_pid = -1;
    }
    
    pthread_mutex_destroy(&log_mutex);
}