/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/send_events/src/MonitorInputThread.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include <string>
#include <iostream>
#include "app/trending/trend_viewer/send_events/src/MonitorInputThread.h"

void MonitorInputThread::run()
{
    std::string input("");
    while(!m_done)
    {
        std::cin >> input;
        if(input == "quit")
        {
            m_quit = true;
            m_done = true;
        }
    }
}