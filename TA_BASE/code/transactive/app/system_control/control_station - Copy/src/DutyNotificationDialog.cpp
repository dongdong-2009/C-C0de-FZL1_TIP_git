/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/DutyNotificationDialog.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A dialog which lists:
  *  - the duties that this operator has previously requested and gained from operators holding the duties,
  *  - the duties that this operator has previously requested and denied by operators holding the duties, 
  *  - the duties that are lost to another operator.
  *
  * It is possible for the dialog to display the gained and denied requests together in a single dialog.
  */

#include "app/system_control/control_station/src/stdafx.h"
#include "app/system_control/control_station/src/controlstation.h"
#include "app/system_control/control_station/src/DutyNotificationDialog.h"
#include "app/system_control/control_station/src/SessionManager.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/SoundPlayerException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString DutyNotificationDialog::GAIN_FORMAT = "你已经得到权限\n\t%s/%s\n";
const CString DutyNotificationDialog::GAIN_DUTYLESS_FORMAT = "你已经得到权限\n";
const CString DutyNotificationDialog::DENIED_FORMAT = "你的权限请求被拒绝:\n\t%s/%s\n";
const CString DutyNotificationDialog::LOST_FORMAT = "你已失去权限到\n\t%s/%s\n";
const CString DutyNotificationDialog::CHANGED_FORMAT = "你拥有的权限变成\n\t%s/%s\n"; // TD19075
const CString DutyNotificationDialog::CHANGE_DUTYLESS_FORMAT = "你的权限发生变化\n"; // TD19075
const CString DutyNotificationDialog::LOST_DUTYLESS_FORMAT = "你已经失去权限\n";

const CString DutyNotificationDialog::LOCATION_FORMAT = "\t%s\n";
const CString DutyNotificationDialog::SUBSYSTEM_FORMAT= "\t%s\n";


/////////////////////////////////////////////////////////////////////////////
// DutyNotificationDialog dialog


DutyNotificationDialog::DutyNotificationDialog(CWnd* pParent, DutyManager::DutyNotificationDetail* detail)
	: CDialog(DutyNotificationDialog::IDD, NULL), m_parent(pParent), m_detail(detail), m_soundInterface(NULL)
{
    TA_ASSERT(m_parent != NULL, "Parent is NULL");
    TA_ASSERT(m_detail != NULL, "Detail is NULL");
	//{{AFX_DATA_INIT(DutyNotificationDialog)
	//}}AFX_DATA_INIT
}


DutyNotificationDialog::~DutyNotificationDialog()
{
	if( m_soundInterface != NULL )
	{
		delete m_soundInterface;
		m_soundInterface = NULL;
	}
    delete m_detail;
}


void DutyNotificationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DutyNotificationDialog)
	DDX_Control(pDX, IDC_DUTY_TEXT, m_textCtrl);
	// TD19075++
	DDX_Control(pDX, IDC_DUTY_TEXT2, m_textCtrl2);
	DDX_Control(pDX, IDC_DUTY_TEXT3, m_textCtrl3);
	DDX_Control(pDX, IDC_DUTY_TEXT4, m_textCtrl4);
	// ++TD19075
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DutyNotificationDialog, CDialog)
	//{{AFX_MSG_MAP(DutyNotificationDialog)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_FORCE_CANCEL, OnCancel)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// DutyNotificationDialog message handlers

BOOL DutyNotificationDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TD19075
	// set View counts
	int nViewCnt = 0;
	
	// format the content
	CString out;
	if (0 != m_detail->gained.size())
	{
		++nViewCnt;
		out.Empty();
		out = formatContent(GAINED, m_detail->gained);
		m_textCtrl.SetWindowText(out);
		m_textCtrl.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
	}
	
	if (0 != m_detail->lost.size())
	{
		++nViewCnt;
		out.Empty();
		out = formatContent(LOST, m_detail->lost);
		if (1 == nViewCnt)
		{
			m_textCtrl.SetWindowText(out);
			m_textCtrl.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		}
		else
		{
			m_textCtrl2.SetWindowText(out);
			m_textCtrl2.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		}
	}

	if (0 != m_detail->changed.size())
	{
		++nViewCnt;
		out.Empty();
		out = formatContent(CHANGED, m_detail->changed);
		if (1 == nViewCnt)
		{
			m_textCtrl.SetWindowText(out);
			m_textCtrl.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		}
		else if (2 == nViewCnt)
		{
			m_textCtrl2.SetWindowText(out);
			m_textCtrl2.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		}
		else
		{
			m_textCtrl3.SetWindowText(out);
			m_textCtrl3.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		}
	}

	// some denied exist
	if(0 != m_detail->denied.size())
	{
		++nViewCnt;
		out.Empty();
		out = formatContent(DENIED, m_detail->denied);
		if(1 == nViewCnt)
		{
			m_textCtrl.SetWindowText(out);
			m_textCtrl.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		}
		else if(2 == nViewCnt)
		{
			m_textCtrl2.SetWindowText(out);
			m_textCtrl2.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		}
		else if(3 == nViewCnt)
		{
			m_textCtrl3.SetWindowText(out);
			m_textCtrl3.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		}
		else
		{
			m_textCtrl4.SetWindowText(out);
			m_textCtrl4.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		}
	}
	
	setView(nViewCnt);
	// ++TD19075
    // if we can get the current cursor position, then centre the dialog on that screen
    // otherwise, just centre it on the middle screen.
    CPoint pt;
    RECT screen, rect, newRect;
    if (!GetCursorPos(&pt))
    {
        // Make the Window always on top.
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE);
        return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

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
            std::string message("Caught while attempting to initialise sound for Duty Notification dialog, with error: ");
            message += ex.what();
            LOG_EXCEPTION_CATCH(SourceInfo,"SoundPlayerException",message);
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception","Caught an exception while attempting to initialise sound. No sound will be played");
        }
    }

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


