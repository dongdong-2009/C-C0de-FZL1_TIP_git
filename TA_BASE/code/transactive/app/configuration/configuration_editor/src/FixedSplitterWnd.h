/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/FixedSplitterWnd.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This inherits from CSplitterWnd and implements a splitter that cannot be resized.
  */

#if !defined(AFX_FIXEDSPLITTERWND_H__3AFB8654_A989_4CC3_B993_C5096E29D524__INCLUDED_)
#define AFX_FIXEDSPLITTERWND_H__3AFB8654_A989_4CC3_B993_C5096E29D524__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    class CFixedSplitterWnd : public CSplitterWnd
    {
    public:
        /**
          * Constructor
          */
	    CFixedSplitterWnd();


        /**
          * Destructor
          */
	    virtual ~CFixedSplitterWnd();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CFixedSplitterWnd)
	    //}}AFX_VIRTUAL

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CFixedSplitterWnd)
	    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	    //}}AFX_MSG
        afx_msg void OnInvertTracker(const CRect& rect);
	    DECLARE_MESSAGE_MAP()

    private:
        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        CFixedSplitterWnd( const CFixedSplitterWnd&){ };


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        CFixedSplitterWnd& operator=(const CFixedSplitterWnd&);

    };

}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXEDSPLITTERWND_H__3AFB8654_A989_4CC3_B993_C5096E29D524__INCLUDED_)
