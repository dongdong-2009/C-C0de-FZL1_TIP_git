/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/TrendViewerException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Exception class thrown by Trend Viewer. It is derived from TransActiveException.
  */

#if !defined(AFX_TREND_VIWER_EXCEPTION_CPP__44BE1875_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_TREND_VIWER_EXCEPTION_CPP__44BE1875_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TrendViewerException.h"
#include <string>

namespace TA_Base_Core
{
    TrendViewerException::TrendViewerException(const EExceptionType& exType, const std::string& details)
        : TransactiveException(details), m_type(exType)
    {
    }

	
    TrendViewerException::EExceptionType TrendViewerException::getType() const
    {
        return m_type;
    }

  
}

#endif // !defined(AFX_TREND_VIWER_EXCEPTION_CPP__44BE1875_4870_11D7_B150_0080C8E32E69__INCLUDED_)

