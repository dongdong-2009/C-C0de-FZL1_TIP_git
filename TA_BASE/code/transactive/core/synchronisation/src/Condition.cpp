/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/Condition.cpp $
  * @author:  Rob Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class provides a wrappered thread condition implementation. This can be used to pause threads
  * until signalled (or optionally timed out waiting). The underlying omni_condition is automatically 
  * linked to an underlying omni_mutex. The linked mutex is intrinsically released before condition enters wait() 
  * or timedWait(), and intrinsically re-acquired upon completion of the wait(). This intrinsic behaviour allows 
  * client code to mutually exclude the setting of state related to the condition, with the checking of that state
  * before entering into a wait() or timedWait(). The linked mutex is wrappered here so as to be ReEntrant (can be 
  * acquired multiple times consecutively by the same Thread) and is accessible from this class through 
  * getLinkedThreadLockable().
  */

#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "omnithread.h"

namespace TA_Base_Core
{

    //
    // Default Constructor
    //
    Condition::Condition()
        : m_linkedThreadLockable( 0 ),
          m_condition( 0 )
    {
        omni_mutex* pMutex = new omni_mutex;
        m_linkedThreadLockable = new ReEntrantThreadLockable( pMutex ); // Assumes ownership memory allocated for omni_mutex so no leak.
        m_condition = new omni_condition( pMutex );
    }


    //
    // Destructor
    //
    Condition::~Condition()
    {
        delete m_condition;
        delete m_linkedThreadLockable;
    }


    //
    // wait
    //
    void Condition::wait()
    {
        ThreadGuard guard( *m_linkedThreadLockable );

        // Backup the re-entrant acquisition state so we can
        // restore on other side of wait.
        m_linkedThreadLockable->backupReEntrantAcquisitionState();
        
        // Note: Mutex intrinsically released.
        m_condition->wait();
        // Note: Mutex intrinisically acquired.

        // Now that mutex has been reacquired, restore the re-entrant
        // acquisition state.
        m_linkedThreadLockable->restoreReEntrantAcquisitionState();
    }


    //
    // timedWait
    //
    int Condition::timedWait(unsigned long milliSeconds)
    {
        ThreadGuard guard( *m_linkedThreadLockable );

        // Backup the re-entrant acquisition state so we can
        // restore on other side of wait.
        m_linkedThreadLockable->backupReEntrantAcquisitionState();

        // convert from millisecs to secs, nanosecs AND
        // convert to absolute time from relative time from now.
        unsigned long absoluteSecs = 0;
        unsigned long absoluteNanoSecs = 0;
        omni_thread::get_time(&absoluteSecs, &absoluteNanoSecs, 
                                milliSeconds / 1000, (milliSeconds % 1000) * 1000000);

        // now call omni implementation
        
        // Note: Mutex intrinsically released.
        int result = m_condition->timedwait(absoluteSecs, absoluteNanoSecs);
        // Note: Mutex intrinisically acquired.

        // Now that mutex has been reacquired, restore the re-entrant
        // acquisition state.
        m_linkedThreadLockable->restoreReEntrantAcquisitionState();

        return result;
    }


    //
    // signal
    //
    void Condition::signal()
    {
        ThreadGuard guard( *m_linkedThreadLockable );

        m_condition->signal();
    }


    //
    // broadcast
    //
    void Condition::broadcast()
    {
        ThreadGuard guard( *m_linkedThreadLockable );

        m_condition->broadcast();
    }


    //
    // getLinkedThreadLockable
    //
    IThreadLockable& Condition::getLinkedThreadLockable()
    { 
        return *m_linkedThreadLockable; 
    }
}
