/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: $
  * @author 
  * @version $Revision:  $
  * Last modification: $DateTime: $
  * Last modified by:  $Author:  $
  * 
  * This Class is responsible for performing GUI related function such as 
  * ChangeFocus, Move window etc.
  */

#include "stdafx.h"
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#include "GUINotificationObserver.h"
#include "core\utilities\src\DebugUtil.h"


using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;
using namespace System::Drawing;

using TA_Base_Core::DebugUtil;

namespace NET_ISCS_Bridge 
{

	GUINotificationObserver::GUINotificationObserver()
	{
		m_pMainFrm = nullptr;
	}

	void GUINotificationObserver::SetMainFrm(Form^ frm)
	{
		m_pMainFrm = frm;
	}

	void GUINotificationObserver::ChangeFocus(ManagedEFocusType focus)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "ChangeFocus" );
		if(m_pMainFrm)
		{
			if (m_pMainFrm->InvokeRequired)
			{
				array<Object^>^ myArray = gcnew array<Object^>(1);
				myArray[0] = focus;
				m_pMainFrm->BeginInvoke(gcnew ChangeFocusEvent(this, &GUINotificationObserver::ChangeFocusInUIThread),myArray);
			}
			else
			{
				ChangeFocusInUIThread(focus);
			}
		}

		  LOG( SourceInfo, DebugUtil::FunctionExit, "ChangeFocus" );
	}

	void GUINotificationObserver::ChangeFocusInUIThread(ManagedEFocusType focus)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "ChangeFocusInUIThread" );
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Change Focus to: (%d)",focus);
		FormCollection ^ forms = Application::OpenForms;
		switch (focus)
		{
		case ManagedEFocusType::Minimise:
			for each(Form^ frm in forms)
			{
				frm->WindowState = FormWindowState::Minimized;
			}
			break;
		case ManagedEFocusType::Maximise:
			for each(Form^ frm in forms)
			{
				frm->WindowState = FormWindowState::Maximized;
				frm->Activate();
			}          
			break;
		case ManagedEFocusType::GetFocus:
			for each(Form^ frm in forms)
			{
				frm->WindowState = FormWindowState::Normal;
				frm->Activate();
			}
			break;
		}
		 LOG( SourceInfo, DebugUtil::FunctionExit, "ChangeFocusInUIThread" );
	}

	void GUINotificationObserver::SetPosition(ManagedRECT ^ windowRect,ManagedEWindowPosition windowPos)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "SetPosition" );
		if(m_pMainFrm)
		{
			if (m_pMainFrm->InvokeRequired)
			{
				array<Object^>^ myArray = gcnew array<Object^>(2);
				myArray[0] = windowRect;
				myArray[1] = windowPos;
				m_pMainFrm->BeginInvoke(gcnew SetPositionEvent(this, &GUINotificationObserver::SetPositionInUIThread),myArray);
			}
			else
			{
				SetPositionInUIThread(windowRect,windowPos);
			}
		}
	}


	void GUINotificationObserver::SetPositionInUIThread(ManagedRECT ^ windowRect,ManagedEWindowPosition windowPos)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "SetPositionInUIThread" );
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Change Form Position to: (%li,%li,%li,%li) and windowPosition %d",windowRect->left,windowRect->top,windowRect->right,windowRect->bottom,windowPos);
		if(windowPos == ManagedEWindowPosition::MINIMISE)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "SetPositionInUIThread to minisize");
			ChangeFocus(ManagedEFocusType::Minimise);
			return;
		}
		if(windowPos != ManagedEWindowPosition::REPOSITION_NO_RESTORE)
        {
            if(m_pMainFrm->WindowState == FormWindowState::Minimized)
            {
				ChangeFocus(ManagedEFocusType::GetFocus);
            }
        }

		switch (windowPos)
        {
		case ManagedEWindowPosition::MAXIMISE:
			ChangeFocus(ManagedEFocusType::Maximise);
			break;
		case ManagedEWindowPosition::FOCUS:
		case ManagedEWindowPosition::NO_REPOSITION:
            break;
        case ManagedEWindowPosition::REPOSITION:
        case ManagedEWindowPosition::REPOSITION_NO_RESTORE:
            {					
				
                if(m_pMainFrm)
				{
					m_pMainFrm->Location = Point(windowRect->left,windowRect->top);
					// --- Now application have fixed height and width.. no need to change now.
					//int height = windowRect->bottom - windowRect->top;
					//int width = windowRect->right - windowRect->left;
					//m_pMainFrm->Height = height; 
					//m_pMainFrm->Width = width;
					//log new coordinates
				}

				FormCollection ^ forms = Application::OpenForms;
				for each(Form^ frm in forms)
				{
					if(frm->TopMost)
					{
						frm->StartPosition = FormStartPosition::CenterScreen;
					}
				}				
            }
            break;
        case ManagedEWindowPosition::CENTRE:
            if(m_pMainFrm)
			{
				m_pMainFrm->StartPosition  = FormStartPosition::CenterScreen;
			}
            break;
        default:
			break;
        }

		//Bring application to focus..
		ChangeFocus(ManagedEFocusType::GetFocus);
	}

	ManagedRECT^ GUINotificationObserver::GetWindowCoordinates()
	{
		ManagedRECT^ rec = gcnew ManagedRECT();
		//Bounds - Gets or sets the size and location of the control including its nonclient elements, 
		//              in pixels, relative to the parent control.
		if(m_pMainFrm)
		{
			Rectangle^ rect = m_pMainFrm->Bounds ;
			rec->left = 	rect->Left;
			rec->top = rect->Top;
			rec->right=rect->Right;
			rec->bottom= rect->Bottom;
		}
		return rec;
	}

	bool GUINotificationObserver::IsMainFrm()
	{
		if(m_pMainFrm != nullptr)
			return true;
		return false;
	}

	void  GUINotificationObserver::setTopPosition(long top)
	{
		Point loc = m_pMainFrm->Location;
		loc.Y = top;
		m_pMainFrm->Location = loc;
	}
}
