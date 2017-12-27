/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/UserSettingsException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#include "core\exceptions\src\UserSettingsException.h"

namespace TA_Base_Core
{

    UserSettingsException::UserSettingsException(const char* msg) 
        : TransactiveException(msg)
    {

    }	

    UserSettingsException::~UserSettingsException()
    {

    }



} // End TA_Base_Core
