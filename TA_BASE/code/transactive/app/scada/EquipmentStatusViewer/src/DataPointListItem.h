/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataPointListItem.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A DataPoint item to be displayed in (or filtered from) the EquipmentStatusListCtrl.
  * This class contains the required datapoint proxy.
  *
  */

#if !defined(AFX_DATAPOINTLISTITEM_H__B075AB09_2E3D_4E03_8D88_7F74C1420955__INCLUDED_)
#define AFX_DATAPOINTLISTITEM_H__B075AB09_2E3D_4E03_8D88_7F74C1420955__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EquipmentStatusListItem.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"

namespace TA_Base_Core
{
    class DataNodeEntityData;
};

namespace TA_Base_App
{

    class DataPointListItem : public EquipmentStatusListItem
    {
    public:
        /** 
          * DataPointListItem
          *
          * Construct the ESLItem derivation for this datapoint entity using its corbaname.
          * This will allocate the appropriate datapoint proxy to manage config updates.
          *
          * @param corbaName        The datapoint entity to examine.
          * @param hwndToNotify     Observer window that will host the GUI updates when the config
          *                         updates are posted up to the UI thread.
          */
        DataPointListItem(unsigned long entitykey, HWND hwndToNotify);

        /** 
          * ~DataPointListItem
          *
          * Destructor.
          * This does not have to do much since the datapoint and datanode proxies are kept through 
          * smart (auto) pointers and will be freed when the DataPointListItem is deallocated.
          */
        virtual ~DataPointListItem();

        /** 
          * processEntityUpdateEvent
          *
          * Required by IEntityUpdateEventProcessor.
          * This specialised version attempts to load the datapoint's parent datanode.
          *
          * @param entityKey    Who the update is for; probably will be this->m_corbaName.getEntityKey()
          *                     or its parent entity id.
          * @param updateType   What type of update (see bus/scada/proxies2/ProxyDefs.h)
          *
          */
        virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

        /** 
          * getItemType
          *
          * The SCADA level of this item.
          *
          * @return     The type of the derived ESL Item: DATAPOINT
          */
        virtual Node_Item getItemType() const {return DATAPOINT;};

        /** 
          * CheckFilters
          *
          * Refer to EquipmentStatusListItem::CheckFilters().
          * This specialised version checks if the "DataPoints only" filter element matches,
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
          * This override provides values using the datapoint proxy.
          *
          * @param  column      Which column to get the text for.
          *
          * @return CString     The new text
          */
        virtual CString buildNewColumnText( TA_Base_App::EslColumns column );

        /** 
          * buildNewColumnIcon
          *
          * Get the new icon for the requested column.
          * At present the only icon is for datapoint quality display in the value column.
          *
          * @param  column      Which column to get the icon for.
          *
          * @return CString     The index number of the new icon (see CEquipmentStatusListCtrl::m_imageList)
          */
        virtual int     buildNewColumnIcon( TA_Base_App::EslColumns column );

    private:
		/** 
		 * Every list item needs a DataNode entitydata, either for itself (DataNode items) or
		 * its parent (DataPoint items). This entitydata is used to provide the Asset Name and
		 * Equipment description for this item regardless of its item-type.
		 */
        TA_Base_Core::DataNodeEntityData*                m_dataNode;

        /** 
          * loadParentDataNode
          *
          * Checks whether the parent datanode proxy is loaded and if not then
          * attempts to load it and catches the ScadaProxyException if necessary.
          * Call this function as often as required.
          */
	    void loadParentDataNode();

        /** 
          * buildStatusText
          *
          * Helper function to build the text for the status column depending
          * on the inhibit and override values.
          *
          * @return     The text representing the current datapoint status.
          */
        CString buildStatusText();

		/**
		 * The DataPoint Proxy that represents this item.
         * This is what is created in the constructor (not m_dataNode).
  		 */
        TA_Base_Bus::DataPointProxySmartPtr m_dataPoint;

    };

} // TA_Base_App

#endif // !defined(AFX_DATAPOINTLISTITEM_H__B075AB09_2E3D_4E03_8D88_7F74C1420955__INCLUDED_)
