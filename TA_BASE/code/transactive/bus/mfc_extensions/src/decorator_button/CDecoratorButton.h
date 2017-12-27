/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/decorator_button/CDecoratorButton.h $
 * @author:  F. Stuart
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * MFC extension of CButton. This class will support a decorator pattern.
 *
 * NOTE: The BS_OWNERDRAW button style used by this class does not seem to be compatible with 
 * the radio button styles (BS_AUTORADIOBUTTON and BS_RADIOBUTTON) - these styles should not be used. Nor
 * should the CButton methods GetCheck() and SetCheck() be used. They do not function correctly.
 * Instead, this class provides an alternate "check mechanism" (see isChecked(), etc). 
 * Note that checked buttons are drawn as "pressed".
 * This also means that the client of this class is responsible for checking and un-checking "grouped" buttons. 
 */

#if !defined(AFX_DECORATORBUTTON_H__B2902216_E715_4ECA_ABDF_B198F29DE4BE__INCLUDED_)
#define AFX_DECORATORBUTTON_H__B2902216_E715_4ECA_ABDF_B198F29DE4BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecoratorButton.h : header file
//

#include "bus/mfc_extensions/src/decorator_button/IDecoratorButtonBehaviourStrategy.h"
#include "bus/mfc_extensions/src/decorator_button/AbstractDecoratorButton.h"

#include <afxwin.h>         // MFC core and standard components

/////////////////////////////////////////////////////////////////////////////
// CDecoratorButton window

#ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) CDecoratorButton : public CButton, public TA_Base_Bus::AbstractDecoratorButton
#else
    class AFX_EXT_CLASS CDecoratorButton : public CButton, public TA_Base_Bus::AbstractDecoratorButton
#endif
    {
    public:

	    /**
	      * Constructor
	      * 
	      */
        CDecoratorButton();

	    virtual void Draw(LPDRAWITEMSTRUCT lpDrawItemStruct);

	    virtual void setFinalDecorator(TA_Base_Bus::AbstractDecoratorButton* pFinalDec);
    
        void setButtonBehaviorStrategy(TA_Base_Bus::IDecoratorButtonBehaviourStrategy* pStrategy);

	    /**
	      * setCheck
	      * 
	      * Checks or un-checks the button.
	      * Checked buttons are drawn as "pressed"
	      * This method is NOT an override of the CButton SetCheck(int) method.
	      * See class description above for info on why standard CButton check functionality
	      * is not used.	  *
	      *
	      * @param checkIt specifies if the button is to be checked or un-checked
	      *
	      */
	    virtual void setCheck(bool checkIt);

	    /**
	      * isChecked
	      * 
	      * Reports if the button is checked or un-checked.
	      * This method should be used instead of the  CButton GetCheck() method.
	      * See class description above for info on why standard CButton check functionality
	      * is not used.
	      *
	      * @return  true if button is checked, false if it is not
	      *
	      */
	    bool isChecked();

    // Attributes
    public:

    // Operations
    public:

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CDecoratorButton)
	    public:
	    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	    //}}AFX_VIRTUAL

    // Implementation
    public:
	    virtual ~CDecoratorButton();

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CDecoratorButton)
	    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	    afx_msg void OnDropFiles(HDROP hDropInfo);
	    virtual void PreSubclassWindow();
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()


    private:

        /**
	      * Callback object for the button behavior
          */
	    TA_Base_Bus::IDecoratorButtonBehaviourStrategy* m_pDecoratorButtonBehaviourStrategy;

    private:
        bool m_isChecked;	

    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECORATORBUTTON_H__B2902216_E715_4ECA_ABDF_B198F29DE4BE__INCLUDED_)
