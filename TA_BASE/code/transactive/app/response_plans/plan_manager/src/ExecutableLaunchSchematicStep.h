/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Facilitates the launching of a display when executing a Launch Schematic step.
  *
  */

#if !defined(AFX_EXECUTABLELAUNCHSCHEMATICSTEP_H__1958B41C_198B_4BEC_97DF_BBDC94A86BC6__INCLUDED_)
#define AFX_EXECUTABLELAUNCHSCHEMATICSTEP_H__1958B41C_198B_4BEC_97DF_BBDC94A86BC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStep.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutableLaunchSchematicStep

    class ExecutableLaunchSchematicStep : public InteractiveStep
    {
    // Operations
    public:
        ExecutableLaunchSchematicStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutableLaunchSchematicStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLELAUNCHSCHEMATICSTEP_H__1958B41C_198B_4BEC_97DF_BBDC94A86BC6__INCLUDED_)
