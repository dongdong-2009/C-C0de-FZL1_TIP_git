/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of agents
  * and the right side as a view for the user to configure agent details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/agent/src/StdAfx.h"
#include "app/configuration/config_plugins/agent/src/AgentFrameworkView.h"

#include "app/configuration/config_plugins/agent/src/AgentTreeView.h"
#include "app/configuration/config_plugins/agent/src/AgentDetailView.h"
#include "app/configuration/config_plugins/agent/src/EntityView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(AgentFrameworkView, CView)


    AgentFrameworkView::AgentFrameworkView() 
    {
    }


    AgentFrameworkView::~AgentFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(AgentFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(AgentFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AgentFrameworkView::setComponentInterface(IComponent* component)
    {
        FUNCTION_ENTRY("setComponentInterface");
        
        AbstractFrameworkView::setComponentInterface(component);

        AgentTreeView* tree = dynamic_cast<AgentTreeView*>(getListView());
        if (tree != NULL)
        {
            tree->setFramework( *this );
        }

        FUNCTION_EXIT;
    }


   
    void AgentFrameworkView::changeDetailView(CRuntimeClass* newView)
    {
        FUNCTION_ENTRY("changeDetailView");

        // Replace the bottom right view with the one for this selected component
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "About to delete the old detail view");
        getSplitterWindow().DeleteView(ZERO_ROW,RIGHT_PANE);

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "About to create the new view for the bottom-right pane");

        if (!getSplitterWindow().CreateView(ZERO_ROW, RIGHT_PANE, newView, CSize(0, 0), NULL))
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugFatal, "View could not be created. This is an internal error that needs to be fixed");

            CString itemName;
            itemName.LoadString(IDS_DETAIL_VIEW);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030063);

            // Since this failed then lets load our default view back in so next time everything works nicely
            getSplitterWindow().CreateView(ZERO_ROW, RIGHT_PANE, getRuntimeClassForDetailView(), CSize(0,0), NULL);
        }

        // Resize the panes so they are visible
	    getSplitterWindow().RecalcLayout();


  
//        dynamic_cast<CFrameWnd*>(AfxGetMainWnd())->SetActiveView( dynamic_cast<CView*>(getSplitterWindow().GetPane(ZERO_ROW,RIGHT_PANE)) );

        // Retrieve the newly created detail view
        IDetailView* detailView = dynamic_cast<IDetailView*>(getSplitterWindow().GetPane(ZERO_ROW,RIGHT_PANE));
        
        // Now update the detail view in our base class
        setDetailView(*detailView);
        detailView->setListView( getListView() );
        detailView->setDatabaseAccessor( getComponent()->getDatabaseAccessor() );
        detailView->setMessageSender( getComponent()->getMessageSender() );

        // Finally let the list view know that it has a new detail view to talk to
        getListView()->setDetailView( detailView );

        FUNCTION_EXIT;
    }

 
    void AgentFrameworkView::itemSelected(const CString& item)
    {
        FUNCTION_ENTRY("itemSelected");

        AgentTreeView* tree = dynamic_cast<AgentTreeView*>(getListView());

        TA_ASSERT( tree != NULL, "The item list stored was NULL or of the wrong type");

        if (item == AbstractComponent::MENU_COPY_ITEM ||
            item == AbstractComponent::TOOLBAR_COPY_ITEM )
        {
            tree->copyItem();
        }
        else if (item == AbstractComponent::MENU_DELETE_ITEM ||
                 item == AbstractComponent::TOOLBAR_DELETE_ITEM )
        {
            tree->deleteItem();
        }
        else if (item == AbstractComponent::MENU_NEW_ITEM ||
                 item == AbstractComponent::TOOLBAR_NEW_ITEM )
        {
            tree->newItem();
        }

        FUNCTION_EXIT;
    }


    CRuntimeClass* AgentFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(AgentDetailView);
    }


    CRuntimeClass* AgentFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(AgentTreeView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // AgentFrameworkView diagnostics

    #ifdef _DEBUG
    void AgentFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void AgentFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}




