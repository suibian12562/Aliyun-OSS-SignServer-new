#ifndef LOGGER_H
#define LOGGER_H

#include "Poco/Logger.h"

using  Poco::Logger;                    // for global decorator  

namespace MyLogger
{
#define logger_handle   (Logger::get("logger"))

    void Logger_initiation();  // initiation
    void Setup_logger();       // init only once 

}

#endif