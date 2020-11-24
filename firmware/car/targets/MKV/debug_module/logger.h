///**
//@file
//@purpose   HALFRED Hardware Abstraction Layer
//@version   $Revision: 1500 $
//@license   $License$
//@copyright $Copyright$
//@brief     Logging component
//
//*/
///** defgroup LOG LOG module
// *
// * <b> Introduction </b>
// *
// * The logging module in HAL (called LOG) provides a standard way for user
// * applications to report their condition. The LOG module supports:
// * - up to 255 logging channels, each capable of using different I/O interface
// * - Compile-time managed subchannels, for even better control of output messages
// * - Syslog-compatible standard log severity levels
// * - Variety of compile-time switches that reduces footprint of disabled options to 0
// * - cstdio-like API for comfortable and fast formatted output
// * - Runtime assertions
// *
// *  * The LOG module is also used internally by other HAL modules, to report their
// * errors and log status.
// *
// * <b> Module architecture </b>
// * The module itself consists of 1 header file and 1 source file.
// * The vast majority of log customization is provided via preprocessing layer.
// * Runtime operation uses standard C library, or  proprietary "printf" functions  for formatting output.
// * The attached custom library is vastly faster than the stdio implementation, and does not require using dynamic memory allocation.
// *
// * <b> Diagnostic level </b>
// * This module uses standard syslog log detail naming.
// * Below we list all of the detail levels in order from least detail to the most:
// * - LOG_LEVEL_EMERGENCY (1)
// * - LOG_LEVEL_ALERT (2)
// * - LOG_LEVEL_CRITICAL (3)
// * - LOG_LEVEL_ERROR (4)
// * - LOG_LEVEL_WARNING (5)
// * - LOG_LEVEL_NOTICE (6)
// * - LOG_LEVEL_INFO (7)
// * - LOG_LEVEL_DEBUG (8)
// *
// * The Log module provides simple way to manage globally the log detail of all channels:
// * In order to override default value (INFORMATIONAL level), user should define macro
// * LOG_MAX_LEVEL with the level of choice.
// *
// * <b> Log channels </b>
// * <b> Log subchannels </b>
// * <b> Assertions </b>
// * <b> Compile time API </b>
// * Global:
// * [Required] LOG_ENABLE (0) Global log on/off switch
// * [Optional] LOG_LAST_CHANNEL (0) amount of log channels available
// * [Optional] LOG_LEVEL (LOG_LEVEL_INFO) Global maximum verbosity of log messages. All messages of higher detail will be discarded at compile
// * time
// * Per module:
// * [Optional] LOG_CHANNEL (DEFAULT) Defines the Log channel. Every channel has to have unique name. If not provided, the LOG module will use
// * DEFAULT channel
// * [Required/Optional] ($CHANNEL)_LOG_CHANNEL (0) Defines descriptor number of LOG channel (basically each channel should have distinct
// * ($CHANNEL)_LOG_CHANNEL value).
// * The value 0 is reserved for DEFAULT LOG channel
// * [Optional] ($CHANNEL)_LOG_CHANNEL_ENABLED (1) main on/off switch of the LOG channel
// * [Optional] ($CHANNEL)_LOG_CHANNEL_LEVEL (LOG_LEVEL)
// * [Optional] LOG_SUBCHANNEL () Allows to create virtual subchannel over the same interface as parent channel.
// * [Optional] ($CHANNEL)_($SUBCHANNEL)_LOG_SUBCHANNEL_ENABLED (1) main on/off switch of the LOG subchannel
// * [Optional] ($CHANNEL)_LOG_CHANNEL_LEVEL (($CHANNEL)_LOG_CHANNEL_LEVEL)
// *
// */
//#ifndef LOG_H_
//#define LOG_H_
//
//extern "C" {
//
//
//
//#include <inttypes.h>
//#include <stdbool.h>
//#include <stdint.h>
//#include <string.h>
//
//void log_write(const uint8_t channel, const uint8_t level, const char* const channelName, const char* const filename, const int line,
//               const char* format, ...);
//}
//// clang-format on
//#endif  // #ifndef LOG_H_
/**
@file
@purpose   HALFRED Hardware Abstraction Layer
@version   $Revision: 1500 $
@license   $License$
@copyright $Copyright$
@brief     Logging component

*/
/** defgroup LOG LOG module
 *
 * <b> Introduction </b>
 *
 * The logging module in HAL (called LOG) provides a standard way for user
 * applications to report their condition. The LOG module supports:
 * - up to 255 logging channels, each capable of using different I/O interface
 * - Compile-time managed subchannels, for even better control of output messages
 * - Syslog-compatible standard log severity levels
 * - Variety of compile-time switches that reduces footprint of disabled options to 0
 * - cstdio-like API for comfortable and fast formatted output
 * - Runtime assertions
 *
 *  * The LOG module is also used internally by other HAL modules, to report their
 * errors and log status.
 *
 * <b> Module architecture </b>
 * The module itself consists of 1 header file and 1 source file.
 * The vast majority of log customization is provided via preprocessing layer.
 * Runtime operation uses standard C library, or  proprietary "printf" functions  for formatting output.
 * The attached custom library is vastly faster than the stdio implementation, and does not require using dynamic memory allocation.
 *
 * <b> Diagnostic level </b>
 * This module uses standard syslog log detail naming.
 * Below we list all of the detail levels in order from least detail to the most:
 * - LOG_LEVEL_EMERGENCY (1)
 * - LOG_LEVEL_ALERT (2)
 * - LOG_LEVEL_CRITICAL (3)
 * - LOG_LEVEL_ERROR (4)
 * - LOG_LEVEL_WARNING (5)
 * - LOG_LEVEL_NOTICE (6)
 * - LOG_LEVEL_INFO (7)
 * - LOG_LEVEL_DEBUG (8)
 *
 * The Log module provides simple way to manage globally the log detail of all channels:
 * In order to override default value (INFORMATIONAL level), user should define macro
 * LOG_MAX_LEVEL with the level of choice.
 *
 * <b> Log channels </b>
 * <b> Log subchannels </b>
 * <b> Assertions </b>
 * <b> Compile time API </b>
 * Global:
 * [Required] LOG_ENABLE (0) Global log on/off switch
 * [Optional] LOG_LAST_CHANNEL (0) amount of log channels available
 * [Optional] LOG_LEVEL (LOG_LEVEL_INFO) Global maximum verbosity of log messages. All messages of higher detail will be discarded at compile
 * time
 * Per module:
 * [Optional] LOG_CHANNEL (DEFAULT) Defines the Log channel. Every channel has to have unique name. If not provided, the LOG module will use
 * DEFAULT channel
 * [Required/Optional] ($CHANNEL)_LOG_CHANNEL (0) Defines descriptor number of LOG channel (basically each channel should have distinct
 * ($CHANNEL)_LOG_CHANNEL value).
 * The value 0 is reserved for DEFAULT LOG channel
 * [Optional] ($CHANNEL)_LOG_CHANNEL_ENABLED (1) main on/off switch of the LOG channel
 * [Optional] ($CHANNEL)_LOG_CHANNEL_LEVEL (LOG_LEVEL)
 * [Optional] LOG_SUBCHANNEL () Allows to create virtual subchannel over the same interface as parent channel.
 * [Optional] ($CHANNEL)_($SUBCHANNEL)_LOG_SUBCHANNEL_ENABLED (1) main on/off switch of the LOG subchannel
 * [Optional] ($CHANNEL)_LOG_CHANNEL_LEVEL (($CHANNEL)_LOG_CHANNEL_LEVEL)
 *
 */
