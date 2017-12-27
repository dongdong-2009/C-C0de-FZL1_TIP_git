/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/OnlinePrinterFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/online_printer/src/StdAfx.h"
#include "app/configuration/config_plugins/online_printer/src/OnlinePrinterFrameworkView.h"

#include "app/configuration/config_plugins/online_printer/src/OnlinePrinterListView.h"
#include "app/configuration/config_plugins/online_printer/src/OnlinePrinterDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(OnlinePrinterFrameworkView, CView)


    OnlinePrinterFrameworkView::OnlinePrinterFrameworkView()
    {
    }


    OnlinePrinterFrameworkView::~OnlinePrinterFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(OnlinePrinterFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(OnlinePrinterFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    bool OnlinePrinterFrameworkView::canChangeSelection()
    {
        OnlinePrinterDetailView* detail = dynamic_cast<OnlinePrinterDetailView*>(getDetailView());
        if (detail)
        {
            if (detail->hasChanges())
            {
                if (MessageBox("Save changes?", "Selection change", MB_OKCANCEL) == IDOK)
                {
                    detail->applyChanges();
                    return true;
                }
            }
        }
        return true;
    }

    CRuntimeClass* OnlinePrinterFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(OnlinePrinterDetailView);
    }


    CRuntimeClass* OnlinePrinterFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(OnlinePrinterListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // OnlinePrinterFrameworkView diagnostics

    #ifdef _DEBUG
    void OnlinePrinterFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void OnlinePrinterFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



