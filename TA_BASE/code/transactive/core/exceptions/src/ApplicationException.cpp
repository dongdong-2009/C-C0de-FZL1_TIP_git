/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ApplicationException.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * the RunningApplication class in the Control Station. It is
  * derived from TransactiveException.
  * This exception class is used when errors are encountered 
  * when attempting to perform actions on a process such as launch
  * or terminate.
  */

#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

	ApplicationException::ApplicationException(const std::string& executable, const std::string& msg)
		throw(): TransactiveException(msg)
	{
            using TA_Base_Core::DebugUtil;

        std::string error("Executable is ");
        error += executable;
        LOG(SourceInfo,DebugUtil::ExceptionDetails, "ApplicationException", error.c_str());
	}


	ApplicationException::ApplicationException(const std::string& msg)
		throw(): TransactiveException(msg)
	{
    }

    ApplicationException::~ApplicationException() throw()
	{
	}

} // Closes TA_Base_Core
