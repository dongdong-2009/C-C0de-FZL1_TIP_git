/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/AlarmSupplier.h $
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


// Uses AlarmSender to send alarm messages (detail, update and loaded)
// on the Alarm channel.
class AlarmSupplierHandler : public ISimulationHandler
{
public:
    // senderType determines which AlarmSender method is called.
    enum SenderType { ALARM_DETAIL, ALARM_UPDATE, ALARM_LOADED };
    AlarmSupplierHandler(unsigned long simulatorID, SimpleDb& db, SenderType senderType);

    // Implementation of ISimulationHandler
    virtual void simulationStep(unsigned long stepID);

private:
    unsigned long m_simulatorID;
    SimpleDb& m_db;

    SenderType m_senderType;

    // Relevant columns from NST_SIMULATORCONFIG:
    // entityKey        - entity that sent alarm relates to 
    // locationKey      - location of the supplier
    // contextName      - context that the event will be published with
    // typeName         - type that the event will be published with

    unsigned long m_entityKey;

    // This locationKey is only used in the body of ALARM_DETAIL and 
    // ALARM_UPDATE messages.  Must not be LOCAL_LOCATION or ANY_LOCATION.
    unsigned long m_locationKey;

    std::string m_contextName;
    std::string m_typeName;
};


