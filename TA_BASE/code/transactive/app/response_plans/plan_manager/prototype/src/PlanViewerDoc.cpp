/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanViewerDoc.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanViewerDoc.cpp : implementation of the CPlanViewerDoc class
//

#include "stdafx.h"
#include "PlanViewer.h"

#include "PlanViewerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanViewerDoc

IMPLEMENT_DYNCREATE(CPlanViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlanViewerDoc, CDocument)
    // {{AFX_MSG_MAP(CPlanViewerDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        // DO NOT EDIT what you see in these blocks of generated code!
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanViewerDoc construction/destruction

CPlanViewerDoc::CPlanViewerDoc()
{
    FUNCTION_ENTRY( "CPlanViewerDoc" );

    // TODO: add one-time construction code here

    FUNCTION_EXIT;
}


CPlanViewerDoc::~CPlanViewerDoc()
{
    FUNCTION_ENTRY( "~CPlanViewerDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanViewerDoc::OnNewDocument()
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
// CPlanViewerDoc serialization

void CPlanViewerDoc::Serialize(CArchive& ar)
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
// CPlanViewerDoc diagnostics

#ifdef _DEBUG
void CPlanViewerDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CDocument::AssertValid();

    FUNCTION_EXIT;
}


void CPlanViewerDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CDocument::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanViewerDoc commands
