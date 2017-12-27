/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/DutyRequestDialog.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A dialog which lists requested duties and allow the operator to accept or 
  * deny requests.  The dialog will automatically accept the request if the 
  * operator does not reply before timeout.
  *
  */

#include "app/system_control/control_station/src/stdafx.h"
#include "app/system_control/control_station/src/controlstation.h"
#include "app/system_control/control_station/src/DutyRequestDialog.h"
#include "app/system_control/control_station/src/SessionManager.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/SoundPlayerException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;

const UINT DutyRequestDialog::TIMEOUT_TIMER = 124;
const unsigned int DutyRequestDialog::DEFAULT_TIMER_TIMEOUT = 30;
// This is the id of the clock timer that updates the time and date in the launch bar

//TD15112++
const CString DutyRequestDialog::LOCATION_FORMAT = "\t%s\n";
const CString DutyRequestDialog::SUBSYSTEM_FORMAT= "\t%s\n";
//++TD15112

/////////////////////////////////////////////////////////////////////////////
// DutyRequestDialog dialog


DutyRequestDialog::DutyRequestDialog(CWnd* pParent, DutyManager::DutyRequestDetail* detail)
	: CDialog(DutyRequestDialog::IDD, pParent), m_parent(pParent), m_detail(detail), m_soundInterface(NULL)
{
    TA_ASSERT(m_parent != NULL, "Parent is NULL");
    TA_ASSERT(m_detail != NULL, "Detail is NULL");
    TA_VERIFY(PRE_TIMEOUT_FORMAT.LoadString(IDS_PRE_TIMEOUT_FORMAT)!=0, "Unable to load the PRE_TIMEOUT format string");
    TA_VERIFY(POST_TIMEOUT_FORMAT.LoadString(IDS_POST_TIMEOUT_FORMAT)!=0, "Unable to load the POST_TIMEOUT format string");

	//{{AFX_DATA_INIT(DutyRequestDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


DutyRequestDialog::~DutyRequestDialog()
{
	if( m_soundInterface != NULL )
	{
		delete m_soundInterface;
		m_soundInterface = NULL;
	}
    delete m_detail;
}


void DutyRequestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DutyRequestDialog)
	DDX_Control(pDX, IDOK, m_okCtrl);
	DDX_Control(pDX, IDC_DENY, m_cancelCtrl);
	DDX_Control(pDX, IDC_DUTY_TIMEOUT, m_timeCtrl);
	DDX_Control(pDX, IDC_DUTY_TEXT, m_textCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DutyRequestDialog, CDialog)
	//{{AFX_MSG_MAP(DutyRequestDialog)
	ON_BN_CLICKED(IDOK, OnAccept)
	ON_BN_CLICKED(IDC_DENY, OnDeny)
	ON_WM_TIMER()
    ON_MESSAGE(WM_FORCE_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DutyRequestDialog message handlers

BOOL DutyRequestDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // disable the 'x' button for now
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        pSysMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
    }

    // format the content
    CString out = formatContent();
