/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile_groups/src/ProfileGroupsListView.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all profile groups available to be configured. This view must communicate
  * with the ProfileGroupsDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/profile_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/profile_groups/src/ProfileGroupsListView.h"
#include "app/configuration/config_plugins/profile_groups/src/ProfileGroupsComponent.h"
#include "app/configuration/config_plugins/profile_groups/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/ConfigProfileGroup.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(ProfileGroupsListView, CListView)


    ProfileGroupsListView::ProfileGroupsListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(ProfileGroupsComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_PROFILE) );
    }


    ProfileGroupsListView::~ProfileGroupsListView()
    {
    }


BEGIN_MESSAGE_MAP(ProfileGroupsListView, AbstractListView)
	//{{AFX_MSG_MAP(ProfileGroupsListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ProfileGroupsListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void ProfileGroupsListView::deleteItem()
    {
        FUNCTION_ENTRY("deleteItem");

        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        CString actionName;
        actionName.LoadString(IDS_DELETE);
        if (pos == NULL)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        int numberOfItemsNotDeleted = 0;
        //CString errorMsg = "the item could not be deleted because it is used in Profile config. The items preventing the delete are listed below:\n";
        CString errorMsg = "这些被使用在配置文件中：\n";

        while(pos != NULL)
        {
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            TA_Base_Core::IConfigItem* item = getDatabaseAccessor()->getItem(itemId);
            TA_Base_Core::ConfigProfileGroup* profileGroup = dynamic_cast<TA_Base_Core::ConfigProfileGroup*>(item);
			
			// check whether it it used in the Profile and it should not be new
			bool canBeDeleted = false;
			// this case is a new profile group
			if ( profileGroup != NULL && profileGroup->isNew() ) canBeDeleted = true; 
			// this case is not a new group but may be used in Profile Table
			else if ( profileGroup != NULL && profileGroup->isGroupUsedInProfile(profileGroup->getKey()) == false ) canBeDeleted = true;
            
			if ( canBeDeleted == false )
            {
                if (numberOfItemsNotDeleted < MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                {
                    errorMsg += profileGroup->getName().c_str();
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

        //CString itemType;
        //itemType.LoadString(IDS_PROFILE_GROUPS);
        //TA_Base_Bus::TransActiveMessage userMsg;
        //userMsg << itemType;
        //UINT selectedButton = userMsg.showMsgBox(IDS_UW_030019);

        AbstractListView::deleteItem();

        FUNCTION_EXIT;
    }


    void ProfileGroupsListView::copyItem()
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

		//There is no need to check for validity as any profile group can be duplicated

        AbstractListView::copyItem();

        FUNCTION_EXIT;
    }

    /////////////////////////////////////////////////////////////////////////////
    // ProfileListView diagnostics

    #ifdef _DEBUG
    void ProfileGroupsListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void ProfileGroupsListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

