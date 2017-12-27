/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(AbstractSingleThreadBarrier_H)
#define AbstractSingleThreadBarrier_H

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    /**
     * This will block until it has been signalled N times.
     * 
     * This means a thread can wait for N items to happen, and only continue when they have all finished.
     * 
     * This is different to an ACE Barrier which will block ALL threads until they are all waiting.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 12:17:29 PM
     */
    template<typename Semaphore_Type>
    class AbstractSingleThreadBarrier
    {

    public:


        /**
         * Constructor
         * 
         * @param initial    The number of calls to post required to unblock this semaphore.
         */
        AbstractSingleThreadBarrier(unsigned long initial = 1) 
            : m_semaphore(0)
        {
            // initialise m_semaphore to 0
            //
            // if initial > 0 then m_count = --initial
            // otherwise  m_count = 0
    
            if ( initial > 0 )
            {
                m_count = --initial;
            }
            else
            {
                m_count = 0;
            }
        }


        virtual ~AbstractSingleThreadBarrier()
        {
            // just in case
            m_semaphore.post();
        }


        /**
         * wait (and block) for the semaphore to be signalled the required number of times, or continue if already signalled
         */
        void wait()
        {
            // wait on m_semaphore
            m_semaphore.wait();
        }


        /**
         * Test to see if the semaphore will block on wait.
         * 
         * @return false if the semaphore will block
         */
        bool tryWait()
        {
            // tryWait on m_semaphore
            return m_semaphore.tryWait();
        }


        /**
         * Signals this semaphore, it must be called N times to unblock the semaphore
         */
        void post()
        {
            {
                // acquire m_countLock
                TA_THREADGUARD( m_countLock );

                // check if the semaphore is 0 or more than 0
                // if it is not yet 0, then decrement the count
                // and return without posting
    
                if ( m_count > 0 )
                {
                    --m_count;
                    return;
                }
            }

            // the lock must not be acquired when posting
            // this can cause crashes in certain situations
            // (a thread may be woken up and delete this object while the lock is still acquired)

            // the count was already 0, so the sempahore can be posted
            m_semaphore.post();
        }

    protected:

        /**
         * The semaphore to use, initial value of 0
         */
        Semaphore_Type m_semaphore;

    private:

        /**
         * the lock to protect m_count
         */
        NonReEntrantThreadLockable m_countLock;


        /**
         * The count of how many times this needs to be signalled.
         */
        unsigned long m_count;

    };

}
#endif // !defined(AbstractSingleThreadBarrier_H)
