#ifndef LOGGER_H
#define LOGGER_H
#include <time.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

#define LOG_DEBUG(module, msg)   log_Message(LOG_LEVEL_DEBUG, module, msg)
#define LOG_INFO(module, msg)    log_Message(LOG_LEVEL_INFO, module, msg)
#define LOG_WARNING(module, msg) log_Message(LOG_LEVEL_WARN, module, msg)
#define LOG_ERROR(module, msg)   log_Message(LOG_LEVEL_ERROR, module, msg)

int log_Init(void);
void log_Message(LogLevel level, const char* module, const char* msg);
void log_Cleanup(void);

void log_SetLevel(LogLevel level);
void log_CloseWrite(void);

#endif // LOGGER_H