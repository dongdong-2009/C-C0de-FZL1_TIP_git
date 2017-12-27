#ifndef ScadaCommonException_H_INCLUDED
#define ScadaCommonException_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ScadaCommonException.h $
  *  @author:  Darren Sampson
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */

#include <string>
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class ScadaCommonException : public TransactiveException
		{
		public:
			ScadaCommonException() throw();
			ScadaCommonException(const std::string& msg) throw();
			virtual const char* what() const throw();
			virtual ~ScadaCommonException() throw();
		private:
			const std::string m_msg;
		};
}; // TA_Base_Core

#endif // ScadaCommonException_H_INCLUDED
