/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/ActivePlanAgency.h $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  **/

#ifndef ACTIVEPLANAGENCY_H_INCLUDED
#define ACTIVEPLANAGENCY_H_INCLUDED

#include <map>
#include "ace/Singleton.h"
#include "ace/Mutex.h"

namespace TA_Base_Core
{
    struct CurrentPlanStepDetail;
    struct ActivePlanDetail;
}

namespace TA_Base_Bus
{
    class PlanAgentLibrary;
    class AuthenticationLibrary;
    class SortableActiveStepId;
}

namespace TA_Base_App
{
    class ActivePlanStep;
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "ActivePlanAgency" and that "ActivePlanAgencyImpl" can
	// not be used.
    class ActivePlanAgencyImpl
	{
	// Public methods
	public:

		friend class ACE_Singleton<ActivePlanAgencyImpl, ACE_Recursive_Thread_Mutex>;

        void init();
        void uninit();
        TA_Base_Bus::PlanAgentLibrary* getPlanAgent() const;
        void processActiveStepDetailUpdate(const TA_Base_Core::CurrentPlanStepDetail& currentPlanStepDetail);
        void processActivePlanUpdate(const TA_Base_Core::ActivePlanDetail& refPlanDetail);

	private:
		ActivePlanAgencyImpl();
		~ActivePlanAgencyImpl();
		ActivePlanAgencyImpl( const ActivePlanAgencyImpl& otherActivePlanAgency );
		ActivePlanAgencyImpl& operator=( const ActivePlanAgencyImpl& otherActivePlanAgency );

        void cleanSteps();

    private:

        typedef std::map<TA_Base_Bus::SortableActiveStepId, ActivePlanStep*> ActivePlanStepMap;
        typedef ActivePlanStepMap::iterator ActivePlanStepMapIt;

        ACE_Mutex m_mtxStepsLock;
        ActivePlanStepMap m_activePlanStepMap;

        TA_Base_Bus::PlanAgentLibrary* m_planAgent;
	}; // ActivePlanAgencyImpl
	typedef ACE_Singleton<ActivePlanAgencyImpl, ACE_Recursive_Thread_Mutex> ActivePlanAgency;
} // TA_Base_App

#endif //ACTIVEPLANAGENCY_H_INCLUDED
