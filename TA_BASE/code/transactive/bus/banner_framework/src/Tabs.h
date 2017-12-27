/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/Tabs.h $
  * @author:  C. DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * The class, Tabs, provides a window in which the "tabs" for the main window are displayed.
  * The main window (BannerDlg) uses a framework provided by TStackedDialog and related classes,
  * to provide functionality similar to a tabbed dialog. 
  *
  * Only one of the pages is displayed at a time - like a CPropertyPage.
  * The tabs are required to always be visble. Hence, a seperate window (Tabs) is created
  * to contain the tabs - the Tab window parent is set to the desktop so that its Z-order can be 
  * set to always-on-top. Note that this would not work if the tab window was a child of the BannerDlg
  *
  * The Tabs class provides a mechanism to flash a selected tab(s). This is intended as a notification
  * to the user that there is something in the related page which they should address.
  * Each page provides additional functionality related to what it displays
  *
  */

#if !defined(AFX_TABS1_H__F03CC5FF_9E77_4B04_8879_90CFC0C23715__INCLUDED_)
#define AFX_TABS1_H__F03CC5FF_9E77_4B04_8879_90CFC0C23715__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include "bus/banner_framework/src/PrintButton.h"

namespace TA_Base_Bus
{
    class BannerFrameworkDialog;
    class FlashIconButton;
    struct BannerIcon;

    class Tabs : public CWnd  
    {
    public:
	    //does not create HWND - must call create()
	    Tabs(int width, int height, BannerFrameworkDialog* pParent);
	    virtual ~Tabs();
	    //This will create the tabs HWND.
	    //It will NOT place or show the window. Call move() to place and show
	    //The size of the window is made just large enough to accomidate the buttons
        void create();

		void setHeight(int nHeight);

        /**
         * addButton
         *
         * Adds a button to the Tabs window. The button will appear in the next position below the last added button.
         *
         * @param pageName - the name of the page this button will be used for.
         * @param bannerIcon - the icon that the banner will display.
         *
         */
        void addButton(UINT buttonId, const BannerIcon& bannerIcon);

		//++Noel Print Screen support 
		void addPrintButton();

	    //Move and display the window
	    //pt is where (in screen coordinates) you want the topleft corner of this window to be moved to
	    void move(const CPoint& pt);

	    //Turn flashing on or off for the specified button
	    void flashButton(UINT buttonId, bool flash);

	    void selectButton(UINT buttonId);

        void onTabClicked(UINT ctrlId);

    protected:
        // Generated message map functions
	    //{{AFX_MSG(Tabs)
		afx_msg void OnPrint(); //++Noel Print Screen support
	    //}}AFX_MSG
	    void onTabDoubleClicked(UINT ctrlId);
	    DECLARE_MESSAGE_MAP()

    private:
	    BannerFrameworkDialog* m_bannerDlg;
        int m_width;
        int m_height;

        // A map of dialog IDs/page names to flashing buttons
        std::map< UINT, FlashIconButton* > m_buttons;
        std::map< UINT, UINT > m_buttonIDs;
    
        enum {ID_TAB1 = 101, ID_TAB2, ID_TAB3, ID_TAB4, ID_TAB5, ID_TAB6, ID_TAB7, ID_TAB8, ID_TAB9, ID_TAB10}; 

		//++Noel Print Screen Support
		PrintButton m_btnPrint;

    public:
        static const int BORDER;
		static const int BUTTON_VERTICAL_SPACING;
        static const int BUTTON_HEIGHT;
        static const int MAX_NUMBER_OF_BUTTONS;
	    static const int BUTTON_COLOUR_OFFSET_WHEN_HOT;
		static const std::string RPARAM_DISABLE_PRINT;
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_TABS1_H__F03CC5FF_9E77_4B04_8879_90CFC0C23715__INCLUDED_)
