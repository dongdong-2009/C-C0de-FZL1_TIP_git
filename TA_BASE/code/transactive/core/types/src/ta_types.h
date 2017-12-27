/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/types/src/ta_types.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Contains definitions of platform independent datatypes
  * that should be used in all TransActive softwares to 
  * ensure code could be used cross platform.
  * 
  */

#ifndef _TA_TYPES_h
#define _TA_TYPES_h

/*******************************************

  Platform independent type definitions.

  Signed integers are named:
	ta_int<number of bits>
  where number of bits can be 8, 16, 32 or 64

  Unsigned integers are named:
	ta_uint<number of bits>
  where number of bits can be 8, 16, 32 or 64
	
  Floats:
	ta_float<number of bits>
  where number of bits can be 32 or 64
  extended float should not be used.


  Note:
  - have to include __WORDSIZE=64 in the preprocessor in project settings or 
    #define if 64 bits integers are supported.
  - if the compiler complains about 'ta_int64' : undeclared identifier or 
    something along the same line, __WORDSIZE=64 has not been set.
  - if the compiler complains about integer constant out of range for ta_int64
    then the OS does not support 64 bits integers
  - this header file currently only supports linux, solaris and windows OSs

*******************************************/


// Definition of data types supported by linux
//==========================================================
#if defined( LINUX ) || defined( __linux__ )
	typedef signed char          ta_int8;   // 8 bits long signed integer
	typedef signed short         ta_int16;  // 16 bits long signed integer
	typedef signed long          ta_int32;  // 32 bits long signed integer

	typedef unsigned char        ta_uint8;   // 8 bits long unsigned integer, "byte"
	typedef unsigned short       ta_uint16;  // 16 bits long unsigned integer, "word"
	typedef unsigned long        ta_uint32;  // 32 bits long unsigned integer

	// defining the 64 bits integer.  If the system does not support 64 bits
	// integer then output the compiler error
	#if ( __WORDSIZE == 64 )
		typedef signed long long     ta_int64;  // 64 bits long signed integer
		typedef unsigned long long   ta_uint64;  // 64 bits long unsigned integer
		static const ta_int64 TA_MAX_INT64     =  0x7fffffffffffffff;
		static const ta_int64 TA_MIN_INT64     =  -0x7fffffffffffffff-1;
		static const ta_uint64 TA_MAX_UINT64 = 0xffffffffffffffff;
		static const ta_uint64 TA_MIN_UINT64 = 0;
	#endif
	
	typedef float                ta_float32;  // single, 4 bytes long float
	typedef double               ta_float64;  // double, 8 bytes long float
//	typedef long double          ta_float80; // extended - 12 bytes on Solaris



// Definition of data types supported by Windows
//==========================================================
#elif defined( WIN32 ) || defined( __WIN32__ )
	typedef __int8               ta_int8;   // 8 bits long signed integer
	typedef __int16              ta_int16;  // 16 bits long signed integer
	typedef __int32              ta_int32;  // 32 bits long signed integer

	typedef unsigned __int8      ta_uint8;   // 8 bits long unsigned integer, "byte"
	typedef unsigned __int16     ta_uint16;  // 16 bits long unsigned integer, "word"
	typedef unsigned __int32     ta_uint32;  // 32 bits long unsigned integer

	// defining the 64 bits integer.  If the system does not support 64 bits
	// integer then output the compiler error
	#if defined( _WIN64 ) || ( __WORDSIZE == 64 )
		typedef __int64              ta_int64;  // 64 bits long signed integer
		typedef unsigned __int64     ta_uint64;  // 64 bits long unsigned integer
		static const ta_int64 TA_MAX_INT64     =  0x7fffffffffffffff;
		static const ta_int64 TA_MIN_INT64     =  -0x7fffffffffffffff-1;
		static const ta_uint64 TA_MAX_UINT64 = 0xffffffffffffffffUI64;
		static const ta_uint64 TA_MIN_UINT64 = 0I64;
	#endif

	typedef float                ta_float32;  // single, 4 bytes long float
	typedef double               ta_float64;  // double, 8 bytes long float
//	typedef long double          ta_float80; // extended - 10 bytes on Solaris


// Definition of data types supported by Solaris
//==========================================================
#elif defined( SOLARIS ) || defined( __solaris__ )
	typedef signed char          ta_int8;   // 8 bits long signed integer
	typedef signed short         ta_int16;  // 16 bits long signed integer
	typedef signed long          ta_int32;  // 32 bits long signed integer

	typedef unsigned char        ta_uint8;   // 8 bits long unsigned integer, "byte"
	typedef unsigned short       ta_uint16;  // 16 bits long unsigned integer, "word"
	typedef unsigned long        ta_uint32;  // 32 bits long unsigned integer

	// defining the 64 bits integer.  If the system does not support 64 bits
	// integer then output the compiler error
	#if ( __WORDSIZE == 64 )
		typedef signed long long     ta_int64;  // 64 bits long signed integer
		typedef unsigned long long   ta_uint64;  // 64 bits long unsigned integer
		static const ta_int64 TA_MAX_INT64     =  0x7fffffffffffffff;
		static const ta_int64 TA_MIN_INT64     =  -0x7fffffffffffffff-1;
		static const ta_uint64 TA_MAX_UINT64 = 0xffffffffffffffff;
		static const ta_uint64 TA_MIN_UINT64 = 0;
	#endif

	typedef float                ta_float32;  // single, 4 bytes long float
	typedef double               ta_float64;  // double, 8 bytes long float
//	typedef long double          ta_float80; // extended - 16 bytes on Solaris


#else
#error ta_types is not defined for this platform
#endif


	
// The maximum and minimum values of the datatypes defined
static const ta_int8  TA_MAX_INT8      =  0x7f;
static const ta_int8  TA_MIN_INT8      = -0x7f-1;
static const ta_int16 TA_MAX_INT16     =  0x7fff;
static const ta_int16 TA_MIN_INT16     = -0x7fff-1;
static const ta_int32 TA_MAX_INT32     =  0x7fffffffL;
static const ta_int32 TA_MIN_INT32     = -0x7fffffffL-1;

static const ta_uint8  TA_MAX_UINT8  = 0xff;
static const ta_uint8  TA_MIN_UINT8  = 0;
static const ta_uint16 TA_MAX_UINT16 = 0xffff;
static const ta_uint16 TA_MIN_UINT16 = 0;
static const ta_uint32 TA_MAX_UINT32 = 0xffffffffL;
static const ta_uint32 TA_MIN_UINT32 = 0L;


#endif //_TA_TYPES_h

