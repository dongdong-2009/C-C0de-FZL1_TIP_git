/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/AlarmConsumer.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "Simulator.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

#ifdef WIN32
#include <windows.h>
#endif


// Receives any of the AlarmMessageCorbaDef union types (ie. AlarmDetailCorbaDef,
// AlarmUpdateCorbaDef, AlarmsLoadedNotification) on the Alarm channel.
class AlarmConsumerHandler: 
    public ISimulationHandler,
    public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::AlarmMessageCorbaDef>
{
public:
    AlarmConsumerHandler(unsigned long simulatorID, SimpleDb& db);
    ~AlarmConsumerHandler();

    // Implementation of ISimulationHandler
    virtual void simulationStep(unsigned long stepID);

    // Implementation of SpecialisedMessageSubscriber<CommsMessageCorbaDef>
	virtual void receiveSpecialisedMessage(const TA_Base_Core::AlarmMessageCorbaDef& payload) {}   // NOT CALLED
	virtual void receiveSpecialisedMessage(const CosNotification::StructuredEvent& event, const TA_Base_Core::AlarmMessageCorbaDef& payload);

private:
    unsigned long m_simulatorID;
    SimpleDb& m_db;
    
    // Supplier simulator ID -> expected sequence number of next message from that supplier
    typedef std::map<unsigned long, unsigned long> SimulatorIDToSeqNumMap;
    SimulatorIDToSeqNumMap m_seqNumMap;

    // Relevant columns from NST_SIMULATORCONFIG:
    // entityKey        - entity key that received messages must have. 0 indicates subscription
    //                    is for all entities. 
    // locationKey      - location of the supplier.  Must not be LOCAL_LOCATION or ANY_LOCATION.
    // contextName      - context that received messages must have 
    // typeName         - type that received messages must have

    unsigned long m_entityKey;
    unsigned long m_locationKey;
    std::string m_contextName;
    std::string m_typeName;
};

