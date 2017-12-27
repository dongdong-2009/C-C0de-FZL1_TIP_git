/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/AlarmCacheCapacityExceededException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#include "AlarmCacheCapacityExceededException.h"

namespace TA_Base_Core
{

    AlarmCacheCapacityExceededException::AlarmCacheCapacityExceededException() throw()
    {}

    AlarmCacheCapacityExceededException::AlarmCacheCapacityExceededException(const std::string& msg) 
        throw(): TransactiveException(msg)
    {}

    AlarmCacheCapacityExceededException::~AlarmCacheCapacityExceededException() throw()
    {}
}
