/******************************************************************************
                                  "Object.h"
                               ---------------

******************************************************************************/

#ifndef __OBJECT_H
#define __OBJECT_H

#include "Btype.h"

#define ERROR_MESSAGE   0
#define WARNING_MESSAGE 1
#define GENERAL_MESSAGE 2

#define MESSAGE_GLOBAL 0

class Object
{
 public:
  //Open log file
  static b_bool open_logfile(char* file_name);

  //Close log file
  static void close_logfile();

  //write message to log file that opened by open_logfile
  static void trace(b_i32 type, b_i32 level, char* fmt, ...);
  void trace(b_i32 level, char* fmt, ...);

  //Translate object to string, may implement by sub class
  virtual char* to_string();

  //return object hash code, may implement by sub class
  virtual b_ui16 hash_code();
};

#endif
