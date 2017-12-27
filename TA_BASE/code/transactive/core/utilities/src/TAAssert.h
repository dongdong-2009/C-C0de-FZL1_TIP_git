/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/TAAssert.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#ifndef TAAssert_h
#define TAAssert_h
#include <string>

// The action to be performed when an assertion fails can be set at compile time.
// If TA_ASSERT_BREAK is BREAK_YES, a breakpoint will be executed.  Then execution can
// optionally be aborted or an exception thrown by setting TA_ASSERT_FAIL.

// TA_ASSERT_BREAK must be set to one of the following:
#define BREAK_NO 0              // Do nothing
#define BREAK_YES 1             // Execute a breakpoint
#define BREAK_RUNTIME -1        // Determine whether to execute a breakpoint at runtime (default == BREAK_YES)

// TA_ASSERT_FAIL must be set to one of the following:
#define FAIL_NONE 0			    // Do nothing
#define FAIL_ABORT 1			// Abort
#define FAIL_EXCEPTION 2		// Throw AssertException
#define FAIL_RUNTIME -1         // Determine the action at runtime (default == FAIL_ABORT)

// TA_ASSERT_LOG must be set to one of the following:
#define LOG_NO 0                // Do nothing
#define LOG_YES 1               // Log the assertion


namespace TA_Base_Core
{
    // Sets the breakpoint action that will be executed when an assertion occurs.
    // TA_ASSERT_BREAK must be BREAK_RUNTIME for this call to have any effect.
    // 'breakAction' must be one of (BREAK_NO, BREAK_YES).
    void gSetRuntimeBreakAction(int breakAction);
    int gGetRuntimeBreakAction();

    // Sets the assertion action that will be executed when an assertion occurs.
    // TA_ASSERT_FAIL must be FAIL_RUNTIME for this call to have any effect.
    // 'action' must be one of (FAIL_NONE, FAIL_ABORT, FAIL_EXCEPTION).
    void gSetRuntimeAssertAction(int action);
    int gGetRuntimeAssertAction();

    // To avoid code bloat, we put some of the assertion handling in non-inlined functions.
    // NOT to be called directly by clients.
    void gDoAssertionAction(int action, const char* file, int line, const char* test, const char* info);
    void gDoAssertionAction(int action, const char* file, int line, const char* test, const std::string& info);
    void gLogAssertionMsg(const char* file, int line, const char* test, const char* info);
    void gLogAssertionMsg(const char* file, int line, const char* test, const std::string& info);
}


#ifdef WIN32
// TODO LPP: BREAKPOINT will stop the program, but it won't allow the debugger to
// be attached to the process if the macro occurs inside a try block with a
// catch(...) clause. This is very undesirable.  Is there any workaround?

// When the process is executing in the debugger, the int 3 approach seems to work
// better than divide-by-zero (which is necessary for Solaris) for catch(...) cases.
// This might be because the debugger is configured by default to always stop when
#define BREAKPOINT \
    { \
        _asm int 3 \
    }
#else
#define BREAKPOINT \
    { \
        int a = 0; \
        a = 1 / a; /* abort doesn't work, divide by zero will */    \
    }
#endif // #ifdef WIN32


// Define the default break action, if none has been specified.
#ifndef TA_ASSERT_BREAK
    #if NDEBUG
        #define TA_ASSERT_BREAK     BREAK_NO       // Usually Release build
    #else
        //		#define TA_ASSERT_BREAK     BREAK_YES      // Usually Debug build
        #define TA_ASSERT_BREAK     BREAK_RUNTIME  // Unit tests (and libraries) usually use BREAK_NO
    #endif // NDEBUG
#endif

#if TA_ASSERT_BREAK == BREAK_NO
    #define TA_DOBREAKACTION
#elif TA_ASSERT_BREAK == BREAK_YES
    #define TA_DOBREAKACTION            BREAKPOINT
#elif TA_ASSERT_BREAK == BREAK_RUNTIME
    #define TA_DOBREAKACTION            if (TA_Base_Core::gGetRuntimeBreakAction() == BREAK_YES) BREAKPOINT
