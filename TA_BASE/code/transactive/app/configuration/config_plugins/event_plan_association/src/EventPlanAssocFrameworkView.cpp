/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_plan_association/src/EventPlanAssocFrameworkView.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the top of the splitter as a list of event plan associations
  * and the bottom as a view for the user to configure a single event plan association details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/event_plan_association/src/StdAfx.h"

#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocDetailView.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocFrameworkView.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // These constants are used to pass to the splitter methods so the code is more readable
    const int EventPlanAssocFrameworkView::TOP_PANE      = 0;
    const int EventPlanAssocFrameworkView::BOTTOM_PANE   = 1;
    const int EventPlanAssocFrameworkView::ZERO_COLUMN   = 0;

    const int EventPlanAssocFrameworkView::TOP_PANE_SIZE = 200;


    IMPLEMENT_DYNCREATE(EventPlanAssocFrameworkView, CView)


    EventPlanAssocFrameworkView::EventPlanAssocFrameworkView()
    {
    }


    EventPlanAssocFrameworkView::~EventPlanAssocFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(EventPlanAssocFrameworkView, AbstractFrameworkView)
    //{{AFX_MSG_MAP(EventPlanAssocFrameworkView)
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


    BOOL EventPlanAssocFrameworkView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
    {
        FUNCTION_ENTRY("Create");

        if ( CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext) )
        {
            // Create a splitter with 1 row and 2 columns
            m_splitter.CreateStatic( this, 2, 1, WS_CHILD|WS_VISIBLE );

            // Create the views inside the splitter panes
            m_splitter.CreateView( TOP_PANE,ZERO_COLUMN, getRuntimeClassForListView(), CSize(0,0), NULL );
            m_splitter.CreateView( BOTTOM_PANE,ZERO_COLUMN, getRuntimeClassForDetailView(), CSize(0,0), NULL );

            // Resize the panes so they are visible
            m_splitter.SetRowInfo( TOP_PANE, TOP_PANE_SIZE, 0 );
            m_splitter.RecalcLayout();

            // Set this to be the splitter's parent so we get all it's messages
            m_splitter.SetParent( this );

            // Now retrieve the views that have been created
            IAllView* listView = dynamic_cast<IAllView*>(m_splitter.GetPane( TOP_PANE, ZERO_COLUMN ));
            IDetailView* detailView = dynamic_cast<IDetailView*>(m_splitter.GetPane( BOTTOM_PANE, ZERO_COLUMN ));

            setListView( *listView );
            setDetailView( *detailView );
    
            // Let the views know about each other so they can 'talk' to each other.
            if ( NULL != listView )
            {
                listView->setDetailView( detailView );
            }
            if ( NULL != detailView )
            {
                detailView->setListView( listView );
            }

        }

        FUNCTION_EXIT;
        return TRUE;
    }


    void EventPlanAssocFrameworkView::OnSize(UINT nType, int cx, int cy) 
    {
        CView::OnSize( nType, cx, cy );
        if ( m_splitter.GetSafeHwnd() )
        {
            m_splitter.MoveWindow( 0, 0, cx, cy );
        }
    }


    CRuntimeClass* EventPlanAssocFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(EventPlanAssocDetailView);
    }


    CRuntimeClass* EventPlanAssocFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(EventPlanAssocListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // EventPlanAssocFrameworkView diagnostics

    #ifdef _DEBUG
    void EventPlanAssocFrameworkView::AssertValid() const
    {
        AbstractFrameworkView::AssertValid();
    }

    void EventPlanAssocFrameworkView::Dump(CDumpContext& dc) const
    {
        AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



