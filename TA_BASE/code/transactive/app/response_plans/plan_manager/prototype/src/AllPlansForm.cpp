/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/AllPlansForm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// AllPlansForm.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "AllPlansForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAllPlansForm

IMPLEMENT_DYNCREATE(CAllPlansForm, CFormView)

CAllPlansForm::CAllPlansForm()
    : CFormView(CAllPlansForm::IDD)
{
    FUNCTION_ENTRY( "CAllPlansForm" );

    // {{AFX_DATA_INIT(CAllPlansForm)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    m_notes.SetText("This view will be available only to users with plan editing rights.");

    FUNCTION_EXIT;
}


CAllPlansForm::~CAllPlansForm()
{
    FUNCTION_ENTRY( "~CAllPlansForm" );
    FUNCTION_EXIT;
}


void CAllPlansForm::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CFormView::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CAllPlansForm)
    DDX_Control(pDX, IDC_NOTES_PICT, m_notes);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CAllPlansForm, CFormView)
    // {{AFX_MSG_MAP(CAllPlansForm)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllPlansForm diagnostics

#ifdef _DEBUG
void CAllPlansForm::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFormView::AssertValid();

    FUNCTION_EXIT;
}


void CAllPlansForm::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFormView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAllPlansForm message handlers
