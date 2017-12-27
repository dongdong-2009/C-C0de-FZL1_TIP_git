/******************************************************************************
                                   Btype.h
                               ---------------

******************************************************************************/

#ifndef __BTYPE_H
#define __BTYPE_H

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_)
#define __OS_WIN32_
#endif

#if defined(__hpux)
#define __OS_HPUX_
#endif

#if defined(__alpha)
#define __OS_TRU64_
#endif
#if defined(__unix) || defined(__linux)
#define __OS_UNIX_
#endif

#if defined(__sparc__) || defined(__x86__)
#define __CPU_BITS 32
#endif

#if __CPU_BITS != 32 && __CPU_BITS != 64
//#error Must define macro __CPU_BITS=32(64)
#endif

typedef char b_i8;
typedef unsigned char b_ui8;
typedef b_i8 b_bool;
typedef short b_i16;
typedef unsigned short b_ui16;
typedef unsigned short b_wchar;
typedef int b_i32;
typedef unsigned int b_ui32;



#ifdef __OS_WIN32_
	typedef unsigned __int64     UINT64;
	typedef int socklen_t;   
#else
	typedef unsigned long long     UINT64;
#endif
  

#ifdef __OS_WIN32_
  typedef __int64 b_i64;
  typedef unsigned __int64 b_ui64;
#else
  #if __CPU_BITS==64
    typedef long b_i64;
    typedef unsigned long b_ui64;
  #elif __CPU_BITS==32
    typedef long long b_i64;
    typedef unsigned long long b_ui64;
  #endif
#endif

typedef void* pointer_type;
typedef long pointer_int;

typedef b_ui32 b_in4addr;

#ifdef MAX_B_I8
#undef MAX_B_I8
#endif
#define MAX_B_I8   127

#ifdef MAX_B_UI8
#undef MAX_B_UI8
#endif
#define MAX_B_UI8  255

#ifdef MAX_B_I16
#undef MAX_B_I16
#endif
#define MAX_B_I16  32767

#ifdef MAX_B_UI16
#undef MAX_B_UI16
#endif
#define MAX_B_UI16 65535

#ifdef MAX_B_I32
#undef MAX_B_I32
#endif
#define MAX_B_I32  2147483647

#ifdef MAX_B_UI32
#undef MAX_B_UI32
#endif
#define MAX_B_UI32 4294967295

#ifdef __OS_WIN32_
  #define POINTER_SIZE 4
#else
  #if __CPU_BITS==64
    #define POINTER_SIZE 8
  #elif __CPU_BITS==32
    #define POINTER_SIZE 4
  #endif
#endif

#define b_true  1
#define b_false 0
#define b_null  (void*)0

#ifdef __cplusplus
  #ifdef __OS_WIN32_
    #define B_EXTERN extern "C" __declspec( dllexport )
    #define B_EXPORT __declspec( dllexport )
  #else
    #define B_EXTERN extern "C"
    #define B_EXPORT
  #endif
#else
  #ifdef __OS_WIN32_
    #define B_EXTERN __declspec( dllexport )
    #define B_EXPORT __declspec( dllexport )
  #else
    #define B_EXTERN
    #define B_EXPORT
  #endif
#endif

#ifndef B_LITTLE_ENDIAN
#define B_LITTLE_ENDIAN 1
#endif

#ifndef B_BIG_ENDIAN
#define B_BIG_ENDIAN 2
#endif

#ifdef __OS_WIN32_
#define B_BYTE_ORDER B_LITTLE_ENDIAN
#else
  #ifdef __OS_HPUX_
    #if _BIG_ENDIAN==1
      #define B_BYTE_ORDER B_BIG_ENDIAN
    #elif _LITTLE_ENDIAN==1
      #define B_BYTE_ORDER B_LITTLE_ENDIAN
    #else
      #if __ia64==1
         #define B_BYTE_ORDER B_LITTLE_ENDIAN
      #else
        #include <arpa/nameser_compat.h>
      #endif
    #endif
  #else //ifdef __OS_HPUX_
    #include <netinet/in.h>
  #endif

  #ifndef B_BYTE_ORDER
     #if __BYTE_ORDER == LITTLE_ENDIAN || BYTE_ORDER == LITTLE_ENDIAN
       #define B_BYTE_ORDER B_LITTLE_ENDIAN
     #elif __BYTE_ORDER == BIG_ENDIAN || BYTE_ORDER == BIG_ENDIAN
       #define B_BYTE_ORDER B_BIG_ENDIAN
     #else
       #error "Dont know byte order"
     #endif
  #endif