//    out.TrimRight(_T("\n\r \t"));
    m_textCtrl.SetWindowText(out);
    m_textCtrl.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));

    // if we can get the current cursor position, then centre the dialog on that screen
    // otherwise, just centre it on the middle screen.
    CPoint pt;
    RECT screen, rect, newRect;
    if (GetCursorPos(&pt))
    {
        screen = SessionManager::getInstance().getScreenPositioning().getRect(
                TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
                TA_Base_Bus::TA_GenericGui::AREA_SCREEN,
                pt.x);

        // get our window rect
        GetWindowRect(&rect);

        int width = (rect.right - rect.left);
        newRect.left = screen.left + ((screen.right - screen.left) - width)/2;
        newRect.right = newRect.left + width;

        int height = (rect.bottom - rect.top);
        newRect.top = screen.top + ((screen.bottom - screen.top) - height)/2;
        newRect.bottom = newRect.top + height;

        // Make the Window always on top.
        SetWindowPos(&wndTopMost, newRect.left, newRect.top, width, height, SWP_NOSIZE);
    }
    else
    {
        // Make the Window always on top.
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE);
    }

    // Set the timer to go off every 1000 milliseconds to update the clock
    CString time;
    unsigned int timeout = atoi(RunParams::getInstance().get(TA_Base_App::TA_ControlStation::ACCESS_CONTROL_POPUP_TIMEOUT.c_str()).c_str());

    // atoi returns 0 if the string cannot be converted to a number
    if ( 0 == timeout )
    {
        timeout = DEFAULT_TIMER_TIMEOUT; // ensure a 'good' value
    }
    time.Format(PRE_TIMEOUT_FORMAT, timeout);
    m_timeCtrl.SetWindowText(time);
    m_countDown = timeout;
    

    // Call the sound player    
	try
	{
		m_soundInterface = new TA_Base_Bus::SoundManagerProxy();
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string error = "Unable to initialise SoundManager: ";
		error += ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());

        m_soundInterface = NULL;
	}

    if (m_soundInterface != NULL)
    {
        try
        {
            m_soundInterface->setPriority(TA_Base_App::TA_ControlStation::CS_DUTY_SOUND_CATEGORY,
                TA_Base_App::TA_ControlStation::CS_DUTY_SOUND_PRIORITY);
        }
        // If there are any exceptions, just log and continue
        catch(TA_Base_Core::SoundPlayerException& ex)
        {
            std::string message("Caught while attempting to initialise sound for Duty Request dialog, with error: ");
            message += ex.what();
            LOG_EXCEPTION_CATCH(SourceInfo,"SoundPlayerException",message);
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception","Caught an exception while attempting to initialise sound. No sound will be played");
        }
    }

    // set the timer to go off every 1 second so that we could update the count down value in the dialog
    SetTimer(TIMEOUT_TIMER, 1000, 0);   

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


CString DutyRequestDialog::formatContent()
{
    DutyManager::DutyDetail& data = m_detail->duty;

	//TD15112++

    /*
	// same as DutyNotificationDialog::formatContent
    CString temp, completeLocationAndSubsystemStr;
    for (unsigned int j = 0; j < data.subsystem.size(); j++)
    {
        CString subsystemStr;
        for (unsigned int k = 0; k < data.subsystem[j].subsystemName.size(); k++)
        {
            AfxFormatString1(temp, IDS_SUBSYSTEM_FORMAT, data.subsystem[j].subsystemName[k].c_str());
            subsystemStr += temp;
        }

        CString locationAndSubsystemStr;
        AfxFormatString2(locationAndSubsystemStr, IDS_LOCATION_AND_SUBSYSTEM_FORMAT, data.subsystem[j].regionName.c_str(), subsystemStr);
        completeLocationAndSubsystemStr += locationAndSubsystemStr;
    }

    CString theFormat, out;
    if (m_detail->duty.locationName.empty())
    {
        TA_VERIFY(theFormat.LoadString(IDS_REQUEST_FORMAT_WITHOUT_LOCATION)!=0, "Unable to load the IDS_REQUEST_FORMAT_WITHOUT_LOCATION format string");
        out.Format(theFormat, data.operatorName.c_str(), data.profileName.c_str(), completeLocationAndSubsystemStr);
    }
    else
    {
        TA_VERIFY(theFormat.LoadString(IDS_REQUEST_FORMAT_WITH_LOCATION)!=0, "Unable to load the IDS_REQUEST_FORMAT_WITH_LOCATION format string");
        out.Format(theFormat, data.operatorName.c_str(), data.profileName.c_str(), data.locationName.c_str(), completeLocationAndSubsystemStr);
    }
	*/

	CString format, temp, out;

    if (m_detail->duty.locationName.empty())
    {
        TA_VERIFY(format.LoadString(IDS_REQUEST_FORMAT_WITHOUT_LOCATION)!=0, "Unable to load the IDS_REQUEST_FORMAT_WITHOUT_LOCATION format string");
		temp.Format(format, data.operatorName.c_str(), data.profileName.c_str());
    }
    else
    {
        TA_VERIFY(format.LoadString(IDS_REQUEST_FORMAT_WITH_LOCATION)!=0, "Unable to load the IDS_REQUEST_FORMAT_WITH_LOCATION format string");
		temp.Format(format, data.operatorName.c_str(), data.profileName.c_str(), data.locationName.c_str());
    }
    out += temp;

    for (unsigned int j = 0; j < data.subsystem.size(); j++)
    {
		out += "\n\n下列系统:\n";
        for (unsigned int k = 0; k < data.subsystem[j].subsystemName.size(); k++)
        {
            temp.Format(SUBSYSTEM_FORMAT, data.subsystem[j].subsystemName[k].c_str());
            out += temp;
        }

        out += "\n在:";
        temp.Format(LOCATION_FORMAT, data.subsystem[j].regionName.c_str());
        out += temp;
    }
	out += "\n当前你所控制的系统?";
	
	//++TD15112

    return out;
}


