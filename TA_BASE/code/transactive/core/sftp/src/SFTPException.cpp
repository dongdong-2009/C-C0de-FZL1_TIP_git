/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/ftp/src/FTPException.cpp $
  * @author:  J. Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: grace.koh $
  * 
  * Exception class thrown by the FTP Interface
  *
  */

#include "core/sftp/src/SFTPException.h"

namespace TA_Base_Core
{
	SFTPException::SFTPException() throw() {}

    SFTPException::SFTPException(const std::string& msg) 
            throw(): TransactiveException(msg) {}

    SFTPException::~SFTPException() throw() {}
}