#endif //ifdef __OS_WIN32_

#if B_BYTE_ORDER == B_LITTLE_ENDIAN
#define FILL_BUFFER_DOUBLE(buffer, data)                                  \
  do {                                                                    \
    double __temp_fbd_double = data;                                      \
    *((b_i8*)buffer) = *((b_i64*)&__temp_fbd_double) >> 56;               \
    *((b_i8*)buffer + 1) = (*((b_i64*)&__temp_fbd_double) >> 48) & 0xff;  \
    *((b_i8*)buffer + 2) = (*((b_i64*)&__temp_fbd_double) >> 40) & 0xff;  \
    *((b_i8*)buffer + 3) = (*((b_i64*)&__temp_fbd_double) >> 32) & 0xff;  \
    *((b_i8*)buffer + 4) = (*((b_i64*)&__temp_fbd_double) >> 24) & 0xff;  \
    *((b_i8*)buffer + 5) = (*((b_i64*)&__temp_fbd_double) >> 16) & 0xff;  \
    *((b_i8*)buffer + 6) = (*((b_i64*)&__temp_fbd_double) >> 8) & 0xff;   \
    *((b_i8*)buffer + 7) = *((b_i64*)&__temp_fbd_double) & 0xff;          \
  } while(0)

#define FILL_BUFFER_FLOAT(buffer, data)                                   \
  do {                                                                    \
    float __temp_fbf_float = data;                                        \
    *((b_i8*)buffer) = *((b_i32*)&__temp_fbf_float) >> 24;                \
    *((b_i8*)buffer + 1) = (*((b_i32*)&__temp_fbf_float) >> 16) & 0xff;   \
    *((b_i8*)buffer + 2) = (*((b_i32*)&__temp_fbf_float) >> 8) & 0xff;    \
    *((b_i8*)buffer + 3) = *((b_i32*)&__temp_fbf_float) & 0xff;           \
  } while(0)

#define FILL_BUFFER_I64(buffer, data)                                     \
  do {                                                                    \
    b_i64 __temp_fbi_i64 = data;                                          \
    *((b_i8*)buffer) = *((b_i8*)&__temp_fbi_i64 + 7);                     \
    *((b_i8*)buffer + 1) = *((b_i8*)&__temp_fbi_i64 + 6);                 \
    *((b_i8*)buffer + 2) = *((b_i8*)&__temp_fbi_i64 + 5);                 \
    *((b_i8*)buffer + 3) = *((b_i8*)&__temp_fbi_i64 + 4);                 \
    *((b_i8*)buffer + 4) = *((b_i8*)&__temp_fbi_i64 + 3);                 \
    *((b_i8*)buffer + 5) = *((b_i8*)&__temp_fbi_i64 + 2);                 \
    *((b_i8*)buffer + 6) = *((b_i8*)&__temp_fbi_i64 + 1);                 \
    *((b_i8*)buffer + 7) = *((b_i8*)&__temp_fbi_i64);                     \
  } while(0)

#define FILL_BUFFER_I32(buffer, data)                                     \
  do {                                                                    \
    *((b_i8*)buffer) = ((b_i32)data) >> 24;                               \
    *((b_i8*)buffer + 1) = (((b_i32)data) >> 16) & 0xff;                  \
    *((b_i8*)buffer + 2) = (((b_i32)data) >> 8) & 0xff;                   \
    *((b_i8*)buffer + 3) = ((b_i32)data) & 0xff;                          \
  } while(0)

#define FILL_BUFFER_I16(buffer, data)                                     \
  do {                                                                    \
    *((b_i8*)buffer) = ((b_i16)data) >> 8;                                \
    *((b_i8*)buffer + 1) = ((b_i16)data) & 0xff;                          \
  } while(0)

