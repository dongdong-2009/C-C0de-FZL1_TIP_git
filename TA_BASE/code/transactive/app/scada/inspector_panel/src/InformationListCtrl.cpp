/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/InformationListCtrl.cpp $
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

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/InformationListCtrl.h"
#include "app/scada/inspector_panel/src/InfoTabHelper.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CInformationListCtrl


	enum EColumns
	{
		DummyColumn				= 0,
		DescriptionColumn		= 1,
		TypeColumn				= 2,
		ValueColumn				= 3
	};


	CInformationListCtrl::CInformationListCtrl()
	:
	CAbstractDataPointListCtrl(),
	m_isReset( false ),
	m_dataPointsDisplayedOnScreen ( false )
	{
		FUNCTION_ENTRY ("CInformationListCtrl()");

		// We will manage the cell requests
		SetManager(this);

		QualityColumn = -1;
		TimestampColumn = -1;

		FUNCTION_EXIT;
	}

	CInformationListCtrl::~CInformationListCtrl()
	{
		FUNCTION_ENTRY ("~CInformationListCtrl()");
		FUNCTION_EXIT;
	}

	BEGIN_MESSAGE_MAP(CInformationListCtrl, CAbstractDataPointListCtrl)
		//{{AFX_MSG_MAP(CInformationListCtrl)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CInformationListCtrl message handlers

	// Helper functions

	void CInformationListCtrl::setupList()
	{
		FUNCTION_ENTRY ("setupList()");	

		this->LockWindowUpdate();
		
		if (true == m_isReset)
		{
			// TD18728, Delete the columns before re-setup the list
			if( TimestampColumn != -1 )
				DeleteColumn(TimestampColumn);
			if( QualityColumn != -1)
				DeleteColumn(QualityColumn);
			this->DeleteColumn(ValueColumn);
			this->DeleteColumn(TypeColumn);
			this->DeleteColumn(DescriptionColumn);
			this->DeleteColumn(DummyColumn);

			//Invalidate();
		}
		

		if( InfoTabHelper::getShowQuality() && InfoTabHelper::getShowTimestamp() )
		{
			QualityColumn = 4;
			TimestampColumn = 5;
		}
		else if( !InfoTabHelper::getShowQuality() && InfoTabHelper::getShowTimestamp() )
		{
			QualityColumn = -1;
			TimestampColumn = 4;
		}
		else if( !InfoTabHelper::getShowTimestamp() && InfoTabHelper::getShowQuality())
		{
			QualityColumn = 4;
			TimestampColumn = -1;
		}
		

		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES | CAbstractDataPointListCtrl::LVS_EX_LABELTIP_XXX );

		// Headings
		//this->InsertColumn(DummyColumn, "DeleteMe" );
  // 		this->InsertColumn(DescriptionColumn,		"Description");
		//this->InsertColumn(TypeColumn,				"Type");
  // 		this->InsertColumn(ValueColumn,				"Current Value");

		//
		//if( QualityColumn != -1)
		//	this->InsertColumn(QualityColumn,			"Quality");
		//if( TimestampColumn != -1 )
		//	this->InsertColumn(TimestampColumn,			"Time");

		this->InsertColumn(DummyColumn, "DeleteMe" );
		this->InsertColumn(DescriptionColumn,		"描述");
		this->InsertColumn(TypeColumn,				"类型",LVCFMT_CENTER);
		this->InsertColumn(ValueColumn,				"当前值");


		if( QualityColumn != -1)
			this->InsertColumn(QualityColumn,			"质量");
		if( TimestampColumn != -1 )
			this->InsertColumn(TimestampColumn,			"时间");

		// Using a 'dummy' column for index 0 prevents
		// MFC from indenting the text by the image size for the 'real' first column
		this->SetColumnWidth(DummyColumn, 0);

		if ( QualityColumn == -1 && TimestampColumn == -1)
		{
			this->SetColumnWidth(DescriptionColumn,		320);
			this->SetColumnWidth(TypeColumn,			70);
			this->SetColumnWidth(ValueColumn,			80);
		}
		else
		{
			this->SetColumnWidth(DescriptionColumn,		180);
			this->SetColumnWidth(TypeColumn,			45);

			if ( QualityColumn != -1 && TimestampColumn != -1)
			{
				this->SetColumnWidth(ValueColumn,			90);
				this->SetColumnWidth(QualityColumn,			170);
				this->SetColumnWidth(TimestampColumn,		130);
			}
			else 
			{
				this->SetColumnWidth(ValueColumn,			90);

				if ( QualityColumn != -1 )
					this->SetColumnWidth(QualityColumn,			130);
				else
					this->SetColumnWidth(TimestampColumn,		110);
			}
		}
		this->UnlockWindowUpdate();
		FUNCTION_EXIT;
	}


	void CInformationListCtrl::updateDataPointDisplay(int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPointDisplay()");

		if (NULL == m_equipment)
			return;

		switch(updateType)
		{
		case TA_Base_Bus::ValueStateUpdate:
        case TA_Base_Bus::AlarmSummaryUpdate://limin++, TD20740
        case TA_Base_Bus::AckSummaryUpdate:  //limin++, TD20740
        case TA_Base_Bus::QualitySummaryUpdate: // for the case where only quality changes.
			{
				std::string		value;
				TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);						
				if ( TimestampColumn != -1 )
				{
    				time_t timeStamp = 0L;
    				value = "";
    				try
    				{
    					timeStamp = (time_t) dataPoint->getTimestamp();
    					CTime now(timeStamp);
    					CString s = now.Format("%H:%M:%S");						
    					value = s.GetBuffer(s.GetLength());										

    					setItemTextImage(nItem, TimestampColumn, value, false);
    				}
    				catch (...)
    				{
    					timeStamp = 0L;
    					value = "";		

    					setItemTextImage(nItem, TimestampColumn, value, false);
    				}
				}

                // TES#1034: Always update status (quality) / current value columns of datapoints
                std::string currentValue = getCurrentValueAsString(dataPoint);
    			setItemTextImage(nItem, ValueColumn, currentValue, true);

				if( QualityColumn != -1 )
				{
    				// Quality of the DataPoint as a string
    				value = dataPoint->getQualityAsString();
					//  [3/13/2010 caiwei]

					if (value == "Bad - Not Connected")
					{
						value = "数据无效 - 无连接";
					}
					else if (value == "Good")
					{
						value = "数据正常";
					}
					else if (value == "Good - Local Override")
					{
						value = "数据正常 - 本地超驰";
					}
					else if (value == "Bad - Last Known Value")
					{
						value = "数据无效 - 上次已知值";
					}
					else if (value == "Bad - Configuration Error")
					{
						value = "数据无效 - 配置错误";
					}
					else if (value == "Bad")
					{
						value = "数据无效";
					}
					else if (value == "Bad - Device Failure")
					{
						value = "数据无效 - 设备故障";
					}
					else if (value == "Bad - Sensor Failure")
					{
						value = "数据无效 - 传感器故障";
					}
					else if (value == "Bad - Comm Failure")
					{
						value = "数据无效 - 串口连接失败";
					}
					else if (value == "Bad - Out of Service")
					{
						value = "数据无效 - 服务中断";
					}
					else if (value == "Uncertain")
					{
						value = "数据不确定";
					}
					else if (value == "Uncertain - Last Usable Value")
					{
						value = "数据不确定 - 上次可使用值";
					}
					else if (value == "Uncertain - Sensor not Accurate")
					{
						value = "数据不确定 - 传感器不正确";
					}
					else if (value == "Uncertain - Engineering Unit Exceeded")
					{
						value = "数据不确定 - 超出边界值";
					}
					else if (value == "Uncertain - Sub-normal")
					{
						value = "数据不确定 - 低于正常水平";
					}
					//  [3/13/2010 caiwei]
    				setItemTextImage(nItem, QualityColumn, value, false);
				}
			}
			break;

		default:
			break;
		}

		FUNCTION_EXIT;
	}

	// By using the preprocessed index vector, reduce linear search and ensure only those
	// datapoints had been sorted and config valid were displayed.
	void CInformationListCtrl::displayDataPoints()
	{
		FUNCTION_ENTRY ("displayDataPoints()");
	
		int validItem = 0;
		int difference = 0;
		int totalItem = GetItemCount();

		// No equipment defined yet.
		if (m_equipment==NULL) 
			return;

		int		 nItem;
		std::string  value;
		std::string  description; 

		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();

		// Get the preprocessed indexes with the required config.
		std::vector<unsigned long>& vectorIndex = m_equipment->getNotWriteableIndex(); 

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

    			// Description
				description = (*dataPoints[iPoint])->getDescription();
				if( validItem > GetItemCount() )
				{	
    				nItem = insertItemTextImage(iPoint, validItem-1, description, false);	
					setItemTextImage(nItem, DescriptionColumn, description, false);
					SetItemData(nItem, iPoint);
				}
				else
				{
					nItem = validItem-1;
					SetItemData(nItem, iPoint);
    				SetItemText(nItem, DescriptionColumn, description.c_str());
					
				}
				
    			// Type of the data point
    			value = (*dataPoints[iPoint])->getShortNameOfType();
    			setItemTextImage(nItem, TypeColumn, value, false);
    			
				if( TimestampColumn != -1 )
				{
    				time_t timeStamp = 0L;
    				value = "";

    				try
    				{
    					timeStamp = (time_t) (*dataPoints[iPoint])->getTimestamp();
    					CTime now(timeStamp);
    					CString s = now.Format("%H:%M:%S");						
    					value = s.GetBuffer(s.GetLength());

    					setItemTextImage(nItem, TimestampColumn, value, false);
    				}
    				catch (...)
    				{
    					timeStamp = 0L;
    					value = "";
    					setItemTextImage(nItem, TimestampColumn, value, false);
    				}
				}		

				// TES#1034: Always update status (quality) / current value columns of datapoints
    			// Status of data point
    			std::string currentValue = getCurrentValueAsString((*dataPoints[iPoint]));  
    			setItemTextImage(nItem, ValueColumn, currentValue, true);

				if( QualityColumn != -1 )
				{
    				// Quality of the DataPoint as a string
    				value = (*dataPoints[iPoint])->getQualityAsString();
					//  [3/13/2010 caiwei]

					if (value == "Bad - Not Connected")
					{
						value = "数据无效 - 无连接";
					}
					else if (value == "Good")
					{
						value = "数据正常";
					}
					else if (value == "Good - Local Override")
					{
						value = "数据正常 - 本地超驰";
					}
					else if (value == "Bad - Last Known Value")
					{
						value = "数据无效 - 上次已知值";
					}
					else if (value == "Bad - Configuration Error")
					{
						value = "数据无效 - 配置错误";
					}
					else if (value == "Bad")
					{
						value = "数据无效";
					}
					else if (value == "Bad - Device Failure")
					{
						value = "数据无效 - 设备故障";
					}
					else if (value == "Bad - Sensor Failure")
					{
						value = "数据无效 - 传感器故障";
					}

					else if (value == "Bad - Comm Failure")
					{
						value = "数据无效 - 通信失败";
					}
					else if (value == "Bad - Out of Service")
					{
						value = "数据无效 - 服务中断";
					}
					else if (value == "Uncertain")
					{
						value = "数据不确定";
					}
					else if (value == "Uncertain - Last Usable Value")
					{
						value = "数据不确定 - 上次可使用值";
					}
					else if (value == "Uncertain - Sensor not Accurate")
					{
						value = "数据不确定 - 传感器不准确";
					}
					else if (value == "Uncertain - Engineering Unit Exceeded")
					{
						value = "数据不确定 - 超出边界范围";
					}
					else if (value == "Uncertain - Sub-normal")
					{
						value = "数据不确定 - 低于正常水平";
					}

					//  [3/13/2010 caiwei]
    				setItemTextImage(nItem, QualityColumn, value, false);
				}

				// set the flag to indicate data points are displayed on screen
				m_dataPointsDisplayedOnScreen = true;    
				
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
			}
			catch (TA_Base_Core::TransactiveException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Unexpected Transactive Exception - %s", e.what());
			}
			catch (...)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Unexpected Exception");
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

	// ICellManager Implementation
	ICellManager::CellType CInformationListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellType()");
		FUNCTION_EXIT;

		return CT_NOTEDITABLE;
	};


    unsigned long CInformationListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }


	ICellManager::CellData CInformationListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellData()");

		ICellManager::CellData   data;

		FUNCTION_EXIT;

		return data;
	}

    CString CInformationListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }

    char CInformationListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }

	bool CInformationListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
	{
		FUNCTION_ENTRY ("UpdateCellValue()");
		FUNCTION_EXIT;

		return false;
	}

	bool CInformationListCtrl::isAnyDataPointsDisplayedOnScreen() const
	{
		return m_equipment->hasInformationPoint();
	}

	//TD15438, override the function to get a chance to adjust columns width.
	BOOL CInformationListCtrl::SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags )
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

				int columnWidth = GetColumnWidth(DescriptionColumn);
				int vsrollWidth = GetSystemMetrics(SM_CXVSCROLL);
				
				//the minimum width is 100
				if (columnWidth - vsrollWidth > 100)
				{
					SetColumnWidth(DescriptionColumn, columnWidth - vsrollWidth);
				}
			}
		}
		
		return isSucceeded;		
	}

	void CInformationListCtrl::resetList()
	{
		m_isReset = true;
		setupList();
		m_isReset = false;
	}
}