/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/send_events/src/MonitorInputThread.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(MONITOR_INPUT_THREAD_H)
#define MONITOR_INPUT_THREAD_H

#include "core/threads/src/Thread.h"

class MonitorInputThread : public TA_Base_Core::Thread
{
public:
    MonitorInputThread() : m_quit(false), m_done(false) {}
    virtual ~MonitorInputThread() {}

    void run();
    
    void terminate()
    {
        m_done = true;
    }

    bool quitCalled()
    {
        return m_quit;
    }

private:

    bool m_quit;
    bool m_done;
};

#endif // MONITOR_INPUT_THREAD_H
