/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DatabaseException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// DatabaseException.cpp: implementation of the DatabaseException class.
//
//////////////////////////////////////////////////////////////////////

#include "DatabaseException.h"

namespace TA_Base_Core
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	DatabaseException::DatabaseException(const std::string& msg) throw() :
		  TransactiveException(msg)
	{
	}

} // Closes TA_Base_Core

