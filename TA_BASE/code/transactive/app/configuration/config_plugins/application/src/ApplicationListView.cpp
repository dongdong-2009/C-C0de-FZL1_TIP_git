/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/application/src/ApplicationListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all applications available to be configured. This view must communicate
  * with the ApplicationDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/application/src/StdAfx.h"
#include "app/configuration/config_plugins/application/src/ApplicationListView.h"
#include "app/configuration/config_plugins/application/src/ApplicationComponent.h"
#include "app/configuration/config_plugins/application/src/ApplicationDatabaseAccessor.h"
#include "app/configuration/config_plugins/application/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigGui.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    const CString ApplicationListView::SELECT_GUITYPE_DIALOG_TITLE = "Select a Type";

    const std::string ApplicationListView::NO_TYPE = "<None>";


    IMPLEMENT_DYNCREATE(ApplicationListView, CListView)


    ApplicationListView::ApplicationListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(ApplicationComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_APPLICATION) );
    }


    ApplicationListView::~ApplicationListView()
    {
    }


BEGIN_MESSAGE_MAP(ApplicationListView, AbstractListView)
	//{{AFX_MSG_MAP(ApplicationListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ApplicationListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void ApplicationListView::deleteItem()
    {
        FUNCTION_ENTRY("deleteItem");

        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            CString actionName;
            actionName.LoadString(IDS_DELETE);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        int numberOfItemsNotDeleted = 0;

        CString actionName, errorMsg, item;
        actionName.LoadString(IDS_DELETE);
        errorMsg.LoadString(IDS_RESERVE_ITEM);
        item.LoadString(IDS_ITEMS_PREVENTING_ACTION);
        errorMsg += _T("\n") + item;
        while(pos != NULL)
        {
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            TA_Base_Core::IConfigItem* item = getDatabaseAccessor()->getItem(itemId);
            TA_Base_Core::IConfigGui* gui = dynamic_cast<TA_Base_Core::IConfigGui*>(item);

            TA_ASSERT(gui != NULL, "The configuration item we have is not an IConfigGui*");
            if (!gui->isNew() && !gui->canDelete())
            {
                if (numberOfItemsNotDeleted < MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                {
                    errorMsg += gui->getName().c_str();
                    errorMsg += _T("\n");
                    ++numberOfItemsNotDeleted;
                }
                else
                {
                    CString more;
                    more.LoadString(IDS_MORE);
                    errorMsg += more;
                    //  Increment this again so we can tell outside this for-loop whether the
                    // ... has been added yet. This avoids the problem of having exactly
                    // MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE after this loop and breaking even though
                    // there are more items
                    ++numberOfItemsNotDeleted;
                    break;
                }
            }
        }

        // If we have any entries that are invalid we must prevent the delete
        if (numberOfItemsNotDeleted != 0)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << errorMsg;
            userMsg.showMsgBox(IDS_UE_030060);

            FUNCTION_EXIT;
            return;
        }

        // We can delete this item so proceed in the normal fashion
        AbstractListView::deleteItem();
                
        FUNCTION_EXIT;
    }


    void ApplicationListView::newItem()
    {
        FUNCTION_ENTRY("newItem");

        ApplicationDatabaseAccessor* dbAccessor = dynamic_cast<ApplicationDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT(dbAccessor != NULL, "Database accessor is NULL. Maybe it was the wrong type and could not be cast");

        std::vector<std::string> types = dbAccessor->getEntityTypes();
        types.push_back(NO_TYPE);

        std::string typeToUse;

        // We have to let the user select the entity type

        // Generate the map to pass into the dialog. We are not interested in the id as we only use the
        // name so anything can be used there
        ChangeDialog::ItemsForSelection itemsForSelection;
        for( unsigned int i = 0; i < types.size(); ++i)
        {
            itemsForSelection.insert( ChangeDialog::ItemsForSelection::value_type(i,types[i]) );
        }

        // Pass in blank for the current item since the user hasn't previously selected an entity type
        ChangeDialog dlg(itemsForSelection,SELECT_GUITYPE_DIALOG_TITLE,"", this);

        if (dlg.DoModal() != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the creation of a new gui");
            FUNCTION_EXIT;
            return;
        }

        typeToUse = dlg.getSelectedName();
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected the entity type which is %s", typeToUse.c_str());

        try
        {
            if ( 0 == typeToUse.compare(NO_TYPE) )
            {
                typeToUse = "";
            }

            TA_Base_Core::IConfigGui* newGui = dynamic_cast<TA_Base_Core::IConfigGui*>(dbAccessor->newItem(typeToUse));

            if (newGui !=  NULL)
            {
                // Append something to the end of the name to indicate that this is a new entity
                std::string newName = newGui->getName();
                newName += AbstractDetailView::NEW_LABEL;
                newGui->setName( newName );

                // Insert this item into the list and set it to be the selected item
                int position = GetListCtrl().InsertItem(-1,newName.c_str());
                GetListCtrl().SetItemData(position, newGui->getUniqueIdentifier());

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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not create a new gui");
            CString actionName, reason;
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATA_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not create a new gui");
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
    // ApplicationListView diagnostics

    #ifdef _DEBUG
    void ApplicationListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void ApplicationListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

