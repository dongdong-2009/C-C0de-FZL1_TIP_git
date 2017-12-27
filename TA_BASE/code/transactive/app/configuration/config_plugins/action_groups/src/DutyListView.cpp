/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/action_groups/src/DutyListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all dutys available to be configured. This view must communicate
  * with the DutyDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/action_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/action_groups/src/DutyListView.h"
#include "app/configuration/config_plugins/action_groups/src/DutyComponent.h"
#include "app/configuration/config_plugins/action_groups/src/resource.h"

#include "bus/resources/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(DutyListView, CListView)


    DutyListView::DutyListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(DutyComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_DUTIES) );
    }


    DutyListView::~DutyListView()
    {
    }


BEGIN_MESSAGE_MAP(DutyListView, AbstractListView)
	//{{AFX_MSG_MAP(DutyListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void DutyListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // DutyListView diagnostics

    #ifdef _DEBUG
    void DutyListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void DutyListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

