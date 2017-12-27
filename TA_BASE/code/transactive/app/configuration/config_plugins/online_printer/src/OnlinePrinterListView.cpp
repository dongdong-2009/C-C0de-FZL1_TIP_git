/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/OnlinePrinterListView.cpp $
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
#include "app/configuration/config_plugins/online_printer/src/OnlinePrinterComponent.h"
#include "app/configuration/config_plugins/online_printer/src/resource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/resources/resource.h"

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/src/OnlinePrinterAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;

using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{

    IMPLEMENT_DYNCREATE(OnlinePrinterListView, CListView)


    OnlinePrinterListView::OnlinePrinterListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(OnlinePrinterComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ONLINE_PRINTER) );
    }


    OnlinePrinterListView::~OnlinePrinterListView()
    {
    }


BEGIN_MESSAGE_MAP(OnlinePrinterListView, AbstractListView)
	//{{AFX_MSG_MAP(OnlinePrinterListView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, onItemchanging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



    int OnlinePrinterListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
        // override the AbstractListView::OnCreate so we can have single sel list
        // Set the styles for our list control before it is created
        lpCreateStruct->style = LVS_SINGLESEL | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SORTASCENDING | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | WS_BORDER;

	    if (CListView::OnCreate(lpCreateStruct) == -1)
		    return -1;
	    
	    return 0;
    }

    
    void OnlinePrinterListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void OnlinePrinterListView::onItemchanging(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        TA_ASSERT(pNMListView != NULL, "The NM_LISTVIEW structure passed to us was NULL");

		if (!(LVIF_STATE & pNMListView->uChanged))
		{
			// If this is not a state change (ie change in selection), we may ignore it
			return;
		}

        if ( (pNMListView->uOldState & LVIS_SELECTED) && !(pNMListView->uNewState & LVIS_SELECTED) )
        {
            // confirm to save changes
            CWnd* parentWnd = GetParent()->GetParent();
            OnlinePrinterFrameworkView* parent = dynamic_cast<OnlinePrinterFrameworkView*>(parentWnd);
            if (parent)
            {
                if (!parent->canChangeSelection())
                {
                    *pResult = 1;
                    return;
                }
            }

        }
	    *pResult = 0;
    }

    /////////////////////////////////////////////////////////////////////////////
    // OnlinePrinterListView diagnostics

    #ifdef _DEBUG
    void OnlinePrinterListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void OnlinePrinterListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

