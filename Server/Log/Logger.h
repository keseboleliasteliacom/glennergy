#include <time.h>


void log_message(
    const char* module,
    const char* msg,
    const char* level, // Eventuellt köra Enum istället?
    time_t timestamp
);