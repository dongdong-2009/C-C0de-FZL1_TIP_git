/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobThread.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * Runs an instance of a job.
  *
  */


#include <string>
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"

#include "app/scheduling/scheduling_agent/src/JobRunner.h"
#include "app/scheduling/scheduling_agent/src/IJobInstance.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceExecutionException.h"
#include "app/scheduling/scheduling_agent/src/JobThread.h"


namespace TA_Base_App
{

JobThread::JobThread(const IJobInstance* jobInstance) :
m_jobInstance( const_cast<IJobInstance*>(jobInstance) ), 
m_bDeleted( false )
{
}

JobThread::~JobThread() 
{
    if ( NULL != m_jobInstance )
    {
        delete m_jobInstance;
        m_jobInstance = NULL;
    }
}

void JobThread::markAsDeleted( const bool bDeleted )
{
    m_bDeleted = bDeleted;
}

bool JobThread::isMarkDeleted()
{
    return m_bDeleted;
}

void JobThread::run()
{
    if ( m_bDeleted )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Job thread is mark as deleted when starting execution" );
        return;
    }

    if ( NULL == m_jobInstance )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Job instance pointer is NULL" );
        return;
    }

    try
    {
        m_jobInstance->execute();
    }
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when execute job");
    }
}

void JobThread::terminate()
{
}

}; // TA_Base_App

