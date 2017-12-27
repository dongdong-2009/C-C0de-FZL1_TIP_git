/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/timers/src/MonotonicTimer.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: limin.zhu $
  *
  * Class for peforming time-based operations with a
  * millisecond scale.
  *
  */

#ifndef MONOTONICTIMER_H
#define MONOTONICTIMER_H

namespace TA_Base_Core
{
    class MonotonicTimer
    {
    public:

        MonotonicTimer(bool start_ = true);
        void start();
        void stop();
        void resume();
        void clear();
        bool stoped();
        unsigned long long elapsed();      // milliseconds
        unsigned long elapsed_s();         // seconds

    private:

        unsigned long long elapsedImpl();
        unsigned long long caculate();

    private:

        bool m_stoped;
        unsigned long long m_start;
        unsigned long long m_elapsed;
    };
}

#endif
