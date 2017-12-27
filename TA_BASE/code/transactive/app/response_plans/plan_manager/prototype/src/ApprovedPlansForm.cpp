/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ApprovedPlansForm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ApprovedPlansForm.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "ApprovedPlansForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApprovedPlansForm

IMPLEMENT_DYNCREATE(CApprovedPlansForm, CFormView)

CApprovedPlansForm::CApprovedPlansForm()
    : CFormView(CApprovedPlansForm::IDD)
{
    FUNCTION_ENTRY( "CApprovedPlansForm" );

    // {{AFX_DATA_INIT(CApprovedPlansForm)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CApprovedPlansForm::~CApprovedPlansForm()
{
    FUNCTION_ENTRY( "~CApprovedPlansForm" );
    FUNCTION_EXIT;
}


void CApprovedPlansForm::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CFormView::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CApprovedPlansForm)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CApprovedPlansForm, CFormView)
    // {{AFX_MSG_MAP(CApprovedPlansForm)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApprovedPlansForm diagnostics

#ifdef _DEBUG
void CApprovedPlansForm::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFormView::AssertValid();

    FUNCTION_EXIT;
}


void CApprovedPlansForm::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFormView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CApprovedPlansForm message handlers
