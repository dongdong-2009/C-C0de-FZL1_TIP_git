/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/timers/src/TimerUtil.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2015/02/11 16:09:39 $
  * Last modified by:	$Author: Ouyang $
  *
  * Description:
  *
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER


#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"

#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/TimerUtil.h"

namespace
{
    const unsigned long DEACTIVATE_DELAY( 100 );
    const unsigned long MAX_DEACTIVATE_ATTEMPT( 10 );
}

namespace TA_Base_Core
{

	//
	// TimerUtil
	//
	TimerUtil::TimerUtil()
	{
		// Activate the active queue.
        m_activeQueue.activate();
        m_invalidated = false;
	};

	//
	// ~TimerUtil
	//
	TimerUtil::~TimerUtil()
	{
		unsigned long attemptCount = 0;

        // Deactivate the active queue and wait for its thread to terminate.
        // It is done manually to overcome an observed problem where using the
        // queues wait method would block indefinitely as the condition did not
        // seem to be signalled to allow the thread to terminate.
        do
        {
            m_activeQueue.deactivate();
            Thread::sleep( DEACTIVATE_DELAY );
        } 
		while ( ( 0 < m_activeQueue.thr_count() ) && ( MAX_DEACTIVATE_ATTEMPT >= ++attemptCount ) );
	}

	//
	// scheduleTimer
	//
    long TimerUtil::scheduleTimer(ITimeoutCallback * user, unsigned long timeoutPeriodMSeconds, bool singleShot, void* userData)
    {
		FUNCTION_ENTRY("scheduleTimer");

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[temp] TimerUtil::scheduleTimer begin - user=%p, timeoutPeriodMSeconds=%d, singleShot=%d, userData=%p", user, timeoutPeriodMSeconds, singleShot, userData);

        if ( true == m_invalidated )
        {
            FUNCTION_EXIT;
            return -1;
        }

		if (timeoutPeriodMSeconds == 0 || user == 0)
		{
			FUNCTION_EXIT;
			return -1;
		}

        // schedule a new timer
        TimerItem* userDefinedData = new TimerItem();
        userDefinedData->user = user;
        userDefinedData->singleShot = singleShot;
        userDefinedData->timerId = -1;
        userDefinedData->userData = userData;

		long timeoutSecs = (long)(timeoutPeriodMSeconds / 1000);
		long timeoutUsecs = (long)(timeoutPeriodMSeconds % 1000)*1000;
		
		ACE_Time_Value interval = ACE_Time_Value (timeoutSecs, timeoutUsecs);
        ACE_Time_Value firstRun = ACE_OS::gettimeofday () + interval;
		
        if (singleShot)
        {
            interval = ACE_Time_Value::zero;
        }

        long timerId =  // <- This is the ID you use to cancel the timer
			m_activeQueue.schedule( this, 
			reinterpret_cast<void*>(userDefinedData),  
			firstRun,
			interval);

		if (timerId != -1)
		{
            userDefinedData->timerId = timerId;

			ThreadGuard guard(m_mapLock);

            // add it to the timer map as well
            m_timerMap.insert( std::pair<ITimeoutCallback*, long>(user,timerId) );
		}
		else
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
				 "Can not schedule timer for %d ms", timeoutPeriodMSeconds);

