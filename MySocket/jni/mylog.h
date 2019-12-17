#ifndef com_android_log_print_h
#define com_android_log_print_h

#include <android/log.h>
#include <iostream>

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)

/*
* Android log priority values, in ascending priority order.
typedef enum android_LogPriority {
   ANDROID_LOG_UNKNOWN = 0,
   ANDROID_LOG_DEFAULT,  1  only for SetMinPriority() 
   ANDROID_LOG_VERBOSE,  2
   ANDROID_LOG_DEBUG,    3
   ANDROID_LOG_INFO,     4
   ANDROID_LOG_WARN,     5
   ANDROID_LOG_ERROR,    6
   ANDROID_LOG_FATAL,    7
   ANDROID_LOG_SILENT,   8 only for SetMinPriority(); must be last 
} android_LogPriority; */

#define ERROR ANDROID_LOG_ERROR
#define WRAN  ANDROID_LOG_WARN
#define INFO  ANDROID_LOG_INFO
#define DEBUG ANDROID_LOG_DEBUG

extern int g_logLevel;

#define LogOut(level, cFormat, ...) \
  logOut(__FILENAME__, __FUNCTION__, __LINE__, level, cFormat, ##__VA_ARGS__);
inline void logOut(const char* file, const char* func, int line, int level, const char* format, ...)
{
  if (g_logLevel > level)
    return;

  char buf[1024] = {0};
  va_list args;
  va_start (args, format);
  //vsprintf(buf, format, args);
  vsnprintf(buf, sizeof(buf)-4, format, args);

  va_end(args);

  __android_log_print(level, "MysherCamera", "%s_%d(%s):%s\n", file, line, func, buf);
}

#endif
