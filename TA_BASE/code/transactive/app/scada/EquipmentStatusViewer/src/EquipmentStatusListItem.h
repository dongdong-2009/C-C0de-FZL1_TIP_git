#if !defined(AFX_EQUIPMENTSTATUSLISTITEM_H__15E3D0CC_4D86_426A_B4EA_C0D5FFE9F063__INCLUDED_)
#define AFX_EQUIPMENTSTATUSLISTITEM_H__15E3D0CC_4D86_426A_B4EA_C0D5FFE9F063__INCLUDED_

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusListItem.h $
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "app/scada/EquipmentStatusViewer/src/CustomDefines.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/common_library/src/CommonDefs.h"


namespace TA_Base_Core
{
    // forward declaration
	class IEntityData;
}



namespace TA_Base_App
{    
    /**
     *  Forward declaration of friend class (see comment in protected section)
     */
    class CEquipmentStatusViewerDlg;

    class EquipmentStatusListItem : public TA_Base_Bus::IEntityUpdateEventProcessor  
    {
    public:

        /** 
          * EquipmentStatusListItem
          *
          * Construct the ESLItem for this entity using its corbaname.
          * This will allocate the appropriate proxy to manage config updates.
          *
          * @param corbaName        The entity to examine.
          * @param hwndToNotify     Observer window that will host the GUI updates when the config
          *                         updates are posted up to the UI thread.
          */
        EquipmentStatusListItem(unsigned long entitykey, HWND hwndToNotify);

        /** 
          * ~EquipmentStatusListItem
          *
          * Destructor.
          * This does not have to do much since the proxies are kept through smart (auto) pointers
          * and will be freed when the ESLItem is deallocated.
          */
        virtual ~EquipmentStatusListItem();

        /** 
          * getItemType
          *
          * The SCADA level of the item.
          * This pure function makes EquipmentStatusListItem abstract.
          *
          * @return     The type of the derived ESL Item.
          *             Currently only DATANODE, DATAPOINT and DATAPOINT_SUMMARY are available.
          */
        virtual Node_Item getItemType() const = 0;

        /** 
          * UpdateInList
          *
          * Called in the UI thread to update the on-screen display. This is usually
          * in response to this object handling the processEntityUpdate callback in
          * a pooled thread and posting it to the UI as a Windows user message.
          *
          * @param listCtrl     List control this item is to update
          * @param nItem        Index of the list control item to update;
          *                     default (-1) means search for the item using its entity key.
          * @param nSubItem     Column to update in the list control;
          *                     default (-1) means update all columns.
          *
          * @return             The index of the list control item that has been updated
          *
          */
	    int UpdateInList( CListCtrl & listCtrl, int nItem=-1, int nSubItem=-1);

        /** 
          * compare
          *
          * Standard comparison function to be used by the sort routines.
          *
          * @param &rhs     The other ESLItem to compare against.
          * @param column   Compare the two ESLItems using values in this column.
          *
          * @return         -1 if this < rhs
          *                  0 if equal
          *                 +1 if this > rhs
          *
          */
        virtual int compare( const EquipmentStatusListItem &rhs, TA_Base_App::EslColumns column) const;

        /** 
          * processEntityUpdateEvent
          *
          * Required by IEntityUpdateEventProcessor.
          *
          * @param entityKey    Who the update is for; probably will be this->m_corbaName.getEntityKey()
          *                     or its parent entity id.
          * @param updateType   What type of update (see bus/scada/proxies2/ProxyDefs.h)
          *
          */
        virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

        /** 
          * getColumnText
          *
          * Read-only accessor for m_text[]
          *
          * @param column   Which column to return the text for.
          *
          * @return         The text in the requested column for this ESLItem
          */
        CString const& getColumnText( TA_Base_App::EslColumns column) const;

        /** 
          * getColumnIcon
          *
          * Read-only accessor for m_icon[]
          *
          * @param column   Which column to return the icon for.
          *
          * @return         The imagelist index of the icon in the requested column for this ESLItem.
          */
        int const& getColumnIcon( TA_Base_App::EslColumns column) const;

        /** 
          * getEntityData
          *
          * Read-only accessor for this entity's EntityData
          *
          * @return     This ESLItem's corba name
          */
        unsigned long getEntityKey() const;

        /** 
          * setReportItemKey
          *
          * Write-accessor for this report item's key
          * Currently this key is used only by Configuration Dialog
          *
          * @param key    The key of the report item (not the entity it reports on).
          */
        void setReportItemKey( std::string& key);

        /** 
          * getReportItemKey
          *
          * Read-only accessor for this report item's key
          * (note: this is not the report key but the key of the item within the report)
          *
          * @return     Key of this item in the equipment report entities table
          */
        std::string getReportItemKey();

        /** 
          * isFiltered
          *
          * Is this item to be hidden because it doesn't match the filter?
          *
          * @return bool    True:  the item is to be hidden 
          *                 False: the item is to be displayed
          */
        bool isFiltered() const;

        /** 
          * isToBeDeleted
          *
          * Is this item to be removed entirely (from list control and proxy list) because it
          * is not for the current report? (eg. it is the wrong scada level, neither DATANODE nor DATAPOINT)
          *
          * @return bool    True:  the item is to be removed
          *                 False: the item is to be displayed (or hidden depending on filters)
          */
        bool isToBeDeleted() const;

        /** 
          * CheckFilters
          *
          * Evaluate this ESL Item against the parameter filter set and set the "is Filtered" 
          * flag accordingly. If the item "is Filtered" then it will not be displayed.
          *
          * @param filters  vector of FilterElements    On-off flags and the strings they enable.
          *
          */
        virtual void CheckFilters( FilterFlagsAndStrings const& filters);

