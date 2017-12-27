/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/AlarmListCtrlException.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is used by bus/alarm/alarm_list_control to determine errors. When specified the msg field
  * may hold appropriate error messages to be displayed to the user.
  */

#include "core/exceptions/src/AlarmListCtrlException.h"

namespace TA_Base_Core
{
    AlarmListCtrlException::AlarmListCtrlException() throw()
    {
    }

    AlarmListCtrlException::AlarmListCtrlException(const std::string& msg) 
        throw(): TransactiveException(msg)
    {
    }

    AlarmListCtrlException::~AlarmListCtrlException() throw()
    {
    }
}
