/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointBulkPollingSynchronizer.h $
  * @author:  Darren Sampson
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/09/13 16:40:07 $
  * Last modified by:  $Author: wang.wang $
  *
  * Implementation of the BulkPolled Synchronisation Interface
  *
  */

#ifndef DATAPOINT_AGENT_BULKPOLLED_SYNCHRONISATION_INTERFACE_H
#define DATAPOINT_AGENT_BULKPOLLED_SYNCHRONISATION_INTERFACE_H

#include <set>
#include <string>
#include <sys/timeb.h>

#include "bus/scada/common_library/IDL/src/IDataPointAgentCorbaDef.h"
#include "bus/scada/proxy_library/src/AgentSynchronisationInterface.h"
#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/synchronisation/src/Condition.h"

/**
 * This class is used to synchronise datapoints that are managed in the same
 * process context.  There will only be one instance of this type.
 * @version 1.0
 * @created 28-Jun-2004 03:44:08 PM
 */

namespace TA_Base_Bus
{
    class DataPointSyncManager;
    class DataPointProxyManager;

	class DataPointBulkPollingSynchronizer : public AgentSynchronisationInterface
                                                            
	{
		public:

			DataPointBulkPollingSynchronizer ( const std::string& agentName,
															   unsigned long location,
															   DataPointSyncManager& manager,
															   DataPointProxyManager& callback);

			virtual ~DataPointBulkPollingSynchronizer();

			/**
			 * Define a run() method to do your work. This should block until terminate() is
			 * called eg: CorbaUtil::GetInstance().Run() or m_dialog->DoModal()  NOTE: do not
			 * call this method directly. It needs to be public so that it can be called from
			 * the thread-spawning function
			 */
			virtual void run();

			virtual void terminate();

			virtual void processRequest(RequestItem) {};   

		protected:


			//
			// operations required by the BulkPolledAgentSynchronisationInterface
			//


			virtual void queryAgentInterface();
			virtual void resetObserver();
			virtual void addEntity ( const RequestItem & requestItem );
			virtual void removeEntity ( const RequestItem & requestItem );


		private:

            void initialiseObserver();
            void processRemoveList();
            void processAddList();
            void processAddList( std::vector<unsigned long>& addList );
			void clearAddRemoveLists();
            void queryAgentForUpdates();

			void setToBadNotConnectedQuality ( TA_Base_Bus::ExceptionType exception );

        private:

            DataPointSyncManager& m_manager;
            DataPointProxyManager& m_callback;

            volatile unsigned long m_observerId;
			// int m_corbaErrorPollSkipCount;

			typedef TA_Base_Core::NamedObject< IDataPointAgentCorbaDef,
                                               IDataPointAgentCorbaDef_ptr,
                                               IDataPointAgentCorbaDef_var >   DataPointAgentInterface;

            std::vector<unsigned long> m_addList;
            std::vector<unsigned long> m_removeList;

			std::set<unsigned long>	m_syncEntities;

			DataPointAgentInterface * m_agentInterface;

			unsigned long m_pollPeriod;  // Milliseconds
            time_t m_timeLastConnected;
            
			//TA_Base_Core::Semaphore					m_timerSemaphore;
			TA_Base_Core::Condition					m_timeCondition;
			
			//TA_Base_Core::SingletonTimerUtil&		m_timerUtility;
	};
};

#endif
