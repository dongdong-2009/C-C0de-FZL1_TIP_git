/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/pages/iscs_page/src/AlarmsGuiDlg.cpp $
 * @author:  Andy Siow
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2012/07/13 16:55:49 $
 * Last modified by:  $Author: lu.xiangmei $
 * 
 * AlarmsGuiDlg.cpp : implementation file
 */

#include "stdafx.h"

#include "app/system_control/banner/pages/iscs_page/src/AlarmsGuiDlg.h"
#include "app/system_control/banner/pages/iscs_page/src/Globals.h"
#include "app/system_control/banner/pages/iscs_page/src/IscsBannerPage_rc.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/alarm/alarm_common/src/AlarmConstants.h"
#include "bus/alarm/alarm_list_control/src/AlarmActionFactory.h"
#include "bus/alarm/alarm_list_control/src/Filter.h"

#include "bus/alarm/alarm_list_control/src/actions/ActionDisplayComment.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionSeparator.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAlarm.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedSchematic.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionCloseAndAckAlarm.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionIncidentLogSheet.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionJobRequest.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionPostponeDSS.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedPlanInstance.h" //TD15736
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedDSS.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedEvents.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedInspector.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedTrend.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAllAlarms.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedAvalancheEvents.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionSuppressAlarm.h"

#include "bus/alarm/alarm_list_control/src/columns/ColumnAck.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAckBy.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAsset.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnComment.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAvalanche.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnDecisionSupport.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnDescription.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnIdentifiers.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnMms.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnSourceTimestamp.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnSeverity.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnState.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnValue.h"

#include "bus/banner_framework/src/BannerFrameworkDialog.h"
#include "bus/generic_gui/src/GenericGUI.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/SystemControllerGroupUpdateSubscriber.h"
#include "bus/resources/resource.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/AssertException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/SoundPlayerException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Bus::Sort;

namespace TA_Base_App
{
    const std::string CAlarmsGuiDlg::ALARM_CATEGORY = "Alarm";

    //TD15156++
    const std::string CAlarmsGuiDlg::RPARAM_USE_ALARM_ICON = "UseAlarmIcon";
    //++TD15156

    /////////////////////////////////////////////////////////////////////////////
    // CAlarmsGuiDlg dialog

    CAlarmsGuiDlg::CAlarmsGuiDlg(const std::string& buttonName, int xPos, int yPos, int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog)
    :
    BannerPage(buttonName, CAlarmsGuiDlg::IDD, xPos, yPos, width, height, parentDialog),
    m_unmuteOnNewAlarm( true ),
    m_ptrSettingsMgr( NULL ),
    m_soundInterface( NULL ),
    m_silenceButton( ALARM_CATEGORY ),
    m_groupUpdateSubscriber( NULL )
    {
        //{{AFX_DATA_INIT(CAlarmsGuiDlg)
        m_numNotAckedString = "0";
	    m_numTotalString = "0";
	    //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32

        m_numAlarmsNotAcked = 0;
	    m_numAlarmsTotal = 0;
	    m_exceededMaxDisplayAlarm = false;

	    try
	    {
		    m_soundInterface = new TA_Base_Bus::SoundManagerProxy();
	    }
	    catch(TA_Base_Core::SoundPlayerException& ex)
	    {
		    std::string error = "Unable to initialise SoundManager: ";
		    error += ex.what();
		    LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
	    }

        m_boldFont = NULL;

        m_groupUpdateSubscriber = new TA_Base_Bus::SystemControllerGroupUpdateSubscriber(*this);
    }


    CAlarmsGuiDlg::~CAlarmsGuiDlg()
    {
        if (m_soundInterface != NULL)
        {
            // Stop the sound from playing
            m_soundInterface->setPriority(ALARM_CATEGORY, 0);

		    delete m_soundInterface;
		    m_soundInterface = NULL;
        }
		m_alarmfont.DeleteObject();
    }


