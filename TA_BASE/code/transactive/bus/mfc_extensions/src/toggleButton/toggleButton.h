/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/toggleButton/toggleButton.h $
  * @author:  C. DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class (ToggleButton) combines standard CButton behaviour with the ability to set the 
  * button's background when it is pressed in. This should be useful if you want a toggle button
  * that maintains its pressed in (or not) state after you release the mouse button, and that is a
  * different colour when pressed in.
  * 
  * NOTE: The BS_OWNERDRAW button style used by this class does not seem to be compatible with 
  * the check box button styles (BS_AUTOCHECKBOX  and BS_CHECKBOX) - these styles should not be used. Nor
  * should the CButton methods GetCheck() and SetCheck() be used. They do not function correctly.
  * Instead, this class provides an alternate "check mechanism" (see isChecked(), etc). 
  * Note that checked buttons are drawn as "pressed".
  * This also means that the client of this class is responsible for checking and un-checking the button. 
  * The client should use the new checking methods and should implement "check box" code similar to the following 
  * in the owner class of the button:
  * 
  * 	void Tabs::onBtnClicked()
  * 	{
  *			//See ToggleButton class comments as to why I am not using the standard CButton "checked" functionality	
  * 		m_iscsTButton.setCheck( (tabId == ISCS) ? true : false );
  *     }
  * 
  * This is handy if you need buttons that look like push buttons but provide check box behaviour.
  */

#if !defined(AFX_ToggleButton_H__E025A080_7617_4E81_9FA6_B4138EEF1451__INCLUDED_)
#define AFX_ToggleButton_H__E025A080_7617_4E81_9FA6_B4138EEF1451__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Make this work in a DLL
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) ToggleButton : public CButton
#else
class AFX_EXT_CLASS ToggleButton : public CButton
#endif
{

public:

	/**
	  * ToggleButton CTOR
	  *
	  * Provides the following defaults:
	  *		SetCheckedColour(RGB(255,0,0)), // default to red
	  *
	  */
	ToggleButton();

	/**
	  * ToggleButton DTOR
	  *
	  */
	virtual ~ToggleButton();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ToggleButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	/**
	  * setCheck
	  * 
	  * Checks or un-checks the button.
	  * Checked buttons are drawn as "pressed"
	  * This method is NOT an override of the CButton SetCheck(int) method.
	  * See class description above for info on why standard CButton check functionality
	  * is not used.	  
	  *
	  * @param checkIt specifies if the button is to be checked or un-checked
	  *
	  */
	void setCheck(bool checkIt);


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


	/**
	  * SetCheckedColour
	  * 
	  * Set the colour of the button background when checked.The unpressed 
	  * colour is set to ::GetSysColor(COLOR_BTNFACE).
	  *
	  * @param rgbColor is the colour colour of the button background when checked
	  *
	  */
	virtual void SetCheckedColour(COLORREF rgbColor);
	virtual void SetUnCheckedColour(COLORREF rgbColor);
	virtual void SetBackgroundColour(COLORREF rgbColor);


	// Generated message map functions
protected:
	//{{AFX_MSG(ToggleButton)
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnDestroy();
	//}}AFX_MSG



	DECLARE_MESSAGE_MAP()
private:
	COLORREF m_unCheckedColour;
	COLORREF m_checkedColour;

	//See class comments as to why I am not using the standard CButton "checked" functionality	
	bool m_isChecked;
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ToggleButton_H__E025A080_7617_4E81_9FA6_B4138EEF1451__INCLUDED_)
