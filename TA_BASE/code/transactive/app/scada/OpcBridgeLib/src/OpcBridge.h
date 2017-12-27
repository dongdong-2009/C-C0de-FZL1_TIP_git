/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File$
  * @author:	Darren Sampson
  * @version:	$Revision$
  *
  * Last modification:	$DateTime$
  * Last modified by:	$Author$
  *
  * Description:
  *
  *	DataNodeAgent implementes the GenericAgent interface.  DataNodeAgent monitors and controls the 
  * the entities associated with its process, such as DataNode entities.
  *
  */

#ifndef OPCBRIDGE_H
#define OPCBRIDGE_H

#include <string>

#include "app/scada/OpcBridgeLib/src/OpcTagManager.h"

#include "bus/generic_service/src/IServiceAgentUser.h"

#include "core/timers/src/TimerUtil.h"

namespace TA_Base_App
{
	/*
	** Define constants
	*/

	// {6764A030-070C-11d3-80D6-0060975858BE}
	const GUID CLSID_SVR =
	{ 0x6764a030, 0x70c, 0x11d3, { 0x80, 0xd6, 0x0, 0x60, 0x97, 0x58, 0x58, 0xbe } };

	const std::string SVR_NAME = "TransActiveDataSource";

	const std::string SVR_DESC = "TransActive OPC DataSource";

	const std::string SVR_EXE_PATH = "";

	const UINT SVR_RATE = 50; // in msecs*/

	const std::string VENDOR_INFO = "TransActive OPC Bridge";

 
	class AttributeMapManager;
	class TransActiveDataSource;

	class OpcBridge : public TA_Base_Bus::IServiceAgentUser
	{

	public:

		OpcBridge ( int argc, char* argv[] );
		virtual ~OpcBridge();


		//TD18706
		virtual void checkOperationMode()
		{
			//do nothing
		}
		//
		// operations required by IServiceAgentUser
		//

		/**
          * start
          *
          * This pure virtual method is used to start any processing
		  * after the service agent is set up
          *
          */
		void start();

		//
		// operations required by IGenericAgentUser
		//

		/**
		  * agentTerminate
		  *
		  * This pure virtual method is called from the onTerminate
		  * method in GenericAgent. This allows the agent to do any
		  * of its own cleaning up before exiting.
		  *
		  * If the agent's entities implement CORBA interfaces,
		  * this method should take care of deactivating them. It is not
		  * possible for GenericAgent to do this because the IEntity 
		  * interface is not a ServantBase.
		  *
		  * If the class implementing this interface holds the GenericAgent
		  * as a member variable, DO NOT delete it inside this method.
		  * GenericAgent waits for this call to return before shutting down
		  * the ORB. Deleting the GenericAgent from within this method will
		  * cause obvious problems.
		  */
		virtual void agentTerminate();

		/**
		  * createEntity
		  *
		  * This method takes Entity Configuration data stored in the database and
		  * generates an object that implements the IEntity interface. The returned
		  * objects are stored and managed within Generic Agent. The Agent can get a 
		  * pointer to the objects via the getEntityMap method in GenericAgent.
		  *
		  * @param Entity data retrieved from the database.
		  *
		  * @return The constructed entity
		  */
		virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

		/**
		  * agentSetMonitor
		  *
		  * This pure virtual method is called by GenericAgent whenever it receives
		  * a request from the Process Monitor to change its operating state to monitor.
		  * The derived agent should respond by subscribing/unsubscribing to/from the 
		  * appropriate message channels and performing any other internal house keeping.
		  */
		virtual void agentSetMonitor();

		/**
		  * agentSetControl
		  *
		  * This pure virtual method is called by GenericAgent whenever it receives
		  * a request from the Process Monitor to change its operating state to control.
		  * The derived agent should respond by subscribing/unsubscribing to/from the 
		  * appropriate message channels and performing any other internal house keeping.
		  */
		virtual void agentSetControl();

		/**
		  * registerForStateUpdates
		  *
		  * This pure virtual method forces agents to subscribe for either
		  * state updates or state update requests, depending on whether or not they
		  * are operating in monitor or control mode.
		  */
		virtual void registerForStateUpdates();

		/**
		  * receiveSpecialisedMessage
		  *
		  * This method is called whenever the Agent receives a state update or
		  * state update request (depending on their mode of operation)
		  */
		virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

		/**
		  * processOnlineUpdate
		  *
		  * Generic Agent will handle the receiving of online updates. However, in some
		  * cases, it will be necessary to pass the updates on to the registered agent.
		  * For example, if a child entity needs to be updated.
		  *
		  * @param The update event
		  */
		virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		virtual void notifyGroupOffline( const std::string& group );

		virtual void notifyGroupOnline( const std::string& group );

		/**
		  * getOpcTagManager
		  *
		  * Returns the OpcTagManager
		  *
		  * @return				Returns the OpcTagManager
		  *
		  */
		OpcTagManager& getOpcTagManager();

		/**
		  * getAttributeMapManager
		  *
		  * Returns a pointer to the DataNodeList for the location
		  * This function creates the list if it does not exist
		  *
		  *	@param	location	The list location
		  *	@return	dnList		A pointer to the DataNodeList
		  *
		  */
		AttributeMapManager& getAttributeMapManager();
		
		/**
		  * parseLocationString
		  *
		  *
		  *	@param	location	The location string
		  *	@param	outLocs		A vector of location key which parse from @param location
		  * @return	true if location keys contain zero(means all locations), otherwise false
		  *
		  */
		bool parseLocationString(const std::string& locationStr, std::vector<unsigned long>& outLocs);
    private:
		//TD19439
		//TA_Base_Core::TimerUtil  m_timer;
		bool					m_bStarted;

	protected:

		OpcTagManager*			m_opcTagManager;
		TransActiveDataSource*  m_transActiveDataSource;
		AttributeMapManager*	m_attributeMapManager;

    public:
        static const std::string TRANSACTIVEDATASOUCE_UPDATEITEM_QUEUESIZE;
        static const std::string OPCTAGMANAGER_MAPSIZE;
        static const std::string ATTRIBUTES_COUNT;
        static const std::string OPCTAG_CREATION_TIME;
	};
}
#endif

