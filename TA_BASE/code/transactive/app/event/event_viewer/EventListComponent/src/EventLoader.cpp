/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/EventListComponent/src/EventLoader.cpp $
 * @author:  Nick Jardine
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * EventLoader implements the Thread. It is a thread that loops for the life of the Event List, and is responsible
 * for all database loading.
 */

#include <sstream>

#include "app/event/event_viewer/EventListComponent/src/stdafx.h"
#include "app/event/event_viewer/EventListComponent/src/EventLoader.h"
#include "app/event/event_viewer/EventListComponent/src/EventListComponent.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::DebugUtil;
const int EventLoader::SLEEP_TIME = 200;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EventLoader::EventLoader(Filter* filter) : 
m_filter(filter), m_restLoops(0), m_isTerminated(false), m_firstLoadComplete(false),
m_taskID(1),m_actionRefresh(true),m_currentAction(E_NOACTION),m_refreshsec(20)
{
}

EventLoader::~EventLoader()
{

}

void EventLoader::run()
{ 
	//changxing  TD14760
    //unsigned long loopCount = 0;
	CTime startTime = CTime::GetCurrentTime(); 
	CTime endTime;
	CTime endrefreshTime;
	CTimeSpan elapsedTime;
	CTimeSpan elapref;
	unsigned long refsec=0;
	unsigned long elaps;
	unsigned long refreshsec;
	unsigned long currentTaskID =0;
    while (!m_isTerminated)
    {
		endTime = CTime::GetCurrentTime();
		elapsedTime = endTime - startTime;
		elaps = elapsedTime.GetTotalSeconds();
		
		refreshsec = (m_refreshsec >= refsec ) ? (m_refreshsec) : 0;
		
// 		if ( elaps >= (refreshsec) ) // maybe should be (m_refreshsec - 5), for refresh takes some seconds.
//         {
//             // To execute a refresh just add its action to the queue.
//             setRefresh();
// 
//             // Also reset the loop count, so the refresh period starts again.
//             //loopCount = 0;
//         }

        // Execute the required actions.
        bool load, refresh;
        {
            TA_Base_Core::ThreadGuard queueGuard( m_actionGuard );
			if(currentTaskID!=m_taskID)  //Notice: should use != instead of < , to handle the value overflow situation
			{
				load = true;
				currentTaskID = m_taskID;
			}else
			{
				load = false;
			}
            refresh = m_actionRefresh;
        }
        if ( load )
        {
            
			loadList(); 

        }
        else if ( refresh )
        {
			//changxing TD14760
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "elaps-%d:refresh-%d",elaps,refreshsec);
			startTime = CTime::GetCurrentTime();
		 
			refreshList();
		 
			endrefreshTime = CTime::GetCurrentTime();
			elapref = endrefreshTime - startTime;
			refsec = elapref.GetTotalSeconds();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "the refresh costs %d seconds",refsec);

        }

		int i=0;
		for (i=0;i<m_refreshsec*5 && !m_isTerminated ;i++)
		{
			Thread::sleep(SLEEP_TIME);
		}
    }
}

void EventLoader::terminate()
{
    m_isTerminated = true;
}

void EventLoader::setRefresh()
{
    TA_Base_Core::ThreadGuard queueGuard( m_actionGuard );
    m_actionRefresh = true;
}

void EventLoader::reLoad()
{

	TA_Base_Core::ThreadGuard queueGuard( m_actionGuard );
	m_taskID++;	 
	// If another load is already in progress stop it.
	stopCurrentLoad();
}

void EventLoader::setLoad(bool enableRefresh)
{
    TA_Base_Core::ThreadGuard queueGuard( m_actionGuard );
    //m_actionLoad = true;
	m_taskID++;
	m_actionRefresh=enableRefresh;
    // If another load is already in progress stop it.
    stopCurrentLoad();
}

unsigned long EventLoader::setRefreshPeriod(const unsigned long refreshTimeMs)
{
    unsigned long oldTime = m_restLoops*SLEEP_TIME;
    m_restLoops = refreshTimeMs/SLEEP_TIME;

	if ( refreshTimeMs < 1000 ) m_refreshsec = 1;
	else 	m_refreshsec = refreshTimeMs/1000 ;
    return oldTime;
}

void EventLoader::loadList()
{
    // We're currently loading.
    setCurrentAction( E_LOAD );
	try
	{
		// Load the list.
        m_filter->loadList();
	}             
	catch (...)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "loadList error");
	}

    // Flag that at least one load has taken place. It doen't hurt to set this every load, because setting true to 
    // true makes no difference
    m_firstLoadComplete = true;

    // We're currently doing nothing.
    setCurrentAction( E_NOACTION );
}

void EventLoader::refreshList()
{
    // Don't want to do a refresh if there hasn't been a load (this is a protection for the window between
    // the creation of this thread and the first load taking place)
    if (m_firstLoadComplete)
    {
        // We're currently refreshing.
        setCurrentAction( E_REFRESH );

        // Refresh the list.
		try
		{
			m_filter->refreshList();
		}             
		catch (...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "loadList error");
		}

        // We're currently doing nothing.
        setCurrentAction( E_NOACTION );
    }
}


void EventLoader::changeFilter(Filter* filter)
{
    m_filter = filter;
}

bool EventLoader::isCurrentlyActive()
{
    TA_Base_Core::ThreadGuard actionGuard(m_actionGuard);

    return ( E_NOACTION != m_currentAction );
}

bool EventLoader::isCurrentlyLoading()
{
    TA_Base_Core::ThreadGuard actionGuard(m_actionGuard);

    return ( E_LOAD == m_currentAction );
}

bool EventLoader::isCurrentlyRefreshing()
{
    TA_Base_Core::ThreadGuard actionGuard(m_actionGuard);

    return ( E_REFRESH == m_currentAction );
}

void EventLoader::stopCurrentLoad()
{
    TA_Base_Core::ThreadGuard actionGuard(m_actionGuard);

    if( E_LOAD == m_currentAction )
    {
        m_filter->stopCurrentListDataLoad();
    }
}

void EventLoader::setCurrentAction( ELoaderAction action )
{
    TA_Base_Core::ThreadGuard actionGuard(m_actionGuard);

    m_currentAction = action;
}

