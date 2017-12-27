//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#if !defined(AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_)
#define AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStep.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutableRunPlanStep

    class ExecutableRunPlanStep : public InteractiveStep
    {
    // Operations
    public:
        ExecutableRunPlanStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutableRunPlanStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_)
