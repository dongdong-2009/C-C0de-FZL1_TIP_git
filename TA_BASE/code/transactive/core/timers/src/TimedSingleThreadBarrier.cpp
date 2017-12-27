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

#include "core/timers/src/TimedSingleThreadBarrier.h"


namespace TA_Base_Core
{

    TimedSingleThreadBarrier::~TimedSingleThreadBarrier()
    {
    }


    TimedSingleThreadBarrier::TimedSingleThreadBarrier( unsigned long initial )
        : AbstractSingleThreadBarrier< TimedWaitSemaphore >( initial )
    {
    }


    bool TimedSingleThreadBarrier::timedWait(unsigned long timeoutMsec)
    {
        // Simply timed wait on the semaphore
        return m_semaphore.timedWait( timeoutMsec );
    }


}
