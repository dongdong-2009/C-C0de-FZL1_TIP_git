/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/alarm/alarm_agent/src/DatabaseLoader.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4786)
  #pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "app/event/event_agent/src/DatabaseLoader.h"
#include "app/event/event_agent/src/DbEventRetriever.h"
#include "app/event/event_agent/src/IDataLoader.h"
#include "app/event/event_agent/src/LocalRunParamDefinitions.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::NonReEntrantThreadLockable;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

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
        FUNCTION_ENTRY("run");

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
				//Temporarily Disabled as we don't have capacity requirements yet
// 				catch(const AlarmCacheCapacityExceededException& cee)
// 				{
// 					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
// 						"AlarmCacheCapacityExceededException caught: %s. Agent terminate loading.", std::string(cee.what()).c_str() );
// 					m_terminated = true;
// 				}
                catch( ... )
                {
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					 "Unknown exception caught: Can't load alarms and updates from DB or Control Agent. Caught General Exception" );
				}
            }
        }
        FUNCTION_EXIT;
    }


    //
    // terminate
    //
    void DatabaseLoader::terminate()
    {
		FUNCTION_ENTRY("terminate");
       
        m_terminated = true;

       FUNCTION_EXIT;
    }

	//
	// acquire
	//
	void DatabaseLoader::acquire(bool toAcquire)
	{
		FUNCTION_ENTRY("acquire");
		m_acquire = toAcquire;

		FUNCTION_EXIT;
	}

};
