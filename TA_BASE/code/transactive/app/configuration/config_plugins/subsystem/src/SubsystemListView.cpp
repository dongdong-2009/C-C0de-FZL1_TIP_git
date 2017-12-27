/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/subsystem/src/SubsystemListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all subsystems available to be configured. This view must communicate
  * with the SubsystemDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/subsystem/src/StdAfx.h"
#include "app/configuration/config_plugins/subsystem/src/SubsystemListView.h"
#include "app/configuration/config_plugins/subsystem/src/SubsystemComponent.h"
#include "app/configuration/config_plugins/subsystem/src/resource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IConfigSubsystem.h"
#include "core/data_access_interface/src/ConfigSubsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(SubsystemListView, CListView)


    SubsystemListView::SubsystemListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(SubsystemComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_SUBSYSTEM) );
    }


    SubsystemListView::~SubsystemListView()
    {
    }


BEGIN_MESSAGE_MAP(SubsystemListView, AbstractListView)
	//{{AFX_MSG_MAP(SubsystemListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void SubsystemListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }



    void SubsystemListView::copyItem()
    {
        FUNCTION_ENTRY("copyItem");

        CString actionName;
        actionName.LoadString(IDS_COPY);
        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        CString errorMsg, item;
        errorMsg.LoadString(IDS_NOT_PHYSICAL_SUBSYSTEM);
        item.LoadString(IDS_ITEMS_PREVENTING_ACTION);
        errorMsg += _T("\n") + item;
        int numberOfItemsNotCopied = 0;

        while(pos != NULL)
        {
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            TA_Base_Core::IConfigItem* item = getDatabaseAccessor()->getItem(itemId);
            TA_Base_Core::IConfigSubsystem* subsystem = dynamic_cast<TA_Base_Core::IConfigSubsystem*>(item);

            TA_ASSERT(subsystem != NULL, "The configuration item we have is not an IConfigSubsystem*");
            if ( !subsystem->isPhysical() )
            {
                if (numberOfItemsNotCopied < MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                {
                    errorMsg += subsystem->getName().c_str();
                    errorMsg += _T("\n");
                    ++numberOfItemsNotCopied;
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
                        ++numberOfItemsNotCopied;
                        break;
                }
            }
        }

        // If we have any entries that are invalid we must prevent the copy
        if (numberOfItemsNotCopied != 0)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << errorMsg;
            userMsg.showMsgBox(IDS_UE_030060);

            FUNCTION_EXIT;
            return;
        }

        // We can copy this item so proceed in the normal fashion
        AbstractListView::copyItem();
        
        FUNCTION_EXIT;
    }

    
    void SubsystemListView::deleteItem()
    {
        FUNCTION_ENTRY("deleteItem");

        CString actionName;
        actionName.LoadString(IDS_DELETE);
        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        int numberOfItemsNotDeleted = 0;
        CString errorMsg, item;
        errorMsg.LoadString(IDS_NOT_PHYSICAL_SUBSYSTEM);
        item.LoadString(IDS_ITEMS_PREVENTING_ACTION);
        errorMsg += _T("\n") + item;
        int numberOfListedSubsystems = 0;

        while(pos != NULL)
        {
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            TA_Base_Core::IConfigItem* item = getDatabaseAccessor()->getItem(itemId);
            TA_Base_Core::IConfigSubsystem* subsystem = dynamic_cast<TA_Base_Core::IConfigSubsystem*>(item);

            TA_ASSERT(subsystem != NULL, "The configuration item we have is not an IConfigSubsystem*");
            if (!subsystem->isNew() && !subsystem->canDelete())
            {
                if (numberOfItemsNotDeleted < MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                {
                    errorMsg += subsystem->getName().c_str();
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


    /////////////////////////////////////////////////////////////////////////////
    // SubsystemListView diagnostics

    #ifdef _DEBUG
    void SubsystemListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void SubsystemListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

