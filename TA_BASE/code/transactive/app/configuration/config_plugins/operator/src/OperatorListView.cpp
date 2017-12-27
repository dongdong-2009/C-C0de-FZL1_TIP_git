/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/operator/src/OperatorListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all operators available to be configured. This view must communicate
  * with the OperatorDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/operator/src/StdAfx.h"
#include "app/configuration/config_plugins/operator/src/OperatorListView.h"
#include "app/configuration/config_plugins/operator/src/OperatorComponent.h"
#include "app/configuration/config_plugins/operator/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IConfigOperator.h"

#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/src/SessionAccessFactory.h" //TD15556

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(OperatorListView, CListView)


    OperatorListView::OperatorListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(OperatorComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_OPERATOR) );
    }


    OperatorListView::~OperatorListView()
    {
    }


BEGIN_MESSAGE_MAP(OperatorListView, AbstractListView)
	//{{AFX_MSG_MAP(OperatorListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void OperatorListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void OperatorListView::deleteItem()
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

		int numberOfItemsActive = 0; //TD15556

        int numberOfItemsNotDeleted = 0;
        CString actionName, errorMsg, item;
        actionName.LoadString(IDS_DELETE);
        errorMsg.LoadString(IDS_SYSTEM_OPERATOR);
        item.LoadString(IDS_ITEMS_PREVENTING_ACTION);
        errorMsg += _T("\n") + item;

        while(pos != NULL)
        {
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            TA_Base_Core::IConfigItem* item = getDatabaseAccessor()->getItem(itemId);

			//TD18240 marc_bugfix
			//Since this operator is new , no need to check active operator
			if (item->isNew())
			{
				continue;
			}

            TA_Base_Core::IConfigOperator* selectedOperator = dynamic_cast<TA_Base_Core::IConfigOperator*>(item);

			//TD15556++
			if (isActiveOperator(selectedOperator->getKey()))
			{
				++numberOfItemsActive;
				break;
			}
			//++TD15556

            if ( (selectedOperator != NULL) && (selectedOperator->isSystemOperator()) )
            {
                if (numberOfItemsNotDeleted < MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                {
                    errorMsg += selectedOperator->getName().c_str();
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

		//TD15556++
		// If we have any entries that are active we must prevent the delete
		if (numberOfItemsActive != 0)
        {
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName = "删除";
			CString errorMsg = "该操作员已登录";
			userMsg << actionName << errorMsg;
			userMsg.showMsgBox(IDS_UE_030060);

			FUNCTION_EXIT;
            return;
		}
		//++TD15556

        // If we have any entries that are invalid we must prevent the delete
        if (numberOfItemsNotDeleted != 0)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << errorMsg;
            userMsg.showMsgBox(IDS_UE_030060);

            FUNCTION_EXIT;
            return;
        }

#if 0	//Message unnecessary 
        CString itemType;
        itemType.LoadString(IDS_OPERATOR);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << itemType;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_030019);
#endif

        AbstractListView::deleteItem();

        FUNCTION_EXIT;
    }


    void OperatorListView::copyItem()
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

        int numberOfItemsNotCopied = 0;

        CString errorMsg, item;
        errorMsg.LoadString(IDS_SYSTEM_OPERATOR);
        item.LoadString(IDS_ITEMS_PREVENTING_ACTION);
        errorMsg += _T("\n") + item;

        while(pos != NULL)
        {
            // An item is selected
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            TA_Base_Core::IConfigItem* item = getDatabaseAccessor()->getItem(itemId);
            TA_Base_Core::IConfigOperator* selectedOperator = dynamic_cast<TA_Base_Core::IConfigOperator*>(item);
            if ( (selectedOperator != NULL) && (selectedOperator->isSystemOperator()) )
            {
                if (numberOfItemsNotCopied < MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                {
                    errorMsg += selectedOperator->getName().c_str();
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

        AbstractListView::copyItem();

        FUNCTION_EXIT;
    }


	//TD15556++
    bool OperatorListView::isActiveOperator(unsigned long operatorKey)
    {
		//do not allow deletion of operator when used by active session

		bool isActOp = false;

//		//get sessions in DB 
//		std::vector<TA_Base_Core::ISession*> sessions;
//		sessions = TA_Base_Core::SessionAccessFactory::getInstance().getAllActiveSessions();
//		for (unsigned int i=0; i<sessions.size(); i++)
//		{
//			TA_Base_Core::ISession* session = sessions[i];
//
//			//check if active operator is the operator requested for deletion
//			unsigned long opKey = session->getOperatorKey();
//			if (operatorKey == opKey)
//			{
//				isActOp = true;
//				break;
//			}
//		}
//
//		if (isActOp)
//		{
//			//if found active in DB, no need to check override
//			return isActOp;
//		}

		//for overrided operators, connect to AuthAgent thru AuthLib
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		std::vector<TA_Base_Bus::SessionInfo> activeSessions = m_authLibrary.getActiveSessions(sessionId);
		for (unsigned int j=0; j<activeSessions.size(); j++)
		{
			TA_Base_Bus::SessionInfo sessionInfo = activeSessions[j];
			std::vector<unsigned long> opKeys = sessionInfo.OperatorId;

			//loop through all operators
			for (unsigned int k=0; k<opKeys.size(); k++)
			{
				//check if active operator is the operator requested for deletion
				unsigned long opKey = opKeys[k];
				if (operatorKey == opKey)
				{
					isActOp = true;
					break;
				}
			}
		}

		return isActOp;
    }
	//++TD15556



    /////////////////////////////////////////////////////////////////////////////
    // OperatorListView diagnostics

    #ifdef _DEBUG
    void OperatorListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void OperatorListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

