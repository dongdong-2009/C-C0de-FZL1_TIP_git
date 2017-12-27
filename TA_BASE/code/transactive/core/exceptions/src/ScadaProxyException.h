#ifndef SCADAPROXYEXCEPTION_H_INCLUDED
#define SCADAPROXYEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ScadaProxyException.h $
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
	class ScadaProxyException : public TransactiveException
		{
		public:
			ScadaProxyException() throw();
			ScadaProxyException(const std::string& msg) throw();
			virtual const char* what() const throw();
			virtual ~ScadaProxyException() throw();
		private:
			const std::string m_msg;
		};
}; // TA_Base_Core

#endif // SCADAPROXYEXCEPTION_H_INCLUDED