#define FILL_LE_BUFFER_DOUBLE(buffer, data)                               \
  do {                                                                    \
    double __temp_fbd_double = data;                                      \
    *((b_i8*)buffer + 7) = *((b_i8*)&__temp_fbd_double + 7);              \
    *((b_i8*)buffer + 6) = *((b_i8*)&__temp_fbd_double + 6);              \
    *((b_i8*)buffer + 5) = *((b_i8*)&__temp_fbd_double + 5);              \
    *((b_i8*)buffer + 4) = *((b_i8*)&__temp_fbd_double + 4);              \
    *((b_i8*)buffer + 3) = *((b_i8*)&__temp_fbd_double + 3);              \
    *((b_i8*)buffer + 2) = *((b_i8*)&__temp_fbd_double + 2);              \
    *((b_i8*)buffer + 1) = *((b_i8*)&__temp_fbd_double + 1);              \
    *((b_i8*)buffer) = *((b_i8*)&__temp_fbd_double);                      \
  } while(0)

#define FILL_LE_BUFFER_FLOAT(buffer, data)                                \
  do {                                                                    \
    float __temp_fbf_float = data;                                        \
    *((b_i8*)buffer + 3) = *((b_i32*)&__temp_fbf_float) >> 24;            \
    *((b_i8*)buffer + 2) = (*((b_i32*)&__temp_fbf_float) >> 16) & 0xff;   \
    *((b_i8*)buffer + 1) = (*((b_i32*)&__temp_fbf_float) >> 8) & 0xff;    \
    *((b_i8*)buffer) = *((b_i32*)&__temp_fbf_float) & 0xff;               \
  } while(0)

#define FILL_LE_BUFFER_I64(buffer, data)                                  \
  do {                                                                    \
    b_i64 __temp_fbi_i64 = data;                                          \
    *((b_i8*)buffer + 7) = *((b_i8*)&__temp_fbi_i64 + 7);                 \
    *((b_i8*)buffer + 6) = *((b_i8*)&__temp_fbi_i64 + 6);                 \
    *((b_i8*)buffer + 5) = *((b_i8*)&__temp_fbi_i64 + 5);                 \
    *((b_i8*)buffer + 4) = *((b_i8*)&__temp_fbi_i64 + 4);                 \
    *((b_i8*)buffer + 3) = *((b_i8*)&__temp_fbi_i64 + 3);                 \
    *((b_i8*)buffer + 2) = *((b_i8*)&__temp_fbi_i64 + 2);                 \
    *((b_i8*)buffer + 1) = *((b_i8*)&__temp_fbi_i64 + 1);                 \
    *((b_i8*)buffer) = *((b_i8*)&__temp_fbi_i64);                         \
  } while(0)

#define FILL_LE_BUFFER_I32(buffer, data)                                  \
  do {                                                                    \
    *((b_i8*)buffer + 3) = ((b_i32)data) >> 24;                           \
    *((b_i8*)buffer + 2) = (((b_i32)data) >> 16) & 0xff;                  \
    *((b_i8*)buffer + 1) = (((b_i32)data) >> 8) & 0xff;                   \
    *((b_i8*)buffer) = ((b_i32)data) & 0xff;                              \
  } while(0)

#define FILL_LE_BUFFER_I16(buffer, data)                                  \
  do {                                                                    \
    *((b_i8*)buffer + 1) = ((b_i16)data) >> 8;                            \
    *((b_i8*)buffer) = ((b_i16)data) & 0xff;                              \
  } while(0)

