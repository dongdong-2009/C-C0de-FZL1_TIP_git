/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/AbstractFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of items
  * and the right side as a view for the user to configure item details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/generic_entity/src/StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractFrameworkView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the width of the entity list pane in the splitter window
    const int AbstractFrameworkView::LIST_WIDTH = 200;

    // These constants are used to pass to the splitter methods so the code is more readable
    const int AbstractFrameworkView::LEFT_PANE  = 0;
    const int AbstractFrameworkView::RIGHT_PANE = 1;
    const int AbstractFrameworkView::ZERO_ROW   = 0;

    AbstractFrameworkView::AbstractFrameworkView() : m_listView(NULL),
                                                     m_detailView(NULL),
                                                     m_component(NULL)

    {
    }


    AbstractFrameworkView::~AbstractFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(AbstractFrameworkView, CView)
	//{{AFX_MSG_MAP(AbstractFrameworkView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AbstractFrameworkView::setComponentInterface(IComponent* component)
    {
        FUNCTION_ENTRY("setComponentInterface");

        TA_ASSERT(component != NULL, "AbstractComponent passed is NULL");

        // Pass the database accessor onto the view containing details for a single item
        // so that it can access extra information if required.
        if (m_detailView != NULL)
        {
            m_detailView->setDatabaseAccessor( component->getDatabaseAccessor() );
            m_detailView->setMessageSender( component->getMessageSender() );

            // TD4195: Now force the derived detail view to get its database accessor.
            m_detailView->populateItemDetails( NULL );
        }


        // Pass the database accessor onto the view containing the list and tell it
        // to load all its items.
        if (m_listView != NULL)
        {
            m_listView->setDatabaseAccessor( component->getDatabaseAccessor() );
            m_listView->setMessageSender( &(component->getMessageSender()));
            m_listView->setStatusBar( component->getStatusBar() );
            m_listView->loadAllItems();
        }

        m_component = component;

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // AbstractFrameworkView drawing

    void AbstractFrameworkView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
	    // TODO: add draw code here
    }


    BOOL AbstractFrameworkView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
    {
        FUNCTION_ENTRY("Create");

        if (CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
        {
            // Create a splitter with 1 row and 2 columns
            m_splitter.CreateStatic(this,1,2,WS_CHILD|WS_VISIBLE);

            // Create the views inside the splitter panes
            m_splitter.CreateView(ZERO_ROW,LEFT_PANE,getRuntimeClassForListView(),CSize(0,0),NULL);
            m_splitter.CreateView(ZERO_ROW,RIGHT_PANE,getRuntimeClassForDetailView(),CSize(0,0),NULL);

            // Resize the panes so they are visible
            m_splitter.SetColumnInfo( LEFT_PANE, LIST_WIDTH, 0 );
	        m_splitter.RecalcLayout();

            // Set this to be the splitter's parent so we get all it's messages
            m_splitter.SetParent(this);

            // Now retrieve the views that have been created
            m_listView = dynamic_cast<IAllView*>(m_splitter.GetPane(ZERO_ROW,LEFT_PANE));
            m_detailView = dynamic_cast<IDetailView*>(m_splitter.GetPane(ZERO_ROW,RIGHT_PANE));

            // Let the views know about each other so they can 'talk' to each other.
            if (m_listView != NULL)
            {
                m_listView->setDetailView( m_detailView );
            }
            if (m_detailView != NULL)
            {
                m_detailView->setListView( m_listView );
            }

        }

        FUNCTION_EXIT;
        return TRUE;
    }


    void AbstractFrameworkView::OnSize(UINT nType, int cx, int cy) 
    {
        CView::OnSize(nType, cx, cy);
        if (m_splitter.GetSafeHwnd())
        {
            m_splitter.MoveWindow(0,0,cx,cy);
        }
    }


    void AbstractFrameworkView::itemSelected(const CString& item)
    {
        FUNCTION_ENTRY("itemSelected");

        AbstractListView* list = dynamic_cast<AbstractListView*>(m_listView);

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


    /////////////////////////////////////////////////////////////////////////////
    // AbstractFrameworkView diagnostics

    #ifdef _DEBUG
    void AbstractFrameworkView::AssertValid() const
    {
	    CView::AssertValid();
    }

    void AbstractFrameworkView::Dump(CDumpContext& dc) const
    {
	    CView::Dump(dc);
    }
    #endif //_DEBUG

}


