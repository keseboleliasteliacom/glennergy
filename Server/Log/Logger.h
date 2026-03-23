#ifndef LOGGER_H
#define LOGGER_H
#include <time.h>

/**
 * @file Logger.h
 * @brief Logging interface with support for levels, formatting, and async processing via pipe.
 * @defgroup Logger Logger
 * @{
 */

/**
 * @enum LogLevel
 * @brief Defines log levels used by the logger.
 */
typedef enum {
    LOG_LEVEL_DEBUG,  /**< Debug information, most verbose */
    LOG_LEVEL_INFO,   /**< Standard informational messages */
    LOG_LEVEL_WARN,   /**< Warnings that may require attention */
    LOG_LEVEL_ERROR   /**< Errors that must be addressed */
} LogLevel;

/**
 * @def MODULE_NAME
 * @brief Name of the module being logged. Must be defined in each .c file before including Logger.
 *
 * @note Defaults to "UNKNOWN" if not explicitly defined.
 */
#ifndef MODULE_NAME
#define MODULE_NAME "UNKNOWN"
#endif

/**
 * @def LOG_DEBUG
 * @brief Logs a debug message with module name.
 */
#define LOG_DEBUG(fmt, ...)   log_MessageFmt(LOG_LEVEL_DEBUG, MODULE_NAME, fmt, ##__VA_ARGS__)

/**
 * @def LOG_INFO
 * @brief Logs an info message with module name.
 */
#define LOG_INFO(fmt, ...)    log_MessageFmt(LOG_LEVEL_INFO, MODULE_NAME, fmt, ##__VA_ARGS__)

/**
 * @def LOG_WARNING
 * @brief Logs a warning message with module name.
 */
#define LOG_WARNING(fmt, ...) log_MessageFmt(LOG_LEVEL_WARN, MODULE_NAME, fmt, ##__VA_ARGS__)

/**
 * @def LOG_ERROR
 * @brief Logs an error message with module name.
 */
#define LOG_ERROR(fmt, ...)   log_MessageFmt(LOG_LEVEL_ERROR, MODULE_NAME, fmt, ##__VA_ARGS__)

/**
 * @brief Initializes the logger system.
 *
 * Creates a pipe and spawns a child process responsible for writing logs to file.
 *
 * @param log_path Path to log file. If NULL, a default filename is used.
 * @return 0 on success, -1 on failure.
 *
 * @pre Must be called before any logging functions.
 * @post Logger process is running and ready to receive messages.
 *
 * @warning Uses fork() and pipe(), creating a separate process.
 * @note Log directory must be writable before calling this function.
 */
int log_Init(const char* log_path);

/**
 * @brief Logs a message directly.
 *
 * @param level Log level.
 * @param module Module name.
 * @param msg Message string to log.
 *
 * @pre Logger must be initialized.
 *
 * @warning Thread-safe via internal mutex.
 * @note Message is sent via pipe to logging process.
 */
void log_Message(LogLevel level, const char* module, const char* msg);

/**
 * @brief Logs a formatted message.
 *
 * @param level Log level.
 * @param module Module name.
 * @param fmt printf-style format string.
 *
 * @pre Logger must be initialized.
 *
 * @note Internally formats string before forwarding to log_Message().
 */
void log_MessageFmt(LogLevel level, const char* module, const char* fmt, ...);

/**
 * @brief Cleans up logger resources and waits for child process.
 *
 * @post Pipe is closed.
 * @post Child process is terminated and waited on.
 *
 * @warning Blocks until logger process exits.
 */
void log_Cleanup(void);

/**
 * @brief Sets the global log level.
 *
 * @param level New log level.
 *
 * @note Messages below this level will be ignored.
 */
void log_SetLevel(LogLevel level);

/**
 * @brief Returns string representation of log level.
 *
 * @param level Log level.
 * @return String: "DEBUG", "INFO", "WARNING", "ERROR".
 *
 * @note Returned string is static and must not be freed.
 */
const char* log_GetLevelString(LogLevel level);

/**
 * @brief Closes the write file descriptor without affecting the child process.
 *
 * @post write_fd is closed and set to -1.
 *
 * @note Useful when shutting down parent while allowing logger to flush.
 */
void log_CloseWrite(void);

/** @} */

#endif // LOGGER_H