/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/BannerFrameworkDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/20 22:34:59 $
  * Last modified by:  $Author: huirong.luo $
  *
  */
// bannerDlg.cpp : implementation file
//

#include "stdafx.h"
// Disable invalid warning ('this' is being used correctly in the ctor)
#include <yvals.h>              // warning numbers get enabled in yvals.h 
#pragma warning(disable: 4355)  // 'this' : used in base member initializer list

#include "bus/banner_framework/src/BannerFrameworkDialog.h"
#include "bus/banner_framework/src/BannerPage.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

    BannerFrameworkDialog::BannerFrameworkDialog(UINT bannerDialogId, UINT placeHolderId, TA_Base_Bus::IGUIAccess& gui, int xPos, int yPos, int width, int height, int tabWidth, CWnd* pParent /*=NULL*/)
    :
    TStackedDialog(gui, placeHolderId, bannerDialogId, pParent),
    m_xPos( xPos ),
    m_yPos( yPos ),
    m_width( width ),
    m_height( height ),
    m_hIcon( NULL ),
    m_tabs(tabWidth, height, this)
    {
        TA_Base_Bus::ResizingProperties resizeProperties;
        resizeProperties.canMaximise = false;
        resizeProperties.maxHeight = height;
        resizeProperties.maxWidth = width;
        resizeProperties.minHeight = height;
        resizeProperties.minWidth = width;
        setResizingProperties(resizeProperties);
	    //{{AFX_DATA_INIT(BannerFrameworkDialog)
        // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    }


    void BannerFrameworkDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(BannerFrameworkDialog)
	    //}}AFX_DATA_MAP
    }


    /////////////////////////////////////////////////////////////////////////////
    // BannerFrameworkDialog message handlers
    BEGIN_MESSAGE_MAP(BannerFrameworkDialog, CDialog)
	    //{{AFX_MSG_MAP(BannerFrameworkDialog)
	    ON_WM_GETMINMAXINFO()
	    ON_WM_MOVE()
	    ON_WM_CLOSE()
	    ON_WM_DESTROY()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL BannerFrameworkDialog::OnInitDialog()
    {
	    TStackedDialog::OnInitDialog();

		// DTL-Bannner-PositionWrong - start
		// Remove to avoid unnecessary calls to move window
		//MoveWindow( m_xPos, m_yPos, m_width, m_height );
		// DTL-Bannner-PositionWrong - end

        SetWindowText( "ControlStationBanner" );

	    return TRUE;  
    }


    void BannerFrameworkDialog::AddBannerPage(UINT pageId, const std::string& pageName,
                                            const BannerIcon& bannerIcon, BannerPage* page)
    {
        FUNCTION_ENTRY("AddBannerPage");

        std::map<UINT, BannerPage*>::iterator itPage = m_bannerPages.find(pageId);
        if ( itPage != m_bannerPages.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                "Banner Page ID %d had already been added with name %s; Ignoring second Banner with name %s",
                pageId, itPage->second->getButtonName().c_str(), pageName.c_str() );
            
            FUNCTION_EXIT;
            return;
        }
        
        std::map<std::string, UINT>::iterator itPageIDName = m_bannerIDs.find(pageName);
        if ( itPageIDName != m_bannerIDs.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                "Banner Page %s had already been added with ID %d; Ignoring second Banner with ID %d",
                pageName.c_str(), itPageIDName->second, pageId );
            
            FUNCTION_EXIT;
            return;
        }
        
        // Add pointer to the TStackedPage to multi-index map (indexed by pageId and buttonName)
        m_bannerPages[ pageId ] = page;
        m_bannerIDs[ pageName ] = pageId;

	    moveTabs();
        m_tabs.addButton(pageId, bannerIcon);
        AddPage(pageId);
    }


    TStackedPage* BannerFrameworkDialog::CreatePage(UINT nId)
    {
        if (m_bannerPages.find(nId) == m_bannerPages.end())
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Page ID %d does not exist!", nId );
            return NULL;
        }
        
        return m_bannerPages[ nId ];
    }


    void BannerFrameworkDialog::OnPageChanged(UINT nId, BOOL bActivated)
    {
        // what do we want to do here?
    }


    void BannerFrameworkDialog::OnMove(int x, int y) 
    {
	    CDialog::OnMove(x, y);
	    moveTabs();	
    }


    void BannerFrameworkDialog::moveTabs()
    {
        // TD8807: If ::IsWindow returns true m_tabs.move will succeed. This expression previously checked
        //         to see if the handle was NULL, however it is still possible for a handle to not
        //         be NULL and still not be a valid window handle. This isn't a certain fix, however it seems
        //         to be the most likely place for the original defect to occur.
	    if( ::IsWindow( m_tabs.m_hWnd ) )
	    {
		    CRect rect;
		    GetClientRect(&rect);
		    CPoint org = rect.TopLeft();
		    ClientToScreen(&org);
		    m_tabs.move(CPoint(org.x, org.y));
	    }
    }


    void BannerFrameworkDialog::showPage(const std::string& pageName)
    {
        UINT pageId = m_bannerIDs[ pageName ];

        if (0 == pageId)
        {
            std::stringstream strm;
            strm << "Invalid page: " << pageName;
            TA_ASSERT(false, strm.str().c_str());
        }

        showPage( pageId );
    }


    void BannerFrameworkDialog::showPage(UINT pageId)
    {
		if(pageId == EVENT_TAB_ID){			 
			return;
		}
	    m_tabs.selectButton( pageId );
        SetPage( pageId );
    }


    TA_Base_Bus::IGUIAccess* BannerFrameworkDialog::getGui()
    {
	    return &getGenericGUICallbackObject();
    }


    void BannerFrameworkDialog::OnClose() 
    {
        m_tabs.ShowWindow(SW_HIDE);
	    TStackedDialog::OnClose();
    }


    void BannerFrameworkDialog::OnDestroy() 
    {
	    TStackedDialog::OnDestroy();
        UnregisterHotKey(this->m_hWnd,ID_HOT_KEY_ALTF4);	
    }


    void BannerFrameworkDialog::enableButtonFlash(UINT dialogId)
    {
        m_tabs.flashButton(dialogId, true);
    }


    void BannerFrameworkDialog::disableButtonFlash(UINT dialogId)
    {
        m_tabs.flashButton(dialogId, false);
    }


    void BannerFrameworkDialog::onTabDoubleClick(UINT pageId)
    {
        FUNCTION_ENTRY("onTabDoubleClick");

		//launch eventviewer li zhongyan add
		if(pageId == EVENT_TAB_ID){
		 launchEventViewer(pageId);
			 return;
		}

        std::map<UINT, BannerPage*>::iterator itBannerPage = m_bannerPages.find(pageId);
        if (itBannerPage == m_bannerPages.end())
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Button does not exist!");

            FUNCTION_EXIT;
            return;
        }

        itBannerPage->second->onButtonDoubleClick();
    }


    int BannerFrameworkDialog::getXPosition()
    {
        return m_xPos;
    }

	void BannerFrameworkDialog::launchEventViewer(UINT pageId){
		if(pageId == EVENT_TAB_ID)
		{
			try{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "launching EventViewer");
				RECT rect = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
					TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
					getXPosition());
				TA_Base_Bus::AppLauncher::getInstance().launchApplication(
					EVENT_GUI_APPTYPE, 
					"", 
					TA_Base_Bus::TA_GenericGui::POS_BOUNDED|TA_Base_Bus::TA_GenericGui::POS_RESIZABLE,
					TA_Base_Bus::TA_GenericGui::ALIGN_FIT|TA_Base_Bus::TA_GenericGui::ALIGN_LEFT|TA_Base_Bus::TA_GenericGui::ALIGN_TOP,
					NULL,
					&rect);
			}
			catch(TA_Base_Core::ApplicationException&){
				LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch Event viewer");

				CString applicationName = "EVENT VIEWER GUI";
				//applicationName.LoadString( "EVENT VIEWER GUI");
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << applicationName;
				UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
			}
			catch(...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "GeneralException", "General Exception: Could not launch Event viewer");

				CString applicationName = "EVENT VIEWER GUI";
				//applicationName.LoadString("EVENT VIEWER GUI");
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << applicationName;
				UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
			}
		}
	}

    int BannerFrameworkDialog::getYPosition()
    {
        return m_yPos;
    }


    int BannerFrameworkDialog::getWidth()
    {
        return m_width;
    }


    int BannerFrameworkDialog::getHeight()
    {
        return m_height;
    }

    Tabs* BannerFrameworkDialog::getTabs()
    {
	    return &m_tabs;
    }
	/*void BannerFrameworkDialog::AddButton(UINT buttonId, const BannerIcon& bannerIcon){
		m_tabs.addButton(buttonId, bannerIcon);
	}*/

} // namespace TA_Base_Bus
