/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/FlashIconButton.h $
  * @author:  J. Chevalley
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class (FlashIconButton) combines standard CButton behaviour with the ability to flash the buttons icon
  * 
  */

#if !defined(AFX_FlashIconButton_H__E025A080_7617_4E81_9FA6_B4138EEF1451__INCLUDED_)
#define AFX_FlashIconButton_H__E025A080_7617_4E81_9FA6_B4138EEF1451__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
    class Tabs; // Forward declaration
    struct BannerIcon;

    class FlashIconButton : public CStatic
    {

    public:

	    /**
	      * FlashButton CTOR
	      *
	      * Provides the following defaults:
	      *		SetFlashPeriod(500),  // default to 1/2 second
	      *
	      */
        FlashIconButton(const TA_Base_Bus::BannerIcon& bannerIcon, int buttonId);

	    /**
	      * FlashButton DTOR
	      *
	      */
	    virtual ~FlashIconButton();

    // Implementation
	    
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
	      * ToggleFlash
	      * 
	      * toggles the flash state between flashing or not flashing.
	      *
	      */
	    void ToggleFlash(void);


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

	    void setCheck(bool checkIt);

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(FlashIconButton)
	    afx_msg void OnTimer(UINT nIDEvent);
	    afx_msg void OnDestroy();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
	    bool GetNextFlashState(void);

    private:
	    bool m_bFlashing;
	    bool m_currentFlashStateOn;
	    unsigned long m_flashPeriodMillisec;
	    UINT m_flashTimerID;
	    HBITMAP m_onImage;
	    HBITMAP m_offImage;
	    HBITMAP m_offDownImage;
	    HBITMAP m_onDownImage;
	    int  m_buttonId;
	    //See class comments as to why I am not using the standard CButton "checked" functionality	
	    bool m_isChecked;
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}
#endif // !defined(AFX_FlashIconButton_H__E025A080_7617_4E81_9FA6_B4138EEF1451__INCLUDED_)
