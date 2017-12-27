#ifndef CONDITION_H_INCLUDED
#define CONDITION_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/Condition.h $
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

class omni_mutex;
class omni_condition;

namespace TA_Base_Core
{
    class IThreadLockable;

    // Forward declared as this class needs pointer to this type (opposed to interface above)
    // to use its private backup/restore methods (this class can do so as is declared as friend).
    class ReEntrantThreadLockable;
}

namespace TA_Base_Core
{
    class Condition
    {
    public:

        /** 
          * Condition
          *
          * Construct a new condition object
          *
          */
        Condition();


        /** 
          * ~Condition
          * 
          * condition destructor
          */
        virtual ~Condition();


        /** 
          * wait
          *
          * wait for the condition variable to be signalled, a signal may wake up more
          * than one thread.
          *
          */
        void wait();


        /** 
          * timedwait
          *
          * Wait for signal for maximum number of milliseconds
          *
          * @param unsigned long milliseconds (number milliseconds to wait for)
          *
          * @return int (1 if successfully signalled | 0 if time expired)
          */
        int timedWait(unsigned long milliSeconds);


        /** 
          * signal
          *
          * if one or more threads have called wait(), signal wakes up at least
          * one of them, possibly more.
          *
          */
        void signal();


        /** 
          * signal
          *
          * broadcast is like signal but wakes all threads which have called
          * wait().
          *
          */
        void broadcast();


        /** 
          * getLinkedThreadLockable
          *
          * Returns reference to IThreadLockable linked to this condition. The IThreadLockable
          * shall be wrappered so as to be "ReEntrant" (can be acquired multiple times consecutively by the same
          * Thread). The Linked IThreadLockable is intrinsically released before condition enters wait() or timedWait(),
          * and intrinsically re-acquired upon completion of the wait(). 
          *
          * @return IThreadLockable& - reference to IThreadLockable associated linked to this condition.
          */
        IThreadLockable& getLinkedThreadLockable();

    private:   
        
        //
        // Disable the default copy and assignment constructors as shouldn't used.
        //
        Condition( const Condition& );
        Condition& operator=( const Condition& );

        
        /**
          * The underlying condition implementation
          */
        ReEntrantThreadLockable* m_linkedThreadLockable;
        omni_condition* m_condition;
    };
}

#endif // CONDITION_H_INCLUDED

