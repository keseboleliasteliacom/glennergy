#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <time.h>

static void log_to_file(const char* module, const char* msg, const char* level)
{
    printf("Logging to file: [%s] [%s]: %s\n", level, module, msg);
    FILE* f = fopen("../../Logs/log.txt", "a");
    if (!f) return;

    fprintf(f, "[%s] [%s]: %s\n", level, module, msg);
    fclose(f);
}


// Skriver till syslog(endast Linux) och en lokal fil som backup(Logs/log.txt, enkelt för de som använder windows och WSL)
void log_message(const char* module, const char* msg, const char* level, time_t timestamp)
{
    int priority = LOG_INFO;

    if (strcmp(level, "ERROR") == 0) priority = LOG_ERR;
    else if (strcmp(level, "DEBUG") == 0) priority = LOG_DEBUG;
    else if (strcmp(level, "WARNING") == 0) priority = LOG_WARNING;

    openlog(module, LOG_PID, LOG_USER);
    syslog(priority, "%s", msg);
    closelog();

    /* Extra säkerhet: skriv även till fil */
    log_to_file(module, msg, level);
}