/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataNodeListItem.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A DataNode (Equipment) item to be displayed in (or filtered from) the EquipmentStatusListCtrl.
  * This class contains the required datanode proxy.
  *
  */

#if !defined(AFX_DATANODELISTITEM_H__0892240E_1093_4B41_9457_52258362F56E__INCLUDED_)
#define AFX_DATANODELISTITEM_H__0892240E_1093_4B41_9457_52258362F56E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EquipmentStatusListItem.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    class DataNodeEntityData;
}

namespace TA_Base_App
{

    class DataNodeListItem : public EquipmentStatusListItem
    {
    public:
        /** 
          * DataNodeListItem
          *
          * Construct the ESLItem derivation for this datanode entity using its corbaname.
          * This will allocate the appropriate datanode proxy to manage config updates.
          *
          * @param corbaName        The datanode entity to examine.
          * @param hwndToNotify     Observer window that will host the GUI updates when the config
          *                         updates are posted up to the UI thread.
          */
        DataNodeListItem(unsigned long entitykey, HWND hwndToNotify);

        /** 
          * DataNodeListItem
          *
          * Construct the ESLItem derivation for this datanode entity using its corbaname.
          * This will allocate the appropriate datanode proxy to manage config updates.
          *
          * @param entityData        boost::shared_ptr<TA_Base_Core::DataNodeEntityData> 
          * @param hwndToNotify     Observer window that will host the GUI updates when the config
          *                         updates are posted up to the UI thread.
          */
        DataNodeListItem(boost::shared_ptr<TA_Base_Core::DataNodeEntityData> entityData , HWND hwndToNotify);

        /** 
          * ~DataNodeListItem
          *
          * Destructor.
          * This does not have to do much since the datanode proxy is kept in a smart
          * (auto) pointer and will be freed when the DataNodeListItem is deallocated.
          */
        virtual ~DataNodeListItem();

        /** 
          * processEntityUpdateEvent
          *
          * Required by IEntityUpdateEventProcessor.
          * This specialised version tests whether this datanode has child datanodes and
          * therefore shouldn't be in the list because it is a Location, System or SubSystem.
          * Datanodes with child datanodes have their m_updateType set to DELETE_ITEM so
          * they can be removed in the user interface thread.
          *
          * @param entityKey    Who the update is for (it should be = this->m_corbaName.getEntityKey() )
          * @param updateType   What type of update (see bus/scada/proxies2/ProxyDefs.h)
          *
          */
        virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

        /** 
          * getItemType
          *
          * The SCADA level of this item.
          *
          * @return     The type of the derived ESL Item: DATANODE
          */
        virtual Node_Item getItemType() const {return DATANODE;};

        /** 
          * CheckFilters
          *
          * Refer to EquipmentStatusListItem::CheckFilters().
          * This specialised version checks if the "DataNodes (equipment) only" filter element matches,
          * and otherwise calls the base class implementation.
          *
          * @param filterFlags    vector of bool      If true then that filter element is to be evaluated.
          * @param filterStrings  vector of strings   The values to search for in this item's column text values.
          *
          */
        virtual void CheckFilters( FilterFlagsAndStrings const& filters);
        
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
          * buildNewColumnText
          *
          * Get the new text for the requested column.
          * This override provides column values for status, operator and timestamp
          * if the equipment has been tagged.
          *
          * @param  column      Which column to get the text for.
          *
          * @return CString     The new text
          */
        virtual CString buildNewColumnText( TA_Base_App::EslColumns column );
        
    private:
		/** 
		 * Every list item needs a DataNode Proxy, either for itself (DataNode items) or
		 * its parent (DataPoint items). This proxy is used to provide the Asset Name and
		 * Equipment description for this item regardless of its item-type.
		 */
        TA_Base_Bus::DataNodeProxySmartPtr      m_dataNode;

        /** 
          * buildSummaryText
          *
          * Build the text that will be displayed in the status column, using the 
          * datanode proxy's get~Summary() methods.
          *
          * @return     The status-summary text.
          */
        CString buildSummaryText();
    };

} // TA_Base_App

#endif // !defined(AFX_DATANODELISTITEM_H__0892240E_1093_4B41_9457_52258362F56E__INCLUDED_)
