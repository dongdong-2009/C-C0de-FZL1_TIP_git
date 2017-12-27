/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/BorderDecorator.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Manipuate the "border" color, i.e. the outer frame's color and thickness can be
 * specified. It is also possible to display the "default" border. This can also
 * be used to simulate "flashing" behaviour by alternating between the default and
 * specified background colour on a timed interval.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <afxwin.h>         // MFC core and standard components
#include <vector>
#include "bus/mfc_extensions/src/decorator_button/BorderDecorator.h"

namespace TA_Base_Bus
{
    BorderDecorator::BorderDecorator(AbstractDecoratorButton* pComponent) : m_width(0), 
										    m_borderColor(::GetSysColor(COLOR_BTNFACE)), 
										    ButtonDecorator(pComponent)
    {

    }


    BorderDecorator::~BorderDecorator()
    {

    }


    void BorderDecorator::Draw(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
	    if ( m_pcomponent != NULL )
	    {
		    m_pcomponent->Draw(lpDrawItemStruct);
	    }

        CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
	    CRect rect = lpDrawItemStruct->rcItem;
	    UINT state = lpDrawItemStruct->itemState;

	    int width = m_width;
	    if ((state & ODS_SELECTED) || (state & ODS_CHECKED))
	    {
		    width = m_width/2;
	    }
	    rect.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

	    CPen pen(PS_SOLID, 1, m_borderColor);
	    CPen *oldPen = pDC->SelectObject(&pen);

        // Make the end points to draw the lines. We use 1 pixel wide pen
        // to make square border.
        std::vector<POINT> points;
        for (int i = 0; i < width; ++i)
        {
	        POINT topLeft, topRight, bottomRight, bottomLeft;
            topLeft.x = rect.left + i;
            topLeft.y = rect.top + i;
            topRight.x = rect.right - i - 1;
            topRight.y = topLeft.y;
            bottomRight.x = topRight.x;
            bottomRight.y = rect.bottom - i - 1;
            bottomLeft.x = topLeft.x;
            bottomLeft.y = bottomRight.y;
        
            points.push_back(topLeft);
            points.push_back(topRight);
            points.push_back(bottomRight);
            points.push_back(bottomLeft);
            points.push_back(topLeft);
        }

	    pDC->Polyline(&points[0], points.size());

	    pDC->SelectObject(oldPen);

        if (state & ODS_FOCUS)
        {
            const int edge = 6;
            rect.top += edge;
            rect.left += edge;
            rect.bottom -= edge;
            rect.right -=edge;
        
            DrawFocusRect(pDC->m_hDC, &rect);
        }
    }


    void BorderDecorator::setBorderProperties(COLORREF borderColor, int width)
    {
	    m_borderColor = borderColor;
	    m_width = width;
    }
}

