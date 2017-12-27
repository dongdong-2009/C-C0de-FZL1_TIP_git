/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/CommsSupplier.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "Simulator.h"

#ifdef WIN32
#include <windows.h>
#endif


class CommsSupplierHandler : public ISimulationHandler
{
public:
    CommsSupplierHandler(unsigned long simulatorID, SimpleDb& db, bool broadcast);

    // Implementation of ISimulationHandler
    virtual void simulationStep(unsigned long stepID);

private:
    unsigned long m_simulatorID;
    SimpleDb& m_db;

    // Indicates that this supplier should send broadcast comms messages, rather than
    // normal comms messages.  An important difference is that the message type for
    // normal comms messages is formed from the entity key and message type - eg.
    // 0_999997.
    bool m_broadcast;

    // Relevant columns from NST_SIMULATORCONFIG:
    // srcEntityKey             - entity that received messages must have. Must be 0 if srcLocationKey
    //                            is to be used by the comms subscription filter. Not for broadcast messages.
    // srcLocationKey           - location of the supplier (this is the location that 
    //                            received messages must have in order to pass the filter).
    //                            Not for broadcast messages.
    // msgContextName           - context that received messages must have
    // msgTypeName              - type that received messages must have

    unsigned long m_entityKey;

    // NOTE that location key must not be 0 (this is asserted by createSubscriptionFilterForCommsMessage).
    unsigned long m_locationKey;

    std::string m_contextName;
    std::string m_typeName;
};


// TODO LPP: It might be worthwhile allowing a set of message types (for a single
// context) to be sent.  Would need to work out how this might interact with the
// entity key generating function, and whether this makes validation of subscriptions
// too difficult (because a given consumer won't receive every message sent).

// TODO LPP: Need to allow a set of filterable data (name, value) pairs to be specified.

