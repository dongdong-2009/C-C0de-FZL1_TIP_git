/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DbConnectionNoFile.h $
  * @author Robert Stagg
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Raised by the db code when the file containing the
  * database connection strings can not be opened
  */


#if !defined(DBCONNECTIONNOFILE_H_INCLUDED)
#define DBCONNECTIONNOFILE_H_INCLUDED

#include "DatabaseException.h"
#include <string>

namespace TA_Base_Core
{
	class DbConnectionNoFile : public DatabaseException  
	{
	public:

		DbConnectionNoFile(const std::string& msg) throw();
	};
} // Closes TA_Base_Core

#endif // !defined(DBCONNECTIONNOFILE_H_INCLUDED)

