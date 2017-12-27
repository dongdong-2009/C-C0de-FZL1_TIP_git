/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/DebugSETranslator.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifndef DEBUG_SETRANSLATOR_H
#define DEBUG_SETRANSLATOR_H

#ifdef WIN32
    #include <windows.h>
#else
    // Some typedefs that are defined (differently) in windows.h that allow Solaris builds
    // to contain catch() blocks with the WinXXXException structs below.
    typedef void EXCEPTION_POINTERS;
    typedef int DWORD;
#endif

#include "core/utilities/src/DebugUtil.h"
namespace TA_Base_Core
{
    // The Windows platform passes hardware exceptions to the application as structured
    // exceptions (eg. Access Violation, Divide by Zero).  Normally, the only way to
    // catch structured exceptions is using catch(...), but we then have no information
    // about the type of exception that occurred.  
    // Windows provides a function, _set_se_translator(), that allows structured exceptions
    // to be translated to C++ typed exceptions.

    // Solaris: NOT IMPLEMENTED (Solaris does not pass hardware exceptions to the 
    // application - execution is aborted with a core dump).

    // From MSDN description of EXCEPTION_RECORD struct:
    // EXCEPTION_ACCESS_VIOLATION
    // The thread tried to read from or write to a virtual address for which it does not have the appropriate access. 
    // EXCEPTION_ARRAY_BOUNDS_EXCEEDED 
    // The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking. 
    // EXCEPTION_BREAKPOINT 
    // A breakpoint was encountered. 
    // EXCEPTION_DATATYPE_MISALIGNMENT
    // The thread tried to read or write data that is misaligned on hardware that does not provide alignment. 
    // For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on. 
    // EXCEPTION_FLT_DENORMAL_OPERAND 
    // One of the operands in a floating-point operation is denormal. A denormal value is one that is too small 
    // to represent as a standard floating-point value. 
    // EXCEPTION_FLT_DIVIDE_BY_ZERO 
    // The thread tried to divide a floating-point value by a floating-point divisor of zero. 
    // EXCEPTION_FLT_INEXACT_RESULT 
    // The result of a floating-point operation cannot be represented exactly as a decimal fraction. 
    // EXCEPTION_FLT_INVALID_OPERATION 
    // This exception represents any floating-point exception not included in this list. 
    // EXCEPTION_FLT_OVERFLOW 
    // The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type. 
    // EXCEPTION_FLT_STACK_CHECK 
    // The stack overflowed or underflowed as the result of a floating-point operation. 
    // EXCEPTION_FLT_UNDERFLOW 
    // The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type. 
    // EXCEPTION_ILLEGAL_INSTRUCTION 
    // The thread tried to execute an invalid instruction. 
    // EXCEPTION_IN_PAGE_ERROR 
    // The thread tried to access a page that was not present, and the system was unable to load the page. 
    // For example, this exception might occur if a network connection is lost while running a program over the network. 
    // EXCEPTION_INT_DIVIDE_BY_ZERO 
    // The thread tried to divide an integer value by an integer divisor of zero. 
    // EXCEPTION_INT_OVERFLOW 
    // The result of an integer operation caused a carry out of the most significant bit of the result. 
    // EXCEPTION_INVALID_DISPOSITION 
    // An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a 
    // high-level language such as C should never encounter this exception. 
    // EXCEPTION_NONCONTINUABLE_EXCEPTION 
    // The thread tried to continue execution after a noncontinuable exception occurred. 
    // EXCEPTION_PRIV_INSTRUCTION 
    // The thread tried to execute an instruction whose operation is not allowed in the current machine mode. 
    // EXCEPTION_SINGLE_STEP 
    // A trace trap or other single-instruction mechanism signaled that one instruction has been executed. 
    // EXCEPTION_STACK_OVERFLOW 
    // The thread used up its stack. 

    // Initialises the structured exception to C++ exception translation handler.
    // If this function isn't called, structured exceptions will only be caught by 
    // catch(...) blocks.
    // RPARAM_DEBUGLOGSTACKTRACE must be set to enable stack tracing.  
    // HAS TO BE CALLED FROM EACH THREAD.
    // Solaris: NOT IMPLEMENTED
    void gInitStructuredExceptionHandler();

    struct WinStructuredException
    {
        WinStructuredException( DWORD exceptionCode, EXCEPTION_POINTERS* exceptionInfo ) : 
            m_exceptionCode(exceptionCode),
            m_exceptionInfo(exceptionInfo) 
        {
        }

        DWORD m_exceptionCode;
        EXCEPTION_POINTERS* m_exceptionInfo;
    };

    struct WinAccessViolationException : public WinStructuredException
    {
        WinAccessViolationException(DWORD exceptionCode, EXCEPTION_POINTERS* exceptionInfo) :
            WinStructuredException(exceptionCode, exceptionInfo) 
        {
        }
    };

    struct WinFloatDivideByZeroException : public WinStructuredException
    {
        WinFloatDivideByZeroException(DWORD exceptionCode, EXCEPTION_POINTERS* exceptionInfo) :
            WinStructuredException(exceptionCode, exceptionInfo) 
        {
        }
    };

    struct WinBreakpointException : public WinStructuredException
    {
        WinBreakpointException(DWORD exceptionCode, EXCEPTION_POINTERS* exceptionInfo) :
            WinStructuredException(exceptionCode, exceptionInfo) 
        {
        }
    };

    // Platform independent wrapper to hide access of CONTEXT member
    CONTEXT* gContextFromExceptionInfo(EXCEPTION_POINTERS* exceptionInfo);


    // TODO LPP: How can we do a stack trace of an exception throw site in a
    // catch(std::exception&) or catch(...) block?

}   // namespace TA_Base_Core

#endif // DEBUG_SETRANSLATOR_H
