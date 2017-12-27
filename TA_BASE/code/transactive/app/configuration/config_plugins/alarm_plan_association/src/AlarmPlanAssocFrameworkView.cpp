/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the top of the splitter as a list of alarm plan associations
  * and the bottom as a view for the user to configure a single alarm plan association details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_plan_association/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocFrameworkView.h"

#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocListView.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // These constants are used to pass to the splitter methods so the code is more readable
    const int AlarmPlanAssocFrameworkView::TOP_PANE      = 0;
    const int AlarmPlanAssocFrameworkView::BOTTOM_PANE   = 1;
    const int AlarmPlanAssocFrameworkView::ZERO_COLUMN   = 0;

    const int AlarmPlanAssocFrameworkView::TOP_PANE_SIZE = 200;

    IMPLEMENT_DYNCREATE(AlarmPlanAssocFrameworkView, CView)


    AlarmPlanAssocFrameworkView::AlarmPlanAssocFrameworkView()
    {
    }


    AlarmPlanAssocFrameworkView::~AlarmPlanAssocFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(AlarmPlanAssocFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(AlarmPlanAssocFrameworkView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    BOOL AlarmPlanAssocFrameworkView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
    {
        FUNCTION_ENTRY("Create");

        if (CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
        {
            // Create a splitter with 1 row and 2 columns
            m_splitter.CreateStatic(this,2,1,WS_CHILD|WS_VISIBLE);

            // Create the views inside the splitter panes
            m_splitter.CreateView(TOP_PANE,ZERO_COLUMN,getRuntimeClassForListView(),CSize(0,0),NULL);
            m_splitter.CreateView(BOTTOM_PANE,ZERO_COLUMN,getRuntimeClassForDetailView(),CSize(0,0),NULL);

            // Resize the panes so they are visible
            m_splitter.SetRowInfo( TOP_PANE, TOP_PANE_SIZE, 0 );
	        m_splitter.RecalcLayout();

            // Set this to be the splitter's parent so we get all it's messages
            m_splitter.SetParent(this);

            // Now retrieve the views that have been created
            IAllView* listView = dynamic_cast<IAllView*>(m_splitter.GetPane(TOP_PANE,ZERO_COLUMN));
            IDetailView* detailView = dynamic_cast<IDetailView*>(m_splitter.GetPane(BOTTOM_PANE,ZERO_COLUMN));

            setListView(*listView);
            setDetailView(*detailView);
    
            // Let the views know about each other so they can 'talk' to each other.
            if (listView != NULL)
            {
                listView->setDetailView( detailView );
            }
            if (detailView != NULL)
            {
                detailView->setListView( listView );
            }

        }

        FUNCTION_EXIT;
        return TRUE;
    }


    void AlarmPlanAssocFrameworkView::OnSize(UINT nType, int cx, int cy) 
    {
        CView::OnSize(nType, cx, cy);
        if (m_splitter.GetSafeHwnd())
        {
            m_splitter.MoveWindow(0,0,cx,cy);
        }
    }


    CRuntimeClass* AlarmPlanAssocFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(AlarmPlanAssocDetailView);
    }


    CRuntimeClass* AlarmPlanAssocFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(AlarmPlanAssocListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // AlarmPlanAssocFrameworkView diagnostics

    #ifdef _DEBUG
    void AlarmPlanAssocFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void AlarmPlanAssocFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



