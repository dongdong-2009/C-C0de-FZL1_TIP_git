/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusListItem.cpp $
 * @author:  Alan Brims
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * An item to be displayed in (or filtered from) the EquipmentStatusListCtrl.
 * This class is extended to implement a local cache of datanode and datapoint proxies.
 *
 */

#include "stdafx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "equipmentstatusviewer.h"
#include "EquipmentStatusListItem.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    EquipmentStatusListItem::EquipmentStatusListItem(unsigned long entitykey, HWND hwndToNotify)
        : m_entityKey( entitykey)
        , m_hwndToNotify( hwndToNotify)
        , m_isFiltered( false)
        , m_columnUpdateFlags( 0)
        , m_updateType( ADD_MODIFY_ITEM)
        , m_reportItemKey("")
    {
        for (int nCol = ESL_COLUMN_MIN; nCol < ESL_COLUMN_MAX; nCol++)
        {
            m_icon[ nCol] = -1;
        }
    }

    EquipmentStatusListItem::~EquipmentStatusListItem()
    {
    }

    unsigned long EquipmentStatusListItem::getEntityKey() const
    {
        return m_entityKey;
    }


    std::string EquipmentStatusListItem::getReportItemKey()     {
        return m_reportItemKey;
    }


    void EquipmentStatusListItem::setReportItemKey(std::string& key)
    {
        m_reportItemKey = key;
    }


    bool EquipmentStatusListItem::isFiltered() const
    {
        return m_isFiltered;
    }


    bool EquipmentStatusListItem::isToBeDeleted() const
    {
        return (DELETE_ITEM == m_updateType);
    }


    void EquipmentStatusListItem::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        for ( int col=ESL_COLUMN_MIN; col < ESL_COLUMN_MAX; col++)
        {
            TA_Base_App::EslColumns column = static_cast<TA_Base_App::EslColumns>(col);
            UpdateTextIfChanged( column, buildNewColumnText( column));
            UpdateIconIfChanged( column, buildNewColumnIcon( column));
        }

        if ( (0 < m_columnUpdateFlags) || (ADD_MODIFY_ITEM != m_updateType) )
        {
            unsigned long thisEntityKey = getEntityKey();
            ::PostMessage( m_hwndToNotify, WM_LIST_ITEM_UPDATE, static_cast<WPARAM>(thisEntityKey), 0 );
        }

        //  The update has been triggered so reset the trigger flags
        m_columnUpdateFlags = 0;
    }


    int EquipmentStatusListItem::compare( const EquipmentStatusListItem &rhs, TA_Base_App::EslColumns column) const
    {
        int compareResult = 0;

        //  Timestamp is in dd/mm/yyyy format: not sortable by text comparison
        if ( TIMESTAMP_COLUMN == column)
        {
            if (m_timeStamp < rhs.m_timeStamp)
            {
                compareResult = -1;
            }
            else if (m_timeStamp > rhs.m_timeStamp)
            {
                compareResult = 1;
            }
        }
        else
        {
            //  All other columns are sorted by their text values
            if ( getColumnText( column) < rhs.getColumnText( column) )
            {
                compareResult = -1;
            }
            else if ( getColumnText( column) > rhs.getColumnText( column) )
            {
                compareResult = 1;
            }
        }
        return compareResult;
    }


    CString const& EquipmentStatusListItem::getColumnText( TA_Base_App::EslColumns column) const
    {
        TA_ASSERT( (( column >= ESL_COLUMN_MIN) && (column < ESL_COLUMN_MAX)), "Column out of bounds" );
        return m_text[column];
    }


    int const& EquipmentStatusListItem::getColumnIcon( TA_Base_App::EslColumns column) const
    {
        TA_ASSERT( (( column >= ESL_COLUMN_MIN) && (column < ESL_COLUMN_MAX)), "Column out of bounds" );
        return m_icon[column];
    }


    int EquipmentStatusListItem::FindInList( CListCtrl const& listCtrl, unsigned long entityKey)
    {
        //  Find this report line in the list using the Entity Key
        //  This operation is in this class because the search is looking for
        //  the item's lParam value inserted in UpdateInList().

        LVFINDINFOA findInfo;
        findInfo.flags = LVFI_PARAM;
        findInfo.lParam = entityKey;

        return listCtrl.FindItem( &findInfo );
    }


    int EquipmentStatusListItem::UpdateInList(CListCtrl &listCtrl, int nItem, int nSubItem)
    {
        //  Allocate a local working-copy of the item number parameter
        int nIndex = nItem;

        if (0 > nIndex)
        {
            nIndex = FindInList( listCtrl, getEntityKey());
        }
        else
        {
            TA_ASSERT( (unsigned long) listCtrl.GetItemData( nItem) == getEntityKey(),
                "Update requested for wrong row in list control");
            return -1;
        }

        if ( isFiltered() || isToBeDeleted() )
        {
            //  This report line is not to be displayed: does it need 
            //  to be removed from the list control?
            if (nIndex >= 0)
            {
                listCtrl.DeleteItem( nIndex);
            }
            return nIndex;
        }

        //  This report line is to be displayed. Is the report line
        //  already in the list control? Update it or add it.

        LVITEMA lvItem;
        ZeroMemory( &lvItem, sizeof( lvItem));

        if (nIndex < 0)
        {
            //  Needs to be added first. Just add a stub item and then
            //  update all the columns' text entries.
            lvItem.pszText	= "loading...";

		    lvItem.mask		= LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_INDENT;
		    lvItem.iImage   = -1;
            lvItem.iItem	= listCtrl.GetItemCount();
		    lvItem.iSubItem	= 0;
		    lvItem.lParam	= (LPARAM) getEntityKey();
            lvItem.iIndent  = (ESL_COLUMN_MIN==nSubItem)?-2:0;

            nIndex = listCtrl.InsertItem( &lvItem);
        }

        //  If we don't have a valid item number by now then there is nothing left to do
        if (nIndex < 0)
        {
            return nIndex;
        }

        //  If parameter nSubItem is valid (zero or more) then just update that column
        int nStartColumn = ESL_COLUMN_MIN;
        int nEndColumn = ESL_COLUMN_MAX-1;
        if (0 <= nSubItem)
        {
            nStartColumn = nEndColumn = nSubItem;
        }

        for (int nColumn=nStartColumn; nColumn <= nEndColumn; nColumn++)
        {
            CString sItem   = getColumnText( (EslColumns) nColumn);

            lvItem.pszText	= sItem.GetBuffer( sItem.GetLength());

		    lvItem.mask		= LVIF_TEXT | LVIF_IMAGE;
		    lvItem.iImage   = getColumnIcon( (EslColumns) nColumn);
            lvItem.iItem	= nIndex;
		    lvItem.iSubItem	= nColumn;

            listCtrl.SetItem( &lvItem);

        }

        return nIndex;
    }


    bool EquipmentStatusListItem::UpdateIconIfChanged(EslColumns column, int newIcon)
    {
        TA_ASSERT( (( column >= ESL_COLUMN_MIN) && (column < ESL_COLUMN_MAX)), "Column out of bounds" );
        if ( newIcon != m_icon[column])
        {
            m_icon[column] = newIcon;
            m_columnUpdateFlags|= (1 << column);
            return true;
        }
        return false;
    }


    bool EquipmentStatusListItem::UpdateTextIfChanged(EslColumns column, CString const& newText)
    {
        TA_ASSERT( (( column >= ESL_COLUMN_MIN) && (column < ESL_COLUMN_MAX)), "Column out of bounds" );
        if ( newText != m_text[column])
        {
            m_text[column] = newText;
            m_columnUpdateFlags|= (1 << column);
            return true;
        }
        return false;
    }


    CString EquipmentStatusListItem::buildNewColumnText( TA_Base_App::EslColumns column )
    {
        //  Get the new text for the requested column. By calling this function 
        //  for each column we only need one try-catch(ScadaProxyException) block.

        CString newText= "";
        return newText;
    }


    int EquipmentStatusListItem::buildNewColumnIcon( TA_Base_App::EslColumns column )
    {
        //  Icons will be set by the derived classes. If this base method is called
        //  then there is no icon for this column.
        return -1;
    }


    void EquipmentStatusListItem::CheckFilters( FilterFlagsAndStrings const& filters)
    {
        //  Iterate through columns evaluating their text against the equivalent filter if that
        //  filter flag is "true". Exit as soon as the item fails a non-STATUS filter test 
        //  (ie. it is to be hidden), otherwise check if any active STATUS filters match.

        bool statusMatch = false;

        for ( int nf = ESL_FILTER_OFFSET_MIN; nf < ESL_FILTER_OFFSET_MAX; nf++)
        {
            if ( ( false == filters[nf].filterInUse ) //  this filter element is not ticked in the FilterDialog 
                ||
                ( USE_DERIVED_CLASS == EslFilterMap[nf] )) //  this filter element should have been handled in a derived class' implementation
            {
                continue;
            }

            CString columnText( getColumnText( static_cast<EslColumns>(EslFilterMap[nf]))) ;
            CString filterText( filters[nf].filterString.c_str() );

            // Find is case sensitive, so we need to make both upper or lower before we 
            // start searching
            columnText.MakeLower();
            filterText.MakeLower();

            //  The STATUS filters are ORed together but everything else is ANDed.

            if ( ( STATUS_FILTER_OFFSET < nf) && (STATUS_OVERRIDDEN_FILTER_OFFSET >= nf) )
            {
                if (columnText.Find(filterText) >= 0) // if found
                {
                    //  One of the STATUS filters matches, so display it unless
                    //  it fails a non-STATUS filter test (see else-block below).
                    statusMatch = true;
                }
            }
            else
            {   //  not a STATUS column
                if (columnText.Find(filterText) < 0) // if not found
                {
                    //  No match on this non-STATUS column so it won't be displayed
                    m_isFiltered = true;
                    return;
                }
            }
        }

        //  To get here the item must match all enabled filter text, or just matches
        //  one or more Status filters (Scan inhibited or Overridden or ..).

        m_isFiltered = false;

        if (filters[STATUS_FILTER_OFFSET].filterInUse == true)  // are STATUS filters in use?
        {
            m_isFiltered = !statusMatch;
        }
    }
    
    unsigned long EquipmentStatusListItem::getItemDataNodeEntityKey()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Base EquipmentStatusListItem::getItemDataNodeEntityKey() is called!"); 
        return 0;
    }

    std::string EquipmentStatusListItem::getItemDataNodeEntityName()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Base EquipmentStatusListItem::getItemDataNodeEntityName() is called!"); 
        return 0;
    }


} // TA_Base_App
