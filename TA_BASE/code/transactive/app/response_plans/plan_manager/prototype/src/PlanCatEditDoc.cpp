/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanCatEditDoc.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanCatEditDoc.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "PlanCatEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanCatEditDoc

IMPLEMENT_DYNCREATE(CPlanCatEditDoc, CDocument)

CPlanCatEditDoc::CPlanCatEditDoc()
{
    FUNCTION_ENTRY( "CPlanCatEditDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanCatEditDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    if (!CDocument::OnNewDocument())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    FUNCTION_EXIT;
    return TRUE;
}


CPlanCatEditDoc::~CPlanCatEditDoc()
{
    FUNCTION_ENTRY( "~CPlanCatEditDoc" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanCatEditDoc, CDocument)
    // {{AFX_MSG_MAP(CPlanCatEditDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanCatEditDoc diagnostics

#ifdef _DEBUG
void CPlanCatEditDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CDocument::AssertValid();

    FUNCTION_EXIT;
}


void CPlanCatEditDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CDocument::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanCatEditDoc serialization

void CPlanCatEditDoc::Serialize(CArchive& ar)
{
    FUNCTION_ENTRY( "Serialize" );

    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanCatEditDoc commands
