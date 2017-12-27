#ifndef ACTIVESTEP_H_INCLUDED
#define ACTIVESTEP_H_INCLUDED

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
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{
    class IActivePlanFlow;

	//Class declaration
	class ActiveStep
	{
	// Public methods
	public:
        ActiveStep( const TA_Base_Core::StepDetail& detail, IActivePlanFlow& flowControl, const unsigned long ulTimeout = 0u );
        virtual ~ActiveStep();

		/**
		  * load
		  *
		  * Provided publicly for the specific case where
		  * an object outside the step wishes to force the
		  * step to publish it's state before execute is
		  * called.
		  *
		  */
		void load();

		/**
		  * execute
		  *
		  * Allow a client to execute any step in a uniform manner
		  *
		  */
		virtual void execute();

		/**
		  * wait
		  *
		  * wait the step timeout
		  *
		  */
        void wait();
		
		/**
		  * getStepNumber
		  *
		  * each step must know it's own number
		  *
		  */
		TA_Base_Core::StepNumber getStepNumber() const;
        TA_Base_Core::StepId getStepId() const;

        void processStepExecutionFailure( const std::string& failureReson = "" );

		/**
		  * customise
		  *
		  * change a step's customisable behaviour
		  * (e.g. skip, delay)
		  *
		  */
		virtual void customise( const TA_Base_Core::StepCustomisableDetail& newDetail );

		/**
		  * refreshStartTime
		  *
		  * update the step whenever step is resume or start
		  *
		  */
        void refreshStartTime();

		/**
		  * remark
		  *
		  * update the remark
		  *
		  */
        void remark( const std::string& strRemark );

	protected:
		/**
		  * executeImpl
		  *
		  * where the work is done.`
		  *
		  */
        virtual TA_Base_Core::EActiveStepState executeImpl( void ) = 0;

		std::string dumpContext() const;

    protected:
        std::string m_remark;

		TA_Base_Core::StepDetail m_detail;

        bool m_bSubStep;

		ActiveStep& operator=( const ActiveStep& );
		ActiveStep( const ActiveStep& );

		IActivePlanFlow& m_flowControl;

        // Time that the delay started.
        time_t m_delayStartTime;

        time_t m_tmStartTime;

        // Amount of the delay completed. (Need to keep track of this in case the step is paused in the middle of a delay.)
        time_t m_delayCompleted;

        const time_t m_tmTimeout;
	}; // ActiveStep
}; // TA_Base_App

#endif // ACTIVESTEP_H_INCLUDED
