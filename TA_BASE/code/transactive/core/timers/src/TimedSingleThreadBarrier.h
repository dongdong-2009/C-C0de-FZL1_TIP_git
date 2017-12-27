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
#if !defined(TimedSingleThreadBarrier_H)
#define TimedSingleThreadBarrier_H

#include "core/synchronisation/src/AbstractSingleThreadBarrier.h"
#include "core/timers/src/TimedWaitSemaphore.h"

namespace TA_Base_Core
{
    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 12:24:30 PM
     */
    class TimedSingleThreadBarrier : public AbstractSingleThreadBarrier< TimedWaitSemaphore >
    {

    public:

        /**
         * Constructor
         * 
         * @param initial    The number of calls to post required to unblock this semaphore.
         */
        TimedSingleThreadBarrier(unsigned long initial = 1);
        

         /**
          * Destructor
          */
        virtual ~TimedSingleThreadBarrier();


        /**
         * Waits for the barrier to unblock, or times out in the process
         * 
         * @return true if the barrier unblocked, false if it timed out
         * 
         * @param timeoutMsec
         */
        bool timedWait(unsigned long timeoutMsec);

    };

}
#endif // !defined(TimedSingleThreadBarrier_H)