void DutyRequestDialog::OnTimer(UINT nIDEvent) 
{
	// if it's the clock timer
	if (TIMEOUT_TIMER == nIDEvent)
	{
        if (m_countDown > 0)
        {
            m_countDown--;
            CString time;
            time.Format(PRE_TIMEOUT_FORMAT, m_countDown);
            m_timeCtrl.SetWindowText(time);
        }
        else
        {
            // we've hit 0, so need to kill timer, disable buttons and act as though the
            // operator has clicked OK but leave the dialog open

	        // stop the clock timer
	        KillTimer(TIMEOUT_TIMER);

            // disable buttons
            m_cancelCtrl.EnableWindow(FALSE);
            m_okCtrl.EnableWindow(FALSE);
        
            // enable the 'x' button since that's the only way user could close the dialog now
            CMenu* pSysMenu = GetSystemMenu(FALSE);
            if (pSysMenu != NULL)
            {
                pSysMenu->EnableMenuItem(SC_CLOSE, MF_ENABLED);
            }

            std::string* uniqueId = new std::string(m_detail->uniqueId);
            // tell our parent the user has failed to respond
            m_parent->PostMessage(WM_DUTY_RESPONSE, reinterpret_cast<WPARAM>(uniqueId), static_cast<LPARAM>(TA_Base_App::TA_ControlStation::TIMEOUT));

            m_timeCtrl.SetWindowText(POST_TIMEOUT_FORMAT);
        }
	}
	
	CDialog::OnTimer(nIDEvent);
}


BOOL DutyRequestDialog::create()
{
	return CDialog::Create(DutyRequestDialog::IDD, m_parent);
}


LRESULT DutyRequestDialog::OnCancel(WPARAM, LPARAM)
{
    // Disable the sound

    if (m_soundInterface != NULL)
    {
        try
        {
            m_soundInterface->setPriority(TA_Base_App::TA_ControlStation::CS_DUTY_SOUND_CATEGORY, 0);
        }
        // If there are any exceptions, just log and continue
        catch(TA_Base_Core::SoundPlayerException& ex)
        {
            std::string message("Caught while attempting to clear sound for Duty Request dialog, with error: ");
            message += ex.what();
            LOG_EXCEPTION_CATCH(SourceInfo,"SoundPlayerException",message);
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception","Caught an exception while attempting to clear sound.");
        }
    }
    m_parent->PostMessage(WM_CANCEL_CHILD, reinterpret_cast<WPARAM>(this));
	DestroyWindow();
	return S_OK;
}


void DutyRequestDialog::PostNcDestroy()
{
	delete this;
}


void DutyRequestDialog::OnAccept() 
{
    // tell our parent that user has accepted the request
    std::string* uniqueId = new std::string(m_detail->uniqueId);
    m_parent->PostMessage(WM_DUTY_RESPONSE, reinterpret_cast<WPARAM>(uniqueId), static_cast<LPARAM>(TA_Base_App::TA_ControlStation::ACCEPT));
    OnCancel(0, 0);
}

void DutyRequestDialog::OnDeny() 
{
    // tell our parent that user has rejected the request
    std::string* uniqueId = new std::string(m_detail->uniqueId);
    m_parent->PostMessage(WM_DUTY_RESPONSE, reinterpret_cast<WPARAM>(uniqueId), static_cast<LPARAM>(TA_Base_App::TA_ControlStation::DENY));
    OnCancel(0,0);
}