#else
    // TODO LPP: Need to expand the symbol TA_ASSERT_BREAK in this error message.
    #error "TA_ASSERT_BREAK option is not supported"
#endif

// Define the default assertion action, if none has been specified.
#ifndef TA_ASSERT_FAIL
    #if NDEBUG
        #define TA_ASSERT_FAIL      FAIL_ABORT     // Usually Release build
    #else
        //		#define TA_ASSERT_FAIL      FAIL_ABORT	   // Ensures assertions don't go unnoticed.
        #define TA_ASSERT_FAIL      FAIL_RUNTIME   // Unit tests (and libraries) usually use FAIL_EXCEPTION
    #endif // NDEBUG
#endif

#if TA_ASSERT_FAIL == FAIL_NONE
#define TA_DOASSERTIONACTION(file, line, test, info)
#elif TA_ASSERT_FAIL == FAIL_ABORT
#define TA_DOASSERTIONACTION(file, line, test, info) TA_Base_Core::gDoAssertionAction(FAIL_ABORT, __FILE__, __LINE__, #test, info)
#elif TA_ASSERT_FAIL == FAIL_EXCEPTION
#define TA_DOASSERTIONACTION(file, line, test, info) TA_Base_Core::gDoAssertionAction(FAIL_EXCEPTION, __FILE__, __LINE__, #test, info)
#elif TA_ASSERT_FAIL == FAIL_RUNTIME
#define TA_DOASSERTIONACTION(file, line, test, info)                                                        \
    {                                                                                                       \
        int action = TA_Base_Core::gGetRuntimeAssertAction();                                               \
        if (action != FAIL_NONE) TA_Base_Core::gDoAssertionAction(action, __FILE__, __LINE__, #test, info); \
    }
#else
// TODO LPP: Need to expand the symbol TA_ASSERT_FAIL in this error message.
#error "TA_ASSERT_FAIL option is not supported"
#endif

// Define the default assertion logging action
#ifndef TA_ASSERT_LOG
    #define TA_ASSERT_LOG LOG_YES
#endif

#if TA_ASSERT_LOG == LOG_NO
    #define TA_LOGASSERTIONMSG(file, line, test, info)
#elif TA_ASSERT_LOG == LOG_YES
    #define TA_LOGASSERTIONMSG(file, line, test, info) TA_Base_Core::gLogAssertionMsg(__FILE__, __LINE__, #test, info)
#else
    // TODO LPP: Need to expand the symbol TA_ASSERT_LOG in this error message.
    #error "TA_ASSERT_LOG option is not supported"
#endif


#ifndef NDEBUG
/**
  * TA_VERIFY
  *
  * This macro checks if the condition supplied as the
  * first argument is true. Verify's are executed in
  * both debug and release builds (but the condition is
  * not checked for release builds).
  *
  * @param test   An expression to be tested.
  * @param info   A string outlining the problem if test is false.
  */
#define TA_VERIFY(test, info)					                    \
    {												                \
        if (!(test))                                                \
        {	                                                        \
            TA_LOGASSERTIONMSG(__FILE__, __LINE__, test, info);     \
            try { TA_DOBREAKACTION; }catch(...){};                  \
            TA_DOASSERTIONACTION(__FILE__, __LINE__, test, info);   \
        }											                \
    }

/**
  * TA_ASSERT
  *
  * This macro checks if the condition supplied as the
  * first argument is true. Assert's are executed only in
  * debug builds, not release builds.
  *
  * @param test   An expression to be tested.
  * @param info   A string outlining the problem if test is false.
  */
#define TA_ASSERT(test,info)	TA_VERIFY(test, info)

#else
#define TA_VERIFY(test, info) (test)
#define TA_ASSERT(test,info)
#endif


#define TA_Assert(test)         TA_ASSERT(test, "")
#define TA_Verify(test)         TA_VERIFY(test, "")


// TODO LPP: Remove these #includes - they are here because some files only include TAAssert.
#include "core/utilities/src/DebugUtil.h"
#include <sstream>

#endif // TAAssert_h
