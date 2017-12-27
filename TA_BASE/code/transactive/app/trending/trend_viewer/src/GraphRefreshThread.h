#if !defined(GRAPH_REFRESH_THREAD_H)
#define GRAPH_REFRESH_THREAD_H

/**
  * GraphRefreshThread.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/GraphRefreshThread.h $
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "core/threads/src/Thread.h"


namespace TA_Base_App
{
    //
    // Forward declaration
    //
    class IDataUpdateObserver;

    //
    // GraphRefreshThread
    //
    class GraphRefreshThread : public TA_Base_Core::Thread
    {
    public:

        /**
		  * GraphRefreshThread
		  * 
		  * Constructor.
		  *
		  */
        GraphRefreshThread(IDataUpdateObserver& dataUpdateObserver);
        
        /**
		  * ~GraphRefreshThread
		  * 
		  * Destructor.
		  *
		  */
        virtual ~GraphRefreshThread();

        /**
		  * run
		  * 
		  * Calls a method on the main dialog at a regular interval to update the graph.
          *
		  */
        virtual void run();

        /**
		  * terminate
		  * 
		  * Terminate the thread.
		  *
		  */
        virtual void terminate();

        /**
		  * setRefreshInterval
		  * 
		  * Set the refresh interval.
          *
          * @param refreshInterval - sample period
		  * @param isFieldUpdates - indicates whether we're listening to live field udpates
          *
          * Pre: refreshInterval does not equal zero.
		  */
        void setRefreshInterval(int refreshInterval, bool isFieldUpdates);

    private:

        GraphRefreshThread(const GraphRefreshThread& theThread);
	    GraphRefreshThread& operator=(const GraphRefreshThread&);

        time_t calculateBeginTime();

    private:

        int m_refreshInterval;                      // Refresh sample (in seconds). This will be equivalent to the sample
                                                    // period configured in the Trend Properties Dlg, unless the sample
                                                    // period has been set to 0 (live mode), in which case this variable
                                                    // will be set to 60.

        bool m_isFieldUpdates;                      // Indicates whether the live mode is for live updates or sample udpates.
        bool volatile m_terminated;                 // Has thread been terminated
        IDataUpdateObserver& m_dataUpdateObserver;  // Object to notify when it is time to update the graph
    };
}


#endif // !defined(GRAPH_REFRESH_THREAD_H)





