/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/DatabaseLoader.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4786)
  #pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "app/alarm/alarm_agent/src/DatabaseLoader.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"
#include "app/alarm/alarm_agent/src/IDataLoader.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/AlarmCacheCapacityExceededException.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::NonReEntrantThreadLockable;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::AlarmCacheCapacityExceededException;

namespace TA_Base_App
{
    //
    // Constructor
    //
    DatabaseLoader::DatabaseLoader( IDataLoader* loader, unsigned long p_pollingPeriod, unsigned long p_sleepTime )
    : m_terminated(false), m_loader(loader), 
      m_pollingPeriod( p_pollingPeriod ), m_sleepTime( p_sleepTime ), m_acquire(true)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "constructor" );
    }


    //
    // Destructor
    //
    DatabaseLoader::~DatabaseLoader()
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "destructor" );
    }

    
    //
    // run
    //
    void DatabaseLoader::run()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "run" );

        unsigned long i = m_pollingPeriod;

        while( !m_terminated )
        {
            Thread::sleep(m_sleepTime);
            i+= m_sleepTime;

            if( i >= m_pollingPeriod) // Try again every minute
            {
				try
				{
					i = 0;
					if( m_loader->hasDataLoaded() )
					{
						m_terminated = true;
					}
					else
					{
						if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) || 
							0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_STANDBY) )
						{
							m_loader->loadFromDatabase(); // Try to load from the database
						}
						else if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
						{
							if (m_loader->hasDataLoaded() == false && m_acquire)
							{
								m_loader->requestFromControllingAgent();// Request from controlling agent
							}
							// else do nothing as the cache may be still waiting for reply from control
						}
					}
				}
				catch(const AlarmCacheCapacityExceededException& cee)
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"AlarmCacheCapacityExceededException caught: %s. Agent terminate loading.", std::string(cee.what()).c_str() );
					m_terminated = true;
				}
                catch( ... )
                {
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					 "Unknown exception caught: Can't load alarms and updates from DB or Control Agent. Caught General Exception" );
				}
            }
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "run" );
    }


    //
    // terminate
    //
    void DatabaseLoader::terminate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "terminate" );

        m_terminated = true;

        LOG( SourceInfo, DebugUtil::FunctionExit, "terminate" );
    }

	//
	// acquire
	//
	void DatabaseLoader::acquire(bool toAcquire)
	{
		m_acquire = toAcquire;
	}

};
