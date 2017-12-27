/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/TextDecorator.cpp $
 * @author:  
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Manipulate the text position.
 */


#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "bus/mfc_extensions/src/decorator_button/TextDecorator.h"
#include "bus/mfc_extensions/src/decorator_button/AbstractDecoratorButton.h"

namespace TA_Base_Bus
{
    TextDecorator::TextDecorator(AbstractDecoratorButton* pcomponent) : 
                        m_xPos(-1), m_yPos(-1), m_textColor(::GetSysColor(COLOR_BTNTEXT)),
                        ButtonDecorator(pcomponent)
    {
    }


    TextDecorator::~TextDecorator()
    {

    }


    void TextDecorator::Draw(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
	    if ( m_pcomponent != NULL )
	    {
		    m_pcomponent->Draw(lpDrawItemStruct);
	    }

	    CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
	    CRect rect = lpDrawItemStruct->rcItem;
	    UINT state = lpDrawItemStruct->itemState;
	    
	    rect.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

	    CString strText;
	    int len = ::GetWindowTextLength(lpDrawItemStruct->hwndItem);
	    if (len == 0) return;

	    ::GetWindowText(lpDrawItemStruct->hwndItem, strText.GetBuffer(len + 1), len + 1);
	    strText.ReleaseBuffer(len);
    
	    CRect textRect = rect;

        // can't use DT_VCENTER to center the text, so count height & width and center manually
        DWORD dwFormat = DT_SINGLELINE | DT_END_ELLIPSIS;

        HFONT oldFont = (HFONT) pDC->SelectObject(
                    (HFONT) SendMessage(lpDrawItemStruct->hwndItem, WM_GETFONT, 0, 0));

        int nHeight = ::DrawText(pDC->m_hDC, strText.GetBuffer(0), len,
            &textRect, dwFormat | DT_CALCRECT);
        int nWidth = textRect.right - textRect.left;

	    if ((m_xPos == -1) && (m_yPos == -1))
	    {
            textRect.left = rect.left + (rect.Width() - nWidth) / 2;
            textRect.top = rect.top + (rect.Height() - nHeight) / 2;
        }
        else
        {
            // adjust rect with user specified x & y
            textRect.left = m_xPos;
            textRect.top = m_yPos;
        }

	    if ((state & ODS_SELECTED) || (state & ODS_CHECKED))
	    {
		    // offset the text by 1 pixel right, 1 pixel down, 
            textRect.left += 1;
            textRect.top += 1;
        }

        // readjust bounds
        textRect.bottom = rect.bottom;
        textRect.right = rect.right;

	    int nMode = pDC->SetBkMode(TRANSPARENT);
	    
	    COLORREF prevColor;
    
        if (state & ODS_DISABLED)
        {
            OffsetRect(&textRect, 1, 1);
            prevColor = pDC->SetTextColor(GetSysColor(COLOR_3DHILIGHT));
            ::DrawText(pDC->m_hDC, strText.GetBuffer(0), len,
                    &textRect, dwFormat);
        
            OffsetRect(&textRect, -1, -1);
            pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
            ::DrawText(pDC->m_hDC, strText.GetBuffer(0), len,
                    &textRect, dwFormat);
        }
        else
        {
            prevColor = pDC->SetTextColor(m_textColor);
            ::DrawText(pDC->m_hDC, strText.GetBuffer(0), len,
                    &textRect, dwFormat);
        }

	    pDC->SetBkMode(nMode);
	    pDC->SetTextColor(prevColor);
        pDC->SelectObject(oldFont);


    }


    void TextDecorator::setTextPosition(int nXPos , int nYPos)
    {
	    m_xPos = nXPos;
	    m_yPos = nYPos;
    }

    void TextDecorator::setTextColor(COLORREF textColor)
    {
        m_textColor = textColor;
    }
}


