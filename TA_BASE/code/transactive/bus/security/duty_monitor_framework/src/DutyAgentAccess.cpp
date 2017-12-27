/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyAgentAccess.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 10:20:13 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * Hides the singleton implementation of DutyAgentAccessSingleton from clients.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


DutyAgentAccess::DutyAgentAccess()
{
}


DutyAgentAccess::~DutyAgentAccess()
{
}


void DutyAgentAccess::getDutyState(const SessionId& sessionId, SubsystemStateList& subsystemStates, 
                                   SubsystemDutyList& subsystemDuties, RegionDutyList& regionDuties)
{
    DutyAgentAccessSingleton::getInstance().getDutyState(sessionId, subsystemStates, subsystemDuties, regionDuties);
}

void DutyAgentAccess::getSessionDutyState(const SessionId& sessionId, SubsystemStateList& subsystemStates, 
                                   SubsystemDutyList& subsystemDuties, RegionDutyList& regionDuties)
{
    DutyAgentAccessSingleton::getInstance().getSessionDutyState(sessionId, subsystemStates, subsystemDuties, regionDuties);
}
SubsystemStateList DutyAgentAccess::getSubsystemDutyState(const SessionId& sessionId, const SubsystemKeyList& subsystems, unsigned long locationKey)
{
	return DutyAgentAccessSingleton::getInstance().getSubsystemDutyState(sessionId, subsystems, locationKey);
}

TransferableRegionList DutyAgentAccess::getTransferableRegions(const SessionInfo& session)
{
    return DutyAgentAccessSingleton::getInstance().getTransferableRegions(session);
}


void DutyAgentAccess::getTransferableRegions(const SessionId& sessionId, TransferableRegionList& transferableRegions, SubsystemTreeMap& uncontrolledSubsystems)
{
    DutyAgentAccessSingleton::getInstance().getTransferableRegions(sessionId, transferableRegions, uncontrolledSubsystems);
}


SubsystemTreeMap DutyAgentAccess::getRemovableRegions(const SessionId& sessionId)
{
    return DutyAgentAccessSingleton::getInstance().getRemovableRegions(sessionId);
}


TransferableSubsystemList DutyAgentAccess::getTransferableSubsystems(const SessionInfo& session, const RegionKeyList& regionKeys)
{
    return DutyAgentAccessSingleton::getInstance().getTransferableSubsystems(session, regionKeys);
}


void DutyAgentAccess::transferRegionDuty(const TA_Base_Bus::SessionInfo& sessionInfo, const RegionKeyList& regionKeys)
{
    DutyAgentAccessSingleton::getInstance().transferRegionDuty(sessionInfo, regionKeys);
}


void DutyAgentAccess::transferRegionDuty(const SessionId& requestingSessionId, const SessionId& targetSessionId, const RegionKeyList& regionKeys)
{
    DutyAgentAccessSingleton::getInstance().transferRegionDuty(requestingSessionId, targetSessionId, regionKeys);
}


void DutyAgentAccess::removeRegionDuty(const SessionId& requestingSessionId, const SessionId& targetSessionId, const RegionKeyList& regionKeys)
{
    DutyAgentAccessSingleton::getInstance().removeRegionDuty(requestingSessionId, targetSessionId, regionKeys);
}


void DutyAgentAccess::transferSubsystemDuty(const TA_Base_Bus::SessionInfo& sessionInfo, const SubsystemTreeMap& subsystems)
{
    DutyAgentAccessSingleton::getInstance().transferSubsystemDuty(sessionInfo, subsystems);
}


void DutyAgentAccess::transferAllSubsystemDuty(const TA_Base_Bus::SessionInfo& sessionInfo)
{
    DutyAgentAccessSingleton::getInstance().transferAllSubsystemDuty(sessionInfo);
}


void DutyAgentAccess::degradeAllSubsystems(const SessionId& sessionId)
{
    DutyAgentAccessSingleton::getInstance().degradeAllSubsystems(sessionId);
}

void DutyAgentAccess::degradeSubsystemsAtLocation(const SessionId& sessionId, const SubsystemKeyList& subsystems, unsigned long locationKey)
{
	DutyAgentAccessSingleton::getInstance().degradeSubsystemsAtLocation(sessionId, subsystems, locationKey);
}

void DutyAgentAccess::degradeSubsystemsAtRegions(const SessionId& sessionId, const SubsystemKeyList& subsystems, const RegionKeyList &regions)
{
	DutyAgentAccessSingleton::getInstance().degradeSubsystemsAtRegions(sessionId, subsystems, regions);
}