            delete userDefinedData;
            userDefinedData = NULL;
		}

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[temp] TimerUtil::scheduleTimer end - user=%p, userDefinedData=%p, timerId=%d, userData=%p", user, userDefinedData, timerId, userData);
		FUNCTION_EXIT;
        return timerId;
    }

	//
	// cancelTimer
	//
    std::vector<void*> TimerUtil::cancelTimer(ITimeoutCallback * user)
    {
		FUNCTION_ENTRY("cancelTimer");
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[temp] TimerUtil::cancelTimer - user=%p", user);

		std::vector<void*> userData;

        if ( true == m_invalidated )
        {
            FUNCTION_EXIT;
            return userData;
        }

		if (user == 0)
		{
			FUNCTION_EXIT;
			return userData;
		}

        std::vector<long> timerIds;

		// get the entries in the map
		// and remove them
        {
			ThreadGuard guard(m_mapLock);

            TimerMap::iterator iter = m_timerMap.find(user);
            if (iter != m_timerMap.end())
            {
                // get the last enty for this observer
                TimerMap::iterator lastIter = m_timerMap.upper_bound(user);
                
                // get all timer ids for this observer
                while(iter != lastIter)
                {
                    timerIds.push_back(iter->second);
                    ++iter;
                }
            }

            // erase all elements for this observer
            m_timerMap.erase( user );
		}
		
	
        for (std::vector<long>::iterator idIter = timerIds.begin(); idIter != timerIds.end(); ++idIter )
        {
			// cancel the timer
			const void* constArg=NULL;

			//m_activeQueue.timer_queue().cancel( *idIter, &constArg ); // ACE 5.5 and before
			m_activeQueue.timer_queue()->cancel( *idIter, &constArg ); // ACE 5.6 and later

			// const arg now contains the original pointer
			void* arg = const_cast<void*>( constArg );
			TimerItem* userDefinedData = reinterpret_cast<TimerItem*>(arg);

			if (NULL != userDefinedData && NULL != userDefinedData->userData)
            {
                userData.push_back(userDefinedData->userData);
            }

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[temp] TimerUtil::cancelTimer - user=%p, userDefinedData=%p, timerId=%d", user, userDefinedData, *idIter);

			// delete it - no longer used
			delete userDefinedData;
			userDefinedData = NULL;
        }

		FUNCTION_EXIT;
		return userData;
    }


   void* TimerUtil::cancelTimer(ITimeoutCallback * user, long timerId)
    {
		FUNCTION_ENTRY("cancelTimer");

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[temp] TimerUtil::cancelTimer - user=%p, timerId=%d", user, timerId);

		void* userData = NULL;

        if ( true == m_invalidated )
        {
            FUNCTION_EXIT;
            return userData;
        }

		if (timerId < 0)
		{
			FUNCTION_EXIT;
			return userData;
		}

		// get the entry in the map
		// and remove it
        {
			ThreadGuard guard(m_mapLock);

            TimerMap::iterator iter = m_timerMap.find(user);
            if (iter != m_timerMap.end())
            {
                // get the last entry for this observer
                TimerMap::iterator lastIter = m_timerMap.upper_bound(user);
                while(iter != lastIter)
                {
                    if (iter->second == timerId)
                    {
                        m_timerMap.erase(iter++);
                    }
                    else
                    {
                        ++iter;
                    }
                }
            }
		}

		// cancel the timer
		const void* constArg=NULL;   	    

		//m_activeQueue.timer_queue().cancel( timerId, &constArg ); // ACE 5.5 and before
		m_activeQueue.timer_queue()->cancel( timerId, &constArg ); // ACE 5.6 and later

        if (constArg != NULL)
        {
	        // const arg now contains the original pointer
	        void* arg = const_cast<void*>( constArg );
	        TimerItem* userDefinedData = reinterpret_cast<TimerItem*>(arg);
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[temp] TimerUtil::cancelTimer - user=%p, timerId=%d, userDefinedData=%p", user, timerId, userDefinedData);

            if (NULL != userDefinedData)
            {
			    userData = userDefinedData->userData;

	            // delete it - no longer used
	            delete userDefinedData;
	            userDefinedData = NULL;
            }
        }

		FUNCTION_EXIT;
		return userData;
    }

	//
	// handle_timeout
	//
	int TimerUtil::handle_timeout ( const ACE_Time_Value &tv, const void *constArg )
    {
		FUNCTION_ENTRY("handle_timeout");

        if ( true == m_invalidated )
        {
            return 0;
        }

		if (constArg == NULL)
		{
			return 0;
		}

        ITimeoutCallback * user = NULL;

        // extract the data
        void* arg = const_cast<void*>( constArg );
        TimerItem* userDefinedData = reinterpret_cast<TimerItem*>(arg);
		user = userDefinedData->user;
        long timerId = userDefinedData->timerId;
        void* userData = userDefinedData->userData;

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[temp] TimerUtil::handle_timeout - userDefinedData=%p, user=%p, timerId=%d, userData=%p", userDefinedData, user, timerId, userData);

        // only delete the data if the timer is single shot
        if (userDefinedData->singleShot)
        {
			// remove the expired timer from the map
			// clean up the structure

			ThreadGuard guard(m_mapLock);
            
			// remove the entry in the map
            TimerMap::iterator iter = m_timerMap.find(user);
            if (iter != m_timerMap.end())
            {
                // get the last enty for this observer
                TimerMap::iterator lastIter = m_timerMap.upper_bound(user);
                while(iter != lastIter)
                {
                    if (iter->second == timerId)
                    {
                        // erase the timer id that just expired
                        m_timerMap.erase(iter++);
                    }
                    else
                    {
                        ++iter;
                    }
                }
            }

            delete userDefinedData;
            userDefinedData = NULL;
        }

        // call the handler
        TA_ASSERT(user != NULL, "How did the timer get scheduled with a NULL observer");
        
        try
        {
    		user->timerExpired( timerId, userData );
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "Leaked out of timerExpired");
        }

		FUNCTION_EXIT;
        return 0;
    }

    void TimerUtil::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        if ( false == m_invalidated )
        {
            m_invalidated = true;
            
            ThreadGuard guard(m_mapLock);

            for ( TimerMap::iterator it = m_timerMap.begin(); it != m_timerMap.end(); ++it )
            {
                const void* constArg=NULL;
                m_activeQueue.timer_queue()->cancel( it->second, &constArg );
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[temp] TimerUtil::invalidate - constArg=%p, user=%p, timerId=%d", constArg, it->first, it->second);
                delete reinterpret_cast<TimerItem*>( const_cast<void*>( constArg ) );
            }
        }

        FUNCTION_EXIT;
    }

}
