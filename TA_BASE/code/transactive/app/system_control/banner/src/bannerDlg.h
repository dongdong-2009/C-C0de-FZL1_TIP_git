/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/src/bannerDlg.h $
  * @author:  C. DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * The class, BannerDlg, provides the "main window" for this application.
  * The app uses the GenericGUI framework, and  BannerDlg is a AbstractDialogGUI.
  * The BannerDlg uses a framework provided by TStackedDialog and related classes,
  * to provide functionality similar to a tabbed dialog. There are four tabbed pages,
  * which provide a list of:
  * 1) ISCS (Transactive) alarms
  * 2) ATS alarms
  * 3) Communications (to the operator)
  * 4) Plans - all plans currently running in the system
  *
  * Only one of the above pages is displayed at a time - like a CPropertyPage
  * To the left of the displayed page there are four buttons, ISCS, ATS, Plans, and Calls
  * These buttons act as the "tabs"
  * It should be noted that these tabs are required to always be visble. Hence, a seperate window
  * is used to contains the tabs - its parent is set to the desktop so that its Z-order can be 
  * set to always-on-top. Note that this would not work if the tabs were a child of the BannerDlg
  *
  * The BannerDlg provides a mechanism to flash a selected tab(s). This is intended as a notification
  * to the user that there is something in the related page which they should address.
  * Each page provides additional functionality related to what it displays
  *
  */

#if !defined(AFX_BANNERDLG_H__2190A8E3_4F74_44CB_84B5_A70F391EE691__INCLUDED_)
#define AFX_BANNERDLG_H__2190A8E3_4F74_44CB_84B5_A70F391EE691__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app\system_control\banner\src\ComponentLibraryAccessor.h"
#include "bus\banner_framework\src\BannerFrameworkDialog.h"
#include "core\utilities\src\RunParams.h"


namespace TA_Base_App
{

    class BannerGUI;

    class BannerDlgImpl : public TA_Base_Bus::BannerFrameworkDialog
    {
    public:

        BannerDlgImpl(BannerGUI& gui, CWnd* parent = NULL);
//		void AddButton(UINT buttonId, const BannerIcon& bannerIcon){}
	    
       // Dialog Data
	    //{{AFX_DATA(BannerDlg)
	    enum { IDD = IDD_BANNER_DIALOG };
	    // NOTE: the ClassWizard will add data members here
	    //}}AFX_DATA
		//TD18095, jianghp, to fix the performance of showing manager application
		void init(int xPos, int yPos, int width, int height, const std::string& startTab);
    protected:
	    HICON m_hIcon;

	    // Generated message map functions
	    //{{AFX_MSG(BannerDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnClose();
	    afx_msg void OnDestroy();
	    //}}AFX_MSG
        afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	    DECLARE_MESSAGE_MAP()

        void sessionIdChanged(const std::string& sessionId);
        
    private:
	    
        void setBannerPage();
	    
        LRESULT setFlashState(WPARAM wParam, LPARAM lParam);
        
	    // The tab that is initialy selected
	    std::string m_startTab;

        static const int TAB_WIDTH;
        static const ULONG PROFILE_LOCAL;
    
        static const char* LEFT_BANNER;
        static const char* CENTRE_BANNER;
        static const char* RIGHT_BANNER;

        static const char* LEFT_BANNER_DEFAULT_STR;
        static const char* CENTRE_BANNER_DEFAULT_STR;
        static const char* RIGHT_BANNER_DEFAULT_STR;

        // The component that performs DLL related tasks
        ComponentLibraryAccessor m_componentLibraryAccessor;

        std::string m_sessionId;

        int m_pageWidth;
        int m_pageHeight;
        int m_pageXPos;
        int m_pageYPos;

		// Remove the unsigned to initialize variable to -1
        long m_currentProfileId;
    };


    //
    // This is necessary because a compilation error occurred for ON_REGISTERED_MESSAGE
    // when multiple inheritance is used. It appears that the casting of method pointers 
    // (for ON_REGISTERED_MESSAGE) seems to change when multiple inheritance is used.
    // 
    class BannerDlg : public BannerDlgImpl, public TA_Base_Core::RunParamUser
    {
    public:

        BannerDlg(BannerGUI& gui, CWnd* parent = NULL);

        virtual void onRunParamChange(const std::string& name, const std::string& value);

    };

} // namespace TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BANNERDLG_H__2190A8E3_4F74_44CB_84B5_A70F391EE691__INCLUDED_)
