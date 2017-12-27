/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_type/src/AlarmTypeListView.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all alarm types available to be configured. This view must communicate
  * with the AlarmTypeDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IConfigAlarmType.h"

#include "app/configuration/config_plugins/alarm_type/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_type/src/AlarmTypeListView.h"
#include "app/configuration/config_plugins/alarm_type/src/AlarmTypeComponent.h"
#include "app/configuration/config_plugins/alarm_type/src/resource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/resources/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(AlarmTypeListView, CListView)


    AlarmTypeListView::AlarmTypeListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(AlarmTypeComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ALARM_TYPE) );
    }


    AlarmTypeListView::~AlarmTypeListView()
    {
    }


BEGIN_MESSAGE_MAP(AlarmTypeListView, AbstractListView)
	//{{AFX_MSG_MAP(AlarmTypeListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AlarmTypeListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void AlarmTypeListView::deleteItem()
    {
        FUNCTION_ENTRY("deleteItem");

        // Delete item not currently required for AlarmTypes. This method should never be called.
        TA_ASSERT(false,"This method should be uncommented if it needs to be called");

        /*
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
        item.LoadString(IDS_ITEM_PREVENTING_ACTION);
        errorMsg += _T("\n") + item;
  
        while(pos != NULL)
        {
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            TA_Base_Core::IConfigItem* item = getDatabaseAccessor()->getItem(itemId);
            TA_Base_Core::IConfigAlarmType* alarmType = dynamic_cast<TA_Base_Core::IConfigAlarmType*>(item);

            TA_ASSERT(alarmType != NULL, "The configuration item we have is not an IConfigAlarmType*");
            if (!alarmType->isNew() && !alarmType->canDelete())
            {
                if (numberOfItemsNotDeleted < MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                {
                    errorMsg += alarmType->getName().c_str();
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

        // We can delete these items so proceed in the normal fashion
        AbstractListView::deleteItem();
*/
        FUNCTION_EXIT;
    }




    /////////////////////////////////////////////////////////////////////////////
    // AlarmTypeListView diagnostics

    #ifdef _DEBUG
    void AlarmTypeListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void AlarmTypeListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

