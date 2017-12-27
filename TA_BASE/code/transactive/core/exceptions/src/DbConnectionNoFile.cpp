/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DbConnectionNoFile.cpp $
  *  @author:  Robert Stagg
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implements DbConnectionNoFile.
  */

#include "DbConnectionNoFile.h"

namespace TA_Base_Core
{

	DbConnectionNoFile::DbConnectionNoFile(const std::string& msg) throw() :
		  DatabaseException(msg)
	{
	}

} // Closes TA_Base_Core