#ifndef LOG_H_
#define LOG_H_

extern "C" {

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "logger_config.h"

typedef uint32_t (*log_timeWrapper)(void);
typedef void (*log_writeWrapper)(char c, void* const context);

#undef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG

#ifndef LOG_ENABLE
#define LOG_ENABLE 0
#endif  // #ifndef LOG_ENABLE

#ifndef LOG_LAST_CHANNEL
#define LOG_LAST_CHANNEL 0  // The default channel count is 1 (DEFAULT) since last channel is 0
#endif                      // #ifndef LOG_LAST_CHANNEL

#ifndef LOG_CHANNEL
#define LOG_CHANNEL DEFAULT
#endif  // #ifndef LOG_CHANNEL

#ifndef DEFAULT_LOG_CHANNEL
#define DEFAULT_LOG_CHANNEL 0
#elif DEFAULT_LOG_CHANNEL != 0
#error DEFAULT_LOG_CHANNEL redefined to non-zero value!
#endif  // #ifndef DEFAULT_LOG_CHANNEL

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_NOTICE  // The default log verbosity
#endif                              // #ifndef LOG_LEVEL


// Definitions of valid log levels (according to syslog)
#define LOG_LEVEL_EMERGENCY 0
#define LOG_LEVEL_ALERT 1
#define LOG_LEVEL_CRITICAL 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_WARNING 4
#define LOG_LEVEL_NOTICE 5
#define LOG_LEVEL_INFO 6
#define LOG_LEVEL_DEBUG 7

// The usage of the macros below is similar to csdio's printf function:
// The first argument is the format string, the latter arguments should be variables that will be substituted in the format string
#define log_emergency(...) LOG(LOG_LEVEL_EMERGENCY, __VA_ARGS__)
#define log_alert(...) LOG(LOG_LEVEL_ALERT, __VA_ARGS__)
#define log_critical(...) LOG(LOG_LEVEL_CRITICAL, __VA_ARGS__)
#define log_error(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)
#define log_warning(...) LOG(LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_notice(...) LOG(LOG_LEVEL_NOTICE, __VA_ARGS__)
#define log_info(...) LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define log_debug(...) LOG(LOG_LEVEL_DEBUG, __VA_ARGS__)

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define HAL_ASSERT(condition)                                                \
    if (!(condition)) do {                                                   \
            LOG_CRITICAL("Assertion failed in " __FILE__ ":%u\n", __LINE__); \
    } while (0)
#define HAL_ASSERT_AND_EXECUTE(condition) \
    HAL_ASSERT(condition);                \
    else

// clang-format off
// The content below must NOT be altered (unless you are a developer or the almighty guinea pig)

//#if 1 == LOG_ENABLE
// Sanity checks
   #if (LOG_LAST_CHANNEL < UINT16_MIN) || (LOG_LAST_CHANNEL > (UINT16_MAX - 1))
     #error LOG_MAX_CHANNEL_NUMBER is outside of bounds
   #endif // #if (LOG_LAST_CHANNEL < UINT16_MIN) || (LOG_LAST_CHANNEL > (UINT16_MAX - 1))

   #if (LOG_LEVEL < LOG_LEVEL_EMERGENCY) || (LOG_LEVEL > LOG_LEVEL_DEBUG)
     #error LOG_LEVEL is outside of bounds
   #endif // #if (LOG_LEVEL < LOG_LEVEL_EMERGENCY) || (LOG_LEVEL > LOG_LEVEL_DEBUG)

   // Define helper macros
   #define HALI_STR(x) HALI_STR1(x) // Stringifies macro (first level of indirection)
   #define HALI_STR1(x) #x // Stringifies macro (second level of indirection)
   #define CAT(x, y) CAT1(x, y) // Concatenates two macro (first level of indirection)
   #define CAT1(a, b) a ## b // Concatenates two macro (second level of indirection)
   #define SECOND(a1, a2, ...) a2 // Returns second argument
   #define IS_PROBE(...) SECOND(__VA_ARGS__, 0)
   #define PROBE() ~, 1
   #define NOT(x) IS_PROBE(CAT(NOT_, x)) // Logic negation of macro
   #define NOT_0 PROBE()
   #define BOOL(x) NOT(NOT(x)) // Returns logical value of the macro
   #define IF_ELSE(x) IF_ELSE1(BOOL(x)) // Compile time if statement. usage: IF_ELSE(x)(if_true)(else)
   #define IF_ELSE1(x) CAT1(IF_, x)
   #define IF_1(...) __VA_ARGS__ IF_1_ELSE
   #define IF_0(...)             IF_0_ELSE
   #define IF_1_ELSE(...)
   #define IF_0_ELSE(...) __VA_ARGS__


   // Verify if the log channel is enabled by macro ($LOG_CHANNEL)_LOG_CHANNEL_ENABLED (i.e.  APP_LOG_CHANNEL_ENABLED)
   // When the macro was defined and set to 1 (i.e. #define APP_LOG_CHANNEL_ENABLED 1) the value is 1
   // When the macro was defined and set to 0 (i.e. #define APP_LOG_CHANNEL_ENABLED 0) the value is 0
   // When the macro was NOT defined the value is 1
   // Notice that LOG_CHANNEL is always defined either explicitly, or by default to DEFAULT.
   #if 1 == IF_ELSE(CAT(LOG_CHANNEL, _LOG_CHANNEL_ENABLED))(1)(0)
      // Look for descriptor number for this channel (i.e. #define APP_LOG_CHANNEL 1)
      #define HALI_CHANNEL CAT(LOG_CHANNEL, _LOG_CHANNEL)
      // Check if using default channel - if not, perform sanity check on channel number
      // There is no need to check Default channel, as it is already verified
      #define HALI_IF_LOG_CHANNEL_DEFAULT 1
      #if (1 == CAT(HALI_IF_LOG_CHANNEL_, LOG_CHANNEL)) || (HALI_CHANNEL > 0)
         #if (HALI_CHANNEL <= LOG_LAST_CHANNEL)
            // Adjust the maximum log level at user's discretion (i.e #define APP_LOG_CHANNEL_LEVEL LOG_LEVEL_NOTICE)
            #define HALI_LOG_CHANNEL_LEVEL CAT(LOG_CHANNEL, _LOG_CHANNEL_LEVEL)
            #define HALI_LOG_IF_CHANNEL_LEVEL_0 1
            #if (1 == CAT(HALI_LOG_IF_CHANNEL_LEVEL_, HALI_LOG_CHANNEL_LEVEL)) || (HALI_LOG_CHANNEL_LEVEL > 0)
               #if HALI_LOG_CHANNEL_LEVEL <= LOG_LEVEL
                  #undef LOG_LEVEL
                  #define LOG_LEVEL HALI_LOG_CHANNEL_LEVEL
               #else
                  #warning ($LOG_CHANNEL)_LOG_CHANNEL_LEVEL is bigger than LOG_LEVEL - defaults to LOG_LEVEL
               #endif // #if HALI_LOG_CHANNEL_LEVEL <= LOG_LEVEL
            #endif // #if (1 == CAT(HALI_LOG_IF_CHANNEL_LEVEL_, HALI_LOG_CHANNEL_LEVEL)) || (HALI_LOG_CHANNEL_LEVEL > 0)
            #undef HALI_LOG_IF_CHANNEL_LEVEL_0

            // At this point, all requisitions to enable log CHANNEL have been met

            // Check if LOG_SUBCHANNEL macro was defined, that would indicate that this is a subchannel log
            #ifdef LOG_SUBCHANNEL
               // Verify if the log channel is enabled by macro ($LOG_CHANNEL)_($LOG_SUBCHANNEL)_LOG_SUBCHANNEL_ENABLED (i.e.  APP_FOO_LOG_SUBCHANNEL_ENABLED)
               // When the macro was defined and set to 1 (i.e. #define APP_FOO_LOG_CHANNEL_ENABLED 1) the value is 1
               // When the macro was defined and set to 0 (i.e. #define APP_FOO_LOG_CHANNEL_ENABLED 0) the value is 0
               // When the macro was NOT defined the value is 1
               #if 1 == IF_ELSE(CAT(CAT(CAT(LOG_CHANNEL, _), LOG_SUBCHANNEL), _LOG_SUBCHANNEL_ENABLED))(1)(0)
                  #define HALI_LOG_SUBCHANNEL_LEVEL CAT(CAT(CAT(LOG_CHANNEL, _), LOG_SUBCHANNEL), _LOG_SUBCHANNEL_LEVEL)
                  #define HALI_LOG_IF_SUBCHANNEL_LEVEL_0 1
                  #if (1 == CAT(HALI_LOG_IF_SUBCHANNEL_LEVEL_, HALI_LOG_SUBCHANNEL_LEVEL)) || (HALI_LOG_SUBCHANNEL_LEVEL > 0)
                     #if (HALI_LOG_SUBCHANNEL_LEVEL <= LOG_LEVEL)
                        #undef LOG_LEVEL
                        #define LOG_LEVEL HALI_LOG_SUBCHANNEL_LEVEL
                     #else
                        #warning ($LOG_CHANNEL)_($LOG_SUBCHANNEL)_LOG_SUBCHANNEL_LEVEL is bigger than LOG_LEVEL - defaults to LOG_LEVEL
                     #endif // (HALI_LOG_SUBCHANNEL_LEVEL <= LOG_LEVEL)
                  #endif // #if (1 == CAT(HALI_LOG_IF_SUBCHANNEL_LEVEL_, HALI_LOG_SUBCHANNEL_LEVEL)) || (HALI_LOG_SUBCHANNEL_LEVEL > 0)
                  #undef HALI_LOG_IF_CHANNEL_LEVEL_0

                  #define HALI_LOG_HEADER CAT(CAT(LOG_CHANNEL, _), LOG_SUBCHANNEL)
                  #define HALI_LOG_ENABLED 1
               #endif // #if 1 == IF_ELSE(CAT(CAT(CAT(LOG_CHANNEL, _), LOG_SUBCHANNEL), _LOG_SUBCHANNEL_ENABLED))(1)(0)
            #else
               #define HALI_LOG_HEADER LOG_CHANNEL
               #define HALI_LOG_ENABLED 1
            #endif // #ifdef LOG_SUBCHANNEL
         #else
            #warning Channel descriptor (($LOG_CHANNEL)_LOG_CHANNEL) is bigger than LOG_LAST_CHANNEL - the channel will remain disabled
         #endif // #if (HALI_CHANNEL <= LOG_LAST_CHANNEL)
      #else
         #warning Channel descriptor ($LOG_CHANNEL)_LOG_CHANNEL_LEVEL not defined - the channel will remain disabled
      #endif // #if (1 == CAT(HALI_IF_LOG_CHANNEL_, LOG_CHANNEL)) || (HALI_CHANNEL > 0)
      #undef HALI_IF_LOG_CHANNEL_DEFAULT
   #endif // #if 1 == IF_ELSE(CAT(LOG_CHANNEL, _LOG_CHANNEL_ENABLED))(1)(0)
//#endif // #if 1 == LOG_ENABLE

#if 1 == HALI_LOG_ENABLED
void log_write(const uint8_t channel, const uint8_t level, const char* const channelName,  const char* filename, const int line, const char* format, ...);
void log_setTimesourceFunction(log_timeWrapper timestampFunction);
void log_setWriteFunction(log_writeWrapper writeFunction);
  #define LOG(level, ...) if(level <= LOG_LEVEL) log_write(HALI_CHANNEL, UINT8_C(level), HALI_STR(HALI_LOG_HEADER), __FILENAME__, __LINE__, __VA_ARGS__);
#else
#define LOG(level, ...)
#endif // #if 1 == HALI_LOG_ENABLED
}
// clang-format on
#endif  // #ifndef LOG_H_