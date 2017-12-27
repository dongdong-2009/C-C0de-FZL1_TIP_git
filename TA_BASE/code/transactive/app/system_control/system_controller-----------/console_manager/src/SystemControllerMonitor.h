#ifndef SYSTEM_CONTROLLER_MONITOR_H_INCLUDED
#define SYSTEM_CONTROLLER_MONITOR_H_INCLUDED


#include "core\process_management\idl\src\IHeartbeatReceiverCorbaDef.h"
#include "core/corba/src/ServantBase.h"



class SystemControllerMonitor : public virtual POA_TA_Base_Core::IHeartbeatReceiverCorbaDef,
                                public virtual TA_Base_Core::ServantBase
{
public:
    SystemControllerMonitor();
    virtual void receiveHeartbeat(const char* host) {}
};


#endif
