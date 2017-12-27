/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/src/bannerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/13 16:55:49 $
  * Last modified by:  $Author: lu.xiangmei $
  *
  */
// BannerDlgImpl.cpp : implementation file
//

#include "app/system_control/banner/src/stdafx.h"
#include "app/system_control/banner/src/banner.h"
#include "app/system_control/banner/src/BannerDlg.h"
#include "app/system_control/banner/src/BannerGUI.h"

#include "bus/banner_framework/src/constants.h"
#include "bus/banner_framework/src/IBannerPage.h"

#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/resources/resource.h"

#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"

#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/entity_access/src/Banner.h"

using TA_Base_Core::GenericGUIException;
using TA_Base_Core::RunParams;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{

    const int BannerDlgImpl::TAB_WIDTH = 60;
    const ULONG BannerDlgImpl::PROFILE_LOCAL = 0;

    const char* BannerDlgImpl::LEFT_BANNER = "LeftBanner";
    const char* BannerDlgImpl::CENTRE_BANNER = "CentreBanner";
    const char* BannerDlgImpl::RIGHT_BANNER = "RightBanner";

    const char* BannerDlgImpl::LEFT_BANNER_DEFAULT_STR = "LeftBannerDefaultPage";
    const char* BannerDlgImpl::CENTRE_BANNER_DEFAULT_STR = "CentreBannerDefaultPage";
    const char* BannerDlgImpl::RIGHT_BANNER_DEFAULT_STR = "RightBannerDefaultPage";



    BannerDlgImpl::BannerDlgImpl(BannerGUI& gui, CWnd* parent)
        : BannerFrameworkDialog(IDD_BANNER_DIALOG, IDC_PLACEHOLDER, gui, 0, 0, 0, 0, TAB_WIDTH, parent)
		, m_currentProfileId(-1) // Init variable to avoid error when using profile with pkey=0
    {
	    //{{AFX_DATA_INIT(BannerDlgImpl)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
	    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    }


    /////////////////////////////////////////////////////////////////////////////
    // BannerDlgImpl message handlers
    BEGIN_MESSAGE_MAP(BannerDlgImpl, CDialog)
	    //{{AFX_MSG_MAP(BannerDlgImpl)
	    ON_WM_GETMINMAXINFO()
	    ON_WM_MOVE()
	    ON_WM_CLOSE()
	    ON_WM_DESTROY()
        ON_REGISTERED_MESSAGE(BANNER_CONST::WM_SET_FLASH_STATE, setFlashState)
        ON_WM_SYSCOMMAND()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

	//TD18095, jianghp, to fix the performance of showing manager application
	void BannerDlgImpl::init(int xPos, int yPos, int width, int height, const std::string& startTab)
	{
		FUNCTION_ENTRY("BannerDlgImpl::init");
		m_xPos = xPos;
		m_yPos = yPos;
		m_width = width;
		m_height = height;
        TA_Base_Bus::ResizingProperties resizeProperties;
        resizeProperties.canMaximise = false;
        resizeProperties.maxHeight = height;
        resizeProperties.maxWidth = width;
        resizeProperties.minHeight = height;
        resizeProperties.minWidth = width;
        setResizingProperties(resizeProperties);
		m_startTab = startTab;

        MoveWindow( m_xPos, m_yPos, m_width, m_height );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BannerDlgImpl::init Moved to: (X=%d,Y=%d,W=%d,H=%d)", m_xPos, m_yPos, m_width, m_height);

		m_tabs.setHeight(height);
		m_tabs.create();

//		DTL-Bannner-PositionWrong - start
//		- remove this block of code to reduce the number of times we reposition the banner

//         // Load user settings
//         try
//         {
//             getGui()->loadAllUserSettings();
//         }
//         catch( TA_Base_Core::UserSettingsException& use )
//         {
//             LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "Error loading User Settings: %s", use.what() );
// 			
//             TA_Base_Bus::TransActiveMessage userMsg;
//             UINT selectedButton = userMsg.showMsgBox(IDS_UE_010024);
//         }
//         catch ( ... )
//         {
//             LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "An error occurred while loading User Settings" );
// 			
//             TA_Base_Bus::TransActiveMessage userMsg;
//             UINT selectedButton = userMsg.showMsgBox(IDS_UE_010024);
//         }
//		DTL-Bannner-PositionWrong - end
		
		// Get the dimensions of each page.
        m_pageWidth = getWidth() - TAB_WIDTH - 2;
        m_pageHeight = getHeight() - 4;
        m_pageXPos = getXPosition() + TAB_WIDTH + 2;
        m_pageYPos = getYPosition(); 
		
        m_sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "m_sessionId set to %s", m_sessionId.c_str());
		
        std::vector<Component> pages = m_componentLibraryAccessor.getComponents(m_pageXPos, m_pageYPos, m_pageWidth, m_pageHeight, this);
        bool addEvent=false;
        // Add each page to the banner
        for (std::vector<Component>::iterator iter = pages.begin(); iter != pages.end(); ++iter)
        {
            try
            {
                iter->bannerPage->addBannerPage();

				if (addEvent==false)
				{
					//Add Event Button
					TA_Base_Bus::BannerIcon bannerIcon;
					bannerIcon.onIcon = IDB_EVENT_KRTC;
					bannerIcon.offIcon = IDB_EVENTG_KRTC;
					bannerIcon.onDownIcon = IDB_EVENTD_KRTC;
					bannerIcon.offDownIcon = IDB_EVENTDG_KRTC;
					m_tabs.addButton(EVENT_TAB_ID, bannerIcon);
					addEvent=true;
				}
				
            }
            catch(...)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << iter->entityName;
                userMsg.showMsgBox(IDS_UW_020012);
            }
        }
		

		//AddButton(EVENT_BTN_ID, bannerIcon);//AddButton(EVENT_BTN_ID, bannerIcon );
		
        setBannerPage();
		Invalidate();

		FUNCTION_EXIT
	}

	/*void BannerDlgImpl::AddButton(UINT buttonId, const BannerIcon& bannerIcon){
		m_tabs.addButton(buttonId, bannerIcon);
	}*/

	//void BannerDlgImpl::AddButton(UINT buttonId, const BannerIcon& bannerIcon){}

    BOOL BannerDlgImpl::OnInitDialog()
    {
	    BannerFrameworkDialog::OnInitDialog();
   	    return TRUE;
    }


    void BannerDlgImpl::sessionIdChanged(const std::string& sessionId)
    {
	    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "sessionIdChanged" );
	    	    
		// Store the new session id.
		m_sessionId = sessionId;

        // Reset the banner page based on the new profile
        setBannerPage();
        
        LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "sessionIdChanged" );
    }


    void BannerDlgImpl::OnSysCommand(UINT nID, LPARAM lParam)
    {
        if (nID != SC_CLOSE)
        {
            CDialog::OnSysCommand( nID, lParam );
        }
    }


    void BannerDlgImpl::OnClose() 
    {
	    BannerFrameworkDialog::OnClose();
    }


    void BannerDlgImpl::OnDestroy() 
    {
	    BannerFrameworkDialog::OnDestroy();
    }


    LRESULT BannerDlgImpl::setFlashState(WPARAM wParam, LPARAM lParam)
    {
        UINT dialogID = wParam;
        if (lParam != 0)
        {
            enableButtonFlash(dialogID);
        }
        else
        {
            disableButtonFlash(dialogID);
        }
		return S_OK;
    }


    void BannerDlgImpl::setBannerPage()
    {
        // TES 212: Retrieve the configured banner pages from the database
        TA_Base_Bus::SessionInfo sessionInfo;

	    try
	    {
		    // Get some info about the current session.
		    TA_Base_Bus::AuthenticationLibrary authLibrary(true);
		    sessionInfo = authLibrary.getSessionInfo(m_sessionId, m_sessionId);
	    }
	    catch(TA_Base_Core::AuthenticationAgentException&)
	    {
		    LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", "Could not retrieve session information");
		    return;
	    }
	    catch(TA_Base_Core::ObjectResolutionException&)
	    {
		    LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve Authentication Agent");
		    return;
	    }
        catch(...)
        {
        }

        unsigned long locationKey = -1;
        
        TA_Base_Core::IProfile* profile = NULL;
		try
		{
            // Use the last profile
            unsigned int numProfiles = sessionInfo.ProfileId.size();
			TA_Base_Core::IProfile* profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(sessionInfo.ProfileId[numProfiles - 1]);

            // TD 12254 - Only reset the banners to default if there is a change in profile 
            if(profile->getKey() == m_currentProfileId)
            {
                return;
            }
            else
            {
                m_currentProfileId = profile->getKey();
            }

			if (profile->getType() == PROFILE_LOCAL)
			{
				locationKey = sessionInfo.LocationId;
			}
			else  // PROFILE_CENTRAL
			{
                // Central profiles use the location id of 0
				locationKey = 0;
			}

            std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
            std::string newStartTab;
            if(entityName == LEFT_BANNER)
            {
                newStartTab = profile->getParameterActualValue(locationKey, LEFT_BANNER_DEFAULT_STR);
            }
            else if(entityName == CENTRE_BANNER)
            {
                newStartTab = profile->getParameterActualValue(locationKey, CENTRE_BANNER_DEFAULT_STR);
            }
            else if(entityName == RIGHT_BANNER)
            {
                newStartTab = profile->getParameterActualValue(locationKey, RIGHT_BANNER_DEFAULT_STR);
            }
            if ( false == newStartTab.empty() )
            {
                m_startTab = newStartTab;
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Profile parameter for start tab has no value" );
            }

            delete profile;
		}
		catch(TA_Base_Core::DataException&)
		{
			delete profile;
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve profile information.");
            return;
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			delete profile;
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve profile information");
            return;
		}
        
        // Display the page that is configured as the default page for this profile and location.
        try
        {
            if ( false == m_startTab.empty() )
            {
                showPage(m_startTab);
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "start tab has no value" );
            }
        }
        catch(...)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << m_startTab;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_020007);
        }
    }


    BannerDlg::BannerDlg(BannerGUI& gui, CWnd* parent /* = NULL */)
        : BannerDlgImpl(gui, parent) 
    {
        // Register as a run parameter user so the Banner will be aware when the profile
        // has changed so it can reload the default page if necessary.
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
    }

    void BannerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");

        if(name.compare(RPARAM_SESSIONID) == 0)
	    {
		    sessionIdChanged(value);

		    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParamChange received for session ID. New sesion ID is %s", value.c_str());
	    }

        FUNCTION_EXIT;
    }

} // namespace TA_Base_App
