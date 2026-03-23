#define MODULE_NAME "SERVER"
#define _XOPEN_SOURCE 500
/**
 * @file Server.c
 * @brief Implementation of the Server module.
 * @ingroup Server
 *
 * Handles initialization, running, and disposal of server, including:
 * - Starting TCP connections via ConnectionHandler
 * - Forking subprocesses for cache and algorithm modules
 * - Managing Crontab entries for scheduled tasks
 * - Signal handling
 *
 * @note Uses ServerConfig for configuration and ConnectionHandler for TCP connections.
 */

#include "Server.h"
#include "SignalHandler.h"
#include "../Libs/Utils/utils.h"
#include "../Libs/Threads.h"
#include "Log/Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

/**
 * @brief Initialize the Server structure with command-line arguments.
 *
 * @param _Server Pointer to pointer of Server to initialize
 * @param _Argv Command-line arguments array
 * @param _Argc Number of command-line arguments
 * @return 0 on success, negative value on error
 * @pre _Server must not be NULL
 * @post Server is allocated and configured based on argv or defaults
 */
int Server_Initialize(Server **_Server, char **_Argv, int _Argc)
{
    Server *srv = (Server *)malloc(sizeof(Server));
    if (srv == NULL)
        return -1;

    ServerConfig_Init(&srv->config, _Argv, _Argc);

    printf("Port: %d\n", srv->config.port);
    printf("Log level %d\n", srv->config.log_level);

    *_Server = srv;
    return 0;
}

/**
 * @brief Add Crontab entry using a shell script.
 * @note Forks a child process to execute "./crontab_inst.sh add"
 * @post Waits for child to complete.
 */
void Crontab_Add()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        char path[1024];
        realpath("./crontab_inst.sh", path); // resolves the absolute path at runtime
        execlp("/bin/bash", "bash", path, "add", NULL);
        perror("execl failed");
        exit(EXIT_SUCCESS);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * @brief Remove Crontab entry using a shell script.
 * @note Forks a child process to execute "./crontab_inst.sh remove"
 * @post Waits for child to complete.
 */
void Crontab_Remove()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        execlp("./crontab_inst.sh", "crontab.sh", "remove", NULL);
        perror("execl failed");
        exit(EXIT_SUCCESS);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * @brief Run the server, starting ConnectionHandler and subprocesses.
 *
 * @param _Server Pointer to initialized Server
 * @return 0 on success, negative on error
 * @pre _Server must be initialized
 * @post Starts all child processes, TCP listening, and blocks until termination
 * @note Starts Threads, smw subsystem, ConnectionHandler, input cache, and algorithm subprocesses.
 */
int Server_Run(Server *_Server)
{
    SignalHandler_Initialize();
    Crontab_Add();

    int status_pid, status_cache, status_algoritm;

    pid_t pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process: main server work
        Threads threads[POOL_SIZE];
        Threads_Initialize(threads);

        smw_init();

        ConnectionHandler *cHandler = NULL;
        ConnectionHandler_Initialize(&cHandler, _Server->config.port, Threads_AddQueueItem);

        uint64_t monTime = 0;
        while (SignalHandler_Stop() == 0)
        {
            monTime = SystemMonotonicMS();
            smw_work(monTime);
            usleep(100000); // Todo från compiler warning - Byta till använda "nanosleep" från "time.h" istället för "usleep" från "unistd.h"?
        }

        ConnectionHandler_Dispose(&cHandler);
        smw_dispose();
        Threads_Dispose(threads);

        log_CloseWrite();
        exit(EXIT_SUCCESS);
    }

    // Fork child for input cache process
    pid_t pid_cache = fork();
    if (pid_cache < 0)
        exit(EXIT_FAILURE);
    else if (pid_cache == 0)
    {
        execlp("Glennergy-InputCache", "Glennergy-InputCache", NULL);
        LOG_ERROR("Failed to execute Glennergy-InputCache");
        exit(EXIT_SUCCESS);
    }

    // Fork child for algorithm process
    pid_t pid_algoritm = fork();
    if (pid_algoritm < 0)
        exit(EXIT_FAILURE);
    else if (pid_algoritm == 0)
    {
        execlp("Glennergy-Algoritm", "Glennergy-Algoritm", NULL);
        LOG_ERROR("Failed to execute Glennergy-Algoritm");
        exit(EXIT_SUCCESS);
    }

    // Parent waits for all children to finish
    wait(&status_pid);
    wait(&status_cache);
    wait(&status_algoritm);

    Crontab_Remove();

    return 0;
}

/**
 * @brief Dispose a Server instance and free resources.
 *
 * @param _Server Pointer to pointer of Server to dispose
 * @post Closes memory and marks pointer as NULL
 */
void Server_Dispose(Server **_Server)
{
    if (_Server == NULL || *_Server == NULL)
        return;

    Server *srv = *_Server;

    free(srv);
    srv = NULL;
}