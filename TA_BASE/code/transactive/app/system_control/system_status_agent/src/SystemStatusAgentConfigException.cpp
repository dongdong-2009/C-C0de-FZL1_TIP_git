/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/SystemStatusAgentConfigException.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * SystemStatusAgentConfigException class implimentation. See header file for class descrption
  */

#include "SystemStatusAgentConfigException.h"

namespace TA_Base_App
{
	SystemStatusAgentConfigException::SystemStatusAgentConfigException(const std::string& errorMessage) throw()
		: TA_Base_Core::TransactiveException(errorMessage)
	{

	}

	SystemStatusAgentConfigException::~SystemStatusAgentConfigException() throw()
	{

	}
}
