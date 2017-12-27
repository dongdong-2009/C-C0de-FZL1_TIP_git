/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/CfgEquipmentListCtrl.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class is responsible for displaying the ListCtrl in the Configuration dialog.
 * It supports column sorting based on text in the cell
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "CfgEquipmentListCtrl.h"
#include "EquipmentStatusViewerDlg.h"

// NOTE : EquipReport_Data related
#include "core/data_access_interface/src/EquipReportData.h"
#include "core/data_access_interface/src/EquipReportDataAccessFactory.h"
#include "core/data_access_interface/src/ConfigEquipReportData.h"

// NOTE : Entity related

// NOTE : Scada Common Factory
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
// #include "bus\scada\common_library\src\DataProxyInitCallback.h"

// NOTE : Exception related
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "ProgressBar.h"

#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Core;

namespace TA_Base_App
{

	CCfgEquipmentListCtrl::CCfgEquipmentListCtrl( TA_Base_App::EquipmentStatusListItemMap const& reportItems)
        : m_sortParams( reportItems)
	{
		for (int i=0; i<8; i++)
			m_bAscending[i] = false;
	}


	CCfgEquipmentListCtrl::~CCfgEquipmentListCtrl()
	{
	}


	BEGIN_MESSAGE_MAP(CCfgEquipmentListCtrl, CListCtrl)
		//{{AFX_MSG_MAP(CCfgEquipmentListCtrl)
			// NOTE - the ClassWizard will add and remove mapping macros here.
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	void CCfgEquipmentListCtrl::PreSubclassWindow()
	{
        // TD12080: Enable item tooltips.
		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES | LVS_EX_INFOTIP);

        InsertColumn(ASSET_COLUMN,			"Asset");
   		InsertColumn(DESCRIPTION_COLUMN,	"Equipment Description");
		InsertColumn(DATAPOINT_COLUMN,		"Point Description");

		SetColumnWidth(ASSET_COLUMN,		182);
		SetColumnWidth(DESCRIPTION_COLUMN,	240);
        SetColumnWidth(DATAPOINT_COLUMN,    100);

        TA_Base_Bus::FixedHeaderCtrl::expandColumnToFitListWidth(*this, DESCRIPTION_COLUMN, true);

		CListCtrl::PreSubclassWindow();
	}


	void CCfgEquipmentListCtrl::SetSortOrder(int aCol, bool aAscending)
	{
		m_bAscending[aCol] = aAscending;
	}


	bool CCfgEquipmentListCtrl::GetSortOrder(int aCol)
	{
		return m_bAscending[aCol];
	}


    int CALLBACK CCfgEquipmentListCtrl::CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
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


	void CCfgEquipmentListCtrl::SortColumn(int aCol)
	{
        m_sortParams.ascending = m_bAscending[aCol];
        m_sortParams.sortColumn = aCol;
        SortItems( CompareFunc, (LPARAM) &m_sortParams);

		// show the ascending/descending icon
		
		/////////////////////////////////////////////////////////////////////
		//Launch Manager modified by: zhangjunsheng 2008/10/16 13:00:00
		
		//for (int thisCol = ASSET_COLUMN; thisCol <= DESCRIPTION_COLUMN; thisCol++)
		
		for (int thisCol = -1; thisCol <= DESCRIPTION_COLUMN; thisCol++)
			
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

				if (m_bAscending[aCol])
					item.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SORTUP), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
				else
					item.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SORTDOWN), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
			} else
			{
				item.mask = HDI_FORMAT;
				item.fmt &= ~HDF_BITMAP & ~HDF_BITMAP_ON_RIGHT;
			}

			GetHeaderCtrl()->SetItem(thisCol, &item);
		}
	}

} // TA_Base_App
