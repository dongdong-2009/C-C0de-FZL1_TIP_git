/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/MeterListCtrl.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the basic control information in
  * the CControlDialog class.
  *
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/ConfirmCommandDialog.h"
#include "app/scada/inspector_panel/src/MeterListCtrl.h"
#include "app/scada/Inspector_Panel/src/ConfirmAdjustDialog.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace TA_Base_App
{
    const COLORREF CMeterListCtrl::COLOUR_ALARM = 0x000000FF;  // colour to be used for showing alarms
    const COLORREF CMeterListCtrl::COLOUR_DISABLE = 0x00808080; // colour to be used for showing disabled data points
    const COLORREF CMeterListCtrl::COLOUR_NORMAL = 0x00000000; // colour to be used for showing normal data points

    /////////////////////////////////////////////////////////////////////////////
	// CDataPointListCtrl

	enum columns
	{
		DescriptionColumn			= 0,
		DataPointTypeColumn			= 1,
		CurrentValueColumn			= 2,
		MeterAdjustColumn			= 3,
	};

	CMeterListCtrl::CMeterListCtrl()
	:
	CAbstractDataPointListCtrl(),
	m_dataPointsDisplayedOnScreen ( false ),
	m_isInMeterAdjust ( false )
	{
		FUNCTION_ENTRY ("CMeterListCtrl()");

		// We will manage the cell requests
		SetManager(this);

		FUNCTION_EXIT;
	}

	CMeterListCtrl::~CMeterListCtrl()
	{
		FUNCTION_ENTRY ("~CMeterListCtrl()");
		FUNCTION_EXIT;
	}


	BEGIN_MESSAGE_MAP(CMeterListCtrl, CAbstractDataPointListCtrl)
		//{{AFX_MSG_MAP(CMeterListCtrl)
		ON_WM_LBUTTONDOWN()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	// Helper functions

	void CMeterListCtrl::setupList()
	{
		FUNCTION_ENTRY ("setupList()");

		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES | CAbstractDataPointListCtrl::LVS_EX_LABELTIP_XXX);

		// Headings
  // 		this->InsertColumn(DescriptionColumn,			"Description");
		//this->InsertColumn(DataPointTypeColumn,			"Type");
		//this->InsertColumn(CurrentValueColumn,			"Current Value");
		//this->InsertColumn(MeterAdjustColumn,			"Adjust Meter");

		this->InsertColumn(DescriptionColumn,			"描述");
		this->InsertColumn(DataPointTypeColumn,			"类型",LVCFMT_CENTER);
		this->InsertColumn(CurrentValueColumn,			"当前值");
		this->InsertColumn(MeterAdjustColumn,			"测量调整");

		this->SetColumnWidth(DescriptionColumn,		180);
		this->SetColumnWidth(DataPointTypeColumn,	45);
		this->SetColumnWidth(CurrentValueColumn,	250);
		// TES995 - Specify LVSCW_AUTOSIZE_USEHEADER to fill rest of list control's width
		this->SetColumnWidth(MeterAdjustColumn,		LVSCW_AUTOSIZE_USEHEADER);

		FUNCTION_EXIT;
	}

	// By using the preprocessed index vector, reduce linear search and ensure only those
	// datapoints had been sorted and config valid were displayed.
	void CMeterListCtrl::displayDataPoints()
	{
		FUNCTION_ENTRY ("DisplayDataPoints()");

		int validItem = 0;
		int difference = 0;
		int totalItem = GetItemCount();
	
		if (NULL == m_equipment)
			return;

		int				nItem = 0;
		std::string		value;

		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();

		// Get the preprocessed indexes with the required config.
		std::vector<unsigned long>& vectorIndex = m_equipment->getMeterIndex();

		if (0 == vectorIndex.size())
		{
			DeleteAllItems();
			return;
		}
		
		// Only display the datapoints with index in the vector
		for (unsigned int i=0;i<vectorIndex.size();i++)
		{
			unsigned int iPoint = vectorIndex.at(i);

			try
			{
				validItem++;

    			// Description (Equipment Attributes)
				if( validItem > GetItemCount() )
				{
                    std::string description = (*dataPoints[iPoint])->getDescription();
					nItem = insertItemTextImage(iPoint, validItem-1, description, false);	
				}
				else
				{
					nItem = validItem-1;
				}
				SetItemData(nItem, iPoint);

				displayItem ( nItem );

				// set the flag to indicate data points are displayed on screen
				m_dataPointsDisplayedOnScreen = true;
			
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

	//TD15879
	void CMeterListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
	{
		if (false == m_isInMeterAdjust)
		{
			CListCtrl::OnLButtonDown(nFlags, point);
		}
		else
		{
			// Prevent the LButton Down hold by the list before ConfirmAjust Dialog DoModal
			CPoint ptCursor;
			GetCursorPos(&ptCursor);
			ScreenToClient(&ptCursor);
			ReleaseCapture();
			SendMessage(WM_LBUTTONUP, 0, MAKELPARAM(ptCursor.x, ptCursor.y));	
			SendMessage(WM_MOUSEMOVE, 0, MAKELPARAM(ptCursor.x, ptCursor.y));
			SendMessage(WM_KILLFOCUS, 0, MAKELPARAM(ptCursor.x, ptCursor.y));
		}
	}


	// ICellManager Implementation
	ICellManager::CellType CMeterListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellType()");

		// if the specified item is valid
		if (NULL != m_equipment && item >= 0)	
		{
			// only MeterAdjustColumn column is editable
			if ( ( subItem >= 0 ) && ( subItem == MeterAdjustColumn ) )
			{
				// get the DataPoint array from the equipment
				TA_Base_Bus::DataPointProxySmartPtr & dataPoint = m_equipment->getChildDataPoint ( listCtrl->GetItemData ( item ) );

				std::stringstream statusLine;
				statusLine << dataPoint->getDescription() << ": ";

				// check the operator profile for access
				bool isMeterAdjustAllowed = AccessControlHelper::getInstance()->IsActionPermitted ( dataPoint->getEntityKey(), AccessControlHelper::DATA_POINT_METER_ADJUST );

				// if the operator is allowed to adjust the totalised value
				if ( true == isMeterAdjustAllowed )
				{
					m_isInMeterAdjust = true;
					return CT_NUMBER;
				}
				else
				{
					statusLine << "No Access.";
				}
			}
		}

		return CT_NOTEDITABLE;

		FUNCTION_EXIT;
	};


    unsigned long CMeterListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }


	ICellManager::CellData CMeterListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellData()");

		TA_ASSERT (m_equipment!=NULL,"Equipment not defined");

		ICellManager::CellData   data;

		FUNCTION_EXIT;

		return data;
	}

    CString CMeterListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }

    char CMeterListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }

	bool CMeterListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
	{
		FUNCTION_ENTRY ("UpdateCellValue()");

		TA_ASSERT (m_equipment!=NULL,"Equipment not defined.");

		bool cellValueUpdated = false;
		bool adjustFailed = false;

		// if the specified item is valid
		if (item >= 0)	
		{
			// only MeterAdjustColumn column is editable
			if ( ( subItem >= 0 ) && ( subItem == MeterAdjustColumn ) )
			{
				std::string  strValue = value;
				double correction = 0;
				double adjustedValue = 0;
				
				// Get the data point array position
				UINT iPoint = listCtrl->GetItemData(item);
				TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);
				DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();
				
				std::stringstream statusLine;
				statusLine.precision(10);
				statusLine << dataPoint->getDescription() << ": ";

				try
				{
					//
					// sanity checks on value entered by the operator
					//
					
					std::stringstream valueStream;
					valueStream.precision(12);
					valueStream << strValue;

					// if the value entered by the operator is purely a numerical string
					if ( valueStream >> correction )
     				{
						//
						// get the some configured parameters that are required to
						// calculate the adjusted value for the Metered DataPoint, based on
						// the value entered by the operator
						//

						double accumulatedInputValue = 0;

						//TD15121, need not to read from database, use datapoint current value instead
						accumulatedInputValue = dataPoint->getValueAsDouble();

						// calculate the new totalised value, based on the entered correction,
						// the current accumulated input and the meter factor
						// TD15121, accumulatedInputValue already include meterFactor.
						adjustedValue = accumulatedInputValue + correction;

						// if eng unit conversion is defined
						if ( false == dataPoint->getUnit().empty() )
						{
							// if the adjusted value is outside the range
							if ( ( adjustedValue < dataPoint->getLimit ( true ) ) ||	// low eng limit
								 ( adjustedValue > dataPoint->getLimit ( false ) ) )	// high eng limit
							{
								// show text in Status box
								//  [3/14/2010 benxun]
								/*
								statusLine << "Adjusted By " << strValue << " Failed.";
								statusLine << " Valid range is from ";
								statusLine << dataPoint->getLimit(true) << " to ";
								*/

								statusLine <<  strValue << " 调整失败.";
								statusLine << " 有效值从 ";
								statusLine << dataPoint->getLimit(true) << " 到 ";
								//  [3/14/2010 benxun]
								statusLine << dataPoint->getLimit(false) << " ";
								statusLine << dataPoint->getUnit() << ".";													
								adjustFailed = true;
							}
						}
					}

					// if the value entered by the operator is an alpha-numerical string
					else
					{
						// show text in Status box
						if (0 == strValue.compare(""))
						{
							//  [3/14/2010 benxun]
							//statusLine << "Adjust " << strValue << "Failed.";
							statusLine << "调整 " << strValue << "失败.";
							//  [3/14/2010 benxun]
						}
						else
						{
							//  [3/14/2010 benxun]
							//statusLine << "Adjusted By " << strValue << " Failed.";
							statusLine << strValue << " 调整失败.";
							//  [3/14/2010 benxun]
						}
						adjustFailed = true;
					}

					// if the flag is set
					if ( false == adjustFailed )
					{
						//
						// get confirmation from the operator
						//

						CConfirmAdjustDialog     dlg;

						// Confirm update
						std::stringstream displayValue;
						displayValue.precision(12); //14420++
						displayValue << "Adjust Meter: "  << correction << " - Adjusted Current Value: " << adjustedValue;

						dlg.m_equipment = dataNode->getAssetName().c_str();
						dlg.m_equipment += "\r\n";
						dlg.m_equipment += dataNode->getLabel().c_str();
						dlg.m_dataPoint = dataPoint->getDescription().c_str();
						dlg.m_newValue  = displayValue.str().c_str();

						// if acknowledged to go ahead
						if (dlg.DoModal()==IDOK)
						{
							//
							// adjust the data point now
							//

							std::string message = "Adjusting Metered DataPoint \"" + dataPoint->getDescription() + "\" by " + strValue;

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
								"UpdateCellValue() - %s", message.c_str());

							dataPoint->setMeteredCorrectionValue(correction, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

							message = "Adjusted meter DataPoint \"" + dataPoint->getDescription() + "\".";

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
								"UpdateCellValue() - %s", message.c_str());

							// show text in Status box
							//  [3/14/2010 benxun]
							//statusLine << "Adjusted By " << strValue << " Successful.";
							statusLine << strValue << " 调整成功.";
							//  [3/14/2010 benxun]

							// set the flag to indicate animation need to be updated
							cellValueUpdated = true;
						}
					}
				}
				catch ( TA_Base_Core::ScadaProxyException & e )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"UpdateCellValue() - %s", e.what());

					adjustFailed = true;
					//  [3/14/2010 benxun]
					//statusLine << "Adjusted By " << strValue << " Failed.";
					statusLine << strValue << " 调整失败.";
					//  [3/14/2010 benxun]
				}
				catch (...)
				{
					std::string message="Error: Unknown exception when adjusting meter DataPoint " + 
										dataPoint->getEntityName() + ".";

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"UpdateCellValue() - %s", message.c_str());

					adjustFailed = true;
					//  [3/14/2010 benxun]
					//statusLine << "Adjusted By " << strValue << " Failed.";
					statusLine  << strValue << " 调整失败.";
					//  [3/14/2010 benxun]
					
				}

				// Post status message
				if (true == adjustFailed || true == cellValueUpdated)
				{
					std::string* statusLineStr = new std::string(statusLine.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
				}
			}
		}

		// Reset the flag so that the LBUTTONDOWN will be performed normally
		m_isInMeterAdjust = false; 

		return cellValueUpdated;

		FUNCTION_EXIT;
	}

	void CMeterListCtrl::updateDataPointDisplay(int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("UpdateDataPointDisplay()");

		if (NULL == m_equipment)
		{
			return;
		}

		displayItem ( nItem );

		FUNCTION_EXIT;
	}
	

	void CMeterListCtrl::updatePermissions()
	{
		EnableWindow(true);

	}

	void CMeterListCtrl::displayItem(int nItem)
	{
		std::string			value;
		TA_Base_Bus::DataPointProxySmartPtr & dataPoint = m_equipment->getChildDataPoint ( GetItemData( nItem ) );

		// Description (Equipment Attributes)
		value = dataPoint->getDescription();
		setItemTextImage(nItem, DescriptionColumn, value, false);

		// Type of the data point
		value = dataPoint->getShortNameOfType();
		setItemTextImage(nItem, DataPointTypeColumn, value, false);

		// DataPoint Value - may be override or field value
		try
		{
			std::string alarmState = dataPoint->getAlarmStateShortString();

			if (alarmState != "")
			{
				value = alarmState + " ";
			}
			else
			{
				value = "";
			}
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"displayItem() - %s", e.what());

			value = "";
		}	
		
		value +=  dataPoint->getValueAsString();
		
		try
		{
			value += " ";
			value += dataPoint->getUnit();
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"displayItem() - %s", e.what());
		}	

		setItemTextImage(nItem, CurrentValueColumn, value, true);
	}

	bool CMeterListCtrl::isAnyDataPointsDisplayedOnScreen() const
	{
		if (NULL == m_equipment)
			return false;

		return m_equipment->hasMeteredPoint();
	}

	//TD15438, override the function to get a chance to adjust columns width.
	BOOL CMeterListCtrl::SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags )
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
				
				int columnWidth = GetColumnWidth(CurrentValueColumn);
				int vsrollWidth = GetSystemMetrics(SM_CXVSCROLL);
				
				//the minimum width is 100
				if (columnWidth - vsrollWidth > 100)
				{
					SetColumnWidth(CurrentValueColumn, columnWidth - vsrollWidth);
				}
			}
		}
		
		return isSucceeded;	
	}
}
