/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanControlDoc.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanControlDoc.cpp

#include "StdAfx.h"
#include "PlanControlDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPlanControlDoc

IMPLEMENT_DYNCREATE(CPlanControlDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlanControlDoc, CDocument)
    // {{AFX_MSG_MAP(CPlanControlDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        // DO NOT EDIT what you see in these blocks of generated code!
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanControlDoc construction/destruction

CPlanControlDoc::CPlanControlDoc()
{
    FUNCTION_ENTRY( "CPlanControlDoc" );

    // TODO: add one-time construction code here

    FUNCTION_EXIT;
}


CPlanControlDoc::~CPlanControlDoc()
{
    FUNCTION_ENTRY( "~CPlanControlDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanControlDoc::OnNewDocument()
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
// CPlanControlDoc serialization

void CPlanControlDoc::Serialize(CArchive& ar)
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
// CPlanControlDoc diagnostics

#ifdef _DEBUG
void CPlanControlDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CDocument::AssertValid();

    FUNCTION_EXIT;
}


void CPlanControlDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CDocument::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanControlDoc commands
