/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/DataPointListCtrl.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the basic information in
  * the CInformationDialog class.
  *
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/DataPointListCtrl.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/common_library/src/DpValue.h"

#include "core/exceptions/src/ScadaProxyException.h"
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
	// CAbstractDataPointListCtrl

	const int CAbstractDataPointListCtrl::LVS_EX_LABELTIP_XXX = 0x00004000;

	CAbstractDataPointListCtrl::CAbstractDataPointListCtrl()
		: m_equipment(NULL), CEditableListCtrl(true), m_greenIconIndex(-1), m_redIconIndex(-1), m_orangeIconIndex(-1), 
		  m_blueIconIndex(-1), m_greyIconIndex(-1),m_accessAllowed(false)
	{
		FUNCTION_ENTRY ("CAbstractDataPointListCtrl()");

		FUNCTION_EXIT;
	}

	CAbstractDataPointListCtrl::~CAbstractDataPointListCtrl()
	{
		FUNCTION_ENTRY ("~CAbstractDataPointListCtrl()");

		DestroyIcon(m_greenIcon);
		DestroyIcon(m_redIcon);
		DestroyIcon(m_orangeIcon);
		DestroyIcon(m_blueIcon);
		DestroyIcon(m_greyIcon);

		FUNCTION_EXIT;
	}

	/* SetEquipment
	 *
	 * This function will set the name of the equipment that the data points are to be displayed for.It can 
	 * be called before or after the windows control is on the screen.
	 */
	void CAbstractDataPointListCtrl::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");

		m_equipment = equipment;

		// Update the displayed control
		if (m_hWnd!=0 && m_equipment != NULL)
		{
			//deferred to other party to call displayDataPoints 
			//displayDataPoints();
		}
	}

	BEGIN_MESSAGE_MAP(CAbstractDataPointListCtrl, CEditableListCtrl)
		//{{AFX_MSG_MAP(CAbstractDataPointListCtrl)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
        ON_NOTIFY(HDN_DIVIDERDBLCLICKW, 0, onDividerDoubleClick)
	END_MESSAGE_MAP()



	/////////////////////////////////////////////////////////////////////////////
	// CAbstractDataPointListCtrl message handlers

	int CAbstractDataPointListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		FUNCTION_ENTRY ("OnCreate()");

		if (CWnd::OnCreate(lpCreateStruct) == -1)
		{
			return -1;
		}

		setupList();

		displayDataPoints();

		FUNCTION_EXIT;
		
		return 0;
	}


	void CAbstractDataPointListCtrl::PreSubclassWindow() 
	{
		FUNCTION_ENTRY ("PreSubclassWindow()");

		CWnd::PreSubclassWindow();
		
		setupList();
		displayDataPoints();

		FUNCTION_EXIT;
	}


	// Helper functions

	void CAbstractDataPointListCtrl::updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoint()");

		if (m_hWnd==0 || m_equipment==NULL ) //wenching++ (TD13353)
		{
			return;
		}

		//  Find the data point item with the key
		UINT iPoint = m_equipment->getChildDataPointIndexFromEntityKey(key);

		if (iPoint<m_equipment->getChildDataPointCount())
		{
			//  Find the data point in the list view
			for (int nItem=0;nItem<this->GetItemCount();nItem++)
			{
				if (this->GetItemData(nItem)==iPoint)
				{
					updateDataPointDisplay(iPoint, nItem, updateType);

					// Done
					return;
				}
			}
		}

		FUNCTION_EXIT;
	}

	void CAbstractDataPointListCtrl::updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoint()");

		// No DataNode information here...

		FUNCTION_EXIT;
	}

	int CAbstractDataPointListCtrl::getDataPointStatusImageIndex(UINT iPoint)
	{
		FUNCTION_ENTRY ("getDataPointStatusImageIndex()");

		int iconIndex = -1;

		// if equipment is not defined, there is no point going any further
		if (m_equipment == NULL)
		{
			return iconIndex;
		}

		// if icons have not been loaded, load them up now
		if (m_greenIconIndex == -1 || m_redIconIndex == -1 || m_orangeIconIndex == -1 || m_blueIconIndex == -1 || m_greyIconIndex == -1)
		{
					// Load status icons
			m_greenIcon  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_GREEN));
			m_redIcon    = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_RED));
			m_orangeIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ORANGE));
			m_blueIcon   = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_BLUE));
			m_greyIcon	 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_GREY));

			m_greenIconIndex = addIcon(m_greenIcon);
			m_redIconIndex = addIcon(m_redIcon);
			m_orangeIconIndex = addIcon(m_orangeIcon);
			m_blueIconIndex = addIcon(m_blueIcon);
			m_greyIconIndex = addIcon(m_greyIcon);

			TA_ASSERT(m_greenIconIndex != -1 && m_redIconIndex != -1 && m_orangeIconIndex != -1 && m_blueIconIndex != -1 && m_greyIconIndex != -1,
						"Insufficient space in Image List (DataPointListCtrl)");
		}

		//
		// determine the type of icons to be displayed on screen
		//

		try
		{
			TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);

			TA_Base_Bus::DpValue dpValue = dataPoint->getValue();

			switch ( dataPoint->getStatus() & TA_Base_Bus::QUALITY_MASK )
			{
				case TA_Base_Bus::QUALITY_GOOD:
				{
					if (dataPoint->getStatus() == TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON ) 
//						||dataPoint->getStatus() == TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE)
					{

						iconIndex = m_greenIconIndex;

					}
					else
					{
						iconIndex = -1;
					}				

					break;
				}
				case TA_Base_Bus::QUALITY_UNCERTAIN:
				case TA_Base_Bus::QUALITY_BAD:
				{
					//iconIndex = m_blueIconIndex;
					iconIndex = m_greyIconIndex;

					break;
				}
				default:
				{
					iconIndex = -1;

					break;
				}
			}

			//If the input data point is a derived data point, we only need to set the on and off status.
			if (dataPoint->isDerived() && iconIndex == m_greenIconIndex)
			{
				//GF-109
				//unsigned long nValue = dpValue.getEnum();
				std::string strValue = dpValue.getValueAsString().c_str();

				if (strValue == "Î´¶¨Òå")
				{
					iconIndex = m_blueIconIndex;
				}

			}
			// wenching++ (TD13684)
			// if the data point is in alarm state and its quality is anything but not 
			// TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED, make sure a red icon is displayed
			if ( ( TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED != dataPoint->getStatus() ) && 
				 ( dataPoint->isInAlarmState() ) &&
				 ( iconIndex != m_greyIconIndex ) )
			{
				iconIndex = m_redIconIndex;
			}
			// ++wenching (TD13684)

			if (dataPoint->getStatus() == TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE)
			{
				iconIndex = m_orangeIconIndex;
			}


		}
		// if any exception occured
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"getDataPointStatusImageIndex() - %s", e.what());

			iconIndex = m_greyIconIndex;
		}
		catch ( ... )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"getDataPointStatusImageIndex() - %s", "Unknown Exception while accessing the DataPointProxySmartPtr.");

			iconIndex = m_greyIconIndex;
		}

		return iconIndex;

		FUNCTION_EXIT;
	}

	int CAbstractDataPointListCtrl::insertItemTextImage(int iPoint, int insertNItem, std::string& value, bool addImage)
	{
		FUNCTION_ENTRY ("insertItemTextImage()");

		if (m_equipment == NULL)
		{
			return 0;
		}

		LVITEM   listItem;
		int nItem;

		// Description
		nItem = InsertItem(insertNItem,m_equipment->getChildDataPoint(iPoint)->getDescription().c_str());
		SetItemData(nItem,iPoint);
		
		listItem.iItem = nItem;
		listItem.iSubItem = 0;

		if (addImage)
		{
			listItem.iImage = getDataPointStatusImageIndex(iPoint);
		}
		else
		{
			listItem.iImage = -1;
		}

		listItem.mask = LVIF_IMAGE;
		SetItem(&listItem);

		FUNCTION_EXIT;

		return nItem;
	}

	void CAbstractDataPointListCtrl::setItemTextImage(int nItem, int nSubItem, std::string value, bool addImage)
	{
		FUNCTION_ENTRY ("setItemTextImage()");

		LVITEM   listItem;

		SetItemText(nItem,nSubItem,value.c_str());
		listItem.iItem = nItem;
		listItem.iSubItem = nSubItem;

		if (addImage)
		{
			listItem.iImage = getDataPointStatusImageIndex(GetItemData(nItem));
		}
		else
		{
			listItem.iImage = -1;
		}
		listItem.mask = LVIF_IMAGE;
		SetItem(&listItem);

		FUNCTION_EXIT;
	}


	void CAbstractDataPointListCtrl::setItemTextYesNo(int nItem, int nSubItem, bool yes)
	{
		FUNCTION_ENTRY ("setItemTextYesNo()");

		if (yes)
		{
			SetItemText(nItem, nSubItem,"Yes");
		}
		else
		{
			SetItemText(nItem, nSubItem,"No");
		}

		FUNCTION_EXIT;
	}

	void CAbstractDataPointListCtrl::updatePermissions(bool allowed)
	{
		m_accessAllowed = allowed;

		displayDataPoints();

		EnableWindow(m_accessAllowed);
	}


    std::string CAbstractDataPointListCtrl::getCurrentValueAsString(TA_Base_Bus::DataPointProxySmartPtr& dataPoint)
    {
        std::string currentValue("");
		std::string unitString("");

    	// DataPoint Value - may be override or field value
		std::string alarmState = "";
		try
		{
			alarmState = dataPoint->getAlarmStateShortString();
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"displayItem() - %s", e.what());

        }
        catch (...)
        {
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"displayItem() - %s", "Unknown Exception");

        }	
		if (alarmState != "")
		{
			currentValue = alarmState + " ";
		}
		else
		{
			currentValue = "";
		}

		try
		{
			// Change temperature unit format from "0C" to "°C" using ALT+0176 (TD12415)
			unitString = dataPoint->getUnit();

			CString degreeString = unitString.c_str();
			if (degreeString.Find("?C", 0) >= 0)
			{
				degreeString.Replace(_T('?'), _T('?'));
			}
			unitString.assign(degreeString);

			currentValue +=  dataPoint->getValueAsString();
		    currentValue += " ";
		    currentValue += unitString;

		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"displayItem() - %s", e.what());

        }
        catch (...)
        {
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"displayItem() - %s", "Unknown Exception");

        }	
		
        return currentValue;
    }


	bool CAbstractDataPointListCtrl::isAnyDataPointsDisplayedOnScreen() const
	{
		// always true, unless override by specific classes
		return true;
	}


    void CAbstractDataPointListCtrl::onDividerDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) 
	{
        const int IMAGE_SIZE = 16;

		HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

		// The list control does not resize the column to include the image. Need
		// to do this ourselves.

		int numItems = GetItemCount();
		int column = phdn->iItem;
		int maxWidth = 0;
		int currWidth;

        LVITEM currentItem;
        currentItem.mask = LVIF_IMAGE;
        currentItem.iSubItem = column;

		for (int row = 0; row < numItems; row++)
		{
			currWidth = GetStringWidth(GetItemText(row, column).GetBuffer(0));

            // Check if the item contains an image.
            currentItem.iItem = row;
            GetItem(&currentItem);
            if (currentItem.iImage != -1)
            {
                // Row contains an image. 
                currWidth += IMAGE_SIZE;
            }

			if (currWidth > maxWidth)
			{
				maxWidth = currWidth;
			}
		}

        // First column leaves an extra space.
        if (column == 0)
        {
            maxWidth += IMAGE_SIZE;
        }

        // Set the column's width. Add on an extra image width to make it look better.
		SetColumnWidth(column, maxWidth + IMAGE_SIZE);
		
		*pResult = 0;
	}
}
