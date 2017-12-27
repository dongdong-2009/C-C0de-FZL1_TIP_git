/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/CommsConsumer.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "Simulator.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#ifdef WIN32
#include <windows.h>
#endif


class CommsConsumerHandler : 
    public ISimulationHandler,
    public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
public:
    CommsConsumerHandler(unsigned long simulatorID, SimpleDb& db, bool broadcast);
    ~CommsConsumerHandler();

    // Implementation of ISimulationHandler
    virtual void simulationStep(unsigned long stepID);

    // Implementation of SpecialisedMessageSubscriber<CommsMessageCorbaDef>
	virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& payload) {}   // NOT CALLED
	virtual void receiveSpecialisedMessage(const CosNotification::StructuredEvent& event, const TA_Base_Core::CommsMessageCorbaDef& payload);

private:
    unsigned long m_simulatorID;
    SimpleDb& m_db;
    
    // True if this CommsConsumerHandler sends broadcast comms messages, false if it 
    // sends normal comms messages.
    bool m_broadcast;

    // Supplier simulator ID -> expected sequence number of next message from that supplier
    typedef std::map<unsigned long, unsigned long> SimulatorIDToSeqNumMap;
    SimulatorIDToSeqNumMap m_seqNumMap;

    // Relevant columns from NST_SIMULATORCONFIG:
    // srcEntityKey             - entity that received messages must have. Must be 0 if srcLocationKey
    //                            is to be used by the comms subscription filter. Not for broadcast messages.
    // srcLocationKey           - location of the supplier (this is the location that 
    //                            received messages must have in order to pass the filter).
    //                            Not for broadcast messages.
    // msgContextName           - context that received messages must have
    // msgTypeName              - type that received messages must have

    unsigned long m_entityKey;
    unsigned long m_locationKey;
    std::string m_contextName;
    std::string m_typeName;
};

