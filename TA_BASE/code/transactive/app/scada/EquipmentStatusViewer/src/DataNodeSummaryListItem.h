/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataNodeSummaryListItem.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A specialised DataNode Summary item to be displayed in (or filtered from) the EquipmentStatusListCtrl.
  * This class inherits from the DataNodeListItem and extends that class by extracting summary info
  * for the datanode, and then discards the required datanode proxy once it has loaded so that it 
  * unsubscribes from update messages.
  *
  */

#if !defined(AFX_DATANODESUMMARYLISTITEM_H__71ACA546_4D77_4EF2_A9CE_FBC78AB9A31E__INCLUDED_)
#define AFX_DATANODESUMMARYLISTITEM_H__71ACA546_4D77_4EF2_A9CE_FBC78AB9A31E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataNodeListItem.h"

namespace TA_Base_Core
{
    class DataNodeEntityData;
}

namespace TA_Base_App
{

    class DataNodeSummaryListItem : public DataNodeListItem
    {
    public:
        /** 
          * DataNodeSummaryListItem
          *
          * Construct the ESLItem derivation for this datanode entity using its corbaname.
          *
          * @param corbaName        The datanode entity to examine.
          * @param hwndToNotify     Observer window that will host the GUI updates when the config
          *                         updates are posted up to the UI thread.
          */
        DataNodeSummaryListItem(unsigned long entitykey, HWND hwndToNotify);

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
        DataNodeSummaryListItem(boost::shared_ptr<TA_Base_Core::DataNodeEntityData> entityData , HWND hwndToNotify);

        /** 
          * ~DataNodeListItem
          *
          * Destructor.
          * This does not have to do much since the datanode proxy is kept in a smart (auto) 
          * pointer and will be freed when the DataNodeSummaryListItem is deallocated or when
          * the config updates indicate that the proxy is complete.
          */
        virtual ~DataNodeSummaryListItem();

        /** 
          * processEntityUpdateEvent
          *
          * Required by IEntityUpdateEventProcessor.
          * This specialised version checks if the proxy's config is complete and deletes it
          * once the base class handler has been called (the values are now cached in the
          * local m_text[] array) so that no more updates are received.
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
          * @return     The type of the derived ESL Item: DATANODE_SUMMARY
          */
        virtual Node_Item getItemType() const {return DATANODE_SUMMARY;};

    protected:
        /** 
          * buildNewColumnText
          *
          * Get the new text for the requested column.
          * This override provides the column value for status which is built from the
          * datanode proxy's get~Summary() methods.
          *
          * @param  column      Which column to get the text for.
          *
          * @return CString     The new text
          */
        virtual CString buildNewColumnText( TA_Base_App::EslColumns column );

    };

} // TA_Base_App

#endif // !defined(AFX_DATANODESUMMARYLISTITEM_H__71ACA546_4D77_4EF2_A9CE_FBC78AB9A31E__INCLUDED_)
