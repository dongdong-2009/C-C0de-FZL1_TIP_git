/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/LimitListCtrl.cpp $
* @author:  Andy Parker
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2012/07/16 13:33:06 $
* Last modified by:  $Author: hui.wang $
* 
* This class manages the list display used to show the basic information in
* the CInformationDialog class.
*
*/

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/LimitDetailsListCtrl.h"
#include "app/scada/inspector_panel/src/LimitListCtrl.h"
#include "app/scada/inspector_panel/src/AccessControlHelper.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaProxyException.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CLimitListCtrl


	enum columns
	{
		DESCRIPTION_COLUMN		= 0,
	};


	CLimitListCtrl::CLimitListCtrl(CEdit& limitInfo, CLimitDetailsListCtrl& limitDetails) 
		:
	CAbstractDataPointListCtrl(), 
		m_limitInfo (limitInfo),
		m_limitDetails (limitDetails),
		m_dataPointsDisplayedOnScreen ( false )
	{
		FUNCTION_ENTRY ("CLimitListCtrl()");

		// We will manage the cell requests
		SetManager(this);

		FUNCTION_EXIT;
	}

	CLimitListCtrl::~CLimitListCtrl()
	{
		FUNCTION_ENTRY ("~CLimitListCtrl()");
		FUNCTION_EXIT;
	}

	BEGIN_MESSAGE_MAP(CLimitListCtrl, CAbstractDataPointListCtrl)
		//{{AFX_MSG_MAP(CLimitListCtrl)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CLimitListCtrl message handlers

	// Helper functions

	void CLimitListCtrl::setupList()
	{
		FUNCTION_ENTRY ("setupList()");

		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES | CAbstractDataPointListCtrl::LVS_EX_LABELTIP_XXX );

		// Headings
		//this->InsertColumn(DESCRIPTION_COLUMN,"Description");
		this->InsertColumn(DESCRIPTION_COLUMN,"描述");
		this->SetColumnWidth(DESCRIPTION_COLUMN,210);

		FUNCTION_EXIT;
	}


	void CLimitListCtrl::updateDataPointDisplay(int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPointDisplay()");


		switch(updateType)
		{
		case TA_Base_Bus::ConfigStateUpdate:
			{
				std::string		value;
				TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);

				// Update the point	
				value = dataPoint->getDescription();
				SetItemText(nItem, DESCRIPTION_COLUMN, value.c_str());
			}
			break;

		default:
			break;
		}

		FUNCTION_EXIT;
	}

	void CLimitListCtrl::refreshDataPoints()
	{
		FUNCTION_ENTRY ("refreshDataPoints()");

		// No equipment defined yet.
		if (m_equipment==NULL) 
			return;

		displayDataPoints();

		FUNCTION_EXIT;
	}

	// By using the preprocessed index vector, reduce linear search and ensure only those
	// datapoints had been sorted and config valid were displayed.
	void CLimitListCtrl::displayDataPoints()
	{
		FUNCTION_ENTRY ("displayDataPoints()");

		int validItem = 0;
		int difference = 0;
		int totalItem = GetItemCount();

		// No equipment defined yet.
		if (m_equipment==NULL) 
			return;

		int				nItem;
		unsigned int	iPoint;
		std::string		value;

		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();

		// Get the preprocessed indexes with the required config.
		std::vector<unsigned long>& vectorIndex = m_equipment->getLimitIndex();

		if (0 == vectorIndex.size())
		{
			DeleteAllItems();
			m_limitDetails.clearList();

			return;
		}

		// Only display the datapoints with index in the vector
		for (unsigned int i=0;i<vectorIndex.size();i++)
		{
			iPoint = vectorIndex.at(i);

			try
			{
				// 4669 FAT TES 205: Limit Page of Inspector Panel should only applicable to AI
				validItem++;

				// Description
				value = (*dataPoints[iPoint])->getDescription();
				if ( validItem > GetItemCount() )
				{
					nItem = insertItemTextImage(iPoint, validItem-1, value, false);
					setItemTextImage(nItem, DESCRIPTION_COLUMN, value, false);
					SetItemData(nItem, iPoint);
				}
				else
				{
					nItem = validItem-1;
					SetItemData(nItem, iPoint);
					SetItemText(nItem, DESCRIPTION_COLUMN, value.c_str());
				}

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

		if ( GetItemCount() > 0)
		{
			std::ostringstream text;
			iPoint = GetItemData(0);

			if ( (*dataPoints[iPoint])->isConfigValid() )
			{
				SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

				// Show type
				m_limitInfo.SetWindowText("");
				text.clear();
				//text << "Type of \"";
				text << "\"";
				text << (*dataPoints[iPoint])->getDescription();
				//text << "\" is ";
				text << "\"的类型是";
				text << (*dataPoints[iPoint])->getShortNameOfType();
				text << " (";
				text << (*dataPoints[iPoint])->getTypeAsString();
				text << ").";
				m_limitInfo.SetWindowText(text.str().c_str());

				// Show limit details list
				m_limitDetails.setDataPoint(iPoint);
			}
		}

		FUNCTION_EXIT;
	}

	// ICellManager Implementation
	ICellManager::CellType CLimitListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellType()");

		std::ostringstream text;

		// Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
		// or No access to this dialog
		if (item<0 || !m_accessAllowed)	
		{
			return CT_NOTEDITABLE;
		}

		std::vector<unsigned long>& vectorIndex = m_equipment->getLimitIndex();
		if (0 == vectorIndex.size())
		{
			return CT_NOTEDITABLE;
		}
		// We actually don't want to edit anything, but a row has been clicked on, so we will use this trigger
		// to display the correct limit information
		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(GetItemData(item));

		m_limitInfo.SetWindowText("");
		text.clear();
		//text << "Type of \"";
		text << "\"";
		text << dataPoint->getDescription();
		//text << "\" is ";
		text << "\"的类型是";
		text << dataPoint->getShortNameOfType();
		text << " (";
		text << dataPoint->getTypeAsString();
		text << ").";
		m_limitInfo.SetWindowText(text.str().c_str());
		/*

		// Clear the edit box
		m_limitInfo.SetWindowText("");

		text.clear();

		// Show type first
		text << "Type of \"";
		text << dataPoint->getDescription();
		text << "\" is ";
		text << dataPoint->getShortNameOfType();
		text << " (";
		text << dataPoint->getTypeAsString();
		text << ").";
		m_limitInfo.SetWindowText(text.str().c_str());
		*/

		//
		// determine the right to change the alarm limits of the data point.  According to
		// GUI note, only consider for AI, everything else has no rights
		//

		// find if the operator profile has the right to change the alarm limits
		bool isLimitsTabAccessAllowed = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
			dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::IP_LIMITS_TAB_ACCESS);

		// update screen accordingly
		m_limitDetails.updatePermissions ( isLimitsTabAccessAllowed );

		// display the alarm limits detail
		m_limitDetails.setDataPoint(GetItemData(item));

		FUNCTION_EXIT;

		return CT_NOTEDITABLE;
	};

	unsigned long CLimitListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		return 0;
	}


	ICellManager::CellData CLimitListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellData()");

		ICellManager::CellData   data;

		FUNCTION_EXIT;

		return data;
	}

	CString CLimitListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		TA_ASSERT(false, "There are no cells of type CT_MASK");

		return "";
	}

	char CLimitListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		TA_ASSERT(false, "There are no cells of type CT_MASK");

		return 0;
	}

	bool CLimitListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
	{
		FUNCTION_ENTRY ("UpdateCellValue()");
		FUNCTION_EXIT;

		return false;
	}

	bool CLimitListCtrl::isAnyDataPointsDisplayedOnScreen() const
	{
		return m_equipment->hasInformationPoint();
	}
}