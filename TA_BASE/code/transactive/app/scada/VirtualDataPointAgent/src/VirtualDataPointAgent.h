/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/VirtualDataPointAgent/src/VirtualDataPointAgent.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2012/06/12 13:35:44 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	VirtualDataPointAgent implements the GenericAgent interface.  VirtualDataPointAgent monitors and controls
  * the virtual data point entities associated with its process.
  *
  */

#ifndef VIRTUAL_DATAPOINT_AGENT_H
#define VIRTUAL_DATAPOINT_AGENT_H

#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_Base_Core
{
	class AuditMessageSender;
    class StateUpdateMessageSender;
};

namespace TA_Base_Bus
{
	class GenericAgent;
	class DataPointFactory;
	class ScadaProxyFactory;
	class AgentStateUpdateQueueSingletonManager;
};

class VirtualDataPointAgent : public TA_Base_Bus::IGenericAgentUser,
							  public TA_Base_Bus::IDataPointAgent
{

public:

	VirtualDataPointAgent ( int argc, char* argv[] );
	virtual ~VirtualDataPointAgent();

	//
	// operations required by IGenericAgentUser
	//
	//TD18706,jianghp, handle the operation mode
	virtual void checkOperationMode()
	{
		//do nothing...
	};
	/**
	  * notifyGroupOffline
	  *
	  * This request is passed on by GenericAgent when it receives a call to
      * notifyGroupOffline from the Process Monitor.
      *
      * @param The group name
	  */
    virtual void notifyGroupOffline( const std::string& group );

	/**
	  * notifyGroupOnline
	  *
	  * This request is passed on by GenericAgent when it receives a call to
      * notifyGroupOnline from the Process Monitor.
      *
      * @param The group name
	  */
    virtual void notifyGroupOnline( const std::string& group );

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
	virtual IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

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


	//
	// operations required by IDataPointAgent interface
	//


	virtual TA_Base_Bus::DataPoint * getDataPoint ( unsigned long entityKey );


	//
	// opearations specific to VirtualDataPointAgent
	//


    /**
      * startVirtualDataPointAgent
      *
      * Start this VirtualDataPointAgent process
      *
      */
	void startVirtualDataPointAgent();


	//Mintao++
	virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList );

    virtual TA_Base_Bus::DataPointFactory* getDataPointFactory();
        
// operations
private:

    /**
      * loadAgentParamValues
      *
      * Load the VirtualDataPointAgent entity's parameter from the database
	  *
      */
	void loadAgentParamValues();

	/**
     * submitAuditMessage()
     *
     * Generates a log-only event.
     *
     * @param	eventMsg	the message for the event
     *
     */
	void submitAuditMessage ( const std::string & eventMsg );

	void updateVirtualDataPointAgentConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent);

	void updateDataPointConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent);

	IEntity* createDataPointEntity ( TA_Base_Core::IEntityDataPtr entityData );

// attributes
private:

	TA_Base_Core::ReEntrantThreadLockable m_dataPointListtLock;
	int m_proxyDpMonitoringPeriodSecs;
	unsigned long m_virtualDataPointAgentPrimaryKey;
	bool m_threadTerminated;
	bool m_agentSetControl;
	std::string m_stationName;
	std::string m_agentName;

	TA_Base_Core::ReEntrantThreadLockable m_dataPointListLock;
	TA_Base_Bus::GenericAgent * m_genericAgent;
	TA_Base_Core::StateUpdateMessageSender* m_stateSender;
	TA_Base_Core::AuditMessageSender * m_auditMessageSender;
	TA_Base_Bus::DataPointAgentCorbaDef_Impl * m_bulkPolledInterface;
	TA_Base_Bus::DataPointAccessCorbaDef_Impl * m_DpAccessInterface;
	TA_Base_Bus::DataPointFactory* m_dataPointFactory;
	TA_Base_Bus::ScadaProxyFactory * m_scadaProxyFactory;

	std::map< unsigned long, TA_Base_Bus::DataPoint * > m_virtualDataPointList;
};

#endif

