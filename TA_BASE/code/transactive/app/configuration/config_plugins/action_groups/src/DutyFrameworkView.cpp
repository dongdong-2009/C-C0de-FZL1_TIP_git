/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/action_groups/src/DutyFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of duties
  * and the right side as a view for the user to configure duty details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/action_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/action_groups/src/DutyFrameworkView.h"

#include "app/configuration/config_plugins/action_groups/src/DutyListView.h"
#include "app/configuration/config_plugins/action_groups/src/DutyDetailView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(DutyFrameworkView, CFormView)


    DutyFrameworkView::DutyFrameworkView()
    {
    }


    DutyFrameworkView::~DutyFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(DutyFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(DutyFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* DutyFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(DutyDetailView);
    }


    CRuntimeClass* DutyFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(DutyListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // DutyFrameworkView diagnostics

    #ifdef _DEBUG
    void DutyFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void DutyFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}


