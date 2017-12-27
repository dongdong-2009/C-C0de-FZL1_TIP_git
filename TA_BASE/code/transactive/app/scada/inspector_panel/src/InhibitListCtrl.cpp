/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/InhibitListCtrl.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class manages the list display used to show the basic information in
  * the CInformationDialog class.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/ConfirmOverrideDialog.h"
#include "app/scada/inspector_panel/src/InhibitListCtrl.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"

#include "core/message/types/InspectorPanelAudit_MessageTypes.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	enum columns
	{
		DUMMY_COLUMN				= 0,
		DESCRIPTION_COLUMN			= 1,
		DATAPOINT_TYPE_COLUMN		= 2,
		CONTROL_INHIBIT_COLUMN		= 3,
		SCAN_INHIBIT_COLUMN			= 4,
		ALARM_INHIBIT_COLUMN		= 5,
//		MMS_INHIBIT_COLUMN			= 6,
		CURRENT_VALUE_COLUMN		= 6
	};

	/////////////////////////////////////////////////////////////////////////////
	// CInhibitListCtrl

	CInhibitListCtrl::CInhibitListCtrl() 
	:
	CAbstractDataPointListCtrl(), 
	m_dataPointsDisplayedOnScreen ( false )
	{
		FUNCTION_ENTRY ("CInhibitListCtrl()");

		// We will manage the cell requests
		SetManager(this);

		FUNCTION_EXIT;
	}

	CInhibitListCtrl::~CInhibitListCtrl()
	{
		FUNCTION_ENTRY ("~CInhibitListCtrl()");
		FUNCTION_EXIT;
	}

	/* SetEquipment
	 *
	 * This function will set the name of the equipment that the data points are to be displayed for.It can 
	 * be called before or after the windows control is on the screen.
	 */

	// Helper functions

	void CInhibitListCtrl::setupList()
	{
		FUNCTION_ENTRY ("setupList()");

		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES | CAbstractDataPointListCtrl::LVS_EX_LABELTIP_XXX);

		// Headings
		//this->InsertColumn(DUMMY_COLUMN,				"Dummy");
  // 		this->InsertColumn(DESCRIPTION_COLUMN,			"Description");
		//this->InsertColumn(DATAPOINT_TYPE_COLUMN,		"Type");
  // 		this->InsertColumn(CONTROL_INHIBIT_COLUMN,		"Ctrl");
		//this->InsertColumn(SCAN_INHIBIT_COLUMN,			"Scan");
		//this->InsertColumn(ALARM_INHIBIT_COLUMN,		"Alarm");
		//this->InsertColumn(MMS_INHIBIT_COLUMN,			"MMS");
		//this->InsertColumn(CURRENT_VALUE_COLUMN,		"Current Value");

		this->InsertColumn(DUMMY_COLUMN,				"Dummy");
		this->InsertColumn(DESCRIPTION_COLUMN,			"描述");
		this->InsertColumn(DATAPOINT_TYPE_COLUMN,		"类型",LVCFMT_CENTER);
		this->InsertColumn(CONTROL_INHIBIT_COLUMN,		"控制",LVCFMT_CENTER);
		this->InsertColumn(SCAN_INHIBIT_COLUMN,			"扫描",LVCFMT_CENTER);
		this->InsertColumn(ALARM_INHIBIT_COLUMN,		"告警",LVCFMT_CENTER);
		//this->InsertColumn(MMS_INHIBIT_COLUMN,			"MMS");
		this->InsertColumn(CURRENT_VALUE_COLUMN,		"当前值");

		// Using a 'dummy' column for index 0 prevents
		// MFC from indenting the text by the image size for the 'real' first column
		this->SetColumnWidth(DUMMY_COLUMN,				0);
		this->SetColumnWidth(DESCRIPTION_COLUMN,		180);
		this->SetColumnWidth(DATAPOINT_TYPE_COLUMN,		45);
		this->SetColumnWidth(CONTROL_INHIBIT_COLUMN,	45);
		this->SetColumnWidth(SCAN_INHIBIT_COLUMN,		45);
		this->SetColumnWidth(ALARM_INHIBIT_COLUMN,		45);
		//this->SetColumnWidth(MMS_INHIBIT_COLUMN,		50);
		this->SetColumnWidth(CURRENT_VALUE_COLUMN,		85);

		FUNCTION_EXIT;
	}

	void CInhibitListCtrl::updateDataPointDisplay(int iPoint, int nItem, ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPointDisplay()");

		switch (updateType)
		{
		case InhibitStateUpdate:
		case ValueStateUpdate:
        case AlarmSummaryUpdate://limin++, TD20740
        case AckSummaryUpdate:  //limin++, TD20740
        case QualitySummaryUpdate:
			displayItem(nItem);
			break;

		default:
			break;
		}

		FUNCTION_EXIT;
	}

	// By using the preprocessed index vector, reduce linear search and ensure only those
	// datapoints had been sorted and config valid were displayed.
	void CInhibitListCtrl::displayDataPoints()
	{
		FUNCTION_ENTRY ("displayDataPoints()");

		int validItem = 0;
		int difference = 0;
		int totalItem = GetItemCount();

		// No equipment defined yet.
		if (m_equipment==NULL) 
			return;

		int				nItem = 0;
		std::string		value;
		unsigned int	iPoint;

		std::vector<DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();

		// Get the preprocessed indexes with the required config.
		std::vector<unsigned long> writeableVectorIndex = m_equipment->getWriteableIndex();
		std::vector<unsigned long> notWriteableVectorIndex = m_equipment->getNotWriteableIndex();

		if (0 == writeableVectorIndex.size() && 0 == notWriteableVectorIndex.size())
		{
			DeleteAllItems();
			return;
		}

		// Read-only Data Points first
		// Only display the datapoints with index in the vector
		size_t i=0;
		for (;i<notWriteableVectorIndex.size();i++)
		{
			iPoint = notWriteableVectorIndex.at(i);

			try
			{
				validItem++;

    			// Description (Equipment Attributes)
				if( validItem > GetItemCount() )
				{
                    std::string description = (*dataPoints[iPoint])->getDescription();
					nItem = insertItemTextImage(iPoint, validItem-1, description, false);						
					SetItemData(nItem, iPoint);
				}
				else
				{
					nItem = validItem-1;
					SetItemData(nItem, iPoint);
				}

				displayItem(nItem);

				// set the flag to indicate data points are displayed on screen
				m_dataPointsDisplayedOnScreen = true;
				
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
			}
		}
		
		// Write-only data points next
		// Only display the datapoints with index in the vector
		for (i=0;i<writeableVectorIndex.size();i++)
		{
			iPoint = writeableVectorIndex.at(i);

			try
			{
				validItem++;

				// Description (Equipment Attributes)
				value = (*dataPoints[iPoint])->getDescription();	
				if( validItem > GetItemCount() )
				{
					nItem = insertItemTextImage(iPoint, validItem-1, value, false);
				}
				else
				{
					nItem = validItem-1;
					SetItemData(nItem, iPoint);
				}

				displayItem(nItem);		
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
			}
		}

		// Delete invalid item
		if( validItem > 0 )
		{
			difference = totalItem - validItem;

			if ( difference > 0 )
			{
				for ( int i=0; i<difference; i++)
					DeleteItem(totalItem-1-i);
			}
		}
		else if( validItem == 0 && totalItem != 0 )
		{
			for ( int i=0; i<totalItem; i++)
				DeleteItem(i);
		}

		FUNCTION_EXIT;
	}

	void CInhibitListCtrl::refreshDataPoints()
	{
		FUNCTION_ENTRY ("refreshDataPoints()");

		// No equipment defined yet.
		if (m_equipment==NULL) 
			return;

		displayDataPoints(); 

		FUNCTION_EXIT;
	}


	// ICellManager Implementation
	ICellManager::CellType CInhibitListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellType()");

		// Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
		if (item<0 || !m_accessAllowed)	
		{
			return CT_NOTEDITABLE;
		}

		DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(GetItemData(item));

		switch (subItem)
		{
		case SCAN_INHIBIT_COLUMN:
		case ALARM_INHIBIT_COLUMN:
		//case MMS_INHIBIT_COLUMN:
		{
			if (dataPoint->isWriteable() == false)
				return (isCellEnabled (item, subItem) ? CT_CHECKBOX : CT_CHECKBOX_DISABLED);
			else
				return CT_NOTEDITABLE;
		}
		case CONTROL_INHIBIT_COLUMN:
			{
				if (dataPoint->isWriteable() != false)
					return (isCellEnabled (item, subItem) ? CT_CHECKBOX : CT_CHECKBOX_DISABLED);
				else
					return CT_NOTEDITABLE;
			}

		default:
			return CT_NOTEDITABLE;
		}

		FUNCTION_EXIT;

		return CT_NOTEDITABLE;
	}


    unsigned long CInhibitListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }


	ICellManager::CellData CInhibitListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellData()");

		TA_ASSERT (m_equipment!=NULL,"Equipment not defined");

		ICellManager::CellData   data;

		FUNCTION_EXIT;

		return data;
	}

    CString CInhibitListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }

    char CInhibitListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }

	bool CInhibitListCtrl::inhibitOverrideConfirm(const int inhibitOverrideItem, DataPointProxySmartPtr& dataPoint, bool enable)
	{
		FUNCTION_ENTRY ("inhibitOverrideConfirm()");

        CString actionName, actionType;
        actionType.LoadString(inhibitOverrideItem);

		CString dpDescription = dataPoint->getDescription().c_str();
		int pos = dpDescription.Find ( "%", 0 );
		if ( -1 != pos )
		{
			dpDescription.Insert ( pos, "%" );
		}

        AfxFormatString2(actionName, enable?IDS_ENABLE_DATAPOINT:IDS_DISABLE_DATAPOINT, actionType, dpDescription );

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003, "监控面板");        
        FUNCTION_EXIT;
		return (selectedButton == IDYES);
	}

	bool CInhibitListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
	{
		FUNCTION_ENTRY ("UpdateCellValue()");

		TA_ASSERT (m_equipment!=NULL,"Equipment not defined.");

		std::string  strValue = value;
		std::string message;
		bool updateAnimation = false;

		// Get the data point array position
		UINT iPoint = listCtrl->GetItemData(item);
		DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);

		std::stringstream statusLine;
		statusLine << dataPoint->getDescription();

		try
		{
			switch (subItem)
			{
				case SCAN_INHIBIT_COLUMN:
				{	
					bool prevInhibitState = dataPoint->isScanInhibited(); 
					
					// if the inhibit state is different
					if ((strValue == "Yes") != prevInhibitState)
					{
						// get confirmation from the operator
						if (inhibitOverrideConfirm(IDS_SCAN_INHIBIT, dataPoint, (strValue == "Yes")))
						{
							//statusLine << ": Scan Inhibit " << ( (strValue == "Yes")?"ON":"OFF" );
							statusLine << ": 禁止扫描设置 " << ( (strValue == "Yes")?"启动":"取消" );

							try
							{
                                
								if ( prevInhibitState == true && strValue != "Yes")
								{
                                    dataPoint->setInputInhibitStatus(TA_Base_Bus::InhibitAlarm, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));	
								}
								else 
								{
									dataPoint->setInputInhibitStatus((strValue == "Yes")?TA_Base_Bus::InhibitScan:TA_Base_Bus::NoInputInhibit, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));										
								}
								//  [3/14/2010 benxun]
								//statusLine << " Successful.";
								statusLine << " 成功.";
								//  [3/14/2010 benxun]
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

								// set the flag to update the screen
								updateAnimation = true;
							}
							catch (TA_Base_Core::ScadaProxyException &e)
							{
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", e.what());

								//  [3/14/2010 benxun]
								//statusLine << " Failed.";
								statusLine << " 失败.";
								//  [3/14/2010 benxun]
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch ( const TA_Base_Core::ObjectResolutionException & ex )
							{
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", ex.what());

								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch ( const TA_Base_Core::BadParameterException & bex)
							{
								std::string text = "UpdateCellValue() - " + std::string(bex.reason);

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"BadParameterException - %s", text.c_str());

								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch ( const TA_Base_Core::OperationModeException & ex )
							{
								std::string reason( "OperationModeException: "+std::string(ex.reason.in()) );

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", reason.c_str());

								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

							}
							catch ( const CORBA::Exception & cex )
							{
								std::string reason( "UpdateCellValue() - CORBA::Exception: "+TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex) );
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );

								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch (...)
							{
								std::string message="Error: Internal Error when updating DataPoint \"";
								message += dataPoint->getDescription() + "\".";

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", message.c_str());

								//  [3/14/2010 benxun]
								//statusLine << " Failed.";
								statusLine << " 失败.";
								//  [3/14/2010 benxun]
								
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
						}
					}

					// if the flag is set
					if ( true == updateAnimation )
					{
						listCtrl->showCheckIcon(item, subItem, strValue == "Yes", true);
						displayItem(item);
					}

					break;
				} 

				case ALARM_INHIBIT_COLUMN:
				{
					bool prevInhibitState = dataPoint->isAlarmInhibited(); 

					// if the inhibit state is different
					if ((strValue == "Yes") != prevInhibitState) 
					{
						// get confirmation from the operator
						if (inhibitOverrideConfirm(IDS_ALARM_INHIBIT, dataPoint, (strValue == "Yes")))
						{
							//statusLine << ": Alarm Inhibit " << ( (strValue == "Yes")?"ON":"OFF" );
							statusLine << ": 禁止报警设置 " << ( (strValue == "Yes")?"启动":"取消" );

							try
							{
                               
								//if ( prevInhibitState == true && strValue != "Yes")
								//{
								//	dataPoint->setInputInhibitStatus(TA_Base_Bus::InhibitMmsAlarm, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
								//}
								//else
								//{
								dataPoint->setInputInhibitStatus((strValue == "Yes")?TA_Base_Bus::InhibitAlarm:TA_Base_Bus::NoInputInhibit, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
								//}

								//  [3/14/2010 benxun]
								//statusLine << " Failed.";
								statusLine << " 成功.";
								//  [3/14/2010 benxun]
								
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

								// set the flag to update the screen
								updateAnimation = true;
							}
							catch (TA_Base_Core::ScadaProxyException &e)
							{
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", e.what());

								//  [3/14/2010 benxun]
								//statusLine << " Failed.";
								statusLine << " 失败.";
								//  [3/14/2010 benxun]
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch ( const TA_Base_Core::ObjectResolutionException & ex )
							{
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", ex.what());

								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch ( const TA_Base_Core::BadParameterException & bex)
							{
								std::string text = "UpdateCellValue() - " + std::string(bex.reason);

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"BadParameterException - %s", text.c_str());

								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch ( const TA_Base_Core::OperationModeException & ex)
							{
								std::string reason( "OperationModeException: "+std::string(ex.reason.in()) );

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", reason.c_str());


								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

							}
							catch ( const CORBA::Exception & cex )
							{
								std::string reason( "UpdateCellValue() - CORBA::Exception: "+TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex) );
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );

								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}

							catch (...)
							{
								std::string message="Error: Internal Error when updating DataPoint \"";
								message += dataPoint->getDescription() + "\".";

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", message.c_str());

								//  [3/14/2010 benxun]
								//statusLine << " Failed.";
								statusLine << " 失败.";
								//  [3/14/2010 benxun]
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
						}
					}

					// if the flag is set
					if ( true == updateAnimation )
					{
						listCtrl->showCheckIcon(item, subItem, strValue == "Yes", true);
						displayItem(item);
					}
				
					break;
				}

				//case MMS_INHIBIT_COLUMN:
				//{
				//	// if inhibit state is different
				//	if ((strValue == "Yes") != dataPoint->isMmsAlarmInhibited())
				//	{
				//		// get confirmation from the operator
				//		if (inhibitOverrideConfirm(IDS_MMS_INHIBIT, dataPoint, (strValue == "Yes")))
				//		{
				//			statusLine << ": MMS Inhibit " << ( (strValue == "Yes")?"ON":"OFF" );

				//			try
				//			{
    //                            // TD12137: Log an inhibit requested event BEFORE attempting to set the datapoint's MMS alarm inhibition state.
				//				submitAuditMessage("MMS Inhibit", (strValue == "Yes")?"ADD":"REMOVE", 
				//									dataPoint->getEntityName(),
				//									dataPoint->getEntityKey());

				//				dataPoint->setInputInhibitStatus((strValue == "Yes")?TA_Base_Bus::InhibitMmsAlarm:TA_Base_Bus::NoInputInhibit, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

				//				statusLine << " Successful.";
				//				std::string* statusLineStr = new std::string(statusLine.str());
				//				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

				//				// set the flag to update the screen
				//				updateAnimation = true;
				//			}
				//			catch (TA_Base_Core::ScadaProxyException &e)
				//			{
				//				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				//					"UpdateCellValue() - %s", e.what());

				//				statusLine << " Failed.";
				//				std::string* statusLineStr = new std::string(statusLine.str());
				//				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
				//			}
				//			catch (...)
				//			{
				//				std::string message="Error: Internal Error when updating DataPoint \"";
				//				message += dataPoint->getDescription() + "\".";

				//				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				//					"UpdateCellValue() - %s", message.c_str());

				//				statusLine << " Failed.";
				//				std::string* statusLineStr = new std::string(statusLine.str());
				//				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
				//			}
				//		}
				//	}

				//	// if the flag is set
				//	if ( true == updateAnimation )
				//	{
				//		listCtrl->showCheckIcon(item, subItem, strValue == "Yes", true);
				//		displayItem(item);
				//	}
				//
				//	break;
				//}

				case CONTROL_INHIBIT_COLUMN:
				{
					// if the inhibit state is different
					if ((strValue == "Yes") != dataPoint->isControlInhibited())
					{
						// get confirmation from the operator
						if (inhibitOverrideConfirm(IDS_CONTROL_INHIBIT, dataPoint, (strValue == "Yes")))
						{
							//statusLine << ": Control Inhibit " << ( (strValue == "Yes")?"ON":"OFF" );
							statusLine << ":禁止控制设置 " << ( (strValue == "Yes")?"启动":"取消" );

							try
							{
								
								dataPoint->setOutputInhibitStatus((strValue == "Yes")?TA_Base_Bus::InhibitControl:TA_Base_Bus::NoOutputInhibit, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

								//  [3/14/2010 benxun]
								//statusLine << " Failed.";
								statusLine << " 成功.";
								//  [3/14/2010 benxun]
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

								// set the flag to update the screen
								updateAnimation = true;
							}
							catch (TA_Base_Core::ScadaProxyException &e)
							{
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", e.what());

								//  [3/14/2010 benxun]
								//statusLine << " Failed.";
								statusLine << " 失败.";
								//  [3/14/2010 benxun]
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch ( const TA_Base_Core::ObjectResolutionException & ex )
							{
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", ex.what());

								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch ( const TA_Base_Core::BadParameterException & bex)
							{
								std::string text = "UpdateCellValue() - " + std::string(bex.reason);

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"BadParameterException - %s", text.c_str());
								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch ( const TA_Base_Core::OperationModeException & oex)
							{
								std::string reason( "OperationModeException: "+std::string(oex.reason.in()) );

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", reason.c_str());


								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

							}
							catch ( const CORBA::Exception & cex )
							{
								std::string reason( "UpdateCellValue() - CORBA::Exception: "+TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex) );
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", reason.c_str() );
        
								statusLine << " Failed.";
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
							catch (...)
							{
								std::string message="Error: Internal Error when updating DataPoint \"";
								message += dataPoint->getDescription() + "\".";

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									"UpdateCellValue() - %s", message.c_str());

								//  [3/14/2010 benxun]
								//statusLine << " Failed.";
								statusLine << " 失败.";
								//  [3/14/2010 benxun]
								std::string* statusLineStr = new std::string(statusLine.str());
								AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							}
						}
					}

					// if the flag is set
					if ( true == updateAnimation )
					{
						listCtrl->showCheckIcon(item, subItem, strValue == "Yes", true);
						displayItem(item);
					}

					break;
				}

				default:
				{
					break;
				}
			}
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"UpdateCellValue() - %s", e.what());
		}
	
		FUNCTION_EXIT;

		// Display is always handled here.
		return false;
	}

	void CInhibitListCtrl::displayItem(int nItem)
	{
		FUNCTION_ENTRY ("displayItem()");

		DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(GetItemData(nItem));

		std::string  value;

		if (dataPoint->isWriteable() == false)
		{
			// Description (Equipment Attributes)
			value = dataPoint->getDescription();
			setItemTextImage(nItem, DESCRIPTION_COLUMN, value, false);

			// Type of the data point
			value = dataPoint->getShortNameOfType();
			setItemTextImage(nItem, DATAPOINT_TYPE_COLUMN, value, false);

			// Control Inhibit - not used for Information Data Points
			// Scan Inhibit
			bool isChecked, isEnabled;
			
			try
			{
				isChecked = dataPoint->isScanInhibited();
				isEnabled = isCellEnabled(nItem, SCAN_INHIBIT_COLUMN);
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				// Not always available
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"displayItem() - %s", e.what());

				isChecked = false;
				isEnabled = false;
			}

			showCheckIcon(nItem, SCAN_INHIBIT_COLUMN, isChecked, isEnabled);

			// Alarm Inhibit
			try
			{
				isChecked|= dataPoint->isAlarmInhibited();
				isEnabled = isCellEnabled(nItem, ALARM_INHIBIT_COLUMN);
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				// Not always available
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"displayItem() - %s", e.what());

				isChecked = false;
				isEnabled = false;
			}

			showCheckIcon(nItem, ALARM_INHIBIT_COLUMN, isChecked, isEnabled);
			
			// MMS Inhibit
			try
			{
				//isChecked|= dataPoint->isMmsAlarmInhibited();
				//isEnabled = isCellEnabled(nItem, MMS_INHIBIT_COLUMN);
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				// Not always available
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"displayItem() - %s", e.what());

				isChecked = false;
				isEnabled = false;
			}

			//showCheckIcon(nItem, MMS_INHIBIT_COLUMN, isChecked, isEnabled);

			// should not show control inhibit checkbox & always update value
			destroyCheckIcon(nItem, CONTROL_INHIBIT_COLUMN);

			std::string currentValue = getCurrentValueAsString(dataPoint);
			setItemTextImage(nItem, CURRENT_VALUE_COLUMN, currentValue, true);
		}
		else
		{
			// Description (Equipment Attributes)
			value = dataPoint->getDescription();
			setItemTextImage(nItem, DESCRIPTION_COLUMN, value, false);

			// Type of the data point
			value = dataPoint->getShortNameOfType();
			setItemTextImage(nItem, DATAPOINT_TYPE_COLUMN, value, false);
			
			// Control Inhibit
			bool isChecked, isEnabled;
			
			try
			{
				isChecked = dataPoint->isControlInhibited();
				isEnabled = isCellEnabled(nItem, CONTROL_INHIBIT_COLUMN);
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				// Not always available
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"displayItem() - %s", e.what());

				isChecked = false;
				isEnabled = false;
			}

			showCheckIcon(nItem, CONTROL_INHIBIT_COLUMN, isChecked, isEnabled);

			// remove scan, alarm, mms inhibit checkboxes and no current value displayed
			destroyCheckIcon(nItem, SCAN_INHIBIT_COLUMN);
			destroyCheckIcon(nItem, ALARM_INHIBIT_COLUMN);
			//destroyCheckIcon(nItem, MMS_INHIBIT_COLUMN);
			setItemTextImage(nItem, CURRENT_VALUE_COLUMN, "", false); 
		}
	
		FUNCTION_EXIT;
	}

	bool CInhibitListCtrl::isCellEnabled (int nItem, int nSubItem)
	{
		FUNCTION_ENTRY ("isCellEnabled()");

		// Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
		if (nItem<0 || !m_accessAllowed)	
			return false;

		DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(GetItemData(nItem));
		DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();

		try
		{
			if (dataPoint->isWriteable() == false)
			{
				switch (nSubItem)
				{
				case SCAN_INHIBIT_COLUMN:
					{
						// cell should be disabled if the TagInhibitState is scaninhibit
						if ( true == dataNode->isTagged() && 
							 TA_Base_Bus::InhibitScan == dataNode->getTagInhibitState().inputInhibit)
						{
							return false;
						}

						// get the access right
						bool isScanInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
							dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_SCAN_INHIBIT);

						bool isScanUnInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
													dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_SCAN_UNINHIBIT);
								
						// if the operator profile has access to both actions
						if ( ( true == isScanInhibitPermitted ) &&
							 ( true == isScanUnInhibitPermitted ) )
						{
							return true;
						}

						// if the operator profile has no access to both actions
						else if ( ( false == isScanInhibitPermitted ) &&
								  ( false == isScanUnInhibitPermitted ) )
						{
							return false;
						}

						// if the operator profile has access to one of the actions
						else
						{
							// if the operator profile has access to inhibit but
							// not to uninhibit the scan
							if ( ( true == isScanInhibitPermitted ) &&
								 ( false == isScanUnInhibitPermitted ) )
							{
								// if the data point has not been inhibited yet
								if ( false == dataPoint->isScanInhibited() )
								{
									// enable the cell so that operator can inhibit the scan
									return true;
								}
								// else if the data point has been inhibited
								else
								{
									// disable the cell so that the operator cannot uninhibit
									// the scan
									return false;
								}
							}
							// if the operator profile has access to uninhibit but
							// not to inhibit the scan
							else if ( ( false == isScanInhibitPermitted ) &&
									  ( true == isScanUnInhibitPermitted ) )
							{
								// if the data point has been inhibited
								if ( true == dataPoint->isScanInhibited() )
								{
									// enable the cell so that operator can uninhibit the scan
									return true;
								}
								// else if the data point has not been inhibited yet
								else
								{
									// disable the cell so that the operator cannot inhibit
									// the scan
									return false;
								}
							}
							// if none of the above conditions
							else
							{
								return false;
							}
						}
					}
				case ALARM_INHIBIT_COLUMN:
					{
						// cell should be disabled if the TagInhibitState is scaninhibit or alarm inhibit
						if ( true == dataNode->isTagged() )
						{
							 if (TA_Base_Bus::InhibitScan == dataNode->getTagInhibitState().inputInhibit ||
								 TA_Base_Bus::InhibitAlarm == dataNode->getTagInhibitState().inputInhibit )
							 {
								 return false;
							 }
						}
					
						// cell should be disabled if the datapoint was scan inhibited
						if (dataPoint->isScanInhibited())
						{	
							return false;
						}

						// get the access right
						bool isAlarmInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
													dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_ALARM_INHIBIT);

						bool isAlarmUnInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
													dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_ALARM_UNINHIBIT);

						// if the operator profile has access to both actions
						if ( ( true == isAlarmInhibitPermitted ) &&
							 ( true == isAlarmUnInhibitPermitted ) )
						{
							return true;
						}

						// if the operator profile has no access to both actions
						else if ( ( false == isAlarmInhibitPermitted ) &&
								  ( false == isAlarmUnInhibitPermitted ) )
						{
							return false;
						}

						// if the operator profile has access to one of the actions
						else
						{
							// if the operator profile has access to inhibit but
							// not to uninhibit the alarms
							if ( ( true == isAlarmInhibitPermitted ) &&
								 ( false == isAlarmUnInhibitPermitted ) )
							{
								// if the data point has not been inhibited yet
								if ( false == dataPoint->isAlarmInhibited() )
								{
									// enable the cell so that operator can inhibit the alarm
									return true;
								}
								// else if the data point has been inhibited
								else
								{
									// disable the cell so that the operator cannot uninhibit
									// the alarm
									return false;
								}
							}
							// if the operator profile has access to uninhibit but
							// not to inhibit the alarms
							else if ( ( false == isAlarmInhibitPermitted ) &&
									  ( true == isAlarmUnInhibitPermitted ) )
							{
								// if the data point has been inhibited
								if ( true == dataPoint->isAlarmInhibited() )
								{
									// enable the cell so that operator can uninhibit the alarm
									return true;
								}
								// else if the data point has not been inhibited yet
								else
								{
									// disable the cell so that the operator cannot inhibit
									// the alarm
									return false;
								}
							}
							// if none of the above conditions
							else
							{
								return false;
							}
						}
					}
				//case MMS_INHIBIT_COLUMN:
				//	{
				//		// cell should be disabled if the TagInhibitState is scaninhibit or alarm inhibit or mms inhibit
				//		if ( true == dataNode->isTagged() )
				//		{
				//			 if (TA_Base_Bus::InhibitScan == dataNode->getTagInhibitState().inputInhibit ||
				//				 TA_Base_Bus::InhibitAlarm == dataNode->getTagInhibitState().inputInhibit ||
				//				 TA_Base_Bus::InhibitMmsAlarm == dataNode->getTagInhibitState().inputInhibit )
				//			 {
				//				 return false;
				//			 }
				//		}

				//		// if data point is scan inhibited or the alarm function is inhibited
				//		if ( dataPoint->isScanInhibited() || dataPoint->isAlarmInhibited() )
				//		{
				//			return false;
				//		}

				//		// get access right
				//		bool isMMSInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
				//									dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_MMS_INHIBIT);
				//		bool isMMSUnInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
				//									dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_MMS_UNINHIBIT);
				//		
				//		// if the operator profile has access to both actions
				//		if ( ( true == isMMSInhibitPermitted ) &&
				//			 ( true == isMMSUnInhibitPermitted ) )
				//		{
				//			return true;
				//		}

				//		// if the operator profile has no access to both actions
				//		else if ( ( false == isMMSInhibitPermitted ) &&
				//				  ( false == isMMSUnInhibitPermitted ) )
				//		{
				//			return false;
				//		}

				//		// if the operator profile has access to one of the actions
				//		else
				//		{
				//			// if the operator profile has access to inhibit but
				//			// not to uninhibit the alarms
				//			if ( ( true == isMMSInhibitPermitted ) &&
				//				 ( false == isMMSUnInhibitPermitted ) )
				//			{
				//				// if the data point has not been inhibited yet
				//				if ( false == dataPoint->isMmsAlarmInhibited() )
				//				{
				//					// enable the cell so that operator can inhibit the alarm
				//					return true;
				//				}
				//				// else if the data point has been inhibited
				//				else
				//				{
				//					// disable the cell so that the operator cannot uninhibit
				//					// the alarm
				//					return false;
				//				}
				//			}
				//			// if the operator profile has access to uninhibit but
				//			// not to inhibit the alarms
				//			else if ( ( false == isMMSInhibitPermitted ) &&
				//					  ( true == isMMSUnInhibitPermitted ) )
				//			{
				//				// if the data point has been inhibited
				//				if ( true == dataPoint->isMmsAlarmInhibited() )
				//				{
				//					// enable the cell so that operator can uninhibit the alarm
				//					return true;
				//				}
				//				// else if the data point has not been inhibited yet
				//				else
				//				{
				//					// disable the cell so that the operator cannot inhibit
				//					// the alarm
				//					return false;
				//				}
				//			}
				//			// if none of the above conditions
				//			else
				//			{
				//				return false;
				//			}
				//		}
				//	}
				default:
					{
						return false;
					}					
				}
			}
			else
			{
				if (nSubItem == CONTROL_INHIBIT_COLUMN)
				{
					// cell should be disabled if the TagInhibitState is control inhibit
					if ( true == dataNode->isTagged() &&
						 TA_Base_Bus::InhibitControl == dataNode->getTagInhibitState().outputInhibit )
					{
						return false;
					}

					bool isControlInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
													dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_CONTROL_INHIBIT);
					bool isControlUnInhibitPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
													dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_CONTROL_UNINHIBIT);

					// if the operator profile has access to both actions
					if ( ( true == isControlInhibitPermitted ) &&
						 ( true == isControlUnInhibitPermitted ) )
					{
						return true;
					}

					// if the operator profile has no access to both actions
					else if ( ( false == isControlInhibitPermitted ) &&
							  ( false == isControlUnInhibitPermitted ) )
					{
						return false;
					}

					// if the operator profile has access to one of the actions
					else
					{
						// if the operator profile has access to inhibit but
						// not to uninhibit the control
						if ( ( true == isControlInhibitPermitted ) &&
							 ( false == isControlUnInhibitPermitted ) )
						{
							// if the data point control has not been inhibited yet
							if ( false == dataPoint->isControlInhibited() )
							{
								// enable the cell so that operator can inhibit the control
								return true;
							}
							// else if the data point control has been inhibited
							else
							{
								// disable the cell so that the operator cannot uninhibit
								// the control
								return false;
							}
						}
						// if the operator profile has access to uninhibit but
						// not to inhibit the control
						else if ( ( false == isControlInhibitPermitted ) &&
								  ( true == isControlUnInhibitPermitted ) )
						{
							// if the data point control has been inhibited
							if ( true == dataPoint->isControlInhibited() )
							{
								// enable the cell so that operator can uninhibit the control
								return true;
							}
							// else if the data point control has not been inhibited yet
							else
							{
								// disable the cell so that the operator cannot inhibit
								// the control
								return false;
							}
						}
						// if none of the above conditions
						else
						{
							return false;
						}
					}
				}
				else
				{
					return false;
				}
			}
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"isCellEnabled() - %s", e.what());
		}

		FUNCTION_EXIT;

		return false;
	}

	bool CInhibitListCtrl::isAnyDataPointsDisplayedOnScreen() const
	{
		return m_equipment->hasInformationPoint();
	}

	//TD15438, override the function to get a chance to adjust columns width.
	BOOL CInhibitListCtrl::SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags )
	{
		
		//Call the base class member first
		BOOL isSucceeded = CListCtrl::SetWindowPos(pWndInsertAfter, x, y, cx, cy, nFlags);
		
		//To adjust the column width if needed.
		if (isSucceeded)
		{
			CRect rectItem, rectHeader, rectList;
			GetItemRect(0, &rectItem, LVIR_BOUNDS);
			GetHeaderCtrl()->GetWindowRect(&rectHeader);
			GetClientRect(&rectList);
			
			//if horizontal and scroll bar appear, adjust a column to let horizontal scroll bar dispear
			if (rectList.Height() < rectItem.Height() * GetItemCount() + rectHeader.Height() &&
				rectList.Width() < rectItem.Width())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Column Width is adjusted: itemH=%d; itemNo=%d; itemW=%d; headH=%d; listH=%d; listW=%d", 
					rectItem.Height(), GetItemCount(), rectItem.Width(), rectHeader.Height(), rectList.Height(), rectList.Width());
				
				int columnWidth = GetColumnWidth(CURRENT_VALUE_COLUMN);
				int vsrollWidth = GetSystemMetrics(SM_CXVSCROLL);
				
				//the minimum width is 100
				if (columnWidth - vsrollWidth > 100)
				{
					SetColumnWidth(CURRENT_VALUE_COLUMN, columnWidth - vsrollWidth);
				}
			}
		}
		
		return isSucceeded;
		
	}
	
}