CString DutyNotificationDialog::formatContent(const EMessageType& type, const std::vector<DutyManager::DutyDetail>& data)
{
	CString format, temp, out;
    switch (type)
    {
    case GAINED:
        format = GAIN_FORMAT;
        break;
    case DENIED:
        format = DENIED_FORMAT;
        break;
    case LOST:
        format = LOST_FORMAT;
        break;
        // TD19075++
	case CHANGED:
        format = CHANGED_FORMAT;
        break;
        // ++19075
    default:
        // shouldn't happen
        TA_ASSERT(false, "Unknown type");
    }

    for (unsigned int i = 0; i < data.size(); i++)
    {
        if (!data[i].operatorName.empty())
        {
            temp.Format(format, data[i].operatorName.c_str(), data[i].profileName.c_str());
        }
        else
        {
            if (type == GAINED)
            {
                temp.Format(GAIN_DUTYLESS_FORMAT);
            }
            else if (type == LOST)
            {
                temp.Format(LOST_DUTYLESS_FORMAT);
            } // 19075++
			else if (CHANGED == type)
			{
				temp.Format(CHANGE_DUTYLESS_FORMAT);
			} // ++19075
        }

        out += temp;

        for (unsigned int j = 0; j < data[i].subsystem.size(); j++)
        {
			out += "\n下列系统:\n";
            for (unsigned int k = 0; k < data[i].subsystem[j].subsystemName.size(); k++)
            {
                temp.Format(SUBSYSTEM_FORMAT, data[i].subsystem[j].subsystemName[k].c_str());
                out += temp;
            }

            out += "在:";
            temp.Format(LOCATION_FORMAT, data[i].subsystem[j].regionName.c_str());
            out += temp;
        }
        out += "\n";
    }	
    return out;
}


BOOL DutyNotificationDialog::create()
{
	return CDialog::Create(DutyNotificationDialog::IDD, m_parent);
}


LRESULT DutyNotificationDialog::OnCancel(WPARAM, LPARAM)
{
    // If the sound interface exists, then silence it
    if (NULL != m_soundInterface)
    {
        try
        {
            m_soundInterface->setPriority(TA_Base_App::TA_ControlStation::CS_DUTY_SOUND_CATEGORY, 0);
        }
        // If there are any exceptions, just log and continue
        catch(TA_Base_Core::SoundPlayerException& ex)
        {
            std::string message("Caught while attempting to clear sound for Duty Notification, with error: ");
            message += ex.what();
            LOG_EXCEPTION_CATCH(SourceInfo,"SoundPlayerException",message);
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception","Caught an exception while attempting to clear Duty Notificaiton sound.");
        }
    }
    m_parent->PostMessage(WM_CANCEL_CHILD, reinterpret_cast<WPARAM>(this));
	DestroyWindow();
	return S_OK;
}


void DutyNotificationDialog::PostNcDestroy()
{
	delete this;
}



void DutyNotificationDialog::OnOK() 
{
    // We want OK and Cancel to do the same thing (i.e. close and delete the dialog), so call OnCancel from within
    // OnOK.
    OnCancel(0, 0);
}

// TD19075
void DutyNotificationDialog::setView(int cnt)
{
	if (0 == cnt)
	{
		return;
	}
	CRect rectAll, rectLine3, rectLine2, rectLine1, rectB;
	GetWindowRect(&rectAll);
	GetDlgItem(IDC_LINE3)->GetWindowRect(&rectLine3);
	GetDlgItem(IDC_LINE2)->GetWindowRect(&rectLine2);
	GetDlgItem(IDC_LINE1)->GetWindowRect(&rectLine1);
	GetDlgItem(IDOK)->GetWindowRect(&rectB);
	int nbtPos = (rectB.right - rectB.left + 1) / 2;
	
	if(3 == cnt)
	{
		int nPosA = (rectLine3.right - rectAll.left + 1) / 2;
		ScreenToClient(&rectLine3);
		SetWindowPos(NULL, rectAll.left, rectAll.top, rectLine3.right + 1, rectAll.Height(), SWP_NOZORDER);
		ScreenToClient(&rectB);
		rectB.left = nPosA - nbtPos;
		rectB.right = nPosA + nbtPos;
		GetDlgItem(IDOK)->MoveWindow(&rectB);
	}
	else if(2 == cnt)
	{
		int nPosA = (rectLine2.right - rectAll.left + 1) / 2;
		ScreenToClient(&rectLine2);
		SetWindowPos(NULL, rectAll.left, rectAll.top, rectLine2.right + 1, rectAll.Height(), SWP_NOZORDER);
		ScreenToClient(&rectB);
		rectB.left = nPosA - nbtPos;
		rectB.right = nPosA + nbtPos;
		GetDlgItem(IDOK)->MoveWindow(&rectB);
	}
	else if(1 == cnt)
	{
		int nPosA = (rectLine1.right - rectAll.left + 1) / 2;
		ScreenToClient(&rectLine1);
		SetWindowPos(NULL, rectAll.left, rectAll.top, rectLine1.right + 1, rectAll.Height(), SWP_NOZORDER);
		ScreenToClient(&rectB);
		rectB.left = nPosA - nbtPos;
		rectB.right = nPosA + nbtPos;
		GetDlgItem(IDOK)->MoveWindow(&rectB);
	}
	else
	{
		int nPosA = (rectAll.right - rectAll.left + 1) / 2;
		ScreenToClient(&rectB);
		rectB.left = nPosA - nbtPos;
		rectB.right = nPosA + nbtPos;
		GetDlgItem(IDOK)->MoveWindow(&rectB);		
	}
}
// ++TD19075
