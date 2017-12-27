#ifndef ACTIVEPLANMESSAGEEXCEPTION_H_INCLUDED
#define ACTIVEPLANMESSAGEEXCEPTION_H_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_App
{
	class ActivePlanMessageException : public TA_Base_Core::TransactiveException
	{
	public:
		ActivePlanMessageException() throw();
		ActivePlanMessageException(const std::string& msg) throw();
		virtual ~ActivePlanMessageException() throw();
	};
}; // TA_Base_App

#endif // ACTIVEPLANMESSAGEEXCEPTION_H_INCLUDED
