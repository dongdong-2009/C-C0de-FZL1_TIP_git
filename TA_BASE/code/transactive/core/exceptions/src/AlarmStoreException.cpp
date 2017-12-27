/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/AlarmStoreException.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#include "AlarmStoreException.h"

namespace TA_Base_Core
{

    AlarmStoreException::AlarmStoreException() throw()
    {}

    AlarmStoreException::AlarmStoreException(const std::string& msg) 
        throw(): TransactiveException(msg)
    {}

    AlarmStoreException::~AlarmStoreException() throw()
    {}
}
