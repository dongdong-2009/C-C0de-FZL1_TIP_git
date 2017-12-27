/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ButtonListCtrl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class extends a list control. It is a list control where each of the icons
  * is a button. Each of those buttons is linked to an application that can be
  * launched by the user.
  * 
  * This list control determines which applications should be showing and adds them
  * to the list. It also performs the launching of the application when clicked. It
  * contains a lot of the drawing code for ensuring the buttons look like real buttons.
  */


#if !defined(AFX_BUTTONLISTCTRL_H__B37E12AA_D2B3_408E_97B8_842F7CB96AF5__INCLUDED_)
#define AFX_BUTTONLISTCTRL_H__B37E12AA_D2B3_408E_97B8_842F7CB96AF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CButtonListCtrl window

class Application;

class CButtonListCtrl : public CListCtrl
{

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

    /**
    * Constructor
    */
	CButtonListCtrl();

    /**
    * Destructor
    */
    virtual ~CButtonListCtrl();


	/**
	 * reset
	 *
	 * This will ensure that the list control re-populates the control as
     * the applications available may have changed. This basically just calls
     * loadApps() so provides the same functionality but through a public interface
	 */
    void reset();


	// Generated message map functions
protected:
	//{{AFX_MSG(CButtonListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnGetInfoTipLaunchList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	/**
	 * insertAppItem
	 *
	 * Insert the app item in the CListCtrl & store it in the ListCtrl data member
	 *
	 * @param Application* - the application to store
	 */
	void insertAppItem(Application* pItem);


	/**
	 * getApplication
	 *
	 * Return the Application data associated with the 
	 * item data in the list ctrl
	 *
	 * @return Application* - the application, NULL if not found
	 */
	Application* getApplication(int index);


    /**
     * offsetColor
     *
     * This function applies an offset to the RGB components of the specified color.
     * This function can be seen as an easy way to make a color darker or lighter than
     * its default value.
     *
     * @param color The color to change
     * @param shOffset A short value indicating the offset to apply to the color.
     *                 This value must be between -255 and 255.
     *
     * @return DWORD The newly converted colour. If the offset passed is invalid
     *               then the same colour will be returned.
     */
    DWORD offsetColor(DWORD color, short shOffset);

    static int CALLBACK compareApp(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	/**
	 * loadApps
	 *
	 * This will retrieve the list of applications from the ProcessManager and repopulate
     * the list control.
     */
	void loadApps();


    // This is the maximum number of items that can fit in the list control before it
    // needs to be expanded,
    static const int MAXIMUM_APPLICATIONS;
    static const int ICON_SPACING;
    static const int ICON_SIZE;
    static const int NUM_COLUMN;
    static const int NUM_ROW;


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONLISTCTRL_H__B37E12AA_D2B3_408E_97B8_842F7CB96AF5__INCLUDED_)
