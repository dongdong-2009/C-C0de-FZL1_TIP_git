/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ScadaFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a tree of scadas entities
  * and the right side as a view for the user to configure an item's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/StdAfx.h"
#include "app/configuration/config_plugins/scada/src/ScadaFrameworkView.h"

#include "app/configuration/config_plugins/scada/src/ScadaTreeView.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(ScadaFrameworkView, CView)


    ScadaFrameworkView::ScadaFrameworkView()
    {
    }


    ScadaFrameworkView::~ScadaFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(ScadaFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(ScadaFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    void ScadaFrameworkView::itemSelected(const CString& item)
    {
        FUNCTION_ENTRY("itemSelected");

        ScadaTreeView* list = dynamic_cast<ScadaTreeView*>(getListView());

        TA_ASSERT( list != NULL, "The item list stored was NULL or of the wrong type");

        if (item == AbstractComponent::MENU_COPY_ITEM ||
            item == AbstractComponent::TOOLBAR_COPY_ITEM )
        {
            list->copyItem();
        }
        else if (item == AbstractComponent::MENU_DELETE_ITEM ||
                 item == AbstractComponent::TOOLBAR_DELETE_ITEM )
        {
            list->deleteItem();
        }
        else if (item == AbstractComponent::MENU_NEW_ITEM ||
                 item == AbstractComponent::TOOLBAR_NEW_ITEM )
        {
            list->newItem();
        }

        FUNCTION_EXIT;
    }


    CRuntimeClass* ScadaFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(ScadaDetailView);
    }


    CRuntimeClass* ScadaFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(ScadaTreeView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // ScadaFrameworkView diagnostics

    #ifdef _DEBUG
    void ScadaFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void ScadaFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