#define GET_BUFFER_DOUBLE(buffer, data)                                   \
  do {                                                                    \
    double __temp_gbd_double;                                             \
    *((b_i8*)&__temp_gbd_double) = *((b_i8*)buffer + 7);                  \
    *((b_i8*)&__temp_gbd_double + 1) = *((b_i8*)buffer + 6);              \
    *((b_i8*)&__temp_gbd_double + 2) = *((b_i8*)buffer + 5);              \
    *((b_i8*)&__temp_gbd_double + 3) = *((b_i8*)buffer + 4);              \
    *((b_i8*)&__temp_gbd_double + 4) = *((b_i8*)buffer + 3);              \
    *((b_i8*)&__temp_gbd_double + 5) = *((b_i8*)buffer + 2);              \
    *((b_i8*)&__temp_gbd_double + 6) = *((b_i8*)buffer + 1);              \
    *((b_i8*)&__temp_gbd_double + 7) = *((b_i8*)buffer);                  \
    data = __temp_gbd_double;                                             \
  } while(0)

#define GET_BUFFER_FLOAT(buffer, data)                                    \
  do {                                                                    \
    b_i32 __temp_gbf_i32;                                                 \
    __temp_gbf_i32 = (*((b_i8*)buffer) << 24)                  |          \
                     ((*((b_i8*)buffer + 1) << 16) & 0xff0000) |          \
                     ((*((b_i8*)buffer + 2) << 8) & 0xff00)    |          \
                     (*((b_i8*)buffer + 3) & 0xff);                       \
    data = *((float*)&__temp_gbf_i32);                                    \
  } while(0)

#define GET_BUFFER_I64(buffer, data)                                      \
  do {                                                                    \
    b_i64 __temp_gbi_i64;                                                 \
    *((b_i8*)&__temp_gbi_i64) = *((b_i8*)buffer + 7);                     \
    *((b_i8*)&__temp_gbi_i64 + 1) = *((b_i8*)buffer + 6);                 \
    *((b_i8*)&__temp_gbi_i64 + 2) = *((b_i8*)buffer + 5);                 \
    *((b_i8*)&__temp_gbi_i64 + 3) = *((b_i8*)buffer + 4);                 \
    *((b_i8*)&__temp_gbi_i64 + 4) = *((b_i8*)buffer + 3);                 \
    *((b_i8*)&__temp_gbi_i64 + 5) = *((b_i8*)buffer + 2);                 \
    *((b_i8*)&__temp_gbi_i64 + 6) = *((b_i8*)buffer + 1);                 \
    *((b_i8*)&__temp_gbi_i64 + 7) = *((b_i8*)buffer);                     \
    data = __temp_gbi_i64;                                                \
  } while(0)

#define GET_BUFFER_I32(buffer, data)                                      \
  do {                                                                    \
    data = (b_i32)((*((b_i8*)buffer) << 24)                  |            \
                   ((*((b_i8*)buffer + 1) << 16) & 0xff0000) |            \
                   ((*((b_i8*)buffer + 2) << 8) & 0xff00)    |            \
                   (*((b_i8*)buffer + 3) & 0xff));                        \
  } while(0)

#define GET_BUFFER_I16(buffer, data)                                      \
  do {                                                                    \
    data = (b_i16)(((*((b_i8*)buffer) << 8))       |                      \
                   (*((b_i8*)buffer + 1) & 0xff));                        \
  } while(0)

#define GET_LE_BUFFER_DOUBLE(buffer, data)                                \
  do {                                                                    \
    double __temp_gbd_double;                                             \
    *((b_i8*)&__temp_gbd_double) = *((b_i8*)buffer);                      \
    *((b_i8*)&__temp_gbd_double + 1) = *((b_i8*)buffer + 1);              \
    *((b_i8*)&__temp_gbd_double + 2) = *((b_i8*)buffer + 2);              \
    *((b_i8*)&__temp_gbd_double + 3) = *((b_i8*)buffer + 3);              \
    *((b_i8*)&__temp_gbd_double + 4) = *((b_i8*)buffer + 4);              \
    *((b_i8*)&__temp_gbd_double + 5) = *((b_i8*)buffer + 5);              \
    *((b_i8*)&__temp_gbd_double + 6) = *((b_i8*)buffer + 6);              \
    *((b_i8*)&__temp_gbd_double + 7) = *((b_i8*)buffer + 7);              \
    data = __temp_gbd_double;                                             \
  } while(0)

