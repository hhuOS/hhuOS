#include "LoggerInterface.h"
#include "../../../lib/util/base/String.h"
#include "../../../kernel/log/Logger.h"

Kernel::Logger logger_service = Kernel::Logger::get("USB");
Kernel::Logger logger_controller = Kernel::Logger::get("UHCI");
Kernel::Logger logger_device = Kernel::Logger::get("USB_Device");
Kernel::Logger logger_default = Kernel::Logger::get("USB DEFAULT"); // for all other purposes 

const uint8_t USB_SERVICE_LOGGER_TYPE = 0;
const uint8_t USB_CONTROLLER_LOGGER_TYPE = 1;
const uint8_t USB_DEVICE_LOGGER_TYPE = 2;

const uint8_t LOGGER_LEVEL_TRACE = 0;
const uint8_t LOGGER_LEVEL_DEBUG = 1;
const uint8_t LOGGER_LEVEL_INFO  = 2;
const uint8_t LOGGER_LEVEL_WARN  = 3;
const uint8_t LOGGER_LEVEL_ERROR = 4;

void setLevel_c(struct Logger_C* logger_c, const char* buffer){
    ((Kernel::Logger*)logger_c->logger_pointer)->setLevel(buffer);
}

void trace_c(struct Logger_C* logger_c, const char* message, ...){
    va_list args;
    va_start(args,message);
    const Util::String s = Util::String::vformat(message,args);
    va_end(args);
    ((Kernel::Logger*)logger_c->logger_pointer)->trace(s);
}

void debug_c(struct Logger_C* logger_c, const char* message, ...){
    va_list args;
    va_start(args,message);
    const Util::String s = Util::String::vformat(message,args);
    va_end(args);
    ((Kernel::Logger*)logger_c->logger_pointer)->debug(s);
}

void info_c(struct Logger_C* logger_c, const char* message, ...){
    va_list args;
    va_start(args,message);
    const Util::String s = Util::String::vformat(message,args);
    va_end(args);
    ((Kernel::Logger*)logger_c->logger_pointer)->info(s);
}

void warn_c(struct Logger_C* logger_c, const char* message, ...){
    va_list args;
    va_start(args,message);
    const Util::String s = Util::String::vformat(message,args);
    va_end(args);

    ((Kernel::Logger*)logger_c->logger_pointer)->warn(s);
}

void error_c(struct Logger_C* logger_c, const char* message, ...){
    va_list args;
    va_start(args,message);
    const Util::String s = Util::String::vformat(message,args);
    va_end(args);

    ((Kernel::Logger*)logger_c->logger_pointer)->error(s);
}

void new_logger(struct Logger_C* logger, uint8_t type, uint8_t level){
    logger->setLevel_c = &setLevel_c;
    logger->trace_c = &trace_c;
    logger->debug_c = &debug_c;
    logger->info_c = &info_c;
    logger->warn_c = &warn_c;
    logger->error_c = &error_c;

    switch(type){
        case USB_SERVICE_LOGGER_TYPE: logger->logger_pointer = &logger_service;
            break;
        case USB_CONTROLLER_LOGGER_TYPE: logger->logger_pointer = &logger_controller;
            break;
        case USB_DEVICE_LOGGER_TYPE: logger->logger_pointer = &logger_device;
            break;
        default:
            logger->logger_pointer = &logger_default;    
    }

    if(logger->logger_pointer != (void*)0){
        switch(level){
            case LOGGER_LEVEL_TRACE : logger->setLevel_c(logger, "TRC");
                break;
            case LOGGER_LEVEL_DEBUG : logger->setLevel_c(logger, "DBG");
                break;
            case LOGGER_LEVEL_INFO  : logger->setLevel_c(logger, "INF");
                break;
            case LOGGER_LEVEL_WARN  : logger->setLevel_c(logger, "WRN");
                break;
            case LOGGER_LEVEL_ERROR : logger->setLevel_c(logger, "ERR");
                break;
            default: break;                    
        }
    }
}