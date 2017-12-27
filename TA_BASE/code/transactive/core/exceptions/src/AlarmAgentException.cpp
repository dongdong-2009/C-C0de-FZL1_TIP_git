/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/AlarmAgentException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#include "AlarmAgentException.h"

namespace TA_Base_Core
{

    AlarmAgentException::AlarmAgentException() throw()
    {}

    AlarmAgentException::AlarmAgentException(const std::string& msg) 
        throw(): TransactiveException(msg)
    {}

    AlarmAgentException::~AlarmAgentException() throw()
    {}
}
