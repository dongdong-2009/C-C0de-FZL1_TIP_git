/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *	DataPointAgentCorbaDef_Impl is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing DataPoint configurations
  * and updates to remote clients.
  *
  */

#ifndef DATAPOINT_CORBA_IMPL_H
#define DATAPOINT_CORBA_IMPL_H

#include <map>
#include <sys/timeb.h>

#include "core/corba/src/ServantBase.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"
#include "core/timers/src/SingletonTimerUtil.h"

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/IDataPointAgentCorbaDef.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointStateContainer.h"

namespace TA_Base_Bus
{
	class DataPointObserver;
    class DataPointAgentSynHandler;
	class DataPoint;
    class DataPointDisplayClient;
    class DataPointFactory;

	class IDataPointAgent
	{
	public:

		virtual ~IDataPointAgent() {};


		/**
		 * getDataPoint
		 *
		 * To be implemented by the specific data point agent.  The method allows the class
		 * DataPointAgentCorbaDef_Impl to obtain a reference to the data point, specified by
		 * the pkey, which is created and maintained by the agent.  A null pointer
		 * is returned if the specific data point agent does not have a data point whose pkey
		 * matches with the specified pkey.
		 *
		 * @param	entityKey	The pkey of the wanted data point
		 *
		 */
		virtual TA_Base_Bus::DataPoint * getDataPoint ( unsigned long entityKey ) = 0;

        // liuyu++
        /**
		 * getAllDataPoints
		 *
		 * To be implemented by the specific data point agent.  The method allows the class
		 * DataPointAgentCorbaDef_Impl to obtain all the references to the data points which 
         * is created and maintained by the agent.
		 *
		 */
        virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList ) = 0;
        // ++liuyu
        
		/**
		 * getDataPointFactory
		 *
		 * To be implemented by the specific data point agent.  The method allows the class
		 * DataPointAccessCorbaDef_Impl to obtain a reference to the data point factory
		 *
		 */
		virtual TA_Base_Bus::DataPointFactory * getDataPointFactory () = 0;
	};


	//TD15618, use timer to trigger if the observers are timeout
	class DataPointAgentCorbaDef_Impl : public virtual TA_Base_Core::ServantBase,
										public virtual POA_TA_Base_Bus::IDataPointAgentCorbaDef,
										public virtual TA_Base_Core::AbstractThreadedTimeoutUser
	{
	public:


		DataPointAgentCorbaDef_Impl ( const std::string & agentName,
									  TA_Base_Bus::IDataPointAgent * specificDataPointAgent );


		virtual ~DataPointAgentCorbaDef_Impl();

		//
		// Operations required by TA_Base_Bus::DataPointAgentCorbaDef interface
		//
		virtual CORBA::ULong createObserver();
		virtual void addToObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& dpKeys);
		virtual void removeFromObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& dpKeys);
		virtual void destroyObserver(CORBA::ULong observerId);
		virtual DataPointCorbaTypes::DynamicSyncUpdateSequence* getUpdates(CORBA::ULong observerId);
		//for opc bridge use 
		virtual CORBA::ULong regiesterDisplayDataPointClient();
		virtual void unregiesterDisplayDataPointClient(CORBA::ULong cleintId);
		virtual DataPointCorbaTypes::DisplayStateSequence* getDisplayDataPointUpdate(CORBA::ULong clientId, CORBA::Boolean getall);

        //Mintao++ for display datapoint only
    	DataPointDisplayClient * findDisplayClient ( unsigned long clientId );
        //Mintao++ for display datapoint only
		//
		// Operations required by datapoint synchronization
		//
		//regiesterDataPointSynState
		/**
		  * regiesterDataPointSynState
		  *
		  * This method requests the Agent to create an registered client and if successful
		  * returns a non-zero registered client id.
		  *
		  * @return The id of the observer that has been created
		  *
		  * @exception			TA_Base_Core::OperationModeException raised if the agent is in Monitor mode.
		  */
		virtual CORBA::ULong regiesterDataPointSynState();
		
		/**
		 * setToControl
		 *
		 * This method let the DataPointAgentCorbaDef_Impl instance knows that it
		 * must activate its servant base and start receiving incoming requests
		 *
		 */
		virtual void setToControl();


		/**
		 * setToMonitor
		 *
		 * This method let the DataPointAgentCorbaDef_Impl instance knows that it
		 * must deactivate its servant base and stop receiving incoming requests
		 *
		 */
		virtual void setToMonitor();

		virtual void setOperationMode(TA_Base_Core::EOperationMode mode);

		//
		// Operations required by ITimeoutCallback
		//
	
		/**
		*
         * threadedTimerExpired
		*
		* To be implemented by users of TimerUtil.  This operation will be called
		* when the registered timeout period has expired.
		*
		*/
		virtual void threadedTimerExpired(long timerId, void* userData);
        
	private:

        DataPointAgentCorbaDef_Impl() {};
		DataPointAgentCorbaDef_Impl ( const DataPointAgentCorbaDef_Impl & theInstance ) {};
		DataPointAgentCorbaDef_Impl & operator= ( const DataPointAgentCorbaDef_Impl & rhs ) {};

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
		DataPointObserver * findObserver ( unsigned long observerId );

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

		void cleanupObserversAndClients();

		std::string m_agentName;
		TA_Base_Core::EOperationMode m_operationMode;
		unsigned long m_generatedObservedID;
		TA_Base_Bus::IDataPointAgent * m_specificAgent;
		std::map < unsigned long, DataPointObserver * > m_listOfObservers;
		TA_Base_Core::ReEntrantThreadLockable m_observerListLock;

		unsigned long m_generatedClientID;
		std::map < unsigned long, DataPointDisplayClient * > m_listOfDisplayClients;
		TA_Base_Core::ReEntrantThreadLockable m_clientListLock;
        DataPointStateContainer * m_stateContainer;

        DataPointAgentSynHandler *m_synHandler;
        //only one datapoint synchronization client is allowed
        unsigned long m_synClientID;
        bool m_synClientExisted;
	};
}

#endif

