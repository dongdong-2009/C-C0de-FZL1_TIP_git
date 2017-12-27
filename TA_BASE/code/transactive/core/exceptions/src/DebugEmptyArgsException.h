/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DebugEmptyArgsException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#if !defined DEBUGEMPTYARGSEXCEPTION_H
#define DEBUGEMPTYARGSEXCEPTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{
	class DebugEmptyArgsException : public TransactiveException
	{
	public:	
		DebugEmptyArgsException(const std::string& desc) throw()
			: TransactiveException(desc.c_str()) {}

		virtual ~DebugEmptyArgsException() throw() {}

	private:
		DebugEmptyArgsException() throw();
	};
}

#endif // DEBUGEMPTYARGSEXCEPTION_H

