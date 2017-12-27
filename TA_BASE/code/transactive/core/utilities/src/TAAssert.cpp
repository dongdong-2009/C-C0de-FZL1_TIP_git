/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/TAAssert.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/AssertException.h"
//#include <iostream>
//#include <cstdlib>

namespace TA_Base_Core
{
    // TODO LPP: Static initialisation ordering between this and other modules might be a problem.

    static int s_breakAction = BREAK_YES;
    // static int s_assertAction = FAIL_EXCEPTION;
    static int s_assertAction = FAIL_ABORT;

    void gSetRuntimeBreakAction(int breakAction)
    {
        if (breakAction == BREAK_NO || breakAction == BREAK_YES)
        {
            s_breakAction = breakAction;
        }
        else
        {
            gDoAssertionAction(FAIL_ABORT, __FILE__, __LINE__, "", "gSetRuntimeBreakAction(): Invalid breakAction");
        }
    }

    int gGetRuntimeBreakAction()
    {
        return s_breakAction;
    }

    void gSetRuntimeAssertAction(int action)
    {
        if (action == FAIL_NONE || action == FAIL_ABORT || action == FAIL_EXCEPTION)
        {
            s_assertAction = action;
        }
        else
        {
            gDoAssertionAction(FAIL_ABORT, __FILE__, __LINE__, "", "gSetRuntimeAssertAction(): Invalid action");
        }
    }

    int gGetRuntimeAssertAction()
    {
        return s_assertAction;
    }

    std::string constructAssertionMsg(const char* file, int line, const char* test, const char* info)
    {
        std::ostringstream stm;
        stm << "ASSERT failed:"
            << "\n\tFile: " << file
            << "\n\tLine: " << line
            << "\n\tFailed Condition: " << test
            << "\n\tDescription: " << info;

        return stm.str();
    }

    void gDoAssertionAction(int action, const char* file, int line, const char* test, const char* info)
    {
        if (action == FAIL_ABORT)
        {
            std::cout << constructAssertionMsg(file, line, test, info) << "\n\n";
            abort();
        }
        else if (action == FAIL_EXCEPTION)
        {
            throw TA_Base_Core::AssertException(constructAssertionMsg(file, line, test, info).c_str());
        }
    }

    void gLogAssertionMsg(const char* file, int line, const char* test, const char* info)
    {
        LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
             constructAssertionMsg(file, line, test, info).c_str());

        DebugUtil::getInstance().logStackTrace();
    }

    void gDoAssertionAction(int action, const char* file, int line, const char* test, const std::string& info)
    {
        gDoAssertionAction(action, file, line, test, info.c_str());
    }

    void gLogAssertionMsg(const char* file, int line, const char* test, const std::string& info)
    {
        gLogAssertionMsg(file, line, test, info.c_str());
    }
};
