/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/Timer.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Utility class to manage the Win32 SDK Timer functions and callback. To use, implement
  * the ITimerSubscriber pure virtual class; subscribe to object of this type;
  * then call "start(unsigned int timercycle = 50)" on the object of this type.
  */

#include "StdAfx.h"
#include <map>
#include "core/utilities/src/TAAssert.h"
#include "Timer.h"
#include "ITimerSubscriber.h"


namespace TA_Base_App
{
    typedef std::map<unsigned long, Timer *> TimerMap;
}

using namespace TA_Base_App;


static const unsigned long UNDEFINED_TIMER_ID = 0L;

static TimerMap timerMap;

static void CALLBACK TimerCallback(HWND targetWnd, UINT messageId, UINT timerId, DWORD sysTime)
{
    TimerMap::iterator iter = timerMap.find(timerId);

    if (iter != timerMap.end())
    {
        Timer *timer = iter->second;

        if (timer != NULL)
        {
            timer->onTimeout();
        }
    }
}


Timer::Timer() : m_subscriber(NULL), m_timerId(UNDEFINED_TIMER_ID)
{
    FUNCTION_ENTRY( "Timer" );
    FUNCTION_EXIT;
}


Timer::~Timer(void)
{
    FUNCTION_ENTRY( "~Timer" );

    stop();
    unsubscribe();

    FUNCTION_EXIT;
}


void Timer::subscribe(ITimerSubscriber* subscriber)
{
    FUNCTION_ENTRY( "subscribe" );

    stop();
    m_subscriber = subscriber;

    FUNCTION_EXIT;
}


void Timer::unsubscribe()
{
    FUNCTION_ENTRY( "unsubscribe" );

    m_subscriber = NULL;

    FUNCTION_EXIT;
}


void Timer::start(unsigned int timerCycle)
{
    FUNCTION_ENTRY( "start" );

    stop();

    m_timerId = ::SetTimer(NULL, 0, timerCycle, (TIMERPROC) TimerCallback);

    if (m_timerId > UNDEFINED_TIMER_ID)
    {
        timerMap.insert(std::make_pair(m_timerId, this));
    }

    FUNCTION_EXIT;
}


void Timer::stop()
{
    FUNCTION_ENTRY( "stop" );

    if (m_timerId > UNDEFINED_TIMER_ID)
    {
        ::KillTimer(NULL, m_timerId);

        timerMap.erase(m_timerId);

        m_timerId = UNDEFINED_TIMER_ID;
    }

    FUNCTION_EXIT;
}


void Timer::onTimeout()
{
    FUNCTION_ENTRY( "onTimeout" );

    if (m_timerId > UNDEFINED_TIMER_ID && m_subscriber != NULL)
    {
        m_subscriber->timerExpired(*this);
    }

    FUNCTION_EXIT;
}


