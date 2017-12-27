/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DebugMsgConstructionException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#if !defined DEBUGMSGCONSTRUCTIONEXCEPTION_H
#define DEBUGMSGCONSTRUCTIONEXCEPTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{
	class DebugMsgConstructionException : public TransactiveException
	{
	public:	
		DebugMsgConstructionException(const std::string& desc) throw()
			: TransactiveException(desc.c_str()) {}

		virtual ~DebugMsgConstructionException() throw() {}

	private:
		DebugMsgConstructionException() throw();
	};
};

#endif // DEBUGMSGCONSTRUCTIONEXCEPTION_HH


