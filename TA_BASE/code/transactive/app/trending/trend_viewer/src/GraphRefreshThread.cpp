#if !defined(GRAPH_REFRESH_THREAD_CPP)
#define GRAPH_REFRESH_THREAD_CPP

/**
  * GraphRefreshThread.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/GraphRefreshThread.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This thread refreshes the graph at a regular interval. It moves the graph forward to the latest
  * time and plots another point (retrieves the last value) at that time for each trend shown in 
  * the graph.
  *
  */

#include <ctime>

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/GraphRefreshThread.h"
#include "app/trending/trend_viewer/src/IDataUpdateObserver.h"
#include "app/trending/trend_viewer/src/globals.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/TrendViewerException.h"

#include "ace/OS.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::FunctionEntry;

namespace TA_Base_App
{
    //
    // GraphRefreshThread
    //
    GraphRefreshThread::GraphRefreshThread(IDataUpdateObserver& dataUpdateObserver)
        : m_refreshInterval(60),
          m_isFieldUpdates(false),
          m_terminated(true),
          m_dataUpdateObserver(dataUpdateObserver)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "GraphRefreshThread");

        LOG(SourceInfo, DebugUtil::FunctionExit, "GraphRefreshThread");
    }


    //
    // ~GraphRefreshThread
    //
    GraphRefreshThread::~GraphRefreshThread()
    {
    }


    //
    // setRefreshInterval
    //
    void GraphRefreshThread::setRefreshInterval(int refreshInterval, bool isFieldUpdates)
    {
        TA_ASSERT(refreshInterval != 0, "Refresh interval == 0");

        m_refreshInterval = refreshInterval;
        m_isFieldUpdates = isFieldUpdates;
    }
    
    
    //
    // run
    //
    void GraphRefreshThread::run()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "run");

        m_terminated = false;

        time_t beginTime = calculateBeginTime();
        
        time_t timeNow;
        time_t nextTime;

        time(&timeNow);
        while(!m_terminated && timeNow < beginTime)
        {
            const int SLEEP_IN_MILLISECS = 100;
            Thread::sleep(SLEEP_IN_MILLISECS);
            time(&timeNow);
        }

        nextTime = timeNow;
        bool firstLoop = true;

        long samplePeriod = m_refreshInterval;
        if(m_isFieldUpdates == true)
        {
            samplePeriod = LIVE_FIELD_UPDATES;
        }

        while(!m_terminated)
        {
          
            //time(&timeNow);
            if(firstLoop == false)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Calling updateWithCurrentData()");
                
                // Notify GraphedItem that it is time to update the graph with the latest values
                m_dataUpdateObserver.updateWithCurrentData(timeNow, samplePeriod);
            }
            else
            {
                firstLoop = false;
            }

            const int SLEEP_IN_MILLISECS = 1000;
            const int ITERATIONS = 100;
            for(int i = 0; (i < m_refreshInterval*ITERATIONS && !m_terminated); ++i)
            {
                time(&timeNow);
                if(timeNow < nextTime)
                {
                    Thread::sleep(SLEEP_IN_MILLISECS/ITERATIONS);
                }
                else
                {
                    break;
                }
            }

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "End while: timeNow: %lu nextTime: %lu", timeNow, nextTime);

            nextTime += m_refreshInterval;

        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "run");
    }


    //
    // terminate
    //
    void GraphRefreshThread::terminate()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "terminate");

        m_terminated = true;

        LOG(SourceInfo, DebugUtil::FunctionExit, "terminate");
    }


    time_t GraphRefreshThread::calculateBeginTime()
    {
        //
        // Get the current time
        //
        struct tm currentTime;
        time_t theTime;
        time(&theTime);

        if(ACE_OS::localtime_r(&theTime, &currentTime) == NULL)
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TIMESTAMP_INVALID, "Received an invalid timestamp"));
        }

        int currentTimeHour = currentTime.tm_hour;
        int currentTimeMin = currentTime.tm_min;
        int currentTimeSec = currentTime.tm_sec;

        const int SODHour = 0;
        const int SODMinute = 0;
        const int SODSec = 0;
       
        // The only difference in the Current time and the Start of Day time would be the 
        // Hour and Minute attributes. All other attributes should be the same, since it 
        // is the same day, year, etc. Hence to find the difference, we only need to 
        // compute the difference between the Hour and Minute by converting them into
        // seconds first.
        //
        int SODSeconds = static_cast<int>(SODHour * SECONDS_IN_HOUR + SODMinute * SECONDS_IN_MINUTE + SODSec);
        int currentSeconds = static_cast<int>(currentTimeHour * SECONDS_IN_HOUR + currentTimeMin * SECONDS_IN_MINUTE + currentTimeSec);
        time_t beginProcessTime;

        //
        // Find the number of seconds that has elapsed from the 
        // Start of Day time to the Current time.
        //
        int elapsedSeconds = currentSeconds - SODSeconds;

        //
        // Divide elapsed by 'Sample Time' and truncate to determine
        // how many intervals are within this elapsed time, and which
        // interval we are up to.
        //
        int interval = static_cast <int> (elapsedSeconds / m_refreshInterval);

        //
        // Move up to the next interval and convert this back to seconds
        //
        int extraTime = (interval + 1) * m_refreshInterval;

        //
        // Calculate the time (in seconds) at which to begin processing
        //
        beginProcessTime = theTime - static_cast <long> (elapsedSeconds) + static_cast <long> (extraTime);
        
        return beginProcessTime;
    }
}

#endif // GRAPH_REFRESH_THREAD_CPP
