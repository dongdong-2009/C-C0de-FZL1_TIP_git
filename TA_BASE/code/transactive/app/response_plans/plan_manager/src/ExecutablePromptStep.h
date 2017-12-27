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

#if !defined(AFX_EXECUTABLEPROMPTSTEP_H__20522C28_336C_4624_9E69_0BA656E5A139__INCLUDED_)
#define AFX_EXECUTABLEPROMPTSTEP_H__20522C28_336C_4624_9E69_0BA656E5A139__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStep.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutablePromptStep

    class ExecutablePromptStep : public InteractiveStep
    {
    // Operations
    public:
        ExecutablePromptStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutablePromptStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLEPROMPTSTEP_H__20522C28_336C_4624_9E69_0BA656E5A139__INCLUDED_)
