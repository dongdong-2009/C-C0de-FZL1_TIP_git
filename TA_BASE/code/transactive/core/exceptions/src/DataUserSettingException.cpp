/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DataUserSettingException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#include "core\exceptions\src\DataUserSettingException.h"

namespace TA_Base_Core
{

    DataUserSettingException::DataUserSettingException(const char* msg) 
        : m_failType(GENERIC), TransactiveException(msg)
    {

    }	

    DataUserSettingException::DataUserSettingException(const char* msg, const Type failType)
        : m_failType(failType), TransactiveException(msg)
    {

    }
    DataUserSettingException::~DataUserSettingException()
    {

    }



} // End TA_Base_Core
