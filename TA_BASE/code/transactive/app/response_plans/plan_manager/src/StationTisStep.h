/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/StationTisStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents a Station Traveller Info System step.
  *
  **/

#if !defined(AFX_STATIONTISSTEP_H__60E39874_7371_4B6C_9964_266A0BEE2706__INCLUDED_)
#define AFX_STATIONTISSTEP_H__60E39874_7371_4B6C_9964_266A0BEE2706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"
#include "PidsStepParameter.h"
#include "TisPidListStepParameter.h"


namespace TA_Base_App
{
	class PidsStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // StationTisStep

    class StationTisStep : public PlanStep
    {
    // Operations
    public:
        StationTisStep(PlanNode* parent, const TA_Base_Core::StepDetail& stepDetail);
        StationTisStep(PlanNode* parent, PlanStepId stepId);  // For new plan steps
        virtual ~StationTisStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const  { return false; }
        virtual bool canIgnoreFailure() const { return true; }
       
		PidsStepParameter* getPidsStepParameter()		  { return m_pidsstepParameter;}
		TisPidListStepParameter* getTisPidListStepParameter(){return m_pidListParameter;}

    protected:
        StationTisStep(const StationTisStep& other);

        virtual void createPlanStepParameters(PlanStepParameterList& stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters& parameterSet,
                                              PlanStepParameterList& stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        StationTisStep& operator=(const StationTisStep& other);


    // Attributes
    private:        
		PidsStepParameter*        m_pidsstepParameter;
		TisPidListStepParameter*  m_pidListParameter;
    };
}

#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#endif // !defined(AFX_STATIONTISSTEP_H__60E39874_7371_4B6C_9964_266A0BEE2706__INCLUDED_)
