#ifndef PLANEXECUTIONENGINE_H_INCLUDED
#define PLANEXECUTIONENGINE_H_INCLUDED

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
  *  Provides an interface to control the execution of 
  *  plans.
  */

#include <string>
#include <map>

#include "ace/Task.h"
#include "ace/Future.h"
#include "ace/Mutex.h"
#include "ace/RW_Mutex.h"
#include "ace/Activation_Queue.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    struct ActivePlanId;
}

namespace TA_Base_App
{
    class ActivePlanMessageQueue;
    class SortablePlanId;
    class ActivePlan;

	//Class declaration
	class PlanExecutionEngine : public ACE_Task<ACE_MT_SYNCH>
	{
	// Public methods
	public:
		PlanExecutionEngine();
		virtual ~PlanExecutionEngine();

		virtual int open( void* );
		virtual int close( u_long flags = 0 );

		/**
		  * svc
		  *
		  * This is the entry point for the callback from ACE_Task.
		  *
		  */
		virtual int svc( void );

		/**
		  * run
		  *
		  * Asynchronous interface for running plans.
		  *
		  * @return ACE_Future<int> can be used to block or ignored.
		  *
		  */
		ACE_Future<int> run( const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, 
            const int initialState, const bool manExecution, const bool bStartFromMonitorMode );

		/**
		  * spawn
		  *
		  * Asynchronous interface for running plans from other plans.
		  *
		  * @return ACE_Future<int> can be used to block or ignored.
		  *
		  */
		ACE_Future<int> spawn( const std::string& session, const std::string& pmId, const TA_Base_Core::ActivePlanId& parentPlan, 
            TA_Base_Core::ActivePlanId& childPlan, const int initialState, const bool bStartFromMonitorMode );
	
		/**
		  * runImpl
		  *
		  * Implementation of the async run method. This
		  * should only be called by a "Method Object".
		  *
		  */
		void runImpl( std::auto_ptr<ActivePlan> activePlan );
	

		/**
		  * put
		  *
		  * Should be used by all callers for putting messages on
		  * the tasks queue. This method will multiplex messages
		  * out to the appropriate plans.
		  *
		  */
		virtual int put(ACE_Message_Block *mb, ACE_Time_Value *tv = 0);

        void clearAllMessageQueue();

        void signalAllPlansIntoControl();

        void consolidateMq(); // jian.huang need be called by PlanHouseKeepingMethodObject, should be public

        void terminateAndWait();

        void publishPlanStart( const std::string& session, const std::string& pmId, 
            const TA_Base_Core::ActivePlanId& plan, const int initialState, const bool manExecution );

	private:

		PlanExecutionEngine& operator=( const PlanExecutionEngine& );
		PlanExecutionEngine( const PlanExecutionEngine& );

		/**
		  * runInterlocked
		  *
		  * Internal equivalent for the run method (minus the plan loading).
		  *
		  */
		ACE_Future<int> runInterlocked( std::auto_ptr<ActivePlan> activePlan );

		void processMessages( ActivePlan& activePlan, boost::shared_ptr<ActivePlanMessageQueue> mq );

        boost::shared_ptr<ActivePlanMessageQueue> getMq( const TA_Base_Core::ActivePlanId& plan ) const;
        void makeMq( const TA_Base_Core::ActivePlanId& plan );

		ACE_Activation_Queue m_activationQueue;

		typedef std::map< SortablePlanId, boost::shared_ptr<ActivePlanMessageQueue> > PlanMqMap;
		typedef PlanMqMap::iterator PlanMqMapIterator;
		typedef PlanMqMap::const_iterator ConstPlanMqMapIterator;

		PlanMqMap m_mqMap;
		mutable ACE_RW_Mutex m_mqMapLock;
		mutable ACE_Mutex    m_consolidateMqLock;

	// Protected attributes (deprecated)
	}; // PlanAgentImpl
}; // TA_Base_App

#endif // PLANEXECUTIONENGINE_H_INCLUDED
