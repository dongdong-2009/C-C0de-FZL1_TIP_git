#if !defined(FTPEXCEPTION)
#define FTPEXCEPTION

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/ftp/src/FTPException.h $
  * @author:  J Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: grace.koh $
  * 
  * Exception class thrown by the FTP Interface
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
	class SFTPException : public TA_Base_Core::TransactiveException  
	{
    public:
   		SFTPException() throw();
        SFTPException(const std::string& msg) throw();
        virtual ~SFTPException() throw();
	};    
}

#endif // !defined(FTPEXCEPTION)
