/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ProcessAlreadyRunningException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#if !defined(PROCESSALREADYRUNNINGEXCEPTION)
#define PROCESSALREADYRUNNINGEXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
		class ProcessAlreadyRunningException : public TA_Base_Core::TransactiveException  
	{
    public:
        ProcessAlreadyRunningException() throw();
        ProcessAlreadyRunningException(const std::string& msg) throw();
        virtual ~ProcessAlreadyRunningException() throw();
    };    
}

#endif // !defined(PROCESSALREADYRUNNINGEXCEPTION)