#define GET_LE_BUFFER_FLOAT(buffer, data)                                 \
  do {                                                                    \
    b_i32 __temp_gbf_i32;                                                 \
    __temp_gbf_i32 = (*((b_i8*)buffer + 3) << 24)              |          \
                     ((*((b_i8*)buffer + 2) << 16) & 0xff0000) |          \
                     ((*((b_i8*)buffer + 1) << 8) & 0xff00)    |          \
                     (*((b_i8*)buffer) & 0xff);                           \
    data = *((float*)&__temp_gbf_i32);                                    \
  } while(0)

#define GET_LE_BUFFER_I64(buffer, data)                                   \
  do {                                                                    \
    b_i64 __temp_gbi_i64;                                                 \
    *((b_i8*)&__temp_gbi_i64) = *((b_i8*)buffer);                         \
    *((b_i8*)&__temp_gbi_i64 + 1) = *((b_i8*)buffer + 1);                 \
    *((b_i8*)&__temp_gbi_i64 + 2) = *((b_i8*)buffer + 2);                 \
    *((b_i8*)&__temp_gbi_i64 + 3) = *((b_i8*)buffer + 3);                 \
    *((b_i8*)&__temp_gbi_i64 + 4) = *((b_i8*)buffer + 4);                 \
    *((b_i8*)&__temp_gbi_i64 + 5) = *((b_i8*)buffer + 5);                 \
    *((b_i8*)&__temp_gbi_i64 + 6) = *((b_i8*)buffer + 6);                 \
    *((b_i8*)&__temp_gbi_i64 + 7) = *((b_i8*)buffer + 7);                 \
    data = __temp_gbi_i64;                                                \
  } while(0)