        /** 
          * FindInList
          *
          * Find this report line in the list using the Entity Key
          *
          * @param listCtrl     The list control to search.
          * @param entityKey    The entity key to search for.
          *
          * @return int         The index of the found item, or -1 if not found (see CListCtrl::FindItem)
          *
          */
        static int FindInList( CListCtrl const& listCtrl, unsigned long entityKey);

		TA_Base_Core::ReEntrantThreadLockable   m_lock;

        /** 
          * getItemDataNodeEntityKey
          *
          * Read-only accessor for this entity's EntityData
          *
          * @return     This ESLItem's corba name
          */
        virtual unsigned long getItemDataNodeEntityKey();

        /** 
          * getItemDataNodeEntityName
          *
          * Read-only accessor for this entity's EntityData
          *
          * @return     This ESLItem's corba name
          */
        virtual std::string getItemDataNodeEntityName();


    protected:
        /** 
          * CEquipmentStatusViewerDlg is a (temporary) friend to access buildNewColumnText()
          * in its OnRdblClickEquipmentList(); for developer use
          */
        friend CEquipmentStatusViewerDlg;

        /** 
          * buildNewColumnText
          *
          * Get the new text for the requested column. Derived classes should override
          * this member to provide entity-type specialisations for the relevant columns.
          * For example, the datapoint description column will be filled by the DataPoint
          * derivation but left blank by the DataNode derivation.
          *
          * This is where the proxy is queried and ScadaProxyExceptions are caught, so
          * this function will always return a value even if it is blank.
          * The value returned from this function is compared against the stored value
          * to determine if the column needs updating and therefore whether to post
          * an update to the user inferface thread.
          *
          * @param  column      Which column to get the text for.
          *
          * @return CString     The new text
          */
        virtual CString buildNewColumnText( TA_Base_App::EslColumns column );

        /** 
          * buildNewColumnIcon
          *
          * Get the new icon for the requested column. Derived classes should override
          * this member to provide entity-type specialisations for the relevant columns.
          * At present the only icon is for datapoint quality display in the value column. 
          *
          * @param  column      Which column to get the icon for.
          *
          * @return CString     The index number of the new icon (see CEquipmentStatusListCtrl::m_imageList)
          */
        virtual int     buildNewColumnIcon( TA_Base_App::EslColumns column );

        /** 
          * UpdateTextIfChanged
          *
          * Compares the string parameter to the current value for this column and updates
          * the column if different. Also sets the relevant bit in m_columnUpdateFlags to
          * indicate that column has changed. If m_columnUpdateFlags is not blank after 
          * calling UpdateTextIfChanged() on all columns then post an update to the user
          * interface to refresh the list control display.
          *
          * @param column   The column to check the new string against.
          * @param newText  The new value for this column.
          *
          * @return         True if the column text was updated.
          *
          */
        bool UpdateTextIfChanged( EslColumns column, CString const& newText);

        /** 
          * UpdateIconIfChanged
          *
          * Compares the icon-index parameter to the current value for this column and updates
          * the column if different. Also sets the relevant bit in m_columnUpdateFlags to
          * indicate that column has changed. If m_columnUpdateFlags is not blank after 
          * calling UpdateIconIfChanged() on all columns then post an update to the user
          * interface to refresh the list control display.
          *
          * @param column   The column to check the new string against.
          * @param newIcon  The new icon for this column.
          *
          * @return         True if the icon was updated.
          *
          */
        bool UpdateIconIfChanged( EslColumns column, int newIcon);

		/** 
		 * This item's entitykey
		 */
		unsigned long                           m_entityKey;

		/** 
		 * Is this item currently filtered (hidden) from the display?
		 */
        bool            m_isFiltered;

		/** 
		 * Bitflag to indicate which columns have had their text or icon changed.
		 */
        DWORD           m_columnUpdateFlags;

		/** 
		 * Flag to indicate how to handle this item. Mainly used to delete the item
		 * from its container when update type is DELETE_ITEM.
		 */
        ItemUpdateType  m_updateType;

		/** 
		 * The observer window (eg. ESVDlg or Config Dlg) that will handle
		 * the WM_LIST_ITEM_UPDATE message posted by this item.
		 */
        HWND            m_hwndToNotify;         //  associated window (eg. ESList or Config dlg)

        /** 
		 * DataNode or DataPoint (these maintain live proxies), 
		 * or DataNode Summary (this one kills its proxy when loading is complete)
		 */
        Node_Item       m_itemType;             //  DATANODE, DATAPOINT or DATANODE_SUMMARY

        /** 
		 * The time value in the TIMESTAMP column for sort-comparison 
		 * (the displayed timestamp in m_text[TIMESTAMP] is in dd/mm/yy format)
		 */
        unsigned long   m_timeStamp;

    private:
        //
        // Copy constructor and assignment operator are not used
        //
    //    EquipmentStatusListItem( const EquipmentStatusListItem& theSource) {};
		EquipmentStatusListItem& operator=(const EquipmentStatusListItem &) {};

        CString m_text[ESL_COLUMN_MAX];
        int     m_icon[ESL_COLUMN_MAX];

        /** 
		 * Key of this item's row in the SC_EQUIP_REPORT_ENTITIES table
		 * Currently used only in ConfigurationDialog.
		 */
        std::string m_reportItemKey;
    };

} // TA_Base_App


#endif // !defined(AFX_EQUIPMENTSTATUSLISTITEM_H__15E3D0CC_4D86_426A_B4EA_C0D5FFE9F063__INCLUDED_)
