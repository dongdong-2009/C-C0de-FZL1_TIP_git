/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanStepEditDoc.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanStepEditDoc.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "PlanStepEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDoc

IMPLEMENT_DYNCREATE(CPlanStepEditDoc, CDocument)

CPlanStepEditDoc::CPlanStepEditDoc()
{
    FUNCTION_ENTRY( "CPlanStepEditDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanStepEditDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    if (!CDocument::OnNewDocument())
    {
        FUNCTION_EXIT;
        return FALSE;
    }


    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)
    FUNCTION_EXIT;
    return TRUE;
}


CPlanStepEditDoc::~CPlanStepEditDoc()
{
    FUNCTION_ENTRY( "~CPlanStepEditDoc" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditDoc, CDocument)
    // {{AFX_MSG_MAP(CPlanStepEditDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDoc diagnostics

#ifdef _DEBUG
void CPlanStepEditDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CDocument::AssertValid();

    FUNCTION_EXIT;
}


void CPlanStepEditDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CDocument::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDoc serialization

void CPlanStepEditDoc::Serialize(CArchive& ar)
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
// CPlanStepEditDoc commands
