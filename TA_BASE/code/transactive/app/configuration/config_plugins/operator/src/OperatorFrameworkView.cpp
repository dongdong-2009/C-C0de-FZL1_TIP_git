/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/operator/src/OperatorFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of operators
  * and the right side as a view for the user to configure operator details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/operator/src/StdAfx.h"
#include "app/configuration/config_plugins/operator/src/OperatorFrameworkView.h"

#include "app/configuration/config_plugins/operator/src/OperatorListView.h"
#include "app/configuration/config_plugins/operator/src/OperatorDetailView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(OperatorFrameworkView, CFormView)


    OperatorFrameworkView::OperatorFrameworkView()
    {
    }


    OperatorFrameworkView::~OperatorFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(OperatorFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(OperatorFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* OperatorFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(OperatorDetailView);
    }


    CRuntimeClass* OperatorFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(OperatorListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // OperatorFrameworkView diagnostics

    #ifdef _DEBUG
    void OperatorFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void OperatorFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}


