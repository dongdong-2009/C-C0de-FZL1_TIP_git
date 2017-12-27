/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorDoc.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main document of the framework. We will not use the document extensively as we
  * are not creating new documents and saving etc. This is really here to complete the framework.
  */

#if !defined(AFX_CONFIGURATION_EDITORDOC_H__D0E5B0FF_ECF2_4F0F_AC0E_F01265E93D30__INCLUDED_)
#define AFX_CONFIGURATION_EDITORDOC_H__D0E5B0FF_ECF2_4F0F_AC0E_F01265E93D30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    class CConfigurationEditorDoc : public CDocument
    {
    protected:

        /**
          * Constructor
          *
          * Create from serialization only
          */
	    CConfigurationEditorDoc();

	    DECLARE_DYNCREATE(CConfigurationEditorDoc)

    // Operations
    public:

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CConfigurationEditorDoc)
	    public:
	    virtual BOOL OnNewDocument();
	    virtual void Serialize(CArchive& ar);
	    //}}AFX_VIRTUAL

    // Implementation
    public:
        /**
          * Destructor
          */
        virtual ~CConfigurationEditorDoc();


    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

    // Generated message map functions
    protected:
	    //{{AFX_MSG(CConfigurationEditorDoc)
		    // NOTE - the ClassWizard will add and remove member functions here.
		    //    DO NOT EDIT what you see in these blocks of generated code !
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        CConfigurationEditorDoc( const CConfigurationEditorDoc&){ };


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        CConfigurationEditorDoc& operator=(const CConfigurationEditorDoc&);

    };
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURATION_EDITORDOC_H__D0E5B0FF_ECF2_4F0F_AC0E_F01265E93D30__INCLUDED_)
