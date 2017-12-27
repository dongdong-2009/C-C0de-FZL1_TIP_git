/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/generic_entity/src/EntityFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of entities
  * and the right side as a view for the user to configure entity details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/generic_entity/src/StdAfx.h"
#include "app/configuration/config_plugins/generic_entity/src/EntityFrameworkView.h"

#include "app/configuration/config_plugins/generic_entity/src/EntityListView.h"
#include "app/configuration/config_plugins/generic_entity/src/EntityView.h"

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
    IMPLEMENT_DYNCREATE(EntityFrameworkView, CFormView)


    EntityFrameworkView::EntityFrameworkView()
    {
    }


    EntityFrameworkView::~EntityFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(EntityFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(EntityFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* EntityFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(EntityView);
    }


    CRuntimeClass* EntityFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(EntityListView);
    }

    
  
    /////////////////////////////////////////////////////////////////////////////
    // EntityFrameworkView diagnostics

    #ifdef _DEBUG
    void EntityFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void EntityFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}


