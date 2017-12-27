/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/text_colour_edit/TextColourEdit.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is basically an edit control but it can be set whether the text is valid or not. If the text is
  * invalid it will be shown in red.
  */

#include "TextColourEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    TextColourEdit::TextColourEdit(bool greyoutWhenReadOnly)
      : m_textType(VALID), m_greyOutWhenReadOnly(greyoutWhenReadOnly)
    {
    }


    TextColourEdit::~TextColourEdit()
    {
    }


BEGIN_MESSAGE_MAP(TextColourEdit, CEdit)
	//{{AFX_MSG_MAP(TextColourEdit)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // TextColourEdit message handlers

    void TextColourEdit::PreSubclassWindow() 
    {
        m_backgroundBrush.CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
        m_disabledBackgroundBrush.CreateSolidBrush( GetSysColor(COLOR_INACTIVEBORDER) );
	    
	    CEdit::PreSubclassWindow();
    }


    HBRUSH TextColourEdit::CtlColor ( CDC* pDC, UINT nCtlColor )
    {
		pDC->SetBkMode ( TRANSPARENT );

        if (m_textType == INVALID)
        {
            pDC->SetTextColor( RGB(255,0,0) );
        }
        else
        {
		    pDC->SetTextColor( RGB(0,0,0) );
        }

        // 4669FAT TD#782: These controls need to grey out when set to read only
        // (so if option specified, and style is read only, return inactive background colour)
        if (!IsWindowEnabled() ||
            (m_greyOutWhenReadOnly && 0 != (GetStyle() & ES_READONLY)) )
        {
            return m_disabledBackgroundBrush;
        }
        
        return m_backgroundBrush;
    }
}
