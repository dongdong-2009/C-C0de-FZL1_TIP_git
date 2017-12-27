/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent_entity/src/EntityListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all entities available to be configured. This view must communicate
  * with the EntityView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/agent_entity/src/StdAfx.h"
#include "app/configuration/config_plugins/agent_entity/src/EntityListView.h"
#include "app/configuration/config_plugins/agent_entity/src/EntityDatabaseAccessor.h"
#include "app/configuration/config_plugins/agent_entity/src/AgentEntityComponent.h"
#include "app/configuration/config_plugins/agent_entity/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_App
{
    const CString EntityListView::SELECT_ENTITYTYPE_DIALOG_TITLE = "Select a Type";


    IMPLEMENT_DYNCREATE(EntityListView, CListView)


    EntityListView::EntityListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(AgentEntityComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ENTITY) );
    }


    EntityListView::~EntityListView()
    {
    }


BEGIN_MESSAGE_MAP(EntityListView, AbstractListView)
	//{{AFX_MSG_MAP(EntityListView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, onItemchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void EntityListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void EntityListView::newItem()
    {
        FUNCTION_ENTRY("newItem");

        EntityDatabaseAccessor* dbAccessor = dynamic_cast<EntityDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT(dbAccessor != NULL, "Database accessor is NULL. Maybe it was the wrong type and could not be cast");

        std::vector<std::string>& types = dbAccessor->getEntityTypes();
        std::string typeToUse;
        if (types.size() == 1)
        {
            // If we are only representing one type then we know this is the type we should create
            typeToUse = types[0];
        }
        else
        {
            // Otherwise we have to let the user select the entity type

            // Generate the map to pass into the dialog. We are not interested in the id as we only use the
            // name so anything can be used there
            ChangeDialog::ItemsForSelection itemsForSelection;
            for( unsigned int i = 0; i < types.size(); ++i)
            {
                itemsForSelection.insert( ChangeDialog::ItemsForSelection::value_type(i,types[i]) );
            }

            // Pass in blank for the current item since the user hasn't previously selected an entity type
            ChangeDialog dlg(itemsForSelection,SELECT_ENTITYTYPE_DIALOG_TITLE,"", this);

            if (dlg.DoModal() != IDOK)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the creation of a new entity");
                FUNCTION_EXIT;
                return;
            }

            typeToUse = dlg.getSelectedName();
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected the entity type which is %s", typeToUse.c_str());
        }

        try
        {
            TA_Base_Core::IConfigEntity* newEntity = dynamic_cast<TA_Base_Core::IConfigEntity*>(dbAccessor->newItem(typeToUse));

            if (newEntity !=  NULL)
            {
                // Append something to the end of the name to indicate that this is a new entity
                std::string newName = newEntity->getName();
                newName += AbstractDetailView::NEW_LABEL;
                newEntity->setName( newName );

                // Insert this item into the list and set it to be the selected item
                int position = GetListCtrl().InsertItem(-1,newName.c_str());
                GetListCtrl().SetItemData(position, newEntity->getUniqueIdentifier());

                // Unselect the current selections
                POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
                while (pos != NULL)
                {
                    int selected = GetListCtrl().GetNextSelectedItem(pos);
                    GetListCtrl().SetItemState(selected,0,LVIS_SELECTED);
                }

                // By setting this as the selected item the entity detail view will automatically
                // be updated.
                GetListCtrl().SetItemState(position,0xFFFFFF,LVIS_SELECTED);
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not create a new entity");
            CString actionName, reason;
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATA_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not create a new entity");
            CString actionName, reason;
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }


        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // EntityListView diagnostics

    #ifdef _DEBUG
    void EntityListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void EntityListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

