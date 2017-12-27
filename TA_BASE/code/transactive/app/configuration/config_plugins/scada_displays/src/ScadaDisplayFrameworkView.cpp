/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_displays/src/ScadaDisplayFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of Scada displays
  * and the right side as a view for the user to configure Scada display details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada_displays/src/StdAfx.h"
#include "app/configuration/config_plugins/scada_displays/src/ScadaDisplayFrameworkView.h"

#include "app/configuration/config_plugins/scada_displays/src/ScadaDisplayListView.h"
#include "app/configuration/config_plugins/scada_displays/src/ScadaDisplayDetailView.h"

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
    IMPLEMENT_DYNCREATE(ScadaDisplayFrameworkView, CFormView)


    ScadaDisplayFrameworkView::ScadaDisplayFrameworkView()
    {
    }


    ScadaDisplayFrameworkView::~ScadaDisplayFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(ScadaDisplayFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(ScadaDisplayFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* ScadaDisplayFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(ScadaDisplayDetailView);
    }


    CRuntimeClass* ScadaDisplayFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(ScadaDisplayListView);
    }

   
    /////////////////////////////////////////////////////////////////////////////
    // ScadaDisplayFrameworkView diagnostics

    #ifdef _DEBUG
    void ScadaDisplayFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void ScadaDisplayFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}


