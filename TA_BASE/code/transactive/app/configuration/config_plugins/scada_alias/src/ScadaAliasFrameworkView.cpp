/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_alias/src/ScadaAliasFrameworkView.cpp $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of entities
  * and the right side as a view for the user to configure entity details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada_alias/src/StdAfx.h"
#include "app/configuration/config_plugins/scada_alias/src/ScadaAliasFrameworkView.h"
#include "app/configuration/config_plugins/scada_alias/src/ScadaAliasListView.h"
#include "app/configuration/config_plugins/scada_alias/src/ScadaAliasDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IAllView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IDetailView.h"

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
    // These constants are used to pass to the splitter methods so the code is more readable
    const int ScadaAliasFrameworkView::TOP_PANE      = 0;
    const int ScadaAliasFrameworkView::BOTTOM_PANE   = 1;
    const int ScadaAliasFrameworkView::ZERO_COLUMN   = 0;
    
    const int ScadaAliasFrameworkView::TOP_PANE_SIZE = 200;

    IMPLEMENT_DYNCREATE(ScadaAliasFrameworkView, CFormView)


    ScadaAliasFrameworkView::ScadaAliasFrameworkView()
    {
    }


    ScadaAliasFrameworkView::~ScadaAliasFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(ScadaAliasFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(ScadaAliasFrameworkView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    BOOL ScadaAliasFrameworkView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
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
            TA_Base_App::IAllView* listView = dynamic_cast<TA_Base_App::IAllView*>(m_splitter.GetPane(TOP_PANE,ZERO_COLUMN));
            TA_Base_App::IDetailView* detailView = dynamic_cast<TA_Base_App::IDetailView*>(m_splitter.GetPane(BOTTOM_PANE,ZERO_COLUMN));

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

    void ScadaAliasFrameworkView::OnSize(UINT nType, int cx, int cy) 
    {
        CView::OnSize(nType, cx, cy);
        if (m_splitter.GetSafeHwnd())
        {
            m_splitter.MoveWindow(0,0,cx,cy);
        }
    }
    
    CRuntimeClass* ScadaAliasFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(ScadaAliasDetailView);
    }


    CRuntimeClass* ScadaAliasFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(ScadaAliasListView);
    }

    
  
    /////////////////////////////////////////////////////////////////////////////
    // ScadaAliasFrameworkView diagnostics

    #ifdef _DEBUG
    void ScadaAliasFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void ScadaAliasFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}


