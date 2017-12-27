/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusListCtrl.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class is responsible for displaying the ListCtrl in the main window of the application.
 * It handles the column preparation and supports list sorting, column hiding/unhiding and
 * the image list (icons) to display.
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786 4284)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "stdafx.h"

#include "equipmentstatusviewer.h"
#include "EquipmentStatusListCtrl.h"
#include "CustomDefines.h"
#include "EquipmentStatusViewerDlg.h"

#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

// NOTE : Exception related
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

// custom progress bar
#include "progressbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

	BEGIN_MESSAGE_MAP(CEquipmentStatusListCtrl, CListCtrl)
		//{{AFX_MSG_MAP(CEquipmentStatusListCtrl)
        ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	CEquipmentStatusListCtrl::CEquipmentStatusListCtrl( TA_Base_App::EquipmentStatusListItemMap const& reportItems)
        :   m_sortParams( reportItems), m_currentSortCol(ESL_COLUMN_MAX)
	{
		for (int i=0; i<ESL_COLUMN_MAX; i++)
			m_bAscending[i] = true;

	}

	CEquipmentStatusListCtrl::~CEquipmentStatusListCtrl()
	{
	}


	void CEquipmentStatusListCtrl::PreSubclassWindow() 
	{
        // TD12080: Enable item tooltips.
		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES | LVS_EX_INFOTIP);

    	m_icons[ESL_GREEN]  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_GREEN));
		m_icons[ESL_RED]    = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_RED));
		m_icons[ESL_ORANGE] = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ORANGE));
		m_icons[ESL_BLUE]   = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_BLUE));

        m_imageList.Create(16, 16, ILC_COLOR8, 4, 0);

        for (int i = 0; i < ESL_ICON_MAX; ++i )
        {
            HICON icon = getIconHandle(EIcon(i));

            if (0 == icon || m_imageList.Add(icon) < 0)
            {
                TA_ASSERT(FALSE, "Failed to load icon/add to image list");
            }
        }

        // Unfortunately the first column is always indented no matter what
        // (without an icon being set).  If we set the iIndent parameter to 0 or -1,
        // the system ignores it (still indented).  If we set it to -2 it responds by
        // inverting backwards properly (so
        // must have special case checking to inhibit removing the indent)
        SetImageList(&m_imageList, LVSIL_SMALL);

        //TD16889 Mintao 
		// Column Headings

        // DUMMY_COLUMN Used to get rid of indentation on Asset column, as when we set
        // icons for the list control the first column always has indentation by one icon width..
        //InsertColumn(DUMMY_COLUMN,          "");
   		InsertColumn(ASSET_COLUMN,			"Asset");
   		InsertColumn(DESCRIPTION_COLUMN,	"Equipment Description");
		InsertColumn(DATAPOINT_COLUMN,		"Point Description");
		InsertColumn(TYPE_COLUMN,			"Type");
		InsertColumn(VALUE_COLUMN,			"Value");
        InsertColumn(QUALITY_COLUMN,        "Quality");
		InsertColumn(STATUS_COLUMN,			"Status");
		InsertColumn(MODIFIED_BY_COLUMN,	"Operator");
		InsertColumn(TIMESTAMP_COLUMN,		"Timestamp");

        // TES #760: The ASSET and DESCRIPTION columns should have the same
        // width as the same columns in the Alarm Banner - these being
        // 250 pixels for asset
        // 280 pixels for description
		//cl-17484-lkm
		// Column Widths
        //hideColumn(DUMMY_COLUMN);
		SetColumnWidth(ASSET_COLUMN,		180);   // Used to be 150//240
		SetColumnWidth(DESCRIPTION_COLUMN,	330);   // Used to be 246//285
		SetColumnWidth(DATAPOINT_COLUMN,	150);	// +60//160
		SetColumnWidth(TYPE_COLUMN,			50);	// +40//60
		SetColumnWidth(VALUE_COLUMN,		120);	// +80//120
        hideColumn(QUALITY_COLUMN);
		SetColumnWidth(STATUS_COLUMN,		120);//115
        SetColumnWidth(MODIFIED_BY_COLUMN,  150);//150
        //TD16889 Mintao 
		//cl-17484-lkm
		//
		// TD 4858: Reinstate TimeStamp column
		//
        // TES #761: Hide "Time"  column unless command line option enables them

        //if (TA_Base_Core::RunParams::getInstance().isSet("DisplayTimeColumn"))
        //{
            // User requested we display columns

		/////////////////////////////////////////////////////////////////////
		//ColumnWidth question modified by: zhangjunsheng 2008/12/10 17:00:00

        //  SetColumnWidth(TIMESTAMP_COLUMN, 150);//125
			SetColumnWidth(TIMESTAMP_COLUMN, 180);//125

		//ColumnWidth question/////////////////////////////////////////////////////

            // Must reduce the widths of other columns by 60 pixels each
            // to make room for this new column
            //SetColumnWidth(DATAPOINT_COLUMN, GetColumnWidth(DATAPOINT_COLUMN) - 60);
            //SetColumnWidth(VALUE_COLUMN, GetColumnWidth(VALUE_COLUMN) - 60);
        //}
        //else
        //{
        //	SetColumnWidth(TIMESTAMP_COLUMN,	0);
            // Default behaviour is to hide columns
        //  hideColumn(TIMESTAMP_COLUMN);
        //}

        // Any extra space can be assigned to description column
        TA_Base_Bus::FixedHeaderCtrl::expandColumnToFitListWidth(*this, DESCRIPTION_COLUMN, true);

		CListCtrl::PreSubclassWindow();
	}


    void CEquipmentStatusListCtrl::prepareColumns(const std::string& reportKey)
    {
        hideColumn(QUALITY_COLUMN);
        if (ACTIVE_PROFILE_MMS_REPORT == reportKey)
        {
			/////////////////////////////////////////////////////////////////////
			//ColumnWidth question modified by: zhangjunsheng 2008/12/10 17:00:00
			
		    DeleteColumn(DESCRIPTION_COLUMN);
   		    InsertColumn(DESCRIPTION_COLUMN, "Subsystem");
		    hideColumn(ASSET_COLUMN);
		    hideColumn(DATAPOINT_COLUMN);
		    hideColumn(TYPE_COLUMN);
		    hideColumn(VALUE_COLUMN);
			SetColumnWidth(ASSET_COLUMN,		250);   // Used to be 150//240
			SetColumnWidth(STATUS_COLUMN,		150);//115
			SetColumnWidth(TIMESTAMP_COLUMN, 280);//125
			SetColumnWidth(MODIFIED_BY_COLUMN,  200);//150
			SetColumnWidth(DESCRIPTION_COLUMN, 400);//285//cl-17484-lkm

			//ColumnWidth question/////////////////////////////////////////////////////
        }
        else
        {
            //TD16889 Mintao 
		    DeleteColumn(DESCRIPTION_COLUMN);
   		    InsertColumn(DESCRIPTION_COLUMN, "Equipment Description");
		    SetColumnWidth(DESCRIPTION_COLUMN, 330);//285//cl-17484-lkm
		    showColumn(ASSET_COLUMN, 180);//240//cl-17484-lkm
		    showColumn(DATAPOINT_COLUMN, 150);//160//cl-17484-lkm
		    showColumn(TYPE_COLUMN, 50);//60//cl-17484-lkm
		    showColumn(VALUE_COLUMN, 120);//120//cl-17484-lkm
            //TD16889 Mintao 
        }
    }


	void CEquipmentStatusListCtrl::SortColumn(int aCol)
	{
        m_sortParams.ascending = m_bAscending[aCol];
        m_sortParams.sortColumn = aCol;
        //TD16889 Mintao 
        m_currentSortCol = aCol;
        SortItems( CompareFunc, (LPARAM) &m_sortParams);

		// show the ascending/descending icon
		
		/////////////////////////////////////////////////////////////////////
		//Launch Manager modified by: zhangjunsheng 2008/10/16 13:00:00
		
		//for (int thisCol = ESL_COLUMN_MIN; thisCol < ESL_COLUMN_MAX; thisCol++)
		for (int thisCol = -1; thisCol < ESL_COLUMN_MAX; thisCol++)
			
		//Launch Manager/////////////////////////////////////////////////////
		{
			HDITEM item;
			GetHeaderCtrl()->GetItem(thisCol, &item);

			// delete the bitmap object
			if (item.hbm != NULL)
			{
				DeleteObject(item.hbm); 
				item.hbm = NULL; 
			}

			if (thisCol == aCol)
			{
				item.mask = HDI_BITMAP | HDI_FORMAT;
				item.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;

				if (m_bAscending[aCol] == true)
					item.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SORTUP), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
				else
					item.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SORTDOWN), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
			} else
			{
				item.mask = HDI_FORMAT;
				item.fmt &= ~HDF_BITMAP;
				item.fmt &= ~HDF_BITMAP_ON_RIGHT;
			}

			GetHeaderCtrl()->SetItem(thisCol, &item);
		}
	}


	void CEquipmentStatusListCtrl::SetSortOrder(int aCol, bool aAscending)
	{
		m_bAscending[aCol] = aAscending;
	}


	bool CEquipmentStatusListCtrl::GetSortOrder(int aCol)
	{
		return m_bAscending[aCol];
	}


    int CALLBACK CEquipmentStatusListCtrl::CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        //  Compare two lines (rows) in the list control and return the relative position
        //  of item 1 to item 2
        //  ie. return -1 if item 1 should precede item 2, and +1 if it should go after item 2
        //
        Sort_Parameters& sortParams = *(Sort_Parameters*)lParamSort;

        EquipmentStatusListItemMap::const_iterator it1 = sortParams.reportItems.find( static_cast<unsigned long>(lParam1));
        EquipmentStatusListItemMap::const_iterator it2 = sortParams.reportItems.find( static_cast<unsigned long>(lParam2));
        if ( (sortParams.reportItems.end() == it1) || (sortParams.reportItems.end() == it2) )
        {
            //  Nothing to compare
            return 0;
        }

        int compareResult = it1->second->compare( *it2->second, (EslColumns) sortParams.sortColumn);
        if (!sortParams.ascending)
            compareResult = -compareResult;

        return compareResult;
    }


    HICON CEquipmentStatusListCtrl::getIconHandle(const EIcon& icon)
    {
        IconMap::iterator itr = m_icons.find(icon);
        if (itr == m_icons.end())
        {
            return 0;
        }
        else
        {
            return itr->second;
        }
    }


    void CEquipmentStatusListCtrl::OnDestroy()
    {
        CListCtrl::OnDestroy();

        for (IconMap::iterator itr = m_icons.begin(); itr != m_icons.end(); ++itr )
        {
            HICON handle = itr->second;
            if (0 != handle)
            {
                DestroyIcon(handle);
            }
        }
        m_icons.clear();
    }


    void CEquipmentStatusListCtrl::hideColumn(int colIndex)
    {
        TA_ASSERT(colIndex >= 0 && colIndex < GetHeaderCtrl()->GetItemCount(), "Invalid column index");

        // Resize the column width to zero as it is now hidden
        SetColumnWidth(colIndex, 0);
        m_hiddenCols.insert(std::set<int>::value_type(colIndex));
    }


    void CEquipmentStatusListCtrl::showColumn(int colIndex, int width)
    {
        TA_ASSERT(colIndex >= 0 && colIndex < GetHeaderCtrl()->GetItemCount(), "Invalid column index");
        TA_ASSERT(width >= 0, "Invalid column width");

        // TD12080: hide zero width columns
        if (width == 0)
        {
            hideColumn(colIndex);
            return;
        }

        // Adjust the column to the specified width to make it visible
        SetColumnWidth(colIndex, width);
        m_hiddenCols.erase(std::set<int>::value_type(colIndex)); // TD12080: enable resizing of this column
    }


    BOOL CEquipmentStatusListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
    {
        NMHDR* pNMHDR = (NMHDR*)lParam;

        // Stop any hidden columns from being resized
        if(pNMHDR->code == HDN_BEGINTRACKW || pNMHDR->code == HDN_BEGINTRACKA|| pNMHDR->code == HDN_ITEMCHANGING)
        {
            HD_NOTIFY *pHDN = (HD_NOTIFY*)lParam;

            for (std::set<int>::iterator itr = m_hiddenCols.begin(); itr != m_hiddenCols.end(); ++itr )
            {
                if (pHDN->iItem == (*itr))
                {
                    // This is a hidden column, suppress the notification
                    *pResult = TRUE;
                    return TRUE;
                }
            }
        }
        return CListCtrl::OnNotify(wParam, lParam, pResult);
    }


} // TA_Base_App
