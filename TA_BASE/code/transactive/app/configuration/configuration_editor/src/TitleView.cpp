/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/TitleView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class paints the bar holding the description of what is currently displaying in the
  * Configuration Editor.
  */


#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/TitleView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    // The size of the font for the title
    const int TitleView::TITLE_FONT_SIZE = 90;

    // The font typeface to use for the title
    const CString TitleView::TITLE_FONT_TYPEFACE = "ËÎÌו";    //"Arial Bold";

    // The padding to use in front of the title string passed to this class. If no padding is
    // used then the text will be over the far left and will look squished.
    const CString TitleView::TITLE_PADDING = "  ";


    IMPLEMENT_DYNCREATE(TitleView, CView)

    TitleView::TitleView() : m_titleToDisplay("")
    {
    }


    TitleView::~TitleView()
    {
    }


BEGIN_MESSAGE_MAP(TitleView, CView)
	//{{AFX_MSG_MAP(TitleView)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // TitleView drawing

    void TitleView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();

	    // Add draw code here
    }

    /////////////////////////////////////////////////////////////////////////////
    // TitleView diagnostics

    #ifdef _DEBUG
    void TitleView::AssertValid() const
    {
	    CView::AssertValid();
    }

    void TitleView::Dump(CDumpContext& dc) const
    {
	    CView::Dump(dc);
    }
    #endif //_DEBUG


    void TitleView::setTitleToDisplay(const CString& title)
    {
        // We add padding in front of a title to ensure it is moved away from the edge.
        m_titleToDisplay = TITLE_PADDING + title;
        Invalidate();
    }


    void TitleView::OnPaint() 
    {
        CPaintDC dc(this); // device context for painting

        // Do not call CView::OnPaint() for painting messages
    }


    BOOL TitleView::OnEraseBkgnd(CDC* pDC) 
    {
        // We paint in here so we don't get flicker
        try
        {
            // Get the area we want to draw in
            RECT rect;
            GetClientRect(&rect);

            // Set the brush colour for the background rectangle and draw it
            CBrush brush( GetSysColor(COLOR_APPWORKSPACE) );
            pDC->FillRect(&rect,&brush);	

            // Now set up and draw the text over the top
            COLORREF oldTextColour = pDC->SetTextColor( GetSysColor(COLOR_WINDOW) );
            COLORREF oldBkColour = pDC->SetBkColor( GetSysColor(COLOR_APPWORKSPACE) );
            CFont newFont;
            newFont.CreatePointFont(TITLE_FONT_SIZE, TITLE_FONT_TYPEFACE);
            CFont* oldFont = pDC->SelectObject(&newFont);

            pDC->DrawText(m_titleToDisplay,&rect,DT_LEFT|DT_SINGLELINE|DT_VCENTER);

            // Reset old settings
            pDC->SetTextColor( oldTextColour );
            pDC->SetBkColor( oldBkColour );
		    pDC->SelectObject( oldFont );

            newFont.DeleteObject();
        }
        catch ( const CResourceException& )
        {

        }	
        return TRUE;
    }
}
