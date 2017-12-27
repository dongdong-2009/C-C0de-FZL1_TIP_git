/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/flashing_button/FlashButton.h $
  * @author:  J. Chevalley
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class (FlashButton) combines standard CButton behaviour with the ability to flash the 
  * button's background. 
  * 
  * NOTE: The BS_OWNERDRAW button style used by this class does not seem to be compatible with 
  * the radio button styles (BS_AUTORADIOBUTTON and BS_RADIOBUTTON) - these styles should not be used. Nor
  * should the CButton methods GetCheck() and SetCheck() be used. They do not function correctly.
  * Instead, this class provides an alternate "check mechanism" (see isChecked(), etc). 
  * Note that checked buttons are drawn as "pressed".
  * This also means that the client of this class is responsible for checking and un-checking "grouped" buttons. 
  * The client should use the new checking methods and should implement "radiobutton" code similar to the following 
  * in the owner class of the "grouped" buttons:
  * 
  * 	void Tabs::onTabClicked(UINT tabId)
  * 	{
  *			//See FlashButton class comments as to why I am not using the standard CButton "checked" functionality	
  * 		m_iscsTab.setCheck( (tabId == ISCS) ? true : false );
  * 		m_atsTab.setCheck( (tabId == ISCS) ? true : false );
  * 		m_plansTab.setCheck( (tabId == ISCS) ? true : false );
  * 		m_callsTab.setCheck( (tabId == ISCS) ? true : false );
  *     }
  * 
  * This is handy if you need buttons that look like push buttons but provide radio button behaviour.
  */

#if !defined(AFX_FLASHBUTTON_H__E025A080_7617_4E81_9FA6_B4138EEF1451__INCLUDED_)
#define AFX_FLASHBUTTON_H__E025A080_7617_4E81_9FA6_B4138EEF1451__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS FlashButton : public CButton
{

public:

	/**
	  * FlashButton CTOR
	  *
	  * Provides the following defaults:
	  *		SetFlashPeriod(500),  // default to 1/2 second
	  *		SetFlashColour(RGB(255,0,0)), // default to red
	  *
	  */
	FlashButton();

	/**
	  * FlashButton DTOR
	  *
	  */
	virtual ~FlashButton();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FlashButton)
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
	  * is not used.	  *
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
	  * is not used.	  *
	  *
	  * @return  true if button is checked, false if it is not
	  *
	  */
	bool isChecked();


	/**
	  * ToggleFlash
	  * 
	  * toggles the flash state between flashing or not flashing.
	  *
	  */
	void ToggleFlash(void);


	/**
	  * SetFlashColour
	  * 
	  * Set the flash colour. Note that "flashing" is implimented by 
	  * toggling the background colour toggles between the colour set by this method
	  * and ::GetSysColor(COLOR_BTNFACE).
	  *
	  * @param rgbColor is the colour that the button flashes 
	  *
	  */
	virtual void SetFlashColour(COLORREF rgbColor);


	/**
	  * StartFlashing
	  * 
	  * Start the button flashing.
	  * Note that if it is already flashing this .
	  *
	  */
	virtual void StartFlashing(void);


	/**
	  * StopFlashing
	  * 
	  * Stop the button flashing.
	  */
	virtual void StopFlashing(void);


	/**
	  * SetFlashPeriod
	  * 
	  * Set the flash period, in milliseconds.
	  *
	  * @param flashPeriodMillisec is the flash period, in milliseconds.
	  *
	  */
	virtual void SetFlashPeriod(unsigned long flashPeriodMillisec);


	// Generated message map functions
protected:
	//{{AFX_MSG(FlashButton)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG



	DECLARE_MESSAGE_MAP()
private:
	COLORREF m_currentFlashColour;
	COLORREF GetNextFlashColour(void);
	COLORREF m_flashColour;
	UINT m_flashTimerID;
	bool m_bFlashing;
	unsigned long m_flashPeriodMillisec;

	//See class comments as to why I am not using the standard CButton "checked" functionality	
	bool m_isChecked;
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLASHBUTTON_H__E025A080_7617_4E81_9FA6_B4138EEF1451__INCLUDED_)
