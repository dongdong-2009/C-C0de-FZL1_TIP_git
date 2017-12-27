/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the default view linked to the document. This will not be used and will be left
  * blank.
  */


#if !defined(AFX_CONFIGURATION_EDITORVIEW_H__880DE812_3F92_49CF_88F5_B0449BE82537__INCLUDED_)
#define AFX_CONFIGURATION_EDITORVIEW_H__880DE812_3F92_49CF_88F5_B0449BE82537__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    // Forward Declarations
    class CConfigurationEditorDoc;


    class CConfigurationEditorView : public CView
    {
    protected:

        /**
          * Constructor
          *
          * create from serialization only
          */
	    CConfigurationEditorView();


	    DECLARE_DYNCREATE(CConfigurationEditorView)

    public:

        /**
          * Destructor
          */
        virtual ~CConfigurationEditorView();


        /**
          * GetDocument
          *
          * Retrieves the document associated with this view
          *
          * @return CConfigurationEditorDoc*
          */
	    CConfigurationEditorDoc* GetDocument();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CConfigurationEditorView)
	    public:
	    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	    protected:
	    //}}AFX_VIRTUAL

    // Implementation
    public:
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

    // Generated message map functions
    protected:
	    //{{AFX_MSG(CConfigurationEditorView)
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        CConfigurationEditorView( const CConfigurationEditorView&){ };


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        CConfigurationEditorView& operator=(const CConfigurationEditorView&);

    };


#ifndef _DEBUG  // debug version in ConfigurationEditorView.cpp
    inline CConfigurationEditorDoc* CConfigurationEditorView::GetDocument()
       { return (CConfigurationEditorDoc*)m_pDocument; }
#endif

}


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURATION_EDITORVIEW_H__880DE812_3F92_49CF_88F5_B0449BE82537__INCLUDED_)
