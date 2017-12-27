/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/NotesDialog.cpp $
  * @author:  Darren Sampson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the display and update of notes associated with equipment.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "app/scada/Inspector_Panel/src/stdafx.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"
#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/NotesDialog.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/types/InspectorPanelAudit_MessageTypes.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CNotesDialog dialog


	CNotesDialog::CNotesDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CNotesDialog::IDD, pParent), m_notesChanged(false), m_equipment(NULL), 
		  m_accessAllowed(false), m_notesAccessible(false),
		  m_originalHeight (0)
	{
		FUNCTION_ENTRY ("CNotesDialog()");

		//{{AFX_DATA_INIT(CNotesDialog)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT

		FUNCTION_EXIT;
	}


	void CNotesDialog::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY ("DoDataExchange()");

		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CNotesDialog)
		DDX_Control(pDX, IDC_EDITNOTES, m_notesEdit);
		//}}AFX_DATA_MAP

		FUNCTION_EXIT;
	}


	BEGIN_MESSAGE_MAP(CNotesDialog, CDialog)
		//{{AFX_MSG_MAP(CNotesDialog)
		ON_BN_CLICKED(IDC_REMOVE, OnRemove)
		ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
		ON_EN_CHANGE(IDC_EDITNOTES, OnChangeEditnotes)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CNotesDialog message handlers

	void CNotesDialog::OnRemove() 
	{
		FUNCTION_ENTRY ("OnRemove()");

		if (m_equipment != NULL)
		{
            CString actionName;
            actionName.LoadString(IDS_REMOVE_EQUIPMENT_NOTES);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003, "监控面板");        
			
			//TDCL-20167 tianxiang++
			std::stringstream statusLine;
			std::string equipmentDesc;
			equipmentDesc = m_equipment->getDataNode()->getDescription();
			statusLine << equipmentDesc;
			statusLine << " : ";
			//TDCL-20167 tianxiang++

			if (selectedButton == IDYES)
			{
				DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();

				try
				{					
					dataNode->setNotes("", RunParams::getInstance().get(RPARAM_SESSIONID));
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"OnRemove() - %s", e.what());
					
					//TDCL-20167 tianxiang++
					//  [3/14/2010 benxun]
					//statusLine << "Notes Removed Failed: " << e.what();
					statusLine << "笔记移除失败,原因是 " << e.what();
					//  [3/14/2010 benxun]
					std::string* statusLineStr = new std::string(statusLine.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
					//TDCL-20167 tianxiang++
					
					//CInspectorPanelDlg::addStatusLine(e.what());
				}
				//TDCL-20167 tianxiang++
				catch (...)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"OnRemove() Uknow Exception");
					
					//  [3/14/2010 benxun]
					//statusLine << "Notes Removed Failed: " << "Uknow Exception";
					statusLine << "笔记移除失败，原因是" << "未知异常";
					//  [3/14/2010 benxun]
					std::string* statusLineStr = new std::string(statusLine.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
				

				}
				//TDCL-20167 tianxiang++
				DisplayEquipmentNotes();
				
				//TDCL-20167 tianxiang++
				//  [3/14/2010 benxun]
				//statusLine << "Notes Removed Successful.";
				statusLine << "笔记移除成功.";
				//  [3/14/2010 benxun]
				std::string* statusLineStr = new std::string(statusLine.str());
				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
				//TDCL-20167 tianxiang++

			}
		}

		FUNCTION_EXIT;
	}

	void CNotesDialog::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");

		m_equipment = equipment;

		// Update the displayed control
		if (m_hWnd!=0 && m_equipment != NULL) 
		{
			updatePermissions();
			DisplayEquipmentNotes();
		}

		FUNCTION_EXIT;
	}
	void CNotesDialog::displayDataPoints()
	{
		if(m_hWnd != 0 && m_equipment != NULL)
		{
			updatePermissions();
			DisplayEquipmentNotes();
		}
	}
	void CNotesDialog::updateDataPoint(unsigned long key, ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoints()");

		// No data points on this dialog

		FUNCTION_EXIT;
	}

	void CNotesDialog::updateDataNode(ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataNode(ScadaEntityUpdateType updateType)");

		if (m_hWnd!=0) 
		{
			switch (updateType)
			{
			case NotesStateUpdate:
				DisplayEquipmentNotes();
				break;

			default:
				// No updates for this tab
				break;
			}
		}

		FUNCTION_EXIT;
	}

	bool CNotesDialog::isUnsavedChangeOnTab()
	{
		FUNCTION_ENTRY ("isUnsavedChangeOnTab()");
		FUNCTION_EXIT;

		return m_notesChanged;
	}

	bool CNotesDialog::isOkToChangeTab()
	{
		FUNCTION_ENTRY ("isOkToChangeTab()");

		if (m_equipment != NULL)
		{
			if (m_notesChanged && m_accessAllowed)
			{
                CString actionName;
                actionName.LoadString(IDS_UPDATE_EQUIPMENT_NOTES);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003, "监视面板");        

				switch (selectedButton)
				{
				case IDYES:
					return updateNotes();

				case IDNO:
					DisplayEquipmentNotes();
					return true;

				case IDCANCEL:
					return false;

				default:
					DisplayEquipmentNotes();
					return true;
				}
			}
			else
			{
				DisplayEquipmentNotes();
			}
		}

		FUNCTION_EXIT;

		return true;
	}

	void CNotesDialog::DisplayEquipmentNotes()
	{
		FUNCTION_ENTRY ("DisplayEquipmentNotes()");

		std::string notes = "";

		if (m_equipment != NULL)
		{
			m_notesAccessible = false;
			
			DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();

			try
			{
				notes = dataNode->getNotes();
				m_notesAccessible = true;
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"DisplayEquipmentNotes() - %s", e.what());
			}
		}

		m_notesEdit.SetLimitText(2000);
		m_notesEdit.SetWindowText(notes.c_str());
		m_notesChanged = false;
		
		updateButtons();

		FUNCTION_EXIT;
	}

	void CNotesDialog::OnUpdate() 
	{
		FUNCTION_ENTRY ("OnUpdate()");

		// TODO: Add your control notification handler code here

        CString actionName;
        actionName.LoadString(IDS_UPDATE_EQUIPMENT_NOTES);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003, "监视面板");        
		if (selectedButton == IDYES)
		{
			updateNotes();
		}

		FUNCTION_EXIT;
	}

	void CNotesDialog::OnChangeEditnotes() 
	{
		FUNCTION_ENTRY ("OnChangeEditnotes()");

		// TODO: If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialog::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.
		
		// TODO: Add your control notification handler code here

		m_notesChanged = true;
		CString notesText;
		m_notesEdit.GetWindowText(notesText);
		
		updateButtons();

		FUNCTION_EXIT;
	}

	void CNotesDialog::updateButtons()
	{
		FUNCTION_ENTRY ("updateButtons()");

		if (m_equipment != NULL)
		{
			CString notesText;
			
			m_notesEdit.GetWindowText(notesText);

			GetDlgItem(IDC_REMOVE)->EnableWindow(notesText.GetLength() != 0 && m_accessAllowed && m_notesAccessible);									
			GetDlgItem(IDC_UPDATE)->EnableWindow(m_notesChanged && m_accessAllowed && m_notesAccessible);

			m_notesEdit.EnableWindow(m_accessAllowed && m_notesAccessible);
		}

		FUNCTION_EXIT;
	}

	bool CNotesDialog::updateNotes()
	{
		FUNCTION_ENTRY ("updateNotes()");

		if (m_equipment != NULL)
		{
			CString notesText;
			m_notesEdit.GetWindowText(notesText);
			std::string notes = notesText;
			

			//TDCL-20167 tianxiang++
			std::stringstream statusLine;
			std::string equipmentDesc;
			equipmentDesc = m_equipment->getDataNode()->getDescription();
			statusLine << equipmentDesc;
			statusLine << " : ";
			//TDCL-20167 tianxiang++
			

			try
			{
				DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();
				
				dataNode->setNotes(notes, RunParams::getInstance().get(RPARAM_SESSIONID));
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"updateNotes() - %s", e.what());

				//CInspectorPanelDlg::addStatusLine(e.what());

				DisplayEquipmentNotes();
				//TDCL-20167 tianxiang++
				//  [3/14/2010 benxun]
				//statusLine << "Notes Added Failed : " <<e.what();
				statusLine << "笔记添加失败，原因是: " <<e.what();
				//  [3/14/2010 benxun]
				std::string* statusLineStr = new std::string(statusLine.str());
				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
				//TDCL-20167 tianxiang++
				return false;
			}
			//TDCL-20167 tianxiang++
			catch(...)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"updateNotes() Uknown Exception");

				//CInspectorPanelDlg::addStatusLine(e.what());

				DisplayEquipmentNotes();
				
				//  [3/14/2010 benxun]
				//statusLine << "Notes Added Failed: " << "Unknown Exception";
				statusLine << "笔记添加失败: " << "未知异常";
				//  [3/14/2010 benxun]
				std::string* statusLineStr = new std::string(statusLine.str());
				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
		
				return false;
			}
			//TDCL-20167 tianxiang++
			DisplayEquipmentNotes();
			//TDCL-20167 tianxiang++
			//  [3/14/2010 benxun]
			//statusLine << "Notes Added Successful.";
			statusLine << "笔记添加成功.";
			//  [3/14/2010 benxun]
			std::string* statusLineStr = new std::string(statusLine.str());
			AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
			//TDCL-20167 tianxiang++

			return true;
		}

		FUNCTION_EXIT;

		return false;
	}
	
	void CNotesDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		FUNCTION_ENTRY ("OnShowWindow()");

		CDialog::OnShowWindow(bShow, nStatus);
			
		if (bShow)
		{
			DisplayEquipmentNotes();
		}

		FUNCTION_EXIT;
	}

	void CNotesDialog::getWindowRect(CRect* rect)
	{
		GetWindowRect(rect);
	}

	unsigned int CNotesDialog::getMinimumHeight()
	{
		if (m_originalHeight == 0)
		{
			CRect dialogRect;
			GetWindowRect(&dialogRect);
			dialogRect.NormalizeRect();

			m_originalHeight = dialogRect.Height();
		}

		return m_originalHeight;
	}		

	void CNotesDialog::resize(unsigned int height)
	{
		CRect updateRect, removeRect, editRect, dialogRect;
		m_notesEdit.GetWindowRect(&editRect);
		GetDlgItem(IDC_REMOVE)->GetWindowRect(&removeRect);									
		GetDlgItem(IDC_UPDATE)->GetWindowRect(&updateRect);
		GetWindowRect(&dialogRect);

		ScreenToClient(updateRect);
		ScreenToClient(removeRect);
		dialogRect.NormalizeRect();
		editRect.NormalizeRect();

		int diff = height - dialogRect.Height();

		if (diff > 0)
		{
			dialogRect.InflateRect(0, 0, 0, diff);
			editRect.InflateRect(0, 0, 0, diff);
		}
		else
		{
			dialogRect.DeflateRect(0, 0, 0, -diff);
			editRect.InflateRect(0, 0, 0, diff);
		}

		updateRect.OffsetRect(0, diff);
		removeRect.OffsetRect(0, diff);


		GetDlgItem(IDC_REMOVE)->SetWindowPos(NULL, removeRect.left, removeRect.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		GetDlgItem(IDC_UPDATE)->SetWindowPos(NULL, updateRect.left, updateRect.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		m_notesEdit.SetWindowPos(NULL, 0, 0, editRect.Width(), editRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
		SetWindowPos(NULL, 0, 0, dialogRect.Width(), dialogRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
	}

	void CNotesDialog::updatePermissions(/*TA_Base_Bus::RightsLibrary* rightsMgr*/)
	{
		m_accessAllowed = false;

		if (m_equipment != NULL)
		{
			unsigned long equipmentKey = m_equipment->getDataNode()->getEntityKey();

			if (equipmentKey != 0L)
			{
				m_accessAllowed = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
														equipmentKey,
														TA_Base_App::AccessControlHelper::DATA_NODE_NOTES);
			} 
			else
			{
				m_accessAllowed = false;
			}
		}
	
/*		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

		try
		{
			m_accessAllowed = rightsMgr->isActionPermittedOnResource(
											sessionId, 
											resourceId, 
											TA_Base_Bus::aca_IP_NOTES_TAB_ACCESS, 
											reason, 
											decisionModule);     
		}
		catch (...)
		{
		}
*/
		DisplayEquipmentNotes();
	}

	bool CNotesDialog::isAnyDataPointsDisplayedInTab() const
	{
		// no data point on the Notes page, default to true
		return true;
	}

	void CNotesDialog::resetList()
	{

	}
}

