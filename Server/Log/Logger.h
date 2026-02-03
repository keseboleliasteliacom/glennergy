#ifndef LOGGER_H
#define LOGGER_H
#include <time.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

// Define MODULE_NAME in each .c file before including this header
#ifndef MODULE_NAME
#define MODULE_NAME "UNKNOWN"
#endif

#define LOG_DEBUG(msg)   log_Message(LOG_LEVEL_DEBUG, MODULE_NAME, msg)
#define LOG_INFO(msg)    log_Message(LOG_LEVEL_INFO, MODULE_NAME, msg)
#define LOG_WARNING(msg) log_Message(LOG_LEVEL_WARN, MODULE_NAME, msg)
#define LOG_ERROR(msg)   log_Message(LOG_LEVEL_ERROR, MODULE_NAME, msg)

int log_Init(const char* log_path);  // Pass NULL for default: Logs/log.txt
void log_Message(LogLevel level, const char* module, const char* msg);
void log_Cleanup(void);

void log_SetLevel(LogLevel level);
const char* log_GetLevelString(LogLevel level);
void log_CloseWrite(void);

#endif // LOGGER_H