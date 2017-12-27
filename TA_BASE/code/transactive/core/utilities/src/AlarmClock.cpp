/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/AlarmClock.cpp $
  * @author:   Robert Stagg
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class provides a mechanism to call a function asynchronously after a set period of time.
  */

#include "core/utilities/src/AlarmClock.h"
#include "core/utilities/src/IAlarmClockHelper.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_Core
{
    //
    // Constructor
    //
    AlarmClock::AlarmClock( IAlarmClockHelper* alarmHelper )
    	:m_helper(alarmHelper),
		m_clockRunning(false),
		m_cancelRequested(false),
		m_millisecondsToWait(0)
    {
		m_clockCondition = new Condition();
    }


    //
    // Destructor
    //
    AlarmClock::~AlarmClock()
    {
    	delete(m_clockCondition);
  	}


    //
    // start
    //
    void AlarmClock::start( const unsigned long milliseconds )
    {
    	ThreadGuard guard(m_signalLock);
		m_millisecondsToWait = milliseconds;
		
		if (m_cancelRequested==true) m_cancelRequested = false; // reset the cancel
		if ( m_clockRunning == false )
		{
			// start a new thread
			m_clockRunning = true;
            Thread::start();
		}
		else // let clock know something changed.
		{
			// time changed
			m_clockCondition->signal();
		}
    }

    //
    // run
    //
	// sleep for the request time unless something changes
    void AlarmClock::run()
    {
		// sleep until alarm goes off or something changes
		while ( m_clockCondition->timedWait(m_millisecondsToWait) )
		{
			// something changed
			if (m_cancelRequested == true)
			{
				return; // break from the while loop
				// and return to cancel function
			}
		}

        // alarm clock went off!!
		// m_clockRunning = false; // clock has now stop running		
		if (m_cancelRequested==false) m_helper->executeAlarm( this );
    }

    //
    // terminate
    //
    void AlarmClock::terminate()
    {
		if (m_clockRunning)
		{
			m_clockRunning = false;
			Thread::terminateAndWait();
		}
	}

    //
    // cancel
    //
    void AlarmClock::cancel()
    {
        ThreadGuard guard(m_signalLock);

		if (!m_clockRunning) return;

		// Cancel sign the end of the alarm and immediate release of suppression
        m_cancelRequested = true;
		m_clockCondition->signal();
		terminate();
    }

};
