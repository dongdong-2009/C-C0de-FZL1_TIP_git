/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/TrendViewerException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Exception class thrown by Trend Viewer. It is derived from TransActiveException.
  */

#if !defined(AFX_TREND_VIWER_EXCEPTION_H__44BE1875_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_TREND_VIWER_EXCEPTION_H__44BE1875_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
	class TrendViewerException : public TransactiveException  
	{
    public:
        enum EExceptionType
        {
            NO_USER_SETTINGS,
            TEMPLATE_NOT_EXIST,
            TEMPLATE_ALREADY_EXISTS,
            TEMPLATE_LOADING_ERROR,
            TEMPLATE_INVALID,
            TIMESTAMP_INVALID,
            TEECHART_NOT_INSTALLED,
            DIALOG_INIT_ERROR,
            ITEM_NOT_FOUND,
            DATABASE_ERROR,
            SERIES_NUMBER_NOT_FOUND,
            GRAPH_TYPE_INVALID,
            TEMPLATE_MODIFIED,
            NO_DATA_RETRIEVED,
            TEMPLATE_LARGE_RANGE
        };

	public:
		
		TrendViewerException(const EExceptionType& exType, const std::string& details = "");
        virtual ~TrendViewerException() {}

        EExceptionType getType() const;

    private:

        TrendViewerException() {}

    private:

        EExceptionType m_type;
    };
}

#endif // !defined(AFX_TREND_VIWER_EXCEPTION_H__44BE1875_4870_11D7_B150_0080C8E32E69__INCLUDED_)

