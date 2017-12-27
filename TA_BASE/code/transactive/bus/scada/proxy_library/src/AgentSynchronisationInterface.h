/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/AgentSynchronisationInterface.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  */
///////////////////////////////////////////////////////////
//  AgentSynchronisationInterface.h
//  Implementation of the Class AgentSynchronisationInterface
//  Created on:      28-Jun-2004 03:51:38 PM
///////////////////////////////////////////////////////////

#if !defined(AgentSynchronisationInterface_79B82801_CAF0_4948_91A1_4CB5796D9974__INCLUDED_)
#define AgentSynchronisationInterface_79B82801_CAF0_4948_91A1_4CB5796D9974__INCLUDED_

#include <string>
#include <vector>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/LFThreadPoolSingleton.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/naming/src/INamedObject.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

/**
 * This interface defines the common behaviour provided by all of the
 * synchronisation interfaces
 * @version 1.0
 * @created 28-Jun-2004 03:51:38 PM
 */

namespace TA_Base_Bus
{
    typedef enum
    {
        CORBA_EXCEPTION = 0,
            OBJECT_RESOLUTION_EXCEPTION = 1,
            BAD_PARAMETER_EXCEPTION = 2,
            OPERATION_MODE_EXCEPTION = 3,
            SCADA_PROXY_EXCEPTION = 4,
            UNKNOWN_EXCEPTION = 5
    } ExceptionType;
    
    const long BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS = 5L; // in secs for LM tianxiang
    const long BULK_POLL_COMMS_ERROR_RETRY_PERIOD_MSECS = BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS * 1000L; // in msecs
    const long SET_BAD_QUALITY_RETRY_PERIOD_SECS = 10L; // in secs

    class AgentSynchronisationInterface : public TA_Base_Core::Thread 
    {
    public:
        /**
          * Constructor
          * @param agentName
          * 
          */
        AgentSynchronisationInterface(std::string agentName,unsigned long location);
        virtual ~AgentSynchronisationInterface();

    public:

		 /**
         * Add a data point/data node to the list of managed entities
         */
        void add(unsigned long entityKey);

        /**
         * Remove a data point/data node from the list of managed entities
         */
        void remove(unsigned long entityKey);

        /* 
         * clears request list of multiple requests for the same entity
         */
        void removeOldRequests(unsigned long entityKey);

        /**
         * Define a run() method to do your work. This should block until terminate() is
         * called eg: CorbaUtil::GetInstance().Run() or m_dialog->DoModal()  NOTE: do not
         * call this method directly. It needs to be public so that it can be called from
         * the thread-spawning function
         */
        virtual void run() = 0;

        /**
         * The terminate() method should cause run() to return. eg: CorbaUtil::
         * GetInstance().Shutdown() or EndDialog(m_dialog)  NOTE: probably not a good idea
         * to call this method directly - call terminateAndWait() instead
         */
        virtual void terminate();

       /**
        * Get the name of the agent that is associated with this interface
        */
        std::string getAgentName() { return m_agentName; };

        unsigned long getLocationKey() { return m_locationKey; };

        virtual void enableSynchronisation(bool enabled);
        bool isSynchroniseEnabled() { return m_synchronisationEnabled; };

		typedef enum ERequestType {EmptyQueue, Add, Remove};
		
		typedef struct 
		{
			ERequestType	requestType;
			unsigned long	entityKey;
		} RequestItem;

		virtual void processRequest(RequestItem item) = 0;

    protected:
        RequestItem getNextRequest();
        
        void waitForRequest() { m_requestListSemaphore.wait(); };
        
        bool isTerminating() { return m_terminate;};

        void queueRequest(RequestItem request);
        
        /**
          * List of pending requests for add or delete of data points
          */
        std::vector<RequestItem>                m_requestList;
        TA_Base_Core::ReEntrantThreadLockable   m_requestListLock;
        TA_Base_Core::Semaphore                 m_requestListSemaphore;
        
        std::string                             m_agentName;
        volatile bool                           m_terminate;
        unsigned long                           m_locationKey;
        bool                                    m_synchronisationEnabled;

		//TD15939, indicates if the bulk poll interface is connected to agent, default is 'true'
		//to avoid keep updating all the datapoints/datanodes to 'agent not avaliable' state during disconnected.
		bool									m_isConnected; 
		unsigned long							m_qualityBadPeriod;
		bool									m_isLocal;
	};
};
#endif // !defined(AgentSynchronisationInterface_79B82801_CAF0_4948_91A1_4CB5796D9974__INCLUDED_)
