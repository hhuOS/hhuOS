#ifndef LoggerInterface__include
#define LoggerInterface__include

#include "stdint.h"

typedef void* Logger_P;

struct Logger_C{
    void (*setLevel_c)(struct Logger_C* logger,const char* buffer);
    void (*trace_c)(struct Logger_C* logger ,const char* message, ...);
    void (*debug_c)(struct Logger_C* logger ,const char* message, ...);
    void (*info_c)(struct Logger_C* logger, const char* message, ...);
    void (*warn_c)(struct Logger_C* logger, const char* message, ...);
    void (*error_c)(struct Logger_C* logger, const char* message, ...);

    void (*new_logger)(struct Logger_C* logger, uint8_t type, uint8_t level);
    Logger_P logger_pointer;
};

extern const uint8_t USB_SERVICE_LOGGER_TYPE;
extern const uint8_t USB_CONTROLLER_LOGGER_TYPE;
extern const uint8_t USB_DEVICE_LOGGER_TYPE;

extern const uint8_t LOGGER_LEVEL_TRACE;
extern const uint8_t LOGGER_LEVEL_DEBUG;
extern const uint8_t LOGGER_LEVEL_INFO;
extern const uint8_t LOGGER_LEVEL_WARN;
extern const uint8_t LOGGER_LEVEL_ERROR;

typedef struct Logger_C Logger_C;

#ifdef __cplusplus

extern "C"{
#endif

void setLevel_c(struct Logger_C* logger, const char* buffer);

void trace_c(struct Logger_C* logger, const char* message, ...);
void debug_c(struct Logger_C* logger, const char* message, ...);
void info_c(struct Logger_C* logger, const char* message, ...);
void warn_c(struct Logger_C* logger, const char* message, ...);
void error_c(struct Logger_C* logger, const char* message, ...);

void new_logger(struct Logger_C* logger, uint8_t type, uint8_t level);

#ifdef __cplusplus    
}    
#endif

#endif