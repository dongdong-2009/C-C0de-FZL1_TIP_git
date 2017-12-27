/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the default view linked to the document. This will not be used and will be left
  * blank.
  */

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorView.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorView

    IMPLEMENT_DYNCREATE(CConfigurationEditorView, CView)

    BEGIN_MESSAGE_MAP(CConfigurationEditorView, CView)
	    //{{AFX_MSG_MAP(CConfigurationEditorView)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorView construction/destruction

    CConfigurationEditorView::CConfigurationEditorView()
    {

    }

    CConfigurationEditorView::~CConfigurationEditorView()
    {
    }

    BOOL CConfigurationEditorView::PreCreateWindow(CREATESTRUCT& cs)
    {
	    // Modify the Window class or styles here by modifying the CREATESTRUCT cs

	    return CView::PreCreateWindow(cs);
    }

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorView drawing

    void CConfigurationEditorView::OnDraw(CDC* pDC)
    {
    }

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorView diagnostics

    #ifdef _DEBUG
    void CConfigurationEditorView::AssertValid() const
    {
	    CView::AssertValid();
    }

    void CConfigurationEditorView::Dump(CDumpContext& dc) const
    {
	    CView::Dump(dc);
    }

    CConfigurationEditorDoc* CConfigurationEditorView::GetDocument() // non-debug version is inline
    {
	    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CConfigurationEditorDoc)));
	    return (CConfigurationEditorDoc*)m_pDocument;
    }
    #endif //_DEBUG

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorView message handlers

}

