/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TimeSpanStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents a generic time span parameter (e.g. as used by TIS and PA steps for message duration).
  * The supported time resolutions are: days, hours, minutes, seconds.
  *
  **/

#if !defined(AFX_TIMESPANSTEPPARAMETER_H__92240FA4_FBDF_4378_86C2_3E46C2251217__INCLUDED_)
#define AFX_TIMESPANSTEPPARAMETER_H__92240FA4_FBDF_4378_86C2_3E46C2251217__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NumericStepParameter.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TimeSpanStepParameter

    class TimeSpanStepParameter : public NumericStepParameter
    {
    // Types
    public:
        typedef enum
                {
                    TR_SECONDS,
                    TR_MINUTES,
                    TR_HOURS,
                    TR_DAYS,
                    TR_UNDEFINED
                }
                ETimeResolution;

    // Operations
    public:
        TimeSpanStepParameter(PlanStep& parent, std::string name, unsigned long number, ETimeResolution resolution, bool isHidden = false);
        TimeSpanStepParameter(PlanStep& parent, const TimeSpanStepParameter& parameter);
        TimeSpanStepParameter(const TimeSpanStepParameter& parameter);
        virtual ~TimeSpanStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

    private:
        TimeSpanStepParameter& operator=(const TimeSpanStepParameter& parameter);


    // Attributes
    private:
        ETimeResolution m_resolution;
    };
}

#endif // !defined(AFX_TIMESPANSTEPPARAMETER_H__92240FA4_FBDF_4378_86C2_3E46C2251217__INCLUDED_)