#define GET_LE_BUFFER_I32(buffer, data)                                   \
  do {                                                                    \
    data = (b_i32)((*((b_i8*)buffer + 3) << 24)              |            \
                   ((*((b_i8*)buffer + 2) << 16) & 0xff0000) |            \
                   ((*((b_i8*)buffer + 1) << 8) & 0xff00)    |            \
                   (*((b_i8*)buffer) & 0xff);                             \
  } while(0)

#define GET_LE_BUFFER_I16(buffer, data)                                   \
  do {                                                                    \
    data = (b_i16)(((*((b_i8*)buffer + 1) << 8)) |                        \
                   (*((b_i8*)buffer) & 0xff));                            \
  } while(0)
#else
#define FILL_BUFFER_DOUBLE(buffer, data)                                  \
  do {                                                                    \
    double temp_fbd_double = data;                                        \
    *((b_i8*)buffer + 7) = *((b_i8*)&temp_fbd_double + 7);                \
    *((b_i8*)buffer + 6) = *((b_i8*)&temp_fbd_double + 6);                \
    *((b_i8*)buffer + 5) = *((b_i8*)&temp_fbd_double + 5);                \
    *((b_i8*)buffer + 4) = *((b_i8*)&temp_fbd_double + 4);                \
    *((b_i8*)buffer + 3) = *((b_i8*)&temp_fbd_double + 3);                \
    *((b_i8*)buffer + 2) = *((b_i8*)&temp_fbd_double + 2);                \
    *((b_i8*)buffer + 1) = *((b_i8*)&temp_fbd_double + 1);                \
    *((b_i8*)buffer) = *((b_i8*)&temp_fbd_double);                        \
  } while(0)

#define FILL_BUFFER_FLOAT(buffer, data)                                   \
  do {                                                                    \
    float temp_fbf_float = data;                                          \
    *((b_i8*)buffer + 3) = *((b_i32*)&temp_fbf_float) & 0xff;             \
    *((b_i8*)buffer + 2) = (*((b_i32*)&temp_fbf_float) << 8) & 0xff;      \
    *((b_i8*)buffer + 1) = (*((b_i32*)&temp_fbf_float) << 16) & 0xff;     \
    *((b_i8*)buffer) = *((b_i32*)&temp_fbf_float) << 24;                  \
  } while(0)

#define FILL_BUFFER_I64(buffer, data)                                     \
  do {                                                                    \
    b_i64 temp_fbi_i64 = data;                                            \
    *((b_i8*)buffer + 7) = *((b_i8*)&temp_fbi_i64 + 7);                   \
    *((b_i8*)buffer + 6) = *((b_i8*)&temp_fbi_i64 + 6);                   \
    *((b_i8*)buffer + 5) = *((b_i8*)&temp_fbi_i64 + 5);                   \
    *((b_i8*)buffer + 4) = *((b_i8*)&temp_fbi_i64 + 4);                   \
    *((b_i8*)buffer + 3) = *((b_i8*)&temp_fbi_i64 + 3);                   \
    *((b_i8*)buffer + 2) = *((b_i8*)&temp_fbi_i64 + 2);                   \
    *((b_i8*)buffer + 1) = *((b_i8*)&temp_fbi_i64 + 1);                   \
    *((b_i8*)buffer) = *((b_i8*)&temp_fbi_i64);                           \
  } while(0)

#define FILL_BUFFER_I32(buffer, data)                                     \
  do {                                                                    \
    *((b_i8*)buffer + 3) = ((b_i32)data) & 0xff;                          \
    *((b_i8*)buffer + 2) = (((b_i32)data) << 8) & 0xff;                   \
    *((b_i8*)buffer + 1) = (((b_i32)data) << 16) & 0xff;                  \
    *((b_i8*)buffer) = (((b_i32)data) << 24);                             \
  } while(0)

#define FILL_BUFFER_I16(buffer, data)                                     \
  do {                                                                    \
    *((b_i8*)buffer + 1) = ((b_i16)data) & 0xff;                          \
    *((b_i8*)buffer) = ((b_i16)data) << 8;                                \
  } while(0)

#define FILL_LE_BUFFER_DOUBLE(buffer, data)                               \
  do {                                                                    \
    double temp_fbd_double = data;                                        \
    *((b_i8*)buffer) = *((b_i8*)&temp_fbd_double + 7);                    \
    *((b_i8*)buffer + 1) = *((b_i8*)&temp_fbd_double + 6);                \
    *((b_i8*)buffer + 2) = *((b_i8*)&temp_fbd_double + 5);                \
    *((b_i8*)buffer + 3) = *((b_i8*)&temp_fbd_double + 4);                \
    *((b_i8*)buffer + 4) = *((b_i8*)&temp_fbd_double + 3);                \
    *((b_i8*)buffer + 5) = *((b_i8*)&temp_fbd_double + 2);                \
    *((b_i8*)buffer + 6) = *((b_i8*)&temp_fbd_double + 1);                \
    *((b_i8*)buffer + 7) = *((b_i8*)&temp_fbd_double);                    \
  } while(0)

#define FILL_LE_BUFFER_FLOAT(buffer, data)                                \
  do {                                                                    \
    float temp_fbf_float = data;                                          \
    *((b_i8*)buffer) = *((b_i32*)&temp_fbf_float) & 0xff;                 \
    *((b_i8*)buffer + 1) = (*((b_i32*)&temp_fbf_float) << 8) & 0xff;      \
    *((b_i8*)buffer + 2) = (*((b_i32*)&temp_fbf_float) << 16) & 0xff;     \
    *((b_i8*)buffer + 3) = *((b_i32*)&temp_fbf_float) << 24;              \
  } while(0)

#define FILL_LE_BUFFER_I64(buffer, data)                                  \
  do {                                                                    \
    b_i64 temp_fbi_i64 = data;                                            \
    *((b_i8*)buffer) = *((b_i8*)&temp_fbi_i64 + 7);                       \
    *((b_i8*)buffer + 1) = *((b_i8*)&temp_fbi_i64 + 6);                   \
    *((b_i8*)buffer + 2) = *((b_i8*)&temp_fbi_i64 + 5);                   \
    *((b_i8*)buffer + 3) = *((b_i8*)&temp_fbi_i64 + 4);                   \
    *((b_i8*)buffer + 4) = *((b_i8*)&temp_fbi_i64 + 3);                   \
    *((b_i8*)buffer + 5) = *((b_i8*)&temp_fbi_i64 + 2);                   \
    *((b_i8*)buffer + 6) = *((b_i8*)&temp_fbi_i64 + 1);                   \
    *((b_i8*)buffer + 7) = *((b_i8*)&temp_fbi_i64);                       \
  } while(0)

#define FILL_LE_BUFFER_I32(buffer, data)                                  \
  do {                                                                    \
    *((b_i8*)buffer) = ((b_i32)data) & 0xff;                              \
    *((b_i8*)buffer + 1) = (((b_i32)data) << 8) & 0xff;                   \
    *((b_i8*)buffer + 2) = (((b_i32)data) << 16) & 0xff;                  \
    *((b_i8*)buffer + 3) = ((b_i32)data) << 24;                           \
  } while(0)

#define FILL_LE_BUFFER_I16(buffer, data)                                  \
  do {                                                                    \
    *((b_i8*)buffer) = ((b_i16)data) & 0xff;                              \
    *((b_i8*)buffer + 1) = ((b_i16)data) << 8;                            \
  } while(0)

#define GET_BUFFER_DOUBLE(buffer, data)                                   \
  do {                                                                    \
    double __temp_gbd_double;                                             \
    *((b_i8*)&__temp_gbd_double) = *((b_i8*)buffer);                      \
    *((b_i8*)&__temp_gbd_double + 1) = *((b_i8*)buffer + 1);              \
    *((b_i8*)&__temp_gbd_double + 2) = *((b_i8*)buffer + 2);              \
    *((b_i8*)&__temp_gbd_double + 3) = *((b_i8*)buffer + 3);              \
    *((b_i8*)&__temp_gbd_double + 4) = *((b_i8*)buffer + 4);              \
    *((b_i8*)&__temp_gbd_double + 5) = *((b_i8*)buffer + 5);              \
    *((b_i8*)&__temp_gbd_double + 6) = *((b_i8*)buffer + 6);              \
    *((b_i8*)&__temp_gbd_double + 7) = *((b_i8*)buffer + 7);              \
    data = __temp_gbd_double;                                             \
  } while(0)

#define GET_BUFFER_FLOAT(buffer, data)                                    \
  do {                                                                    \
    b_i32 __temp_gbf_i32;                                                 \
    __temp_gbf_i32 = (*((b_i8*)buffer) >> 24)                  |          \
                     ((*((b_i8*)buffer + 1) >> 16) & 0xff0000) |          \
                     ((*((b_i8*)buffer + 2) >> 8) & 0xff00)    |          \
                     (*((b_i8*)buffer + 3) & 0xff);                       \
    data = *((float*)&__temp_gbf_i32);                                    \
  } while(0)

#define GET_BUFFER_I64(buffer, data)                                      \
  do {                                                                    \
    b_i64 __temp_gbi_i64;                                                 \
    *((b_i8*)&__temp_gbi_i64) = *((b_i8*)buffer + 7);                     \
    *((b_i8*)&__temp_gbi_i64 + 1) = *((b_i8*)buffer + 6);                 \
    *((b_i8*)&__temp_gbi_i64 + 2) = *((b_i8*)buffer + 5);                 \
    *((b_i8*)&__temp_gbi_i64 + 3) = *((b_i8*)buffer + 4);                 \
    *((b_i8*)&__temp_gbi_i64 + 4) = *((b_i8*)buffer + 3);                 \
    *((b_i8*)&__temp_gbi_i64 + 5) = *((b_i8*)buffer + 2);                 \
    *((b_i8*)&__temp_gbi_i64 + 6) = *((b_i8*)buffer + 1);                 \
    *((b_i8*)&__temp_gbi_i64 + 7) = *((b_i8*)buffer);                     \
    data = __temp_gbi_i64;                                                \
  } while(0)

#define GET_BUFFER_I32(buffer, data)                                      \
  do {                                                                    \
    data = (b_i32)((*((b_i8*)buffer) >> 24)                  |            \
                   ((*((b_i8*)buffer + 1) >> 16) & 0xff0000) |            \
                   ((*((b_i8*)buffer + 2) >> 8) & 0xff00)    |            \
                   (*((b_i8*)buffer + 3) & 0xff));                        \
  } while(0)

#define GET_BUFFER_I16(buffer, data)                                      \
  do {                                                                    \
    data = (b_i16)(((*((b_i8*)buffer) >> 8))      |                       \
                   (*((b_i8*)buffer + 1) & 0xff));                        \
  } while(0)

#define GET_LE_BUFFER_DOUBLE(buffer, data)                                \
  do {                                                                    \
    double __temp_gbd_double;                                             \
    *((b_i8*)&__temp_gbd_double) = *((b_i8*)buffer + 7);                  \
    *((b_i8*)&__temp_gbd_double + 1) = *((b_i8*)buffer + 6);              \
    *((b_i8*)&__temp_gbd_double + 2) = *((b_i8*)buffer + 5);              \
    *((b_i8*)&__temp_gbd_double + 3) = *((b_i8*)buffer + 4);              \
    *((b_i8*)&__temp_gbd_double + 4) = *((b_i8*)buffer + 3);              \
    *((b_i8*)&__temp_gbd_double + 5) = *((b_i8*)buffer + 2);              \
    *((b_i8*)&__temp_gbd_double + 6) = *((b_i8*)buffer + 1);              \
    *((b_i8*)&__temp_gbd_double + 7) = *((b_i8*)buffer);                  \
    data = __temp_gbd_double;                                             \
  } while(0)

#define GET_LE_BUFFER_FLOAT(buffer, data)                                 \
  do {                                                                    \
    b_i32 __temp_gbf_i32;                                                 \
    __temp_gbf_i32 = (*((b_i8*)buffer + 3) >> 24)              |          \
                     ((*((b_i8*)buffer + 2) >> 16) & 0xff0000) |          \
                     ((*((b_i8*)buffer + 1) >> 8) & 0xff00)    |          \
                     (*((b_i8*)buffer) & 0xff);                           \
    data = *((float*)&__temp_gbf_i32);                                    \
  } while(0)

#define GET_LE_BUFFER_I64(buffer, data)                                   \
  do {                                                                    \
    b_i64 __temp_gbi_i64;                                                 \
    *((b_i8*)&__temp_gbi_i64) = *((b_i8*)buffer + 7);                     \
    *((b_i8*)&__temp_gbi_i64 + 1) = *((b_i8*)buffer + 6);                 \
    *((b_i8*)&__temp_gbi_i64 + 2) = *((b_i8*)buffer + 5);                 \
    *((b_i8*)&__temp_gbi_i64 + 3) = *((b_i8*)buffer + 4);                 \
    *((b_i8*)&__temp_gbi_i64 + 4) = *((b_i8*)buffer + 3);                 \
    *((b_i8*)&__temp_gbi_i64 + 5) = *((b_i8*)buffer + 2);                 \
    *((b_i8*)&__temp_gbi_i64 + 6) = *((b_i8*)buffer + 1);                 \
    *((b_i8*)&__temp_gbi_i64 + 7) = *((b_i8*)buffer);                     \
    data = __temp_gbi_i64;                                                \
  } while(0)

#define GET_LE_BUFFER_I32(buffer, data)                                   \
  do {                                                                    \
    data = (b_i32)((*((b_i8*)buffer + 3) >> 24)              |            \
                   ((*((b_i8*)buffer + 2) >> 16) & 0xff0000) |            \
                   ((*((b_i8*)buffer + 1) >> 8) & 0xff00)    |            \
                   (*((b_i8*)buffer) & 0xff));                            \
  } while(0)

#define GET_LE_BUFFER_I16(buffer, data)                                   \
  do {                                                                    \
    data = (b_i16)(((*((b_i8*)buffer + 1) >> 8)) |                        \
                   (*((b_i8*)buffer) & 0xff));                            \
  } while(0)
#endif

extern b_i32 atoi32(char* str);
extern b_i64 atoi64(char* str);
extern void bsleep(b_i32 mills);
extern pointer_type bheap_alloc(b_i32 msize);
extern void bheap_free(pointer_type mem);
extern pointer_type bheap_realloc(pointer_type old_mem, b_i32 new_msize);

#endif
