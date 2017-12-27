#ifndef ACTIVEPLAN_H_INCLUDED
#define ACTIVEPLAN_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */

#include <list>
#include <map>

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"

namespace TA_Base_App
{
    class ActiveStep;

    typedef std::list<ActiveStep*> ActiveStepList;
    typedef ActiveStepList::iterator ActiveStepListIterator;
    typedef std::map<TA_Base_Core::StepId, TA_Base_App::ActiveStep*> ActiveStepIndexMap;
    typedef ActiveStepIndexMap::iterator ActiveStepIndexMapIt;

	//Class declaration
	class ActivePlan : public IActivePlanFlow
	{
        friend class ActivePlanState;

	// Public methods
	public:

		enum ENextStepExecuteMethod{ NORMAL_NSEM, INTERLOCKED_NSEM, UNDEFINED_NSEM };

		ActivePlan( const TA_Base_Core::ActivePlanId& plan, const int initialState, const bool bStateSyncDone );
		virtual ~ActivePlan();

		bool getIsActive( void ) { return( m_isActive ); }
		virtual void setIsActive( bool isActive ) { m_isActive = isActive; }

		int executeNextStep( void );

        void stop( const std::string& reason );
		void pause( void );
        void complete( void );
		void resume();
		void reassign(const std::string& sessionId, const std::string& pmId);
		void unassign( void );
        void customiseStep( const TA_Base_Core::StepCustomisableDetail& newDetail );
		void customiseSteps( const TA_Base_Core::StepCustomisableDetails& newDetails );

        virtual void pauseForOperator( void );
		virtual void pausePlan( const std::string& remark, const std::string& failedStepAlarmId = "" );
		virtual std::string getRemark( void );

        //Every step must call only once either of the following setNextStep method.
        // setNextStep(void) should only be used for default next step, whereas
        // setNextStep(step) should be used to set any step in the plan
		virtual void setNextStep( void );
        virtual void setCurrentStepPosition( const unsigned long stepNumber);
		virtual bool setNextStep( const unsigned long step );                    
        void publishNextStepDetail( void ) const;

        virtual void setPlanStepFailed() { m_stepFailed =  true; }
        virtual bool anyPlanStepFailed() { return m_stepFailed; }

		virtual const TA_Base_Core::ActivePlanId& getPlan() const;

		/**
		  * setInactiveInterlock
		  *
		  * Sets a plan that the next step must wait for
		  * before executing. No instance is provided for the
		  * plan as _all_ instances must be in an inactive
		  * state before the step will execute.
		  *
		  */
		virtual void setTerminatedInterlock( const unsigned long interlock );

		/**
		  * setCompletedInterlock
		  *
		  * Sets a plan (and instance) that the next step
		  * must wait for before executing. The given plan
		  * must be in the completed state before the step
		  * executes.
		  *
		  */
		virtual void setCompletedInterlock( const TA_Base_Core::ActivePlanId& interlock );

        /**
          * setOperatorResponseInterlock
          *
          * Sets a plan that the next step must wait for
          * before executing. The given plan must be in the
          * completed state before the step executes.
          */
        virtual void setOperatorResponseInterlock(bool interlock );

        virtual unsigned long getStepNumber();
        virtual ActiveStep* getCurrentStep();
        void scheduleTakeControl( const std::string session, const std::string& pmId );
        void schedulePause();
        bool isPausePending();
        void setInteractiveStep(bool isInteractiveStep);
        void setInteractiveStepFailed();
        void changeState(ActivePlanState* state);

        void setStateSyncDone( bool bSyncDone = true );

	private:
		ActivePlan& operator=( const ActivePlan& );
		ActivePlan( const ActivePlan& );
		bool interlockOk( void );
        int executeNextStepImpl( void );
        int waitCurStepImpl();
		void customiseStepImpl( const TA_Base_Core::StepCustomisableDetail& newDetail );
		void reassignImpl( const std::string& sessionId, const std::string& pmId );
        void transferControl(void);
		std::string dumpContext( void );

    private:
        TA_Base_Core::ActivePlanId m_plan;
        ActivePlanState* m_state;
        bool m_isActive;
        std::string m_remark;
        ActiveStepList m_steps;
        ActiveStepListIterator m_stepIterator;
        ActiveStepIndexMap m_stepsIndexMap;
        bool m_terminatedInterlockIsSet;
        bool m_completedInterlockIsSet;
        bool m_operatorResponseInterlockIsSet;
        bool m_hasPlanCompleted;

        bool m_stepFailed;
        std::string m_failedStepAlarmId;

        unsigned long m_terminatedInterlock;
        TA_Base_Core::ActivePlanId m_completedInterlock;

        // TODO: This will be replaced by identical functionality in the ActivePlanBulletinBoard.
        //no need for a whole list as we only need the last step executed
        unsigned long m_lastExecutedStep;

        //Member variables for taking control
        // In execution mode, Take control can only be effective when current step is complete or failed
        std::string m_newSessionId;
        std::string m_newPmId;
        bool m_isTakeControlPending;
        bool m_isPausePending;
        bool m_isCurrentInteractiveStep;
        bool m_hasInteractiveStepFailed;

        bool m_bStateSyncDone;
	}; // ActivePlan
}; // TA_Base_App



#endif // ACTIVEPLAN_H_INCLUDED

