/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventHelperException.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "core/event/src/EventHelperException.h"

namespace TA_Base_Core
{
	EventHelperException::EventHelperException(const ERROR_CODE errCode) throw()
		: m_errCode(errCode),
		TransactiveException()
	{
	}

	EventHelperException::EventHelperException(const ERROR_CODE errCode, const std::string& what) throw()
		: m_errCode(errCode),
		TransactiveException(what)
	{
	}

	EventHelperException::~EventHelperException() throw()
	{
	}
}