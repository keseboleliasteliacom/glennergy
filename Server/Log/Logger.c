#include "Logger.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

typedef struct {
    time_t timestamp;
    char level[16];
    char module[32];
    char message[256];
} LogMessage;

static int log_pipe_fd = -1;
static pid_t logger_pid = -1;
static LogLevel log_level = LOG_LEVEL_INFO;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE* log_file = NULL;

static void log_ProcessLoop(int read_fd);
static void log_ToFile(const LogMessage* log_msg);
static const char* log_GetLevelString(LogLevel level);

int log_Init(void)
{
    int pipe_fds[2];

    if (pipe(pipe_fds) < 0)
    {
        perror("pipe");
        return -1;
    }
    logger_pid = fork();

    if (logger_pid < 0)
    {
        perror("fork");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return -1;
    }

    if (logger_pid == 0)
    {
        //child process
        close(pipe_fds[1]);
        log_ProcessLoop(pipe_fds[0]);

        close(pipe_fds[0]);
        exit(0);
    }
    else
    {
        //parent process
        close(pipe_fds[0]);
        log_pipe_fd = pipe_fds[1];

        int flags = fcntl(log_pipe_fd, F_GETFL, 0);
        if (flags != -1) {
            fcntl(log_pipe_fd, F_SETFL, flags | O_NONBLOCK);
        }

        printf("Logger initialized (PID: %d)\n", logger_pid);    
    }
    return 0;
}

void log_Message(LogLevel level, const char* module, const char* msg)
{
    if(log_pipe_fd < 0)
    {
        fprintf(stderr, "Logger not initialized!\n");
        return;
    }

    if(level < log_level)
        return;
    
    pthread_mutex_lock(&log_mutex);
    
    LogMessage log_msg = {0};
    log_msg.timestamp = time(NULL);
    
    const char* level_str = log_GetLevelString(level);
    strncpy(log_msg.level, level_str, sizeof(log_msg.level) - 1);
    strncpy(log_msg.module, module, sizeof(log_msg.module) - 1);
    strncpy(log_msg.message, msg, sizeof(log_msg.message) - 1);
    
    ssize_t written = write(log_pipe_fd, &log_msg, sizeof(LogMessage));
    if (written < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            static unsigned long dropped = 0;
            dropped++;
            
            if (dropped % 1000 == 0)
            {
                fprintf(stderr, "WARNING: Log buffer full, %lu messages dropped\n", dropped);
            }
            
            if (level >= LOG_LEVEL_ERROR)
            {
                fprintf(stderr, "[DROPPED ERROR] [%s] %s\n", module, msg);
            }
        }
        else
        {
            perror("log_Message write");
        }
    }
    else if (written != sizeof(LogMessage))
    {
        fprintf(stderr, "WARNING: Partial log write (%zd/%zu bytes)\n", 
                written, sizeof(LogMessage));
    }
    
    pthread_mutex_unlock(&log_mutex);
}

// Skriver till syslog(endast Linux) och en lokal fil som backup(Logs/log.txt, enkelt för de som använder windows och WSL)
static void log_ProcessLoop(int read_fd)
{
    LogMessage log_msg;
    ssize_t bytes_read;

    log_file = fopen("/home/nak2477/glennergy/Logs/log.txt", "a");
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


        //int priority = LOG_INFO;
        //if (strcmp(log_msg.level, "ERROR") == 0) priority = LOG_ERR;
        //else if (strcmp(log_msg.level, "DEBUG") == 0) priority = LOG_DEBUG;
        //else if (strcmp(log_msg.level, "WARNING") == 0) priority = LOG_WARNING;
//
        //openlog(log_msg.module, LOG_PID, LOG_USER);
        //syslog(priority, "%s", log_msg.message);
        //closelog();

        /* Extra säkerhet: skriv även till fil */
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

    //printf("Logging to file: [%s] [%s]: %s\n", log_msg->level, log_msg->module, log_msg->message);
    //FILE* f = fopen("/home/nak2477/glennergy/Logs/log.txt", "a");
    //if (!f)
    //{
    //    perror("log_ToFile fopen");
    //    return;
    //}

    fprintf(log_file, "[%s] [%s] [%s]: %s\n", time_buf, log_msg->level, log_msg->module, log_msg->message);
}

static const char* log_GetLevelString(LogLevel level)
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
    if (log_pipe_fd >= 0)
    {
        close(log_pipe_fd);
        log_pipe_fd = -1;
    }
}

void log_Cleanup(void)
{
    if (log_pipe_fd >= 0)
    {
        close(log_pipe_fd);
        log_pipe_fd = -1;
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