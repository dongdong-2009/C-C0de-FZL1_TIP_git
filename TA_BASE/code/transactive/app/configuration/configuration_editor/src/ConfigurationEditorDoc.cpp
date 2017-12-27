/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorDoc.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main document of the framework. We will not use the document extensively as we
  * are not creating new documents and saving etc. This is really here to complete the framework.
  */

#pragma warning(disable:4786)

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorDoc.h"

#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorDoc

    IMPLEMENT_DYNCREATE(CConfigurationEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CConfigurationEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CConfigurationEditorDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorDoc construction/destruction

    CConfigurationEditorDoc::CConfigurationEditorDoc()
    {

    }

    CConfigurationEditorDoc::~CConfigurationEditorDoc()
    {
    }

    BOOL CConfigurationEditorDoc::OnNewDocument()
    {
	    if (!CDocument::OnNewDocument())
		    return FALSE;

	    return TRUE;
    }


        


    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorDoc serialization

    void CConfigurationEditorDoc::Serialize(CArchive& ar)
    {
	    if (ar.IsStoring())
	    {
		    // TODO: add storing code here
	    }
	    else
	    {
		    // TODO: add loading code here
	    }
    }

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorDoc diagnostics

    #ifdef _DEBUG
    void CConfigurationEditorDoc::AssertValid() const
    {
	    CDocument::AssertValid();
    }

    void CConfigurationEditorDoc::Dump(CDumpContext& dc) const
    {
	    CDocument::Dump(dc);
    }
    #endif //_DEBUG

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorDoc commands

}
