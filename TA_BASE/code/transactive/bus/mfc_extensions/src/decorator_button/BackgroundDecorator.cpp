/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/BackgroundDecorator.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Manipulate the background colour, i.e. the background colour can be specified.
 * It is also possible to display the "default" background colour. This can be
 * used to simulate "flashing" behaviour by alternating between the default and
 * specified background colour on a timed interval.
 */


#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <afxwin.h>         // MFC core and standard components
#include "bus/mfc_extensions/src/decorator_button/BackgroundDecorator.h"

namespace TA_Base_Bus
{
    BackgroundDecorator::BackgroundDecorator(AbstractDecoratorButton* pcomponent) :							
							    m_backgroundColor(::GetSysColor(COLOR_BTNFACE)), 
							    ButtonDecorator(pcomponent)
    {
    }


    BackgroundDecorator::~BackgroundDecorator()
    {

    }

    void BackgroundDecorator::Draw(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
	    if ( m_pcomponent != NULL )
	    {
		    m_pcomponent->Draw(lpDrawItemStruct);
	    }

        CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	    CRect rect  = lpDrawItemStruct->rcItem;
    
	    rect.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

	    CBrush pBrush(m_backgroundColor);
	    pDC->FillRect(rect, &pBrush);

    }


    void BackgroundDecorator::setBackgroundColor(COLORREF backgroundColor)
    {
	    m_backgroundColor = backgroundColor;
    }
}


