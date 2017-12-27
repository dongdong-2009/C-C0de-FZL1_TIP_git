/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/TagDialog.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the display and update of tag settings associated with equipment.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/ConfirmTagDialog.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/TagDialog.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"

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
	// CTagDialog dialog

	CTagDialog::CTagDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTagDialog::IDD, pParent), 
        m_equipment (NULL), 
        m_tagAccessAllowed(false),
        m_ptwTagAccessAllowed(false),
        m_originalHeight(0)
	{
		FUNCTION_ENTRY ("CTagDialog()");

		//{{AFX_DATA_INIT(CTagDialog)
		//}}AFX_DATA_INIT

		FUNCTION_EXIT;
	}


	void CTagDialog::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY ("DoDataExchange()");

		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CTagDialog)
		DDX_Control(pDX, IDC_SCANINHIBIT_CHECK, m_scanInhibitCheck);
		//DDX_Control(pDX, IDC_MMSINHIBIT_CHECK, m_mmsInhibitCheck);
		DDX_Control(pDX, IDC_CONTROLINHIBIT_CHECK, m_controlInhibitCheck);
		DDX_Control(pDX, IDC_ALARMINHIBIT_CHECK, m_alarmInhibitCheck);
		DDX_Control(pDX, IDC_PTW_ID, m_ptwIdEdit);
		DDX_Control(pDX, IDC_COMMENT, m_commentEdit);
	//}}AFX_DATA_MAP

		FUNCTION_EXIT;
	}


	BEGIN_MESSAGE_MAP(CTagDialog, CDialog)
		//{{AFX_MSG_MAP(CTagDialog)
		ON_BN_CLICKED(IDC_APPLY_TAG, onApplyTag)
		ON_BN_CLICKED(IDC_RADIO_TAG_PTW, OnRadioTagPtw)
		ON_BN_CLICKED(IDC_RADIO_NOT_TAGGED, OnRadioNotTagged)
		ON_BN_CLICKED(IDC_RADIO_TAG, OnRadioTag)
		ON_EN_CHANGE(IDC_PTW_ID, OnChangeEdit)
		ON_WM_SHOWWINDOW()
		ON_WM_PAINT()
		ON_EN_CHANGE(IDC_COMMENT, OnChangeEdit)
		ON_BN_CLICKED(IDC_ALARMINHIBIT_CHECK, OnAlarmInhibitCheck)
		ON_BN_CLICKED(IDC_CONTROLINHIBIT_CHECK, OnControlInhibitCheck)
		ON_BN_CLICKED(IDC_MMSINHIBIT_CHECK, OnMmsInhibitCheck)
		ON_BN_CLICKED(IDC_SCANINHIBIT_CHECK, OnScanInhibitCheck)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CTagDialog message handlers

	void CTagDialog::onApplyTag() 
	{
		FUNCTION_ENTRY ("onApplyTag()");

		applyTag();

		FUNCTION_EXIT;
	}


	bool CTagDialog::applyTag()
	{
		FUNCTION_ENTRY ("applyTag()");

		if (m_equipment == NULL)
		{
			FUNCTION_EXIT;
			return false;
		}
		int tagRadioButton = GetCheckedRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW);
		
		CString tagPtwId, tagComment;
		CConfirmTagDialog   dlg;
		
		m_ptwIdEdit.GetWindowText(tagPtwId);
		m_commentEdit.GetWindowText(tagComment);

		DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();

		dlg.m_equipment = dataNode->getAssetName().c_str();
		dlg.m_equipment += "\r\n";
		dlg.m_equipment += dataNode->getDescription().c_str();
	
		if (!(true == m_tagAccessAllowed || true == m_ptwTagAccessAllowed))
		{
			FUNCTION_EXIT;
			return false;
		}

		// Format Initial status string
		switch (tagRadioButton)
		{
		case IDC_RADIO_TAG:
				//dlg.m_tag = "Tag without PTW";
				dlg.m_tag = "不含PTW标签";
				break; 
		case IDC_RADIO_TAG_PTW:
				//dlg.m_tag = "Tag with PTW \'";
				dlg.m_tag = "含PTW标签 PTW \'";
				dlg.m_tag += tagPtwId;
				dlg.m_tag += "\'";
				break;
		case IDC_RADIO_NOT_TAGGED:
				//dlg.m_tag = "Remove Tag";
				dlg.m_tag = "取消标签";
				break;
		}


		// Work out inhibit details
		TA_Base_Bus::InhibitState		inhibitState;
		std::string						inhibitString = "";
		
        inhibitState.inputInhibit = TA_Base_Bus::NoInputInhibit;
		inhibitState.outputInhibit = TA_Base_Bus::NoOutputInhibit;

		if (m_scanInhibitChecked)
		{
			//inhibitString = "Inhibit scan";
			inhibitString = "禁止扫描";
			inhibitState.inputInhibit = TA_Base_Bus::InhibitScan;
		}
		else if (m_alarmInhibitChecked)
		{
			//inhibitString = "Inhibit alarm";
			inhibitString = "禁止报警";
			inhibitState.inputInhibit = TA_Base_Bus::InhibitAlarm;
		}
		//else if (m_mmsInhibitChecked)
		//{
		//	inhibitString = "Inhibit MMS";
		//	inhibitState.inputInhibit = TA_Base_Bus::InhibitMmsAlarm;
		//}

		if (m_controlInhibitChecked)
		{
			if (inhibitString == "")
			{
				//inhibitString = "Inhibit control";
				inhibitString = "禁止控制";
			}
			else
			{
				//inhibitString += " and control";
				inhibitString += "和控制";
			}
	
			inhibitState.outputInhibit = TA_Base_Bus::InhibitControl;
		}
		
		if (inhibitString == "")
		{
			if (tagRadioButton == IDC_RADIO_NOT_TAGGED)
			{
				if (m_originalScanInhibitChecked)
				{
					//inhibitString = "Enable scan";
					inhibitString = "恢复扫描";
				}
				else if (m_originalAlarmInhibitChecked)
				{
					//inhibitString = "Enable alarm";
					inhibitString = "恢复报警";
				}
				//else if (m_originalMmsInhibitChecked)
				//{
				//	inhibitString = "Enable MMS";
				//}

				if (m_originalControlInhibitChecked)
				{
					if (inhibitString == "")
					{
						//inhibitString = "Enable control";
						inhibitString = "恢复控制";
					}
					else
					{
						//inhibitString += " and control";
						inhibitString += "和控制";
					}
				}

				if (inhibitString == "")
				{
					//inhibitString = "Inhibition unchanged";
					inhibitString = "禁止项无变化";
				}
			}
			else
			{
				//inhibitString = "No Inhibition";
				inhibitString = "无禁止";
			}
		}
		
		dlg.m_inhibit = inhibitString.c_str();

		// Populate dialog
		bool apply = (dlg.DoModal()==IDOK);

		if (apply)
		{
			std::stringstream statusLine;
			//  [3/15/2010 benxun]
			//statusLine << "Equipment Tag";
			statusLine << "设备标签";
			//  [3/15/2010 benxun]

			// tag/un-tag equipment
			try
			{					
				std::string ptwId = tagPtwId;
				std::string comment = tagComment;

				std::string message = "Applying tag state \"" + dlg.m_tag;
				message += "/" + dlg.m_inhibit;
				message += "\" to Equipment " + dataNode->getAssetName() + ".";

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"applyTag() - %s", message.c_str());
                
                std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

				switch (tagRadioButton)
				{
				case IDC_RADIO_TAG:
					{
						// if no inhibition selected, compile the message to be displayed in Status Box
                        if ( ( TA_Base_Bus::NoInputInhibit != inhibitState.inputInhibit ) ||
							 ( TA_Base_Bus::NoOutputInhibit != inhibitState.outputInhibit ) )
						{
							//  [3/15/2010 benxun]
							//statusLine << " with";
							statusLine << "存在";
							//  [3/15/2010 benxun]

							if ( TA_Base_Bus::InhibitScan == inhibitState.inputInhibit )
							{
								//  [3/15/2010 benxun]
								//statusLine << " Scan Inhibit";
								statusLine << "禁止扫描";
								//  [3/15/2010 benxun]
							}
							else if ( TA_Base_Bus::InhibitAlarm == inhibitState.inputInhibit )
							{
								//  [3/15/2010 benxun]
								//statusLine << " Alarm Inhibit";
								statusLine << "禁止报警";
								//  [3/15/2010 benxun]
							}
							//else if ( TA_Base_Bus::InhibitMmsAlarm == inhibitState.inputInhibit )
							//{
							//	statusLine << " MMS Inhibit";
							//}

							// wenching++ (TD13621)
							if ( TA_Base_Bus::InhibitControl == inhibitState.outputInhibit )
							{
								//  [3/15/2010 benxun]
								//statusLine << " Control Inhibit";
								statusLine << "控制禁止";
								//  [3/15/2010 benxun]
							}					
							// ++wenching (TD13621)
						}

						// tag the equipment now
						dataNode->setTagState(Tagged, inhibitState.outputInhibit, inhibitState.inputInhibit, 
											 "", comment, sessionId);

						// show message in Status Box
						//  [3/14/2010 benxun]
						//statusLine << " ON Successful.";
						statusLine << " 设置成功.";
						//  [3/14/2010 benxun]
						std::string* statusLineStr = new std::string(statusLine.str());
						AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

						break; 
					}

				case IDC_RADIO_TAG_PTW:
					{
						//statusLine << " With PTW";
						statusLine << "含PTW";

						// if no inhibition selected, compile the message to be displayed in Status Box
						if ( ( TA_Base_Bus::NoInputInhibit != inhibitState.inputInhibit ) ||
							 ( TA_Base_Bus::NoOutputInhibit != inhibitState.outputInhibit ) )
						{
							//statusLine << " with";
							statusLine << "并且";

							if ( TA_Base_Bus::InhibitScan == inhibitState.inputInhibit )
							{
								//statusLine << " Scan Inhibit";
								statusLine << "禁止扫描";
							}

							if ( TA_Base_Bus::InhibitAlarm == inhibitState.inputInhibit )
							{
								//statusLine << " Alarm Inhibit";
								statusLine << "禁止报警";
							}

							//if ( TA_Base_Bus::InhibitMmsAlarm == inhibitState.inputInhibit )
							//{
							//	statusLine << " MMS Inhibit";
							//}

							// wenching++ (TD13621)
							if ( TA_Base_Bus::InhibitControl == inhibitState.outputInhibit )
							{
								//statusLine << " Control Inhibit";
								statusLine << "禁止控制";
							}	
							// ++wenching (TD13621)
						}

						// tag the equipment now
						dataNode->setTagState(TaggedPtw, inhibitState.outputInhibit, inhibitState.inputInhibit, 
											 ptwId, comment, sessionId);						

						// show message in Status Box
						//  [3/14/2010 benxun]
						//statusLine << " ON Successful.";
						statusLine << " 成功.";
						//  [3/14/2010 benxun]
						std::string* statusLineStr = new std::string(statusLine.str());
						AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

						break;
					}

				case IDC_RADIO_NOT_TAGGED:
					{
						dataNode->setTagState(TA_Base_Bus::NotTagged, TA_Base_Bus::NoOutputInhibit, TA_Base_Bus::NoInputInhibit, "", "", sessionId);

						//  [3/14/2010 benxun]
						//statusLine << " OFF Successful.";
						statusLine << " 取消成功.";
						//  [3/14/2010 benxun]
						std::string* statusLineStr = new std::string(statusLine.str());
						AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
						
						break;
					}
				}

				message = "Tag state \"" + dlg.m_tag;
				message += "/" + dlg.m_inhibit;
				message += "\" applied to Equipment " + dataNode->getAssetName() + ".";

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"applyTag() - %s", message.c_str());

			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"applyTag() - %s", e.what());

				if ( IDC_RADIO_NOT_TAGGED == tagRadioButton )
				{
					//  [3/14/2010 benxun]
					//statusLine << " OFF Failed.";
					statusLine << " 取消失败.";
					//  [3/14/2010 benxun]
				}
				else
				{
					//  [3/14/2010 benxun]
					//statusLine << " ON Failed.";
					statusLine << " 启动失败.";
					//  [3/14/2010 benxun]
				}

				std::string* statusLineStr = new std::string(statusLine.str());
				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

				apply = false;
			}
			catch (...)
			{
				std::string message="Error: Unknown exception when tagging Equipment " + 
									dataNode->getAssetName() + ".";

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"applyTag() - %s", message.c_str());

				if ( IDC_RADIO_NOT_TAGGED == tagRadioButton )
				{
					//  [3/14/2010 benxun]
					//statusLine << " OFF Failed.";
					statusLine << " 取消失败.";
					//  [3/14/2010 benxun]
				}
				else
				{
					//  [3/14/2010 benxun]
					//statusLine << " ON Failed.";
					statusLine << " 启动失败.";
					//  [3/14/2010 benxun]
				}

				std::string* statusLineStr = new std::string(statusLine.str());
				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

				apply = false;
			}

			DisplayEquipmentTagStatus();
		}
	
		FUNCTION_EXIT;
		return apply;
	}

	void CTagDialog::OnRadioTagPtw() 
	{
		FUNCTION_ENTRY ("OnRadioTagPtw()");

		CheckRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW, IDC_RADIO_TAG_PTW);	

		/* fTD220 - check all inhibits for tag-ptw by default */
		m_scanInhibitChecked = true;
		m_scanInhibitCheck.SetCheck(m_scanInhibitChecked);
		
		m_alarmInhibitChecked = m_scanInhibitChecked ? true : false;
		m_alarmInhibitCheck.SetCheck(m_alarmInhibitChecked);
		
		//m_mmsInhibitChecked = (m_scanInhibitChecked || m_alarmInhibitChecked) ? true : false;
		//m_mmsInhibitCheck.SetCheck(m_mmsInhibitChecked);
		
		m_controlInhibitChecked = true;
		m_controlInhibitCheck.SetCheck(m_controlInhibitChecked);

		GetDlgItem(IDC_APPLY_TAG)->EnableWindow(isUnsavedChangeOnTab());

		updateRadioButtonAccess();
		checkPtwTextCommentField();	//wenching (TD13741)

		m_commentEdit.EnableWindow(true);
		m_ptwIdEdit.EnableWindow(true);
		m_ptwIdEdit.SetLimitText(40);

		FUNCTION_EXIT;
	}

	void CTagDialog::OnRadioNotTagged() 
	{
		FUNCTION_ENTRY ("OnRadioNotTagged()");

		CheckRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW, IDC_RADIO_NOT_TAGGED);	
		
		GetDlgItem(IDC_APPLY_TAG)->EnableWindow(isUnsavedChangeOnTab());
		
		updateRadioButtonAccess();
		checkPtwTextCommentField(); //wenching (TD13741)

		m_commentEdit.EnableWindow(false);
		m_ptwIdEdit.EnableWindow(false);

		FUNCTION_EXIT;
	}

	void CTagDialog::OnRadioTag() 
	{
		FUNCTION_ENTRY ("OnRadioTag()");

		CheckRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW, IDC_RADIO_TAG);

		/* fTD220 - check all inhibits for tag-ptw by default */
		m_scanInhibitChecked = true;
		m_scanInhibitCheck.SetCheck(m_scanInhibitChecked);
		
		m_alarmInhibitChecked = m_scanInhibitChecked ? true : false;
		m_alarmInhibitCheck.SetCheck(m_alarmInhibitChecked);
		
		//m_mmsInhibitChecked = (m_scanInhibitChecked || m_alarmInhibitChecked) ? true : false;
		//m_mmsInhibitCheck.SetCheck(m_mmsInhibitChecked);
		
		m_controlInhibitChecked = true;
		m_controlInhibitCheck.SetCheck(m_controlInhibitChecked);

		GetDlgItem(IDC_APPLY_TAG)->EnableWindow(isUnsavedChangeOnTab());

		updateRadioButtonAccess();
		checkPtwTextCommentField(); //wenching (TD13741)

		m_commentEdit.EnableWindow(true);
		m_ptwIdEdit.EnableWindow(false);

		FUNCTION_EXIT;
	}

	/* SetEquipment
	 *
	 * This function will set the name of the equipment that the data points are to be displayed for.It can 
	 * be called before or after the windows control is on the screen.
	 */
	void CTagDialog::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");

		m_equipment = equipment;

		// Update the displayed control
		if (m_hWnd!=0 && m_equipment != NULL) 
		{
			/**********************************************
			 ** Defect 4986: Tag display would only become 
			 ** active intermittently. Added this line because 
			 ** updatePermissions() wasn't completeing because 
			 ** m_equipment was NULL when it was called.
			 **/
			/**********************************************/
			/*
			updatePermissions();
			
			DisplayEquipmentTagStatus();
			*/
		}

		FUNCTION_EXIT;
	}
	void CTagDialog::displayDataPoints()
	{
		FUNCTION_ENTRY ("displayDataPoints()");
		if (m_hWnd!=0 && m_equipment != NULL) 
		{			
			updatePermissions();
			
			DisplayEquipmentTagStatus();
		}

		FUNCTION_EXIT;
	}
	void CTagDialog::updateDataPoint(unsigned long key, ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoints()");

		// No data points on this dialog

		FUNCTION_EXIT;
	}

	void CTagDialog::updateDataNode(ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataNode()");

		if (m_hWnd!=0) 
		{
			switch (updateType)
			{
			
			case TA_Base_Bus::TagStateUpdate:
				// Only update on change of tag state or initialisation/invalidation
				DisplayEquipmentTagStatus();
				break;

			default:
				// Do nothing for these updates
				break;
			}
		}

		FUNCTION_EXIT;
	}

	bool CTagDialog::isUnsavedChangeOnTab()
	{
		FUNCTION_ENTRY ("isUnsavedChangeOnTab()");

		int radioButton = GetCheckedRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW);

		bool hasChanged = false;
		CString tagPtwId, tagComment;

		m_ptwIdEdit.GetWindowText(tagPtwId);
		m_commentEdit.GetWindowText(tagComment);

		switch (radioButton)
		{
		case IDC_RADIO_TAG:
			hasChanged = (m_originalTagStatus != TA_Base_Bus::Tagged) || tagComment.Compare(m_originalComment.c_str()) != 0;
			break; 

		case IDC_RADIO_TAG_PTW:
			if (m_ptwTagAccessAllowed)
            {
                hasChanged = (m_originalTagStatus != TA_Base_Bus::TaggedPtw ||
						  tagComment.Compare(m_originalComment.c_str()) != 0 ||
						  tagPtwId.Compare(m_originalPtwId.c_str()) != 0);		
            }
            else
            {
                // Special case - PTW disabled
                hasChanged = false;
            }
            break;

		case IDC_RADIO_NOT_TAGGED:
			hasChanged = (m_originalTagStatus != TA_Base_Bus::NotTagged);
			break;
		}	

		hasChanged |= (m_originalScanInhibitChecked != m_scanInhibitChecked);
		hasChanged |= (m_originalAlarmInhibitChecked != m_alarmInhibitChecked);
		//hasChanged |= (m_originalMmsInhibitChecked != m_mmsInhibitChecked);
		hasChanged |= (m_originalControlInhibitChecked != m_controlInhibitChecked);

		if (!(m_tagAccessAllowed || m_ptwTagAccessAllowed))
		{
			// Special case - no permissions
			hasChanged = false;
		}

		FUNCTION_EXIT;
		return hasChanged;
	}

	bool CTagDialog::isOkToChangeTab()
	{
		FUNCTION_ENTRY ("isOkToChangeTab()");

		if (!isUnsavedChangeOnTab())
		{
			FUNCTION_EXIT;
			return true;
		}

        CString actionName;
        actionName.LoadString(IDS_APPLY_TAG_CHANGES);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003, "监控面板");        

		switch (selectedButton)
		{
		case IDYES:
			FUNCTION_EXIT;
			return applyTag();

		case IDNO:
			DisplayEquipmentTagStatus();
			FUNCTION_EXIT;
			return true;

		case IDCANCEL:
			FUNCTION_EXIT;
			return false;

		default:
			DisplayEquipmentTagStatus();
			FUNCTION_EXIT;
			return true;
		}
	}

	void CTagDialog::DisplayEquipmentTagStatus()
	{
		FUNCTION_ENTRY ("DisplayEquipmentTagStatus()");

		if (m_equipment != NULL && m_hWnd != 0)
		{
			DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();

			try
			{
				m_originalTagStatus = dataNode->getTaggingState();
				TA_Base_Bus::InhibitState tagInhibitStatus = dataNode->getTagInhibitState();

				m_originalScanInhibitChecked = (tagInhibitStatus.inputInhibit == TA_Base_Bus::InhibitScan);
				m_originalAlarmInhibitChecked = (tagInhibitStatus.inputInhibit == TA_Base_Bus::InhibitScan || tagInhibitStatus.inputInhibit == TA_Base_Bus::InhibitAlarm);
				//m_originalMmsInhibitChecked = (tagInhibitStatus.inputInhibit == TA_Base_Bus::InhibitScan || tagInhibitStatus.inputInhibit == TA_Base_Bus::InhibitAlarm || tagInhibitStatus.inputInhibit == TA_Base_Bus::InhibitMmsAlarm);
				m_originalControlInhibitChecked = (tagInhibitStatus.outputInhibit == TA_Base_Bus::InhibitControl);

				m_originalPtwId = dataNode->getPtwId();
				m_originalComment = dataNode->getTagComment();
				 
				switch (m_originalTagStatus)
				{
				case TA_Base_Bus::Tagged:
					CheckRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW, IDC_RADIO_TAG);
					m_commentEdit.EnableWindow();
					m_commentEdit.SetWindowText(m_originalComment.c_str());
					m_ptwIdEdit.EnableWindow(false);
					m_ptwIdEdit.SetWindowText("");
					break; 

				case TA_Base_Bus::TaggedPtw:
					CheckRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW, IDC_RADIO_TAG_PTW);
					m_commentEdit.EnableWindow();
					m_commentEdit.SetWindowText(m_originalComment.c_str());
					m_ptwIdEdit.EnableWindow();
					m_ptwIdEdit.SetWindowText(m_originalPtwId.c_str());
					m_ptwIdEdit.SetLimitText(40);
					break;

				case TA_Base_Bus::NotTagged:
					CheckRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW, IDC_RADIO_NOT_TAGGED);
					m_commentEdit.EnableWindow(false);
					m_commentEdit.SetWindowText("");
					m_ptwIdEdit.EnableWindow(false);
					m_ptwIdEdit.SetWindowText("");
					break;
				}

				m_scanInhibitChecked = m_originalScanInhibitChecked;
				m_scanInhibitCheck.SetCheck(m_scanInhibitChecked);

				m_alarmInhibitChecked = m_scanInhibitChecked?true:m_originalAlarmInhibitChecked;
				m_alarmInhibitCheck.SetCheck(m_alarmInhibitChecked);
				m_alarmInhibitCheck.EnableWindow(!m_scanInhibitChecked);

				//m_mmsInhibitChecked = (m_scanInhibitChecked || m_alarmInhibitChecked)?true:m_originalMmsInhibitChecked;
				//m_mmsInhibitCheck.SetCheck(m_mmsInhibitChecked);
				//m_mmsInhibitCheck.EnableWindow(!(m_scanInhibitChecked || m_alarmInhibitChecked));

				m_controlInhibitChecked = m_originalControlInhibitChecked;
				m_controlInhibitCheck.SetCheck(m_controlInhibitChecked);
				
				// Set up buttons as per Access Control
				updateRadioButtonAccess();
				updatePtwCommentAccess();

				GetDlgItem(IDC_APPLY_TAG)->EnableWindow(false);
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"DisplayEquipmentTagStatus() - %s", e.what());
			}
		}

		FUNCTION_EXIT;
	}

	void CTagDialog::OnChangeEdit() 
	{
		FUNCTION_ENTRY ("OnChangeEdit()");

		if (m_equipment != NULL)
		{		
			// Check access control first and disable apply button if no permissions
			updateRadioButtonAccess();
			updatePtwCommentAccess();
			
			GetDlgItem(IDC_APPLY_TAG)->EnableWindow(isUnsavedChangeOnTab());
			checkPtwTextCommentField(); //wenching++ (TD13741)
		}

		FUNCTION_EXIT;
	}

	// wenching++ (TD12062,TD13140)
	bool CTagDialog::isProfileAccessAllowed(bool buttonChecked, InhibitType type)
	{
		bool inhibitEnable = false;
		bool isInhibitPermitted = false;
		bool isUnInhibitPermitted = false;
		
		switch(type)
		{
		case scanInhibit:	
			isInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
						m_equipment->getDataNode()->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_SCAN_INHIBIT);
			isUnInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
						m_equipment->getDataNode()->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_SCAN_UNINHIBIT);
			break;
		case alarmInhibit:
			isInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
						m_equipment->getDataNode()->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_ALARM_INHIBIT);
			isUnInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
						m_equipment->getDataNode()->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_ALARM_UNINHIBIT);
			break;
		case mmsInhibit:
			isInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
						m_equipment->getDataNode()->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_MMS_INHIBIT);
			isUnInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
						m_equipment->getDataNode()->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_MMS_UNINHIBIT);
			break;
		case controlInhibit:
			isInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
						m_equipment->getDataNode()->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_CONTROL_INHIBIT);
			isUnInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
						m_equipment->getDataNode()->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_CONTROL_UNINHIBIT);
			break;
		}


		// if the operator profile has access to both actions
		if ( ( true == isInhibitPermitted ) && ( true == isUnInhibitPermitted ) )
		{
			inhibitEnable = true;
		}
		// if the operator profile has no access to both actions
		else if ( ( false == isInhibitPermitted ) &&( false == isUnInhibitPermitted ) )
		{
			inhibitEnable = false;
		}
		// if the operator profile has access to one of the actions
		else
		{
			// if the operator profile has access to inhibit but
			// not to uninhibit the scan
			if ( ( true == isInhibitPermitted ) && ( false == isUnInhibitPermitted ) )
			{
				if ( false == buttonChecked )
					inhibitEnable = true;
				else
					inhibitEnable = false;
			}

			// if the operator profile has access to uninhibit but
			// not to inhibit the scan
			else if ( ( false == isInhibitPermitted ) && ( true == isUnInhibitPermitted ) )
			{
				if ( false == buttonChecked )
					inhibitEnable = false;
				else
					inhibitEnable = true;
			}
			// if none of the above conditions
			else
			{
				inhibitEnable = false;
			}
		}

		return inhibitEnable;
	}
	// ++wenching (TD12062,TD13140)
	
	void CTagDialog::updateRadioButtonAccess()
	{
		FUNCTION_ENTRY ("updateRadioButtonAccess()");

		if (m_equipment != NULL)
		{
			//TD15276
			bool enableReset = m_tagAccessAllowed || m_ptwTagAccessAllowed;

			if ( (Tagged == m_originalTagStatus && false == m_tagAccessAllowed)
				|| (TaggedPtw == m_originalTagStatus && false == m_ptwTagAccessAllowed) )
			{
				// Currently tagged option is not equil to the privelege to set/reset the tag option
				enableReset = false;
			}

			GetDlgItem(IDC_RADIO_TAG)->EnableWindow(enableReset && m_tagAccessAllowed);			
            GetDlgItem(IDC_RADIO_TAG_PTW)->EnableWindow(enableReset && m_ptwTagAccessAllowed);
            GetDlgItem(IDC_RADIO_NOT_TAGGED)->EnableWindow(enableReset);

			bool tagged = (GetCheckedRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW) != IDC_RADIO_NOT_TAGGED);

			if (!tagged)
			{
				m_scanInhibitChecked = false;
				m_scanInhibitCheck.SetCheck(false); 
				m_alarmInhibitChecked = false;
				m_alarmInhibitCheck.SetCheck(false);
				//m_mmsInhibitChecked = false;
				//m_mmsInhibitCheck.SetCheck(false); 
				m_controlInhibitChecked = false;
				m_controlInhibitCheck.SetCheck(false);	
			}

			// wenching++ (TD12062,TD13140)
			// Operator profile checking
			bool isInhibitEnable = false;
		
			isInhibitEnable = isProfileAccessAllowed(m_scanInhibitChecked, scanInhibit);
			m_scanInhibitCheck.EnableWindow(isInhibitEnable && tagged && (m_tagAccessAllowed || m_ptwTagAccessAllowed) && m_equipment->hasInformationPoint()); 

			isInhibitEnable = isProfileAccessAllowed(m_alarmInhibitChecked, alarmInhibit);
			m_alarmInhibitCheck.EnableWindow(isInhibitEnable && tagged && (m_tagAccessAllowed || m_ptwTagAccessAllowed) && !m_scanInhibitChecked && m_equipment->hasInformationPoint());
		
			//isInhibitEnable = isProfileAccessAllowed(m_mmsInhibitChecked, mmsInhibit);
			//m_mmsInhibitCheck.EnableWindow(isInhibitEnable && tagged && (m_tagAccessAllowed || m_ptwTagAccessAllowed) && !(m_scanInhibitChecked || m_alarmInhibitChecked) && m_equipment->hasInformationPoint()); 

			isInhibitEnable = isProfileAccessAllowed(m_controlInhibitChecked, controlInhibit);
			m_controlInhibitCheck.EnableWindow(isInhibitEnable && tagged && (m_tagAccessAllowed || m_ptwTagAccessAllowed) && m_equipment->hasControlPoint());	
			// ++wenching (TD12062,TD13140)
		}

		FUNCTION_EXIT;
	}

	void CTagDialog::updatePtwCommentAccess()
	{
		FUNCTION_ENTRY ("updatePtwCommentAccess()");

		if (m_equipment != NULL)
		{
			//TD16016, revert TD15276, must disble window when do not have rights
			if (false == m_ptwTagAccessAllowed)
			{
				m_ptwIdEdit.EnableWindow(false);
			}

			if (false == m_tagAccessAllowed && false == m_ptwTagAccessAllowed)
			{
				m_commentEdit.EnableWindow(false);
			}
		}
	}

	//wenching++ (TD13741)
	void CTagDialog::checkPtwTextCommentField()
	{
		CString tagPtwId, tagComment;

		int tagRadioButton = GetCheckedRadioButton(IDC_RADIO_NOT_TAGGED, IDC_RADIO_TAG_PTW);
		m_ptwIdEdit.GetWindowText(tagPtwId);
		m_commentEdit.GetWindowText(tagComment);

		if (tagRadioButton == IDC_RADIO_TAG_PTW && 
			(tagPtwId.Compare("") == 0 || tagComment.Compare("") == 0) )
		{
			GetDlgItem(IDC_APPLY_TAG)->EnableWindow(false);
		}
		else if (tagRadioButton == IDC_RADIO_TAG)
		{
			GetDlgItem(IDC_APPLY_TAG)->EnableWindow(true);
		}
	}
	//++wenching (TD13741)

	void CTagDialog::OnAlarmInhibitCheck() 
	{
		FUNCTION_ENTRY ("OnAlarmInhibitCheck()");

		bool previousChecked = m_alarmInhibitChecked;

		m_alarmInhibitChecked = !m_alarmInhibitChecked;
		m_alarmInhibitCheck.SetCheck(m_alarmInhibitChecked);

		//m_mmsInhibitChecked = (m_alarmInhibitChecked || m_originalAlarmInhibitChecked || previousChecked)?true:m_originalMmsInhibitChecked;
		//m_mmsInhibitCheck.SetCheck(m_mmsInhibitChecked);

		GetDlgItem(IDC_APPLY_TAG)->EnableWindow(isUnsavedChangeOnTab());

		updateRadioButtonAccess();
		updatePtwCommentAccess();
		checkPtwTextCommentField(); //wenching (TD13741)

		FUNCTION_EXIT;
	}

	void CTagDialog::OnControlInhibitCheck() 
	{
		FUNCTION_ENTRY ("OnControlInhibitCheck()");

		m_controlInhibitChecked = !m_controlInhibitChecked;
		m_controlInhibitCheck.SetCheck(m_controlInhibitChecked);

		GetDlgItem(IDC_APPLY_TAG)->EnableWindow(isUnsavedChangeOnTab());

		updateRadioButtonAccess();
		updatePtwCommentAccess();
		checkPtwTextCommentField(); //wenching (TD13741)

		FUNCTION_EXIT;
	}

	void CTagDialog::OnMmsInhibitCheck() 
	{
		FUNCTION_ENTRY ("OnMmsInhibitCheck()");

		//m_mmsInhibitChecked = !m_mmsInhibitChecked;
		//m_mmsInhibitCheck.SetCheck(m_mmsInhibitChecked);

		GetDlgItem(IDC_APPLY_TAG)->EnableWindow(isUnsavedChangeOnTab());

		updateRadioButtonAccess();
		updatePtwCommentAccess();
		checkPtwTextCommentField(); //wenching (TD13741)

		FUNCTION_EXIT;
	}

	void CTagDialog::OnScanInhibitCheck() 
	{
		FUNCTION_ENTRY ("OnAlarmInhibitCheck()");

		bool previousChecked = m_scanInhibitChecked;

		m_scanInhibitChecked = !m_scanInhibitChecked;
		m_scanInhibitCheck.SetCheck(m_scanInhibitChecked);

		m_alarmInhibitChecked = (m_scanInhibitChecked || m_originalScanInhibitChecked || previousChecked)?true:m_originalAlarmInhibitChecked;
		m_alarmInhibitCheck.SetCheck(m_alarmInhibitChecked);
		
		//m_mmsInhibitChecked = (m_scanInhibitChecked || m_alarmInhibitChecked)?true:m_originalMmsInhibitChecked;
		//m_mmsInhibitCheck.SetCheck(m_mmsInhibitChecked);
		
		GetDlgItem(IDC_APPLY_TAG)->EnableWindow(isUnsavedChangeOnTab());

		updateRadioButtonAccess();
		updatePtwCommentAccess();
		checkPtwTextCommentField(); //wenching (TD13741)

		FUNCTION_EXIT;
	}

	void CTagDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		FUNCTION_ENTRY ("OnShowWindow()");

		CDialog::OnShowWindow(bShow, nStatus);
			
		if (bShow)
		{
			DisplayEquipmentTagStatus();
		}

		FUNCTION_EXIT;
	}

	void CTagDialog::getWindowRect(CRect* rect)
	{
		GetWindowRect(rect);
	}

	unsigned int CTagDialog::getMinimumHeight()
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

	void CTagDialog::resize(unsigned int height)
	{
	}


    bool CTagDialog::isPtwTagAccessAllowed()
    {
        // Can only access the PTW Tag components if the PTW privilege present
        // as well as the basic tag access control for this page
		//TD15276
        return m_ptwTagAccessAllowed;
    }


	void CTagDialog::updatePermissions()
	{
		m_tagAccessAllowed = false;
        m_ptwTagAccessAllowed = false;

		if (m_equipment != NULL)
		{
			unsigned long equipmentKey = m_equipment->getDataNode()->getEntityKey();

			if (equipmentKey != 0L)
			{
				m_tagAccessAllowed = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
														equipmentKey,
														TA_Base_App::AccessControlHelper::DATA_NODE_TAG);

                m_ptwTagAccessAllowed = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
														equipmentKey,
														TA_Base_App::AccessControlHelper::DATA_NODE_TAG_PTW);
			}
		}		

        // Now that rights updated, update associated controls
        // that are disabled based on rights
        DisplayEquipmentTagStatus();
		checkPtwTextCommentField(); //wenching (TD13741)
	}


	bool CTagDialog::isAnyDataPointsDisplayedInTab() const
	{
		// no data point on the Notes page, default to true
		return true;
	}

	void CTagDialog::resetList()
	{

	}

#if 0
	void CTagDialog::setInhibitCheckboxes(bool scanInhibit, bool alarmInhibit, bool mmsInhibit, bool controlInhibit)
	{
		m_scanInhibitChecked = scanInhibit;
		m_scanInhibitCheck.SetCheck(m_scanInhibitChecked);
		
		m_alarmInhibitChecked = m_scanInhibitChecked ? true : alarmInhibit;
		m_alarmInhibitCheck.SetCheck(m_alarmInhibitChecked);
		m_alarmInhibitCheck.EnableWindow(!m_scanInhibitChecked);
		
		m_mmsInhibitChecked = (m_scanInhibitChecked || m_alarmInhibitChecked) ? true : mmsInhibit;
		m_mmsInhibitCheck.SetCheck(m_mmsInhibitChecked);
		m_mmsInhibitCheck.EnableWindow(!(m_scanInhibitChecked || m_alarmInhibitChecked));
		
		m_controlInhibitChecked = controlInhibit;
		m_controlInhibitCheck.SetCheck(m_controlInhibitChecked);
	}
#endif		
}
