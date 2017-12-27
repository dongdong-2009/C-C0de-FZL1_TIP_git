/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/Tabs.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// Tabs.cpp: implementation of the Tabs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "bus/banner_framework/src/Tabs.h"
#include "bus/banner_framework/src/BannerFrameworkDialog.h"
#include "bus/banner_framework/src/FlashIconButton.h"

#include "core/exceptions/src/BannerFrameworkException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "bus/banner_framework/src/PrintScreen.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define IDC_PRINT_BTN 20005

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // Tabs message handlers
    BEGIN_MESSAGE_MAP(Tabs, CWnd)
	    //{{AFX_MSG_MAP(Tabs)
	    //}}AFX_MSG_MAP
	    ON_CONTROL_RANGE( BN_CLICKED, ID_TAB1, ID_TAB10, onTabClicked )
        ON_CONTROL_RANGE( BN_DOUBLECLICKED, ID_TAB1, ID_TAB10, onTabDoubleClicked )
	    ON_CONTROL_RANGE( STN_DBLCLK, ID_TAB1, ID_TAB10, onTabDoubleClicked )
		ON_BN_CLICKED(IDC_PRINT_BTN, OnPrint)
    END_MESSAGE_MAP()

    const int Tabs::BORDER = 3;
	const int Tabs::BUTTON_VERTICAL_SPACING = 2;
    const int Tabs::BUTTON_HEIGHT = 21;
    const int Tabs::MAX_NUMBER_OF_BUTTONS = 10;
    const int Tabs::BUTTON_COLOUR_OFFSET_WHEN_HOT = 30;

	const std::string Tabs::RPARAM_DISABLE_PRINT = "Disableprintbutton";


    Tabs::Tabs(int width, int height, BannerFrameworkDialog* pParent)
    :
    m_bannerDlg(pParent),
    m_width(width),
    m_height(height)
    {
        ASSERT(m_bannerDlg != NULL);
        m_bannerDlg->AssertValid();
    }


    Tabs::~Tabs()
    {
    }


    void Tabs::create()
    {
	    // Register a WNDCLASS.
	    CString strWndClass = AfxRegisterWndClass (
		    CS_DBLCLKS,										// won't be used
		    AfxGetApp ()->LoadStandardCursor (IDC_ARROW),   // Class cursor
		    (HBRUSH) (COLOR_3DFACE + 1),					// Background brush
		    AfxGetApp ()->LoadStandardIcon (IDI_WINLOGO)	// won't be used
	    );

	    // Create the Tabs HWND.
	    // The WS_EX_TOOLWINDOW is used to suppress the entry in the Task Bar	
	    // static CWnd* pDeskTopWin = this->GetDesktopWindow( );
	    CreateEx (WS_EX_TOPMOST | WS_EX_TOOLWINDOW, strWndClass, _T ("not shown"),
		    WS_POPUP, //this should look like a component of the banner even if the desktop owns it
		    0, 0, m_width, m_height-2,
		    m_bannerDlg->GetSafeHwnd()/*pDeskTopWin->m_hWnd*/,  //Make the window a child of the desktop so that it can be always on top
		    NULL);
		// Note: we remove the dependencies of the Tab window from the desktop window
		//		because it has issues with windows 7, it seems that Tab window cannot 
		//		maintain the desktop as its parent window thus losing its capability 
		//		to be always on top.

		if (!TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DISABLE_PRINT.c_str()))
		{
			addPrintButton();
		}
    }


	void Tabs::setHeight(int nHeight)
	{
		m_height = nHeight;
	}


    void Tabs::addButton(UINT buttonId, const BannerIcon& bannerIcon)
    {
        FUNCTION_ENTRY("addButton");

        TA_ASSERT(m_buttons.size() < MAX_NUMBER_OF_BUTTONS, "Too many buttons added to tab!");
    
        // resize the window to hold the extra button
        int numberOfButtons = m_buttons.size() + 1;
	    int buttonWidth = m_width - (BORDER * 2);
        int windowHeight = BUTTON_HEIGHT * numberOfButtons + 1 * numberOfButtons + 1;

        // create the button
	    CPoint topLeft(BORDER, BORDER); //looks better if down two border widths
	    UINT buttonStyle = WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY;
	    CSize buttonSize(buttonWidth, BUTTON_HEIGHT);

	    CRect buttonArea(topLeft,  buttonSize);
        buttonArea.OffsetRect(0, (BUTTON_HEIGHT + BUTTON_VERTICAL_SPACING)*m_buttons.size());
	    
        int ctrlId = ID_TAB1 + m_buttons.size();	    
        FlashIconButton* button = new FlashIconButton(bannerIcon, ctrlId);
        button->Create("", buttonStyle, buttonArea, this, ctrlId);
	    button->SetBitmap((HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(bannerIcon.offIcon), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS)); 

        m_buttons[ buttonId ] = button;
        m_buttonIDs[ ctrlId ] = buttonId;

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Adding button ID %d CtrlID %d", buttonId, ctrlId );

        FUNCTION_EXIT;
    }

	void Tabs::addPrintButton()
	{
		int nleft = 0;
		nleft = (m_width/2) - 14;

		CRect buttonArea(CPoint(nleft, BORDER),CSize(28,28));
		buttonArea.OffsetRect(0, (m_height - 34));

		m_btnPrint.Create("", WS_CHILD | WS_VISIBLE | BS_ICON | BS_PUSHBUTTON | WS_TABSTOP | BS_NOTIFY,
			buttonArea,this, IDC_PRINT_BTN);

		m_btnPrint.intialize();

	}

    void Tabs::move(const CPoint& pt)
    {		
	    SetWindowPos(0, pt.x , pt.y, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION );
	    ShowWindow(SW_SHOW);
    }


    void Tabs::flashButton(UINT buttonId, bool flash)
    {
        FUNCTION_ENTRY("flashButton");

        std::map< UINT, FlashIconButton* >::iterator itButton = m_buttons.find(buttonId);
        if ( itButton == m_buttons.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Button with ID %d, doesn't exist!", buttonId );

            FUNCTION_EXIT;
            return;
        }

	    if ( true == flash )
	    {
		    itButton->second->StartFlashing();	
	    }
	    else
	    {
		    itButton->second->StopFlashing();
	    }
    }


    void Tabs::selectButton(UINT buttonId)
    {
        for( std::map< UINT, FlashIconButton* >::iterator itButton = m_buttons.begin();
             itButton != m_buttons.end();
             ++itButton )
        {
            itButton->second->setCheck(false);
        }
    
        m_buttons[ buttonId ]->setCheck(true);
    }


    void Tabs::onTabClicked(UINT ctrlId)
    {
        if (m_buttonIDs.find(ctrlId) == m_buttonIDs.end())
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Tab with control ID %d doesn't exist", ctrlId );
            return;
        }

        m_bannerDlg->BringWindowToTop();
        m_bannerDlg->showPage( m_buttonIDs[ctrlId] );
    }


    void Tabs::onTabDoubleClicked(UINT ctrlId)
    {
        if (m_buttonIDs.find(ctrlId) == m_buttonIDs.end())
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Tab with control ID %d doesn't exist", ctrlId );
            return;
        }

        m_bannerDlg->onTabDoubleClick( m_buttonIDs[ctrlId] );
    }

	void Tabs::OnPrint()
	{
		PrintScreen objPrintScreen;
		m_btnPrint.setPrintableObject(&objPrintScreen);
		m_btnPrint.print();
	}
} // namespace TA_Base_Bus
