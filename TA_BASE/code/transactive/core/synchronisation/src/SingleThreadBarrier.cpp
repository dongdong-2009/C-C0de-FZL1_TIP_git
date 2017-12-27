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

#include "core/synchronisation/src/SingleThreadBarrier.h"


namespace TA_Base_Core
{

    SingleThreadBarrier::SingleThreadBarrier( unsigned long initial )
        : AbstractSingleThreadBarrier< Semaphore >( initial )
    {
//        FUNCTION_ENTRY( "SingleThreadBarrier" );
//        FUNCTION_EXIT;
    }


    SingleThreadBarrier::~SingleThreadBarrier()
    {
//        FUNCTION_ENTRY( "~SingleThreadBarrier" );
//        FUNCTION_EXIT;
    }


}
