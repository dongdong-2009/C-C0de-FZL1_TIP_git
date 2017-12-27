/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/LimitDetailsListCtrl.cpp $
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
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"
#include "app/scada/inspector_panel/src/LimitDetailsListCtrl.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/common_library/src/DpValue.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/InspectorPanelAudit_MessageTypes.h"
#include "core/exceptions/src/ScadaProxyException.h"


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
		ALARM_TYPE_COLUMN			= 1,
		ALARM_ENABLED_COLUMN		= 2,
		CURRENT_VALUE_COLUMN		= 3,
		THRESHOLD_VALUE_COLUMN		= 4
	};

	/////////////////////////////////////////////////////////////////////////////
	// CLimitDetailsListCtrl

	CLimitDetailsListCtrl::CLimitDetailsListCtrl()
	:
	CAbstractDataPointListCtrl(),
	m_dataPointsDisplayedOnScreen ( false ),
	m_iPoint (-1)
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::CLimitDetailsListCtrl()");

		// We will manage the cell requests
		SetManager(this);

		FUNCTION_EXIT;
	}

	CLimitDetailsListCtrl::~CLimitDetailsListCtrl()
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::~CLimitDetailsListCtrl()");
		FUNCTION_EXIT;
	}

	/* setEquipment
	 *
	 * This function will set the name of the equipment that the details are to be displayed for.It can 
	 * be called before or after the windows control is on the screen.
	 */
	void CLimitDetailsListCtrl::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");

		// call base class
		CAbstractDataPointListCtrl::setEquipment ( equipment );

		FUNCTION_EXIT;
	}

	/* setDataPoint
	 *
	 * This function will set the entity key of the dataPoint that the details are to be displayed for.It can 
	 * be called before or after the windows control is on the screen.
	 */
	void CLimitDetailsListCtrl::setDataPoint(int iPoint)
	{
		FUNCTION_ENTRY ("setDataPoint()");

		m_iPoint = iPoint;

		displayLimitDetails();

		FUNCTION_EXIT;
	}

	BEGIN_MESSAGE_MAP(CLimitDetailsListCtrl, CAbstractDataPointListCtrl)
		//{{AFX_MSG_MAP(CLimitDetailsListCtrl)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()



	/////////////////////////////////////////////////////////////////////////////
	// CDataPointListCtrl message handlers

	int CLimitDetailsListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		FUNCTION_ENTRY ("OnCreate()");

		if (CWnd::OnCreate(lpCreateStruct) == -1)
		{
			return -1;
		}

		setupList();
		
		FUNCTION_EXIT;

		return 0;
	}

	void CLimitDetailsListCtrl::PreSubclassWindow() 
	{
		FUNCTION_ENTRY ("PreSubclassWindow()");

		CWnd::PreSubclassWindow();
		setupList();

		FUNCTION_EXIT;
	}

	// Helper functions

	void CLimitDetailsListCtrl::setupList()
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::SetupList()");

		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES | CAbstractDataPointListCtrl::LVS_EX_LABELTIP_XXX);

		// Headings
		//this->InsertColumn(DUMMY_COLUMN,				"Dummy");
		//this->InsertColumn(ALARM_TYPE_COLUMN,			"Alarm Type");
		//this->InsertColumn(ALARM_ENABLED_COLUMN,		"Enabled");
		//this->InsertColumn(CURRENT_VALUE_COLUMN,		"Current Value");
		//this->InsertColumn(THRESHOLD_VALUE_COLUMN,		"Threshold Value");

		this->InsertColumn(DUMMY_COLUMN,				"Dummy");
		this->InsertColumn(ALARM_TYPE_COLUMN,			"报警类型");
		this->InsertColumn(ALARM_ENABLED_COLUMN,		"允许",LVCFMT_CENTER);
		this->InsertColumn(CURRENT_VALUE_COLUMN,		"当前值");
		this->InsertColumn(THRESHOLD_VALUE_COLUMN,		"阀值");

		// Using a 'dummy' column for index 0 prevents
		// MFC from indenting the text by the image size for the
		// 'real' first column
		this->SetColumnWidth(DUMMY_COLUMN,				0);
		this->SetColumnWidth(ALARM_TYPE_COLUMN,			65);
		this->SetColumnWidth(ALARM_ENABLED_COLUMN,		45);
		this->SetColumnWidth(CURRENT_VALUE_COLUMN,		90);
		this->SetColumnWidth(THRESHOLD_VALUE_COLUMN,	90);

		FUNCTION_EXIT;
	}


	void CLimitDetailsListCtrl::displayDataPoints()
	{
	
	}


	void CLimitDetailsListCtrl::updateDataPointDisplay(int iPoint, int nItem, ScadaEntityUpdateType updateType)
	{
	
	}

	void CLimitDetailsListCtrl::clearList()
	{
		DeleteAllItems();
	}


	void CLimitDetailsListCtrl::displayLimitDetails()
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::displayLimitDetails()");

		DeleteAllItems();

		// No equipment defined yet.
		if (m_equipment==NULL || m_iPoint==-1 ) 
			return; 

		std::string		value;
		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(m_iPoint);
		int nItem = 0;
	
		if ( !dataPoint->isAlarmPropertiesValid() )
			return;

		try
		{
			TA_Base_Bus::AlarmPropertyVect alarmProperties = dataPoint->getAlarmProperties();
			TA_Base_Bus::AlarmPropertyVect::iterator it;

			for (it = alarmProperties.begin(); it != alarmProperties.end(); it++)
			{
				nItem = InsertItem(nItem+1,alarmTypeToString((*it).type).c_str());

				switch(dataPoint->getValue().getType())
				{
				case TA_Base_Bus::DPT_INT16_TYPE:
				case TA_Base_Bus::DPT_INT32_TYPE:
				case TA_Base_Bus::DPT_INT16S_TYPE:
				case TA_Base_Bus::DPT_INT32S_TYPE:
				case TA_Base_Bus::DPT_UINT16_TYPE:
				case TA_Base_Bus::DPT_UINT32_TYPE:
				case TA_Base_Bus::DPT_IEEE32_TYPE:
					SetItemData(nItem,(DWORD)(*it).threshold);
					break;

				case TA_Base_Bus::DPT_ENUM_TYPE:
					SetItemData(nItem,(DWORD)(*it).enumValue);
					break;

                default:
                    break;
				}

				displayItem(nItem);

				// set the flag to indicate data points are displayed on screen
				m_dataPointsDisplayedOnScreen = true;
			}
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"displayLimitDetails() - %s", e.what());
		}

		FUNCTION_EXIT;
	}

	void CLimitDetailsListCtrl::refreshLimitDetails()
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::refreshDataPoints()");

		// No equipment defined yet.
		if (m_equipment==NULL) 
			return;

		displayLimitDetails();
		FUNCTION_EXIT;
	}

	// ICellManager Implementation
	ICellManager::CellType CLimitDetailsListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::GetCellType()");

		// Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
		if (item<0)	
		{
			return CT_NOTEDITABLE;
		}

		// No access to this dialog
		if (!m_accessAllowed)
		{
			return CT_NOTEDITABLE;
		}

		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(m_iPoint);

		switch (subItem)
		{
			case ALARM_ENABLED_COLUMN:
			{
				return CT_CHECKBOX;
			}

			case THRESHOLD_VALUE_COLUMN:
			{
				return CT_NUMBER;
			}

			default:
			{
				return CT_NOTEDITABLE;
			}
		}

		FUNCTION_EXIT;

		return CT_NOTEDITABLE;
	}


    unsigned long CLimitDetailsListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }


	ICellManager::CellData CLimitDetailsListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::GetCellData()");

		TA_ASSERT (m_equipment!=NULL,"Equipment not defined");

		ICellManager::CellData   data;

		FUNCTION_EXIT;

		return data;
	}

    CString CLimitDetailsListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }

    char CLimitDetailsListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }

	bool CLimitDetailsListCtrl::limitDetailChangeConfirm(const int alarmThreshold, TA_Base_Bus::DataPointProxySmartPtr& dataPoint, bool enable)
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::inhibitOverrideConfirm()");
    
        CString actionName, actionType;
        actionType.LoadString(alarmThreshold);
		
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

	bool CLimitDetailsListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::UpdateCellValue()");

		TA_ASSERT (m_equipment!=NULL,"Equipment not defined.");

		bool cellValueUpdated = false;
		std::string  strValue = value;
		std::string message;

		// Get the data point array position
		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(m_iPoint);

		std::stringstream statusLine;
		statusLine << dataPoint->getDescription() << ": ";
		
		if (dataPoint->isWriteable() == false && dataPoint->isAlarmPropertiesValid()) //wenching++ (TD14686)
		{
			std::string sessionId ( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
			bool foundMatchedAlarmType = false;

			// get the alarm type associated with this cell
			CString alarmType = GetItemText( item, ALARM_TYPE_COLUMN );			

			// create an instance of ProxyDataPointAlarmProperty on stack
			TA_Base_Bus::AlarmProperty newAlarmProperty;

			// get the list of the alarms properties associated with the data point
			TA_Base_Bus::AlarmPropertyVect alarmProperties;
			alarmProperties.clear();

			try
			{
				alarmProperties = dataPoint->getAlarmProperties();
			}
			catch ( TA_Base_Core::ScadaProxyException & e )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"UpdateCellValue() - %s", e.what());
			}
			catch (...)
			{
				std::string message="Error: Internal Error when getting the alarm properties of DataPoint \"";
				message += dataPoint->getDescription() + "\".";
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"UpdateCellValue() - %s", message.c_str());
			}

			// for each of the alarm properties in the list
			TA_Base_Bus::AlarmPropertyVect::iterator it;
			for ( it = alarmProperties.begin(); it != alarmProperties.end(); it++ )
			{
				// if found a matched alarm type
				if ( 0 == alarmTypeToString((*it).type).compare ( alarmType ) )
				{
                    newAlarmProperty = (*it);
					// populate data into the new alarm property
//					newAlarmProperty.type = (*it).type;			// AlarmType unchanged
//					newAlarmProperty.enabled = (*it).enabled;	// AlarmEnabled to be CHANGED
//					(*newAlarmProperty.value) = (*(*it).value);		// AlarmValue to be	CHANGED
//					newAlarmProperty.filterTimeFactor = (*it).filterTimeFactor; // AlarmDelay unchanged
//					newAlarmProperty.message = (*it).message;	// AlarmMessage unchanged
//					newAlarmProperty.severity = (*it).severity; // AlarmSeverity unchanged

					// set the flag to indicate that we have found a matched alarm type
					foundMatchedAlarmType = true;

					break;
				}
			}

			// if we have successfully found a match alarm type to modify its properties
			if ( true == foundMatchedAlarmType )
			{
				switch (subItem)
				{
				case ALARM_ENABLED_COLUMN:
					{
						// if the checkbox is checked by the operator
						if (strValue == "Yes")
						{
							// get confirmation from the operator
							std::string typeOfChange = "警报类型 " + alarmType;
							if ( overrideAlarmConditionConfirm ( typeOfChange, dataPoint, ( strValue == "Yes" ) ) )
							{
								// AlarmEnabled changed to true
								newAlarmProperty.enabled = true;

								//  [3/14/2010 benxun]
								//statusLine << "Alarm type \"" << (LPCTSTR) alarmType << "\" enabled ";
								statusLine << "警报类型 \"" << (LPCTSTR) alarmType << "\" 激活";
								//  [3/14/2010 benxun]

								// set the flag to indicate that the cell value will need to be changed
								cellValueUpdated = true;
							}
						}

						// else if the checkbox is unchecked by the operator
						else
						{
							// get confirmation from the operator
							std::string typeOfChange = "警报类型 " + alarmType;
							if ( overrideAlarmConditionConfirm ( typeOfChange, dataPoint, ( strValue == "Yes" ) ) )
							{
								// AlarmEnabled changed to false
								newAlarmProperty.enabled = false;

								//  [3/14/2010 benxun]
								//statusLine << "Alarm type \"" << (LPCTSTR) alarmType << "\" disabled ";
								statusLine << "警报类型 \"" << (LPCTSTR) alarmType << "\" 作废";
								//  [3/14/2010 benxun]


								// set the flag to indicate that the cell value will need to be changed
								cellValueUpdated = true;
							}
						}

						break;
					}

				case THRESHOLD_VALUE_COLUMN:
					{
						// Check if there is changing of value
						// Since this cell only allow value entry, no unit checking
						std::string cellValue = (std::string) value;

						if( newAlarmProperty.threshold == atof(cellValue.c_str()) )
							break;
						
						//TD15160, to ensure the input text is valid number without unit string
						//if the input is a numberic value, it must not equil to the one with '9' added
						std::string longerValue = cellValue + "9";
						if (cellValue.empty() || atof(cellValue.c_str()) == atof(longerValue.c_str()))
						{
							// show text in Status box
							//  [3/14/2010 benxun]
							//statusLine << "The input value " << cellValue.c_str() << " is not a valid number (should exclude unit).";
							statusLine << "输入值 " << cellValue.c_str() << " 是无效数字 (不应该带单位).";
							//  [3/14/2010 benxun]
							std::string* statusLineStr = new std::string(statusLine.str());
							AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							
							break;
						}

						//add the unit for further display
						cellValue += " ";
						cellValue += dataPoint->getUnit();

						//TD15160, to check the limit value range
						if (atof(cellValue.c_str()) < dataPoint->getLimit(true) ||
							atof(cellValue.c_str()) > dataPoint->getLimit(false))
						{
							// show text in Status box
							//  [3/14/2010 benxun]
							/*
							statusLine << "Set new Threshold Value " << cellValue.c_str() << " Failed. ";
							statusLine << "Valid range is from ";
							statusLine << dataPoint->getLimit(true) << " to ";
							statusLine << dataPoint->getLimit(false) << " ";
							statusLine << dataPoint->getUnit() << ".";	
							*/

							statusLine << "设置新限值 " << cellValue.c_str() << " 失败. ";
							statusLine << "有效值域从 ";
							statusLine << dataPoint->getLimit(true) << " 到 ";
							statusLine << dataPoint->getLimit(false) << " ";
							statusLine << dataPoint->getUnit() << ".";													
							std::string* statusLineStr = new std::string(statusLine.str());
							AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
							
							break;
						}
						
						// get confirmation from the operator
						std::stringstream ss;
						ss << "设置警报新阀值:" <<  dataPoint->getEntityName();
						ss << ", "  << dataPoint->getDescription() << ", 从 ";
						ss << newAlarmProperty.threshold << " 到 " << cellValue;

						CString dpDescription = ss.str().c_str();
						int pos = dpDescription.Find ( "%", 0 );
						if ( -1 != pos )
						{
							dpDescription.Insert ( pos, "%" );
						}
						
						CString actionName;
						AfxFormatString1 ( actionName, IDS_OVERRIDE_ALARM_THRESHOLD, dpDescription );

						TA_Base_Bus::TransActiveMessage userMsg;
						userMsg << actionName;
						UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003, "监控面板");       

						if ( selectedButton == IDYES )
						{
							// update the AlarmValue
							std::string newValue = cellValue;
							newAlarmProperty.threshold = atof (newValue.c_str());

							statusLine << "\"" << newValue << "\"" << " Limit Set ";

							// set the flag to indicate that the cell value will need to be changed
							cellValueUpdated = true;
						}
						break;
					}

				default:
					{
						break;
					}					
				}
			}

			// if the flag indicate the alarm properties need to be updated
			if ( true == cellValueUpdated )
			{
				try
				{
					dataPoint->setAlarmProperties ( newAlarmProperty, sessionId );

					//  [3/14/2010 benxun]
					//statusLine << "Successful.";
					statusLine << "成功.";
					//  [3/14/2010 benxun]
					std::string* statusLineStr = new std::string(statusLine.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

					// set the flag to indicate that the cell value has been changed
					cellValueUpdated = true;
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// set the flag to indicate that the cell value has not been changed
					cellValueUpdated = false;

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"UpdateCellValue() - %s", e.what());

					//  [3/14/2010 benxun]
					//statusLine << "Successful.";
					statusLine << "失败.";
					//  [3/14/2010 benxun]
					std::string* statusLineStr = new std::string(statusLine.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
				}
				catch (...)
				{
					// set the flag to indicate that the cell value has not been changed
					cellValueUpdated = false;

					std::string message="Error: Internal Error when updating DataPoint \"";
					message += dataPoint->getDescription() + "\".";
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"UpdateCellValue() - %s", message.c_str());

					//  [3/14/2010 benxun]
					//statusLine << "Successful.";
					statusLine << "失败.";
					//  [3/14/2010 benxun]
					std::string* statusLineStr = new std::string(statusLine.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
				}
			}
		}

		// if the flag is set
		if ( true == cellValueUpdated )
		{
			// update the screen dialog
			displayItem(item);
		}

		return cellValueUpdated;

		FUNCTION_EXIT;
	}

	void CLimitDetailsListCtrl::updateDataPoint(unsigned long key, ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoint()");

		if (this->m_hWnd==0 || m_iPoint==-1 || m_equipment==NULL) 
		{
			return;
		}

		switch(updateType)
		{
		case ValueStateUpdate:
        case AlarmSummaryUpdate://limin++, TD20740
        case AckSummaryUpdate:  //limin++, TD20740
		case ConfigStateUpdate:
		case QualitySummaryUpdate:
			if (m_iPoint == static_cast<int>(m_equipment->getChildDataPointIndexFromEntityKey(key)))
			{
				refreshLimitDetails();
			}
			break;

		default:
			break;
		}

		FUNCTION_EXIT;
	}

	void CLimitDetailsListCtrl::displayItem(int nItem)
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::displayItem()");

		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(m_iPoint);

		std::string alarmType = GetItemText(nItem, 0);
		double alarmValue = (double)GetItemData(nItem);

		if (dataPoint->isConfigValid() == true && dataPoint->isAlarmPropertiesValid() == true && dataPoint->isWriteable() == false && dataPoint->getDisplayOrder() >= 0)
		
		{
			bool foundAlarmType = false;
			bool isChecked, isEnabled;
			std::string type = "";

			try
			{
				TA_Base_Bus::AlarmPropertyVect alarmProperties = dataPoint->getAlarmProperties();
				TA_Base_Bus::AlarmPropertyVect::iterator it;

				for (it = alarmProperties.begin(); it != alarmProperties.end(); it++)
				{
					if (alarmType == alarmTypeToString((*it).type))
					{
						foundAlarmType = true;
						break;
					}
				}

				// if we have found a configured alarm type
				if ( true == foundAlarmType )
				{
					// Alarm Type
					SetItemText(nItem, ALARM_TYPE_COLUMN, alarmType.c_str());

					LVITEM   listItem;
					listItem.iItem = nItem;
					listItem.iSubItem = ALARM_TYPE_COLUMN;
					listItem.iImage = -1;
					listItem.mask = LVIF_IMAGE;
					SetItem(&listItem);

					// Alarm Enabled
					if (it != alarmProperties.end())
					{
						isChecked = (*it).enabled;
						isEnabled = isCellEnabled(nItem, ALARM_ENABLED_COLUMN);
					}
					else
					{
						isChecked = false;
						isEnabled = false;
					}

					showCheckIcon(nItem, ALARM_ENABLED_COLUMN, isChecked, isEnabled);

					// Threshold Value
					std::ostringstream threshold;
					threshold.clear();
					threshold.precision(10);
					if (it != alarmProperties.end())
					{
						threshold << (*it).threshold;

						// if eng unit is defined, attached eng unit to the value
						if ( dataPoint->getType() == DPT_ANALOGUE && false == dataPoint->getUnit().empty() )
						{
							threshold << " " << dataPoint->getUnit();
						}
					}
					
					std::string currentValue = threshold.str() ;
					std::string::size_type posSearch = currentValue.find("0C"); 
					if( posSearch != std::string::npos )
					{
						currentValue.erase( posSearch, 1);
//						currentValue.insert(posSearch, "?);
					}

					// CL16786++
					if( GetItemText( nItem, ALARM_TYPE_COLUMN ) == "OH" )
					{
						currentValue.insert( 0, "> " );
					}
					else if( GetItemText( nItem, ALARM_TYPE_COLUMN ) == "OL" )
					{
						currentValue.insert( 0, "< " );
					}
					// ++CL16786

					SetItemText(nItem, THRESHOLD_VALUE_COLUMN, currentValue.c_str());

			
					// current value	
					currentValue.erase();
					currentValue = getCurrentValueAsString(dataPoint);	

					listItem.iItem = nItem;
					listItem.iSubItem = CURRENT_VALUE_COLUMN;
					listItem.iImage = getDataPointStatusImageIndex(m_iPoint);
					listItem.mask = LVIF_IMAGE;
					SetItem(&listItem);

					SetItemText(nItem, CURRENT_VALUE_COLUMN, currentValue.c_str());
				}
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				// Not always available
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"displayItem() - %s", e.what());

				SetItemText(nItem, ALARM_TYPE_COLUMN, "");
				showCheckIcon(nItem, ALARM_ENABLED_COLUMN, false, false);
				SetItemText(nItem, THRESHOLD_VALUE_COLUMN, "");
			}
		}
		FUNCTION_EXIT;
	}

	bool CLimitDetailsListCtrl::isCellEnabled (int nItem, int nSubItem)
	{
		FUNCTION_ENTRY ("CLimitDetailsListCtrl::isCellEnabled()");

		// Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
		if (nItem<0)
		{
			return false;
		}
		else
		{
			return m_accessAllowed;
		}

		FUNCTION_EXIT;
	}

	void CLimitDetailsListCtrl::updatePermissions(bool allowed)
	{
		// always enable the dialog windows
		if( !(IsWindowEnabled()) )
			EnableWindow ( true );

		// access to individual cell is through this variable
		m_accessAllowed = allowed;
	}

	bool CLimitDetailsListCtrl::overrideAlarmConditionConfirm ( const std::string & typeOfChange, TA_Base_Bus::DataPointProxySmartPtr & dataPoint, bool enable )
	{
		FUNCTION_ENTRY ("overrideAlarmConditionConfirm()");

        CString actionName;
		CString actionType = typeOfChange.c_str();

		CString dpDescription = dataPoint->getDescription().c_str();
		int pos = dpDescription.Find ( "%", 0 );
		if ( -1 != pos )
		{
			dpDescription.Insert ( pos, "%" );
		}

		std::stringstream temp;
		temp << dataPoint->getEntityName() << ", " << (std::string) dpDescription;
		dpDescription = temp.str().c_str();

        AfxFormatString2 ( actionName, enable ? IDS_ENABLE_DATAPOINT : IDS_DISABLE_DATAPOINT, actionType, dpDescription );

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003, "监控面板");        
        
		return ( selectedButton == IDYES );

		FUNCTION_EXIT;
	}

	bool CLimitDetailsListCtrl::isAnyDataPointsDisplayedOnScreen() const
	{
		return m_equipment->hasInformationPoint();
	}

    std::string CLimitDetailsListCtrl::alarmTypeToString(TA_Base_Bus::EDataPointAlarms alarmType)
    {
        std::string result("");

        switch(alarmType) 
        {
        case TA_Base_Bus::DptOverScaleHi:
            result = TA_Base_Bus::DPTALM_OH;
        	break;
        case TA_Base_Bus::DptOverScaleLo:
            result = TA_Base_Bus::DPTALM_OL;
            break;
        case TA_Base_Bus::DptHiHi:
            result = TA_Base_Bus::DPTALM_HIHI;
            break;
        case TA_Base_Bus::DptHi:
            result = TA_Base_Bus::DPTALM_HI;
            break;
        case TA_Base_Bus::DptLo:
            result = TA_Base_Bus::DPTALM_LO;
            break;
        case TA_Base_Bus::DptLoLo:
            result = TA_Base_Bus::DPTALM_LOLO;
            break;
        default:
            //no other alarm types for analogue datapoint
            break;
        }
        
        return result;
    }
    
    
}