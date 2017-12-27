/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanEditDoc.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanEditDoc.cpp

#include "StdAfx.h"
#include "PlanEditDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanEditDoc

IMPLEMENT_DYNCREATE(CPlanEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlanEditDoc, CDocument)
    // {{AFX_MSG_MAP(CPlanEditDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        // DO NOT EDIT what you see in these blocks of generated code!
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanEditDoc construction/destruction

CPlanEditDoc::CPlanEditDoc()
{
    FUNCTION_ENTRY( "CPlanEditDoc" );

    // TODO: add one-time construction code here

    FUNCTION_EXIT;
}


CPlanEditDoc::~CPlanEditDoc()
{
    FUNCTION_ENTRY( "~CPlanEditDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanEditDoc::OnNewDocument()
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


/////////////////////////////////////////////////////////////////////////////
// CPlanEditDoc serialization

void CPlanEditDoc::Serialize(CArchive& ar)
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
// CPlanEditDoc diagnostics

#ifdef _DEBUG
void CPlanEditDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CDocument::AssertValid();

    FUNCTION_EXIT;
}


void CPlanEditDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CDocument::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanEditDoc commands
