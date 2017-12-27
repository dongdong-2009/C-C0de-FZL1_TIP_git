/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyDataStore.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Hides the singleton implementation of DutyDataStoreSingleton from clients.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;

void DutyDataStore::cleanUp()
{
	DutyDataStoreSingleton::cleanUp();
}

void DutyDataStore::populate(bool ignoreSystemOperators, bool ignoreAppSubsystems, bool ignoreOtherSessionSubsystemDuties/*=true*/)
{
    DutyDataStoreSingleton::getInstance().populate(ignoreSystemOperators, ignoreAppSubsystems,ignoreOtherSessionSubsystemDuties);
}

void DutyDataStore::setPopulateCondition(bool ignoreSystemOperators, bool ignoreAppSubsystems,bool ignoreOtherSessionSubsystemDuties)
{
	DutyDataStoreSingleton::getInstance().setPopulateCondition(ignoreSystemOperators, ignoreAppSubsystems,ignoreOtherSessionSubsystemDuties);
}

void DutyDataStore::populateStaticData()
{
	DutyDataStoreSingleton::getInstance().populateStaticData();
}

void DutyDataStore::populateDynamicData()
{
	DutyDataStoreSingleton::getInstance().populateDynamicData();
}

bool DutyDataStore::ignoreSystemOperators() 
{
    return DutyDataStoreSingleton::getInstance().ignoreSystemOperators();
}


bool DutyDataStore::ignoreAppSubsystems() 
{
    return DutyDataStoreSingleton::getInstance().ignoreAppSubsystems();
}


DataCache& DutyDataStore::getDataCache()
{
    return DutyDataStoreSingleton::getInstance().getDataCache();
}


SessionCache& DutyDataStore::getSessionCache()
{
    return DutyDataStoreSingleton::getInstance().getSessionCache();
}


DefaultRegionAssignmentMatrix& DutyDataStore::getDefRegionAssignmentMatrix()
{
    return DutyDataStoreSingleton::getInstance().getDefRegionAssignmentMatrix();
}


RegionDutyMatrix& DutyDataStore::getRegionDutyMatrix()
{
    return DutyDataStoreSingleton::getInstance().getRegionDutyMatrix();
}


SubsystemDutyMatrix& DutyDataStore::getSubsystemDutyMatrix()
{
    return DutyDataStoreSingleton::getInstance().getSubsystemDutyMatrix();
}
