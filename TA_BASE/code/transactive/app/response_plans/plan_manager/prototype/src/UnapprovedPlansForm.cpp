// UnapprovedPlansForm.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "UnapprovedPlansForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnapprovedPlansForm

IMPLEMENT_DYNCREATE(CUnapprovedPlansForm, CFormView)

CUnapprovedPlansForm::CUnapprovedPlansForm()
    : CFormView(CUnapprovedPlansForm::IDD)
{
    FUNCTION_ENTRY( "CUnapprovedPlansForm" );

    // {{AFX_DATA_INIT(CUnapprovedPlansForm)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    m_notes.SetText("This view will be available only to users with plan editing rights.");

    FUNCTION_EXIT;
}


CUnapprovedPlansForm::~CUnapprovedPlansForm()
{
    FUNCTION_ENTRY( "~CUnapprovedPlansForm" );
    FUNCTION_EXIT;
}


void CUnapprovedPlansForm::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CFormView::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CUnapprovedPlansForm)
    DDX_Control(pDX, IDC_NOTES_PICT, m_notes);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CUnapprovedPlansForm, CFormView)
    // {{AFX_MSG_MAP(CUnapprovedPlansForm)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnapprovedPlansForm diagnostics

#ifdef _DEBUG
void CUnapprovedPlansForm::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFormView::AssertValid();

    FUNCTION_EXIT;
}


void CUnapprovedPlansForm::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFormView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUnapprovedPlansForm message handlers