    void CAlarmsGuiDlg::onButtonDoubleClick()
    {
        // Launch AlarmManager
        try
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "launching Alarm Manager");
            RECT rect = TA_Base_Bus::AppLauncher::getInstance().getRect(
                TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
                TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
                getXPosition());
            TA_Base_Bus::AppLauncher::getInstance().launchApplication(
                ALARM_GUI_APPTYPE, 
                "", 
                TA_Base_Bus::TA_GenericGui::POS_BOUNDED|TA_Base_Bus::TA_GenericGui::POS_RESIZABLE,
                TA_Base_Bus::TA_GenericGui::ALIGN_FIT|TA_Base_Bus::TA_GenericGui::ALIGN_LEFT|TA_Base_Bus::TA_GenericGui::ALIGN_TOP,
                NULL,
                &rect);
        }
        catch(TA_Base_Core::ApplicationException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch Alarm Manager viewer");
        
		    CString applicationName;
            applicationName.LoadString(IDS_ALARM_GUI);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << applicationName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
        }
	    catch(...)
	    {
            LOG_EXCEPTION_CATCH(SourceInfo, "GeneralException", "General Exception: Could not launch Alarm Manager viewer");

            CString applicationName;
            applicationName.LoadString(IDS_ALARM_GUI);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << applicationName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
        }
    }


    void CAlarmsGuiDlg::addBannerPage()
    {
        m_alarmList = new TA_Base_Bus::AlarmListCtrl(false, false);
	    
        TA_Base_Bus::BannerIcon bannerIcon;

	    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_USE_ALARM_ICON.c_str()))
	    {
		    bannerIcon.onIcon = IDB_ALARM_KRTC;
		    bannerIcon.offIcon = IDB_ALARMG_KRTC;
		    bannerIcon.onDownIcon = IDB_ALARMD_KRTC;
		    bannerIcon.offDownIcon = IDB_ALARMDG_KRTC;
	    }
	    else
	    {
		    bannerIcon.onIcon = IDB_ALARM;
		    bannerIcon.offIcon = IDB_ALARMG;
		    bannerIcon.onDownIcon = IDB_ALARMD;
		    bannerIcon.offDownIcon = IDB_ALARMDG;
	    }

        getParentDialog()->AddBannerPage(IDD_ISCS_ALARMS, getButtonName(), bannerIcon, this);
    }


    const int CAlarmsGuiDlg::ALARM_SOUND_TIMER = 1;

    void CAlarmsGuiDlg::DoDataExchange(CDataExchange* pDX)
    {
        TStackedPage::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CAlarmsGuiDlg)
	    DDX_Control(pDX, IDC_ISCS_ALARM_LIST, *m_alarmList);
	    DDX_Control(pDX, IDC_SILENCE, m_silenceButton);
	    DDX_Text(pDX, IDC_NOT_ACK, m_numNotAckedString);
	    DDX_Text(pDX, IDC_TOTAL, m_numTotalString);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CAlarmsGuiDlg, TStackedPage)
        //{{AFX_MSG_MAP(CAlarmsGuiDlg)
	    ON_WM_SYSCOMMAND()
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_WM_SIZE()
	    ON_WM_GETMINMAXINFO()
        ON_WM_TIMER()
        ON_WM_CLOSE()
	    ON_WM_COMPAREITEM()
	    ON_WM_DESTROY()
	    ON_BN_CLICKED(IDC_SILENCE, OnSilence)
        ON_COMMAND(IDOK, OnOK)
	    ON_WM_COMPAREITEM()
        ON_MESSAGE(WM_ON_SYSTEM_DEGRADED,OnSystemInDegradedMode)
        ON_MESSAGE(WM_ON_SYSTEM_RECOVERED,OnSystemInNormalMode)
	    //}}AFX_MSG_MAP
	    ON_MESSAGE(WM_UPDATE_ALARMS_MSG, OnUpdateAlarms)
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CAlarmsGuiDlg message handlers


    LRESULT CAlarmsGuiDlg::OnUpdateAlarms(WPARAM wParam, LPARAM lParam)
    {
	    m_numAlarmsTotal = wParam;
	    m_numAlarmsNotAcked = lParam;

	    if (m_exceededMaxDisplayAlarm == true)
	    {
		    m_numTotalString.Format("%d+", m_numAlarmsTotal);
	    }
	    else
	    {
		    m_numTotalString.Format("%d", m_numAlarmsTotal);
	    }

	    m_numNotAckedString.Format("%d", m_numAlarmsNotAcked);
	    UpdateData(false);
	    return NULL;	// This line must be included. Everything above is application specific
    }


    BOOL CAlarmsGuiDlg::OnInitDialog()
    {
        BannerPage::OnInitDialog();

	    INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_PAGESCROLLER_CLASS|ICC_BAR_CLASSES;
        ::InitCommonControlsEx(&icex);

	    m_silenceButton.initialise(false);

	    // Added for Manila - Disable the Close button in the title bar
	    HMENU hmenu = ::GetSystemMenu(AfxGetMainWnd()->m_hWnd, FALSE);
	    ::EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED | MF_BYCOMMAND);


        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);            // Set big icon
        SetIcon(AfxGetApp()->LoadIcon(IDI_SM_BELL), FALSE);        // Set small icon

	    std::string strLevel = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
	    if (!strLevel.empty())
	    {
		    DebugUtil::EDebugLevel level = DebugUtil::getInstance().getDebugLevelFromString(strLevel.c_str());
		    DebugUtil::getInstance().setLevel(level);
	    }

	    // Load the accelerator keys
	    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR1));
   
        // Set Session Id
        m_sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "m_sessionId set to %s", m_sessionId.c_str());
	    RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

        // TD 6950: Start receiving message informing GUI when system is degraded/recovered
        m_groupUpdateSubscriber->startNotifications();

	    // Initialise the Alarm List Control
	    TA_Base_Bus::Filter preFilter;
	    std::vector<unsigned long> locations;    
	    initPreFiltering(preFilter, locations);

        // TD 6950: Check if the system is currently in degraded mode. If so, clear the pre-filter.
        updatePreFilterAccordingToSystemState(preFilter);

        TA_Base_Bus::Filter filter;

        TA_Base_Bus::AlarmActionFactory* factory = 
		    new TA_Base_Bus::AlarmActionFactory(initListControlActions());

	    initListControlColumns();

        TA_ASSERT(m_alarmList != NULL, "Alarm list has not been created!");

	    //To disable the column resize in the banner by subclass the fixedheaderctrl class to the alarm list ctrl
	    m_headerCtrl.SubclassDlgItem(0, m_alarmList);
		m_alarmList->setColumnResizable(false);
	    if( RunParams::getInstance().isSet(RPARAM_KRTCALARMSORTING))
	    {
            m_alarmList->startAlarmList(*this, *factory, m_alarmColumns, preFilter, locations, TA_Base_Bus::COL_SOURCETIMESTAMP, false, filter, m_sessionId, true);
	    }
	    else
	    {
            m_alarmList->startAlarmList(*this, *factory, m_alarmColumns, preFilter, locations, TA_Base_Bus::COL_STATE, true, filter, m_sessionId, true);
	    }
		

		int left = 140;
		int top = 0;
		int width = getWidth() - 2 - left;
		int height = getHeight() - 2;
		m_alarmList->MoveWindow(left, top, width, height);
		
		//set font for enlarge the height of one line li zhongyan
		//CFont font;
		m_alarmfont.CreateFont(17, 0, 0, 0, FW_THIN, FALSE, FALSE, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,"ËÎÌו");
		m_alarmList->SetFont(&m_alarmfont, TRUE);
		SetFont(&m_alarmfont);
		m_alarmList->GetHeaderCtrl()->SetFont(&m_alarmfont, TRUE);
		/*font.CreateFont(17, 0, 0, 0, 50, FALSE, FALSE, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,"ËÎÌו");
		m_alarmList->SetFont(&font, TRUE);*/



	    //TD14896++
	    //From the m_alarmColumns.push_back() calls, COL_DESCRIPTION is 5th (or 4th???) item
	    TA_Base_Bus::FixedHeaderCtrl::expandColumnToFitListWidth(*m_alarmList, 4, true); //TD15005
	    //++TD14896

        TA_Base_Core::IData * dbdata = NULL;
        try
        {
            dbdata = TA_Base_Core::EntityAccessFactory::getInstance().getParameter("ControlStation", "UnmuteOnAlarm");
            if ( NULL != dbdata )
            {
                if ( 0 < dbdata->getNumRows() )
                {
                    m_unmuteOnNewAlarm = dbdata->getBooleanData( 0, "VALUE", true );
                }
            }
        }
        catch ( TA_Base_Core::DatabaseException & e)
        {
            m_unmuteOnNewAlarm = true;

            std::ostringstream desc;
            desc << "Caught DatabaseException - Setting m_unmuteOnNewAlarm = true; Error: " << e.what();
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", desc.str().c_str());
        }
        catch ( TA_Base_Core::DataException & e)
        {
            m_unmuteOnNewAlarm = true;

            std::ostringstream desc;
            desc << "Caught DataException - Setting m_unmuteOnNewAlarm = true; Error: " << e.what();
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", desc.str().c_str());
        }
        catch(...)
        {
            m_unmuteOnNewAlarm = true;

            std::ostringstream desc;
            desc << "Caught Unknown Exception - Setting m_unmuteOnNewAlarm = true";
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", desc.str().c_str());
        }

        delete dbdata;
        dbdata = NULL;

        return TRUE;  // return TRUE  unless you set the focus to a control
    }


    // Pre-process system command to catch call to about box
    void CAlarmsGuiDlg::OnSysCommand(UINT nID, LPARAM lParam)
    {
    //	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    //	{
		    //GenericGUI::CGenericGUI::DisplayAbout();
    //		TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    //	}
    //	else
    //	{
		    TStackedPage::OnSysCommand(nID, lParam);
    //	}
    }

    // If you add a minimize button to your dialog, you will need the code below
    //  to draw the icon.  For MFC applications using the document/view model,
    //  this is automatically done for you by the framework.

    void CAlarmsGuiDlg::OnPaint()
    {
        if (IsIconic())
        {
            CPaintDC dc(this); // device context for painting

            SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

            // Center icon in client rectangle
            int cxIcon = GetSystemMetrics(SM_CXICON);
            int cyIcon = GetSystemMetrics(SM_CYICON);
            CRect rect;
            GetClientRect(&rect);
            int alarmItem = (rect.Width() - cxIcon + 1) / 2;
            int y = (rect.Height() - cyIcon + 1) / 2;

            // Draw the icon
            dc.DrawIcon(alarmItem, y, m_hIcon);
        }
        else
        {
            TStackedPage::OnPaint();
        }
    }


    // The system calls this to obtain the cursor to display while the user drags
    //  the minimized window.
    HCURSOR CAlarmsGuiDlg::OnQueryDragIcon()
    {
        return (HCURSOR) m_hIcon;
    }


    void CAlarmsGuiDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
    {	
	    // Set a minimum size for the Alarm Viewer window
	    lpMMI->ptMinTrackSize.x = 500;
	    lpMMI->ptMinTrackSize.y = 300;
	    
	    TStackedPage::OnGetMinMaxInfo(lpMMI);
    }


    void CAlarmsGuiDlg::OnOK()
    {
        //Do Nothing
    }


    void CAlarmsGuiDlg::OnCancel()
    {
        //Do Nothing
    }


    // DestroyWindow is called when the Control station is closed
    // to terminate running Guis. The WM_DESTROY message is intercepted
    // here in order to save user settings.
    void CAlarmsGuiDlg::OnDestroy()
    {
        if(m_groupUpdateSubscriber != NULL)
        {
            delete m_groupUpdateSubscriber;
            m_groupUpdateSubscriber = NULL;
        }

	    if (m_ptrSettingsMgr)
	    {
            OnClose();
        }
    }



    void CAlarmsGuiDlg::OnClose()
    {
        //
        // Start terminiating the application.  
        //
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "OnClose()");

	    RunParams::getInstance().deregisterRunParamUser(this);

	    //CPD Save the user settings
	    if(m_ptrSettingsMgr != 0)
	    {
		    try
		    {
			    // This method will save the placement of windowPtr (retrieved via CWnd::GetWindowPlacement()) 
			    //to the database. See RestoreWindowPlacement() for why you might want to do this.
			    //Will throw UserSettings::DataBaseEx& if the write to the database fails.
			    //Throws UserSettings::AssertEx& if windowPtr or the handle to windowPtr is NULL
			    //Note that if you call this on more than one CWnd only the last call will be saved. In other
			    //words, this method is only useful for saving the placement of only one window per application
			    m_ptrSettingsMgr->saveWindowPlacement(this);

		    }
		    catch(TA_Base_Core::AssertException& ex) //use the default if the SettingsMgr can't restore the placement
		    {
			    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "SettingsMgr could not save Window placement because: %s",
                    ex.what() );
		    }
		    catch(TA_Base_Core::DatabaseException& ex) //use the default if the SettingsMgr can't restore the placement
		    {
			    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "SettingsMgr could not save Window placement because: %s",
                    ex.what() );
            }

		    if (m_ptrSettingsMgr)
		    {
                delete m_ptrSettingsMgr;
			    m_ptrSettingsMgr = NULL;
		    }
	    }

        TStackedPage::OnCancel();
    }


    BOOL CAlarmsGuiDlg::PreTranslateMessage(MSG* pMsg) 
    {
         if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
         {
             HACCEL hAccel = m_hAccel;
             if (hAccel && ::TranslateAccelerator(AfxGetMainWnd()->m_hWnd, hAccel, pMsg))
             {
                 return TRUE;
             }
         }

	     return TStackedPage::PreTranslateMessage(pMsg);
    }


    void CAlarmsGuiDlg::OnSilence() 
    {
        m_silenceButton.toggleState();
    }


    LRESULT CAlarmsGuiDlg::OnSystemInDegradedMode(WPARAM wParam, LPARAM lParam)
    {
	    FUNCTION_ENTRY( "OnSystemInDegradedMode" );

        // Retrieve the locations related to the current profile. We still only want alarms
        // from these locations.
        TA_Base_Bus::Filter preFilter;
	    std::vector<unsigned long> locations;
	    initPreFiltering(preFilter, locations);

        // Clear pre-filter as we don't want rights to be checked before an alarm is displayed.
        preFilter.removeAll();
	    m_alarmList->updatePreFilterLocationsAndSession(preFilter, locations, m_sessionId);
    
	    FUNCTION_EXIT;
        return NULL;	// This line must be included. Everything above is application specific
    }


    LRESULT CAlarmsGuiDlg::OnSystemInNormalMode(WPARAM wParam, LPARAM lParam)
    {
	    FUNCTION_ENTRY( "OnSystemInNormalMode" );

	    // Reset Pre-Filter
        TA_Base_Bus::Filter preFilter;
	    std::vector<unsigned long> locations;
	    initPreFiltering(preFilter, locations);
	    m_alarmList->updatePreFilterLocationsAndSession(preFilter, locations, m_sessionId);
		    
	    FUNCTION_EXIT;
        return NULL;	// This line must be included. Everything above is application specific
    }


    std::vector<TA_Base_Bus::IAlarmAction*> CAlarmsGuiDlg::initListControlActions()
    {
	    FUNCTION_ENTRY("initListControlActions");

        std::vector<TA_Base_Bus::IAlarmAction*> actions;

	    // Add actions available from the context menu.
	    // TD #7550
	    // Context menu changed to match the alarm list control in the Alarm Manager
        actions.push_back( new TA_Base_Bus::ActionAcknowledgeAlarm() );
	    actions.push_back( new TA_Base_Bus::ActionDisplayComment() );
        actions.push_back( new TA_Base_Bus::ActionSeparator() );
        actions.push_back( new TA_Base_Bus::ActionRelatedSchematic() );
        actions.push_back( new TA_Base_Bus::ActionRelatedInspector() );
        actions.push_back( new TA_Base_Bus::ActionRelatedEvents() );
        actions.push_back( new TA_Base_Bus::ActionSeparator() );
        actions.push_back( new TA_Base_Bus::ActionRelatedDSS() );
	    actions.push_back( new TA_Base_Bus::ActionRelatedPlanInstance() ); 	//TD15736
        actions.push_back( new TA_Base_Bus::ActionPostponeDSS() );
        actions.push_back( new TA_Base_Bus::ActionSeparator() );
        actions.push_back( new TA_Base_Bus::ActionJobRequest() );
        actions.push_back( new TA_Base_Bus::ActionRelatedTrend() );
        actions.push_back( new TA_Base_Bus::ActionRelatedAvalancheEvents() );
        //actions.push_back( new TA_Base_Bus::ActionIncidentLogSheet() );
        actions.push_back( new TA_Base_Bus::ActionSeparator() );
        actions.push_back( new TA_Base_Bus::ActionCloseAndAckAlarm() );
	    actions.push_back( new TA_Base_Bus::ActionSuppressAlarm() );

        FUNCTION_EXIT;
	    return actions;
    }


    void CAlarmsGuiDlg::initListControlColumns()
    {
	    FUNCTION_ENTRY("initListControlColumns");

	    TA_Base_Bus::Sort descSort;
	    TA_Base_Bus::Sort ascSort;

	    if( RunParams::getInstance().isSet(RPARAM_KRTCALARMSORTING) )
	    {
		    m_alarmColumns.push_back(new TA_Base_Bus::ColumnState( 1, true, SORT_BY_DEFAULT, NULL));
	    }
	   
	   else
	    {
		    /*// Add disabled state and ack columns so that default sort will work.
		    ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING); //TD15689
		    ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING); //TD15689
		    ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		    ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);*/
		    m_alarmColumns.push_back(new TA_Base_Bus::ColumnState(1, true,  SORT_BY_DEFAULT, NULL));
	    }
		
  	    // TD11777 / TD3354: This col is added to list but hidden from user to remove the black line affecting 1st col 
	    m_alarmColumns.push_back(new TA_Base_Bus::ColumnAck(30, false, SORT_BY_ACK, NULL));

	    // Add the rest of the columns.
	    //m_alarmColumns.push_back(new TA_Base_Bus::ColumnSourceTimestamp(130, true));
	    unsigned long ulTimestampWidth = TA_Base_Bus::AlarmConstants::DEFAULT_TIMESTAMPCOLWIDTH;
	    if (RunParams::getInstance().isSet(TA_Base_Bus::AlarmConstants::RPARAM_TIMESTAMPCOLWIDTH.c_str()))
	    {
		    ulTimestampWidth = strtoul(RunParams::getInstance().get(TA_Base_Bus::AlarmConstants::RPARAM_TIMESTAMPCOLWIDTH.c_str()).c_str(), NULL, 0);
	    }

	  /*  if( RunParams::getInstance().isSet(RPARAM_KRTCALARMSORTING) )
	    {
		   // ascSort.addSort(Sort::SORT_TIME, Sort::SORT_DESCENDING);
		    m_alarmColumns.push_back(new TA_Base_Bus::ColumnSourceTimestamp(ulTimestampWidth, true,SORT_BY_DATETIME, NULL));
	    }
	    else
	    {
		    m_alarmColumns.push_back(new TA_Base_Bus::ColumnSourceTimestamp(ulTimestampWidth, true));
	    }
		*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnSourceTimestamp(ulTimestampWidth, true,SORT_BY_DATETIME, NULL));

	    m_alarmColumns.push_back(new TA_Base_Bus::ColumnSeverity(66, true, SORT_BY_SEVERITY, NULL)); //TD15005
	    m_alarmColumns.push_back(new TA_Base_Bus::ColumnAsset(210, true, SORT_BY_ASSET, NULL)); //TD15005
	    m_alarmColumns.push_back(new TA_Base_Bus::ColumnDescription(1, true, SORT_BY_DESCRIPTION, NULL)); //TD15005
	    m_alarmColumns.push_back(new TA_Base_Bus::ColumnValue(130, true, SORT_BY_VALUE, NULL));
	    // TD #11287
	    // Changed column order from DAM to MDA
	    m_alarmColumns.push_back(new TA_Base_Bus::ColumnMms(28, true, SORT_BY_MMS, NULL));
	    m_alarmColumns.push_back(new TA_Base_Bus::ColumnDecisionSupport(28, true, SORT_BY_DSS, NULL));
	    m_alarmColumns.push_back(new TA_Base_Bus::ColumnAvalanche(28, true, SORT_BY_AVALANCHE, NULL));
	    
	    FUNCTION_EXIT;
    }


    void CAlarmsGuiDlg::initPreFiltering(TA_Base_Bus::Filter& preFilter, std::vector<unsigned long>& locations)
    {
	    locations.clear();

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

	    // The prefilter requires the session id.
	    preFilter.addFilter(TA_Base_Bus::Filter::FILTER_PROFILE, 
						    TA_Base_Bus::Filter::COMPARE_EQUAL, m_sessionId);

	    bool isCentralProfile = false;
	    TA_Base_Core::IProfile* profile;

	    for (std::vector<unsigned long>::iterator iter = sessionInfo.ProfileId.begin();
		    iter != sessionInfo.ProfileId.end(); iter++)
	    {
		    try
		    {
				profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(*iter);

			    if (profile->getType() == PROFILE_LOCAL)
			    {
				    locations.push_back(sessionInfo.LocationId);
			    }
			    else  // PROFILE_CENTRAL
			    {
				    isCentralProfile = true;
			    }

			    delete profile;
		    }
		    catch(TA_Base_Core::DataException&)
		    {
			    delete profile;
			    LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve profile information");
		    }
		    catch(TA_Base_Core::DatabaseException&)
		    {
			    delete profile;
			    LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve profile information");
		    }
	    }

	    if (isCentralProfile)
	    {
		    // Central profiles require all locations.
		    locations.clear();
	    }
    }


    void CAlarmsGuiDlg::updateAlarmCounts(unsigned int totAlarms, unsigned int totUnacknowledgedAlarms, bool exceededMaxDisplayAlarm, bool isLoadingAlarms)
    {
	    //We must send a message to itself to update the total Alarms counter. 
	    //Handling it here directly will cause an unnecessary assert to occur when
	    //UpdateData(false) is called. This is an MFC bug, Microsoft Knowledge Base Article - 192853.
	    m_exceededMaxDisplayAlarm = exceededMaxDisplayAlarm;

	    if (isLoadingAlarms == true)
	    {
		    // Display wait ie. hour-glass cursor while loading
		    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	    }
	    else
	    {
		    // Display normal ie. Arrow cursor after loading
		    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	    }

	    PostMessage(WM_UPDATE_ALARMS_MSG, totAlarms, totUnacknowledgedAlarms);
	    
	    if(totUnacknowledgedAlarms == 0)
        {
            disableButtonFlash();
        }
        else
        {
            enableButtonFlash();
        }
	    //OnChange(true, true, getHighestSeverity(), true);
    }


    void CAlarmsGuiDlg::updateAlarmSound(unsigned long severity, bool wereAlarmsAdded)
    {
	    try
	    {
            if (m_soundInterface != NULL)
            {
                if (severity == 0)
                {
                    // Play no sound.
                    m_soundInterface->setPriority(ALARM_CATEGORY, 0);
                }
                else
                {
					const int NUM_SEVERITIES = 4;					
					// Need to convert severities (1 highest, descending order) into priorities 
					// (1 lowest, ascending order).
					long alarmPriority = 0;				 
					alarmPriority = NUM_SEVERITIES + 1 - severity; 
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Sound severity= %d before=%d", alarmPriority,severity);
                    m_soundInterface->setPriority(ALARM_CATEGORY, alarmPriority);       //TD16415
                }
            }
	    }
	    catch(TA_Base_Core::SoundPlayerException& ex)
	    {
		    std::string error = "Error setting priority on SoundManager: ";
		    error += ex.what();
		    LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
	    }

	    if (m_unmuteOnNewAlarm && wereAlarmsAdded)
        {
		    // New alarms - clear the mute button if necessary.
		    m_silenceButton.unsilence();
        }

    }


    CProgressCtrl* CAlarmsGuiDlg::getProgressBar()
    {
	    return NULL;
    }


    void CAlarmsGuiDlg::finishedWithProgressBar()
    {
    }


    void CAlarmsGuiDlg::notifyNoMatchingAlarms()
    {
    }

	void CAlarmsGuiDlg::notifyFilterReset()
	{
	}


    void CAlarmsGuiDlg::updateAvailableAlarmActions(const ItemAvailability& availability)
    {
    }


    void CAlarmsGuiDlg::setCurrentDescriptionText(const CString& description)
    {
    }


    void CAlarmsGuiDlg::setWindowTitle(const std::string& windowText) 
    {
	    //DO nothing for the alarm banner container as it has no title bar. 
	    //Important not to delete this method. 
    }


    std::string CAlarmsGuiDlg::getSessionID()
    {
	    return m_sessionId;
    }


    void CAlarmsGuiDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
	    FUNCTION_ENTRY( "onRunParamChange" );
	    
	    if(name.compare(RPARAM_SESSIONID) == 0)
	    {
		    // Store the new session id.
		    m_sessionId = value;

		    // Update the pre-filter and locations.
		    TA_Base_Bus::Filter preFilter;
		    std::vector<unsigned long> locations;
		    initPreFiltering(preFilter, locations);
		    m_alarmList->updatePreFilterLocationsAndSession(preFilter, locations, m_sessionId);
		    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RunParamChange received for session ID. New sesion ID is %s", m_sessionId.c_str());
	    }

	    FUNCTION_EXIT;
    }


    void CAlarmsGuiDlg::notifySystemStateDegraded()
    {
        FUNCTION_ENTRY( "notifySystemStateDegraded" );
	    PostMessage(WM_ON_SYSTEM_DEGRADED, NULL, NULL);
	    FUNCTION_EXIT;
    }


    void CAlarmsGuiDlg::notifySystemStateNormal()
    {
        FUNCTION_ENTRY( "notifySystemStateNormal" );
	    PostMessage(WM_ON_SYSTEM_RECOVERED, NULL, NULL);
	    FUNCTION_EXIT;
    }


    void CAlarmsGuiDlg::updatePreFilterAccordingToSystemState(TA_Base_Bus::Filter& preFilter)
    {
        // TD 6950: Check if the system is currently in degraded mode. If so, clear the pre-filter.
        std::string groupsOffline = RunParams::getInstance().get(RPARAM_GROUPSOFFLINE);
        if( true == groupsOffline.empty())
        {
            return;
        }

        std::string::size_type pos = 0;
        while(pos != std::string::npos)
        {
            pos = groupsOffline.find(",", 0);
            std::string group = groupsOffline.substr(0, pos);

            if(group == m_groupUpdateSubscriber->getCentralServerGroupName())
            {
                // System is degraded
                preFilter.removeAll();
                return;
            }

            if(pos != std::string::npos)
            {
                groupsOffline = groupsOffline.substr(pos+1, groupsOffline.size() - (pos + 1));
            }
        }
    }
}
