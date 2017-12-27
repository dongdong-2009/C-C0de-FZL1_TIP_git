//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/StationPAStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_STATIONPASTEP_H__5706B830_68AE_430A_B32A_DAE44C6B600F__INCLUDED_)
#define AFX_STATIONPASTEP_H__5706B830_68AE_430A_B32A_DAE44C6B600F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"
#include "StationStepParameter.h"
#include    "NumericStepParameter.h"
#include "YesNoStepParameter.h"
#include    "PAMessageStepParameter.h"
#include "PAZonesStepParameter.h"
#include    "TimeSpanStepParameter.h"

namespace TA_Base_App
{

        class StationStepParameter;
        class    PaMessageStepParameter;
        class    NumericStepParameter;
        class YesNoStepParameter;
        class PAZonesStepParameter;
      /////////////////////////////////////////////////////////////////////////////
    // StationPAStep

    class StationPAStep : public PlanStep
    {
    // Operations
    public:
        StationPAStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
         StationPAStep(PlanNode* parent, PlanStepId stepId);
        virtual ~StationPAStep();

        virtual PlanStep* clone();
      virtual bool isStepBranching() const { return false; }
      virtual bool canIgnoreFailure() const { return true; }
        StationStepParameter*   getStationDestinationParameter () { return m_station; }
        PaZonesStepParameter*   getPaZonesParameter                 (){ return m_zones; }
        PaMessageStepParameter* getPaMessageParameter            (){ return m_message; }
        YesNoStepParameter*        getChimeParameter                     (){ return m_chime; }
        YesNoStepParameter*        getCyclicParameter                 (){ return m_cyclic; }
        YesNoStepParameter*        getTISSynch                             (){ return m_TISSynch; }
        NumericStepParameter*    getMessageDuration                 (){ return m_duration;}
        NumericStepParameter*    getMessageInterval                 (){ return m_interval;}
        NumericStepParameter*    getCylicTimes                 (){ return m_cylicTimes;}

    protected:
        StationPAStep(const StationPAStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
      virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                            PlanStepParameterList &stepParameters);

        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        StationPAStep &operator=(const StationPAStep &other);


    // Attributes
    private:
        StationStepParameter*    m_station;
        PaZonesStepParameter*    m_zones;
        PaMessageStepParameter*    m_message;
        YesNoStepParameter*        m_chime;
        YesNoStepParameter*        m_cyclic;
        YesNoStepParameter*        m_TISSynch;
        TimeSpanStepParameter*    m_duration;
        NumericStepParameter*    m_interval;
        NumericStepParameter*    m_cylicTimes;
    };
}

#endif // defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#endif // !defined(AFX_STATIONPASTEP_H__5706B830_68AE_430A_B32A_DAE44C6B600F__INCLUDED_)
