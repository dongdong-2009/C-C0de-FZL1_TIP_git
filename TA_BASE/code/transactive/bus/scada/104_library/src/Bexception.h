/******************************************************************************
                                Bexception.h
                               ---------------
******************************************************************************/

#ifndef __BASE_EXCEPTION_H
#define __BASE_EXCEPTION_H

#include <iostream>
#include <stdio.h>

#include "Btype.h"

#define EXCEPTION_MESSAGE_LEN 80

class BaseException
{
 public:
  BaseException() {
    init_msg();
  }

  virtual void init_msg() {
#ifdef __OS_WIN32_
    _snprintf(msg, EXCEPTION_MESSAGE_LEN, "Has Exception");
#else
    snprintf(msg, EXCEPTION_MESSAGE_LEN,
	     "Has Exception");
#endif
  }

  char* get_msg() { return msg; }

 public:
  friend std::ostream& operator<<(std::ostream& os, BaseException& ex);

 protected:
  char msg[EXCEPTION_MESSAGE_LEN];
};

class MemoryException : public BaseException
{
 public:
  MemoryException() {
    init_msg();
  }

  virtual void init_msg() {
#ifdef __OS_WIN32_
    _snprintf(msg, EXCEPTION_MESSAGE_LEN, "Not enough memory");
#else
    snprintf(msg, EXCEPTION_MESSAGE_LEN,
	     "Not enough memory");
#endif
  }
};

class ProfileException : public BaseException 
{
 public:
  ProfileException() {
    init_msg();
  }

  virtual void init_msg() {
#ifdef __OS_WIN32_
    _snprintf(msg, EXCEPTION_MESSAGE_LEN, "Read profile error");
#else
    snprintf(msg, EXCEPTION_MESSAGE_LEN,
	     "Read profile error");
#endif	     
  }
};

#endif
