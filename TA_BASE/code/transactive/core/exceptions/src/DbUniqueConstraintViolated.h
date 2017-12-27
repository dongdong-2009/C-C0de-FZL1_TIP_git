/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DbUniqueConstraintViolated.h $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Raised by the db code when a unique constraint is violated.
  */


#if !defined(DBUNIQUECONSTRAINTVIOLATED_H_INCLUDED)
#define DBUNIQUECONSTRAINTVIOLATED_H_INCLUDED

#include "DatabaseException.h"
#include <string>

namespace TA_Base_Core
{
	class DbUniqueConstraintViolated : public DatabaseException  
	{
	public:

		DbUniqueConstraintViolated(const std::string& msg) throw();
	};
} // Closes TA_Base_Core

#endif // !defined(DBUNIQUECONSTRAINTVIOLATED_H_INCLUDED)

