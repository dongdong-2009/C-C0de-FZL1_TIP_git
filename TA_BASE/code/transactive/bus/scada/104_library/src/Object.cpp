#include "Btype.h"

#ifdef __OS_WIN32_
#include <windows.h>
#else
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#endif

#include <stdio.h>
#include <stdarg.h>

#include "Object.h"

FILE* log_file = 0;
#ifdef __OS_WIN32_
CRITICAL_SECTION log_file_mutex;
#else
pthread_mutex_t log_file_mutex;
#endif

b_bool Object::open_logfile(char* file_name)
{
  if(log_file)
    close_logfile();

  log_file = fopen(file_name, "a");
  if(!log_file)
    return b_false;

#ifdef __OS_WIN32_
  InitializeCriticalSection(&log_file_mutex);
#else
  if(pthread_mutex_init(&log_file_mutex, 0)) {
    fclose(log_file);
    return b_false;
  }
#endif

  return b_true;
}

void Object::close_logfile()
{
  if(log_file) {
#ifdef __OS_WIN32_
    fclose(log_file);
    DeleteCriticalSection(&log_file_mutex);
#else
    fclose(log_file);
    pthread_mutex_destroy(&log_file_mutex);
    log_file = 0;
#endif
  }

  log_file = 0;
}

void Object::trace(b_i32 level, char* fmt, ...)
{
#ifdef __OS_WIN32_
  SYSTEMTIME prtm;
#else
  time_t timenow;
  struct tm prtm;
#endif

  if(!log_file)
    return;

#ifdef __OS_WIN32_
  GetLocalTime(&prtm);
  EnterCriticalSection(&log_file_mutex);

  fprintf(log_file, "%d-%02d-%02d: %02d:%02d:%02d  ",
	  prtm.wYear, prtm.wMonth, prtm.wDay,
	  prtm.wHour, prtm.wMinute, prtm.wSecond);
#else
  time(&timenow);
  localtime_r(&timenow, &prtm);

  pthread_mutex_lock(&log_file_mutex);
  fprintf(log_file, "%d-%02d-%02d: %02d:%02d:%02d  ",
	  prtm.tm_year + 1900, prtm.tm_mon + 1, prtm.tm_mday,
	  prtm.tm_hour, prtm.tm_min, prtm.tm_sec);
#endif

  if(level == ERROR_MESSAGE)
    fprintf(log_file, " ERROR:   ");
  else if(level == WARNING_MESSAGE)
    fprintf(log_file, " WARNING: ");
  else
    fprintf(log_file, " MESSAGE: ");

  va_list args;

  va_start(args, fmt);
  vfprintf(log_file, fmt, args);
  va_end(args);

  fflush(log_file);

#ifdef __OS_WIN32_
  LeaveCriticalSection(&log_file_mutex);
#else
  pthread_mutex_unlock(&log_file_mutex);
#endif
}

void Object::trace(b_i32 type, b_i32 level, char* fmt, ...)
{
#ifdef __OS_WIN32_
  SYSTEMTIME prtm;
#else
  time_t timenow;
  struct tm prtm;
#endif

  if(!log_file)
    return;

#ifdef __OS_WIN32_
  GetLocalTime(&prtm);
  EnterCriticalSection(&log_file_mutex);

  fprintf(log_file, "%d-%02d-%02d: %02d:%02d:%02d  ",
	  prtm.wYear, prtm.wMonth, prtm.wDay,
	  prtm.wHour, prtm.wMinute, prtm.wSecond);
#else
  time(&timenow);
  localtime_r(&timenow, &prtm);

  pthread_mutex_lock(&log_file_mutex);
  fprintf(log_file, "%d-%02d-%02d: %02d:%02d:%02d  ",
	  prtm.tm_year + 1900, prtm.tm_mon + 1, prtm.tm_mday,
	  prtm.tm_hour, prtm.tm_min, prtm.tm_sec);
#endif

  if(level == ERROR_MESSAGE)
    fprintf(log_file, " ERROR:   ");
  else if(level == WARNING_MESSAGE)
    fprintf(log_file, " WARNING: ");
  else
    fprintf(log_file, " MESSAGE: ");

  va_list args;

  va_start(args, fmt);
  vfprintf(log_file, fmt, args);
  va_end(args);

  fflush(log_file);

#ifdef __OS_WIN32_
  LeaveCriticalSection(&log_file_mutex);
#else
  pthread_mutex_unlock(&log_file_mutex);
#endif
}

char* Object::to_string()
{
  return "Base Object";
}

b_ui16 Object::hash_code()
{
  return (b_ui16)0;
}
