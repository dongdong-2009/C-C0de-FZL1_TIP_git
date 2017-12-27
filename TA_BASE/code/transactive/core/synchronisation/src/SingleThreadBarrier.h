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
#if !defined(SingleThreadBarrier_H)
#define SingleThreadBarrier_H

#include "core/synchronisation/src/AbstractSingleThreadBarrier.h"
#include "core/synchronisation/src/Semaphore.h"

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
     * @created 01-Apr-2008 12:37:53 PM
     */
    class SingleThreadBarrier : public AbstractSingleThreadBarrier< Semaphore >
    {

    public:

        /**
         * Constructor
         *
         * @param initial    The number of calls to post required to unblock this semaphore.
         */
        SingleThreadBarrier(unsigned long initial = 1);

        /**
         * Destructor
         */
        virtual ~SingleThreadBarrier();

    private:

        SingleThreadBarrier( const SingleThreadBarrier& );
        void operator=( const SingleThreadBarrier& );

    };

}
#endif // !defined(SingleThreadBarrier_H)
