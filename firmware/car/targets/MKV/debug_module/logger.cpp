//
// Created by kamil on 04.01.2020.
//

#include <filesystem>

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

namespace fs = std::filesystem;

#include "logger.h"

#include "printf.h"

static const char* const abbreviations[] = {"EMG", "ALT", "CRT ", "ERR", "WRN", "NOT", "INF", "DBG"};

static log_timeWrapper  timestamp = NULL;
static log_writeWrapper write     = NULL;

// -----------------------------------------------------------------------------
//  HAL_LOG_Log
// -----------------------------------------------------------------------------
void log_write(const uint8_t channel, const uint8_t level, const char* const channelName, const char* const filename, const int line, const char* format, ...) {
    (void)channel;
    if (write) {
        std::string str   = filename;
        size_t      shift = str.find_last_of("\\");
        // Checking if channel number is in bounds was done in preprocessing stage
        uint32_t currentTime = 0;
        if (timestamp) {
            currentTime = timestamp();
        }
        fctprintf(write, NULL, "%" PRIu32 " | %s: %4d | %s(%s): ", currentTime, filename + shift + 1, line, channelName, abbreviations[level]);
        // Write the very log message
        va_list args;
        va_start(args, format);
        vfctprintf(write, NULL, format, args);
        va_end(args);
        fctprintf(write, NULL, "\n\r");
    }
} /* HAL_LOG_Log */

void log_setTimesourceFunction(log_timeWrapper timestampFunction) {
    if (!timestampFunction) {
        while (1) {
            ;
        }
    }
    timestamp = timestampFunction;
}

void log_setWriteFunction(log_writeWrapper writeFunction) {
    if (!writeFunction) {
        while (1) {
            ;
        }
    }
    write = writeFunction;
}

