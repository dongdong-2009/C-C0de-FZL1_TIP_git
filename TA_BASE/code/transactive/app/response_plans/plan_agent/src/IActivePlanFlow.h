#ifndef IACTIVEPLANFLOW_H_INCLUDED
#define IACTIVEPLANFLOW_H_INCLUDED

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

#include <string>

namespace TA_Base_Core
{
    struct ActivePlanId;
}

namespace TA_Base_App
{
    class ActivePlanState;

	//Class declaration
	class IActivePlanFlow
	{
	// Public methods
	public:
		IActivePlanFlow(){}
		virtual ~IActivePlanFlow(){}

		virtual void setIsActive( bool isActive ) = 0;
		virtual bool setNextStep( const unsigned long step ) = 0;
		virtual void setNextStep( void ) = 0;
		virtual void setCurrentStepPosition( const unsigned long step) = 0;
		virtual unsigned long getStepNumber() = 0;

		virtual const TA_Base_Core::ActivePlanId& getPlan( void ) const = 0;

		virtual void setTerminatedInterlock( const unsigned long interlock ) = 0; 
		virtual void setCompletedInterlock( const TA_Base_Core::ActivePlanId& interlock ) = 0;
        virtual void setOperatorResponseInterlock(bool interlock ) = 0;

        virtual void setPlanStepFailed() = 0;
        virtual bool anyPlanStepFailed() = 0;

        virtual void changeState(ActivePlanState* pState) = 0;

        virtual void setInteractiveStep(bool isInteractiveStep) = 0;

        virtual void pausePlan( const std::string& remark, const std::string& failedStepAlarmId = "" ) = 0;
        virtual void pauseForOperator() = 0;

		virtual std::string getRemark( void ) = 0;

	private:
		IActivePlanFlow& operator=( const IActivePlanFlow& );
		IActivePlanFlow( const IActivePlanFlow& );

	}; // IActivePlanFlow
}; // TA_Base_App

#endif // IACTIVEPLANFLOW_H_INCLUDED
