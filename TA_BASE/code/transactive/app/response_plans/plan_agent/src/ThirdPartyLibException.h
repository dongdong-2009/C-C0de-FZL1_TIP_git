#ifndef THIRDPARTYLIBEXCEPTION_H_INCLUDED
#define THIRDPARTYLIBEXCEPTION_H_INCLUDED

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
	class ThirdPartyLibException : public TA_Base_Core::TransactiveException
	{
	public:
		ThirdPartyLibException() throw();
		ThirdPartyLibException(const std::string& msg) throw();
		virtual ~ThirdPartyLibException() throw();
	};
}; // TA_Base_App

#endif // THIRDPARTYLIBEXCEPTION_H_INCLUDED

