/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *	DataNodeAgentCorbaDef_Impl is an abstract base class that implements
  * the DataNodeAgentCorbaDef interface, providing DataNode configurations
  * and updates to remote clients.
  *
  */

#ifndef DATA_NODE_AGENT_CORBA_IMPL_H
#define DATA_NODE_AGENT_CORBA_IMPL_H

#include <map>
#include <sys/timeb.h>

#include "core/corba/src/ServantBase.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"
#include "core/timers/src/SingletonTimerUtil.h"

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/IDataNodeAgentCorbaDef.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeStateContainer.h"

namespace TA_Base_Bus
{
	// forward declaration
	class DataNodeObserver;
    class DataNodeAgentSynHandler;
	class DataNode;
    class DataNodeDisplayClient;
	class IDataNodeAgent;

	//TD15618, use timer to trigger if the observers are timeout
	class DataNodeAgentCorbaDef_Impl : public virtual TA_Base_Core::ServantBase,
										public virtual POA_TA_Base_Bus::IDataNodeAgentCorbaDef,
										public virtual TA_Base_Core::AbstractThreadedTimeoutUser
	{
    public:
        
        DataNodeAgentCorbaDef_Impl ( const std::string & agentName,
                                     TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent );

		virtual ~DataNodeAgentCorbaDef_Impl();

		//
		// Operations required by TA_Base_Bus::DataNodeAgentCorbaDef interface
		//


		virtual CORBA::ULong createObserver() ;
		virtual void addToObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& dnKeys) ;
		virtual void removeFromObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& dnKeys) ;
		virtual void destroyObserver(CORBA::ULong observerId) ;
		virtual DataNodeCorbaTypes::SyncUpdateSequence* getUpdates(CORBA::ULong observerId) ;
		virtual CORBA::ULong regiesterDisplayDataNodeClient() ;
		virtual void unregiesterDisplayDataNodeClient(CORBA::ULong cleintId) ;
		virtual DataNodeCorbaTypes::DataNodeDisplayStateSequence* getDisplayDataNodeUpdate(CORBA::ULong clientId, CORBA::Boolean getall) ;


        DataNodeDisplayClient * findDisplayClient ( unsigned long clientId );
		
		//
		// Operations required by datanode synchronization
		//
		//regiesterDataNodeSynState
		/**
		  * regiesterDataNodeSynState
		  *
		  * This method requests the Agent to create an registered client and if successful
		  * returns a non-zero registered client id.
		  *
		  * @return The id of the observer that has been created
		  *
		  * @exception			TA_Base_Core::OperationModeException raised if the agent is in Monitor mode.
		  */
		virtual CORBA::ULong regiesterDataNodeSynState();
		
		//
		// Operations required by TA_Base_Bus::DataNodeAgentCorbaDef interface
		//

		/**
		 * setToControl
		 *
		 * This method let the DataNodeAgentCorbaDef_Impl instance knows that it
		 * must activate its servant base and start receiving incoming requests
		 *
		 */
		virtual void setToControl();

		/**
		 * setToMonitor
		 *
		 * This method let the DataNodeAgentCorbaDef_Impl instance knows that it
		 * must deactivate its servant base and stop receiving incoming requests
		 *
		 */
		virtual void setToMonitor();

		/**
		 * setToStandby
		 *
		 * This method let the DataNodeAgentCorbaDef_Impl instance knows that it
		 * must deactivate its servant base and stop receiving incoming requests
		 *
		 */
		virtual void setToStandby();

		/**
		 * setOperationMode
		 *
		 * This method let the DataNodeAgentCorbaDef_Impl instance into new mode
		 *
		 */
		void setOperationMode(TA_Base_Core::EOperationMode mode);

		//
		// Operations required by ITimeoutCallback
		//
	
		/**
		*
		* timerExpired
		*
		* To be implemented by users of TimerUtil.  This operation will be called
		* when the registered timeout period has expired.
		*
		*/
		virtual void threadedTimerExpired(long timerId, void* userData);

	private:

		DataNodeAgentCorbaDef_Impl();
		DataNodeAgentCorbaDef_Impl( const DataNodeAgentCorbaDef_Impl & theInstance );
		DataNodeAgentCorbaDef_Impl & operator= ( const DataNodeAgentCorbaDef_Impl & rhs );

		/**
		 * generateNextObserverID
		 *
		 * Returns the next observer ID
		 *
		 */
		unsigned long generateNextObserverID();

		/**
		 * findObserver
		 *
		 * Returns a pointer to the existing observer whose ID matches with the ID specified
		 * in the parameter list.  A null pointer is returned if no match is found
		 *
		 * @param	observerId	The ID of the wanted observer
		 *
		 */
		DataNodeObserver * findObserver ( unsigned long observerId );

		/**
		 * generateNextClientID
		 *
		 * Returns the next client ID
		 *
		 */
	    unsigned long generateNextClientID();

	    /**
	      * cleanupDisplayContainer
	      * 
	      *  clean up the display container
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void cleanupDisplayContainer();
        
		std::string m_agentName;
		volatile TA_Base_Core::EOperationMode m_operationMode;
		unsigned long m_generatedObservedID;
		TA_Base_Bus::IDataNodeAgent * m_specificAgent;
		std::map < unsigned long, DataNodeObserver * > m_listOfObservers;
		TA_Base_Core::ReEntrantThreadLockable m_observerListLock;
        
		unsigned long m_generatedClientID;
		std::map < unsigned long, DataNodeDisplayClient * > m_listOfDisplayClients;
		TA_Base_Core::ReEntrantThreadLockable m_clientListLock;
        DataNodeStateContainer * m_stateContainer;

        DataNodeAgentSynHandler *m_synHandler;
        //only one datanode synchronization client is allowed
        unsigned long m_synClientID;
        bool m_synClientExisted;
		bool m_servantActivated;
	};
}

#endif

