/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyDataStoreSingleton.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Consolidates all containers of duty-related data. This includes the config data
  * cache, active session cache, default region assignment matrix, active region duty
  * matrix and active session duty matrix.
  * To ensure that only one copy of each container is created and maintained, this
  * class has been implemented as a singleton.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


DutyDataStoreSingleton* DutyDataStoreSingleton::s_dutyDataStoreSingleton = NULL;
TA_Base_Core::NonReEntrantThreadLockable DutyDataStoreSingleton::s_singletonLock;


DutyDataStoreSingleton& DutyDataStoreSingleton::getInstance()
{
//    FUNCTION_ENTRY("getInstance()");

	if (s_dutyDataStoreSingleton == NULL)
	{
	    TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if (s_dutyDataStoreSingleton == NULL)
        {
		    s_dutyDataStoreSingleton = new DutyDataStoreSingleton();
        }
    }

//    FUNCTION_EXIT;
	return *s_dutyDataStoreSingleton;    
}


DutyDataStoreSingleton::DutyDataStoreSingleton() :
    m_ignoreSystemOperators(true),
    m_ignoreAppSubsystems(true),
	m_ignoreOtherSesSubDuties(true)
{
//    FUNCTION_ENTRY("DutyDataStoreSingleton()");
//    FUNCTION_EXIT;
}


DutyDataStoreSingleton::~DutyDataStoreSingleton()
{

}


void DutyDataStoreSingleton::populate(bool ignoreSystemOperators, bool ignoreAppSubsystems,bool ignoreOtherSesSubDuties /*= true*/)
{
    FUNCTION_ENTRY("populate()");
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[CL-21875] begin DutyDataStoreSingleton::populate");

    // Remember these so that other classes can query their state later (e.g. when processing duty updates).
    m_ignoreSystemOperators = ignoreSystemOperators;
    m_ignoreAppSubsystems = ignoreAppSubsystems;

    // Initialise the contents of the three matrices. Region and subsystem duty matrices can both be set up 
    // with data returned from a single call to the Duty Agent. We'll make that call here and pass the relevant 
    // bits of information into the matrices.
    SubsystemStateList subsystemStates;
    SubsystemDutyList subsystemDuties;
    RegionDutyList regionDuties;

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[CL-21875] in DutyDataStoreSingleton::populate, begin DutyAgentAccess().getDutyState");

	if(ignoreOtherSesSubDuties)
	{
		DutyAgentAccess().getSessionDutyState(getSessionCache().getCurrentSessionId(), subsystemStates, subsystemDuties, regionDuties);
	}
	else
	{
		DutyAgentAccess().getDutyState(getSessionCache().getCurrentSessionId(), subsystemStates, subsystemDuties, regionDuties);
	}
    

   	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, end DutyAgentAccess().getDutyState");
 
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, begin m_regionDutyMatrix.populate");
    m_regionDutyMatrix.populate(regionDuties);
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, end m_regionDutyMatrix.populate");

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, begin m_subsystemDutyMatrix.populate");
	m_subsystemDutyMatrix.populate(subsystemStates,subsystemDuties);
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, end m_subsystemDutyMatrix.populate");

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, begin m_defRegionAssignmentMatrix.populate");
    m_defRegionAssignmentMatrix.populate();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, end m_defRegionAssignmentMatrix.populate");
    FUNCTION_EXIT;
}

void DutyDataStoreSingleton::setPopulateCondition(bool ignoreSystemOperators, bool ignoreAppSubsystems,bool ignoreOtherSessionSubsystemDuties)
{
	FUNCTION_ENTRY("setPopulateCondition()");
	
	m_ignoreSystemOperators = ignoreSystemOperators;
	m_ignoreAppSubsystems = ignoreAppSubsystems;
	m_ignoreOtherSesSubDuties = ignoreOtherSessionSubsystemDuties;

	FUNCTION_EXIT;
}

void DutyDataStoreSingleton::populateStaticData()
{
	FUNCTION_ENTRY("populateStaticData()");
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"DutyDataStoreSingleton::populateStaticData begin");

    m_regionDutyMatrix.populateStaticData();

	m_subsystemDutyMatrix.populateStaticData();
	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"DutyDataStoreSingleton::populateStaticData end");
	FUNCTION_EXIT;
}

void DutyDataStoreSingleton::populateDynamicData()
{
	FUNCTION_ENTRY("populateDynamicData()");
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"DutyDataStoreSingleton::populateDynamicData begin");

	// Initialise the contents of the three matrices. Region and subsystem duty matrices can both be set up 
	// with data returned from a single call to the Duty Agent. We'll make that call here and pass the relevant 
	// bits of information into the matrices.
	SubsystemStateList subsystemStates;
	SubsystemDutyList subsystemDuties;
	RegionDutyList regionDuties;

	if(m_ignoreOtherSesSubDuties)
	{
		DutyAgentAccess().getSessionDutyState(getSessionCache().getCurrentSessionId(), subsystemStates, subsystemDuties, regionDuties);
	}
	else
	{
		DutyAgentAccess().getDutyState(getSessionCache().getCurrentSessionId(), subsystemStates, subsystemDuties, regionDuties);
	}
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, DutyAgentAccess().getDutyState end");
	
	m_regionDutyMatrix.populateDynamicData(regionDuties);
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, m_regionDutyMatrix.populateDynamicData end");
	
	m_subsystemDutyMatrix.populateDynamicData(subsystemStates,subsystemDuties);
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, m_subsystemDutyMatrix.populateDynamicData end");
	
	m_defRegionAssignmentMatrix.populate();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyDataStoreSingleton::populate, m_defRegionAssignmentMatrix.populate end");

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"DutyDataStoreSingleton::populateDynamicData end");
	FUNCTION_EXIT;
}

bool DutyDataStoreSingleton::ignoreSystemOperators() const
{
    return m_ignoreSystemOperators;   
}


bool DutyDataStoreSingleton::ignoreAppSubsystems() const
{
    return m_ignoreAppSubsystems;
}


DataCache& DutyDataStoreSingleton::getDataCache()
{
    return m_dataCache;
}


SessionCache& DutyDataStoreSingleton::getSessionCache()
{
    return m_sessionCache;
}


DefaultRegionAssignmentMatrix& DutyDataStoreSingleton::getDefRegionAssignmentMatrix()
{
    return m_defRegionAssignmentMatrix;
}


RegionDutyMatrix& DutyDataStoreSingleton::getRegionDutyMatrix()
{
    return m_regionDutyMatrix;
}


SubsystemDutyMatrix& DutyDataStoreSingleton::getSubsystemDutyMatrix()
{
    return m_subsystemDutyMatrix;
}

void DutyDataStoreSingleton::cleanUp()
{
	delete s_dutyDataStoreSingleton;
	s_dutyDataStoreSingleton = NULL;
}
