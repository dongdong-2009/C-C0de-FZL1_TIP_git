/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/timers/src/MonotonicTimer.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/08/26 16:15:14 $
  * Last modified by:  $Author: limin.zhu $
  *
  * Class for peforming time-based operations with a
  * millisecond scale.
  */

#include "core/timers/src/MonotonicTimer.h"
#ifdef WIN32
    #include <windows.h>
#else
    #include <time.h>
#endif

namespace TA_Base_Core
{
    MonotonicTimer::MonotonicTimer(bool start_)
        : m_stoped(true),
          m_start(0),
          m_elapsed(0)
    {
        if (start_)
        {
            start();
        }
    }

    void MonotonicTimer::start()
    {
        m_stoped = false;

#ifdef WIN32
        m_start = ::GetTickCount64();
#else
        ::clock_gettime(CLOCK_MONOTONIC, reinterpret_cast<timespec*>(&m_start));
#endif
    }

    void MonotonicTimer::stop()
    {
        if (!m_stoped)
        {
            m_stoped = true;
            m_elapsed += caculate();
        }
    }

    bool MonotonicTimer::stoped()
    {
        return m_stoped;
    }

    void MonotonicTimer::resume()
    {
        if (m_stoped)
        {
            m_stoped = false;
#ifdef WIN32
            m_start = ::GetTickCount64();
#else
            ::clock_gettime(CLOCK_MONOTONIC, reinterpret_cast<timespec*>(&m_start));
#endif
        }
    }

    unsigned long long MonotonicTimer::elapsed()
    {
        return elapsedImpl();
    }

    unsigned long MonotonicTimer::elapsed_s()
    {
        return static_cast<unsigned long>(elapsedImpl() / 1000);
    }

    unsigned long long MonotonicTimer::elapsedImpl()
    {
        if (m_stoped)
        {
            return m_elapsed;
        }
        else
        {
            return m_elapsed + caculate();
        }
    }

    unsigned long long MonotonicTimer::caculate()
    {
        if (0 == m_start)
        {
            return 0;
        }

#ifdef WIN32
        return ::GetTickCount64() - m_start;
#else
        timespec c;
        ::clock_gettime(CLOCK_MONOTONIC, &c);
        timespec& s = *reinterpret_cast<timespec*>(&m_start);
        return static_cast<unsigned long long>(c.tv_sec - s.tv_sec) * 1000 + (c.tv_nsec - s.tv_nsec) / 1000000;
#endif
    }

    void MonotonicTimer::clear()
    {
        m_stoped = true;
        m_start = 0;
        m_elapsed = 0;
    }
}
