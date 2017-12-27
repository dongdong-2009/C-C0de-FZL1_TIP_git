/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/ScheduleDataAccessFactory.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class creates objects that are used to read and change rows in the
  * Schedule database table.
  *
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/data_access_interface/scheduling_agent/src/ScheduleDataAccessFactory.h"
#include "core/data_access_interface/scheduling_agent/src/ConfigScheduleData.h"


namespace TA_Base_Core
{

ScheduleDataAccessFactory* ScheduleDataAccessFactory::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable ScheduleDataAccessFactory::m_singletonLock;


ScheduleDataAccessFactory& ScheduleDataAccessFactory::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    if(m_instance == NULL)
    {
        m_instance = new ScheduleDataAccessFactory();
    }
    
    return *m_instance;
}

ScheduleDataAccessFactory::ScheduleDataAccessFactory()
{
}

ScheduleDataAccessFactory::~ScheduleDataAccessFactory()
{
}


IConfigScheduleData* ScheduleDataAccessFactory::getSchedule( const std::string& id )
{
    return new ConfigScheduleData(id);
}


ScheduleDataAccessFactory::IConfigScheduleDataVector
    ScheduleDataAccessFactory::getAllSchedules()
{
    return ConfigScheduleData::getAllSchedules();
}

IConfigScheduleData* ScheduleDataAccessFactory::createSchedule( const std::string& jobID)
{
    return new ConfigScheduleData(jobID);
}

IConfigScheduleData* ScheduleDataAccessFactory::createSchedule()
{
    return new ConfigScheduleData();
}


};// TA_Base_Core
