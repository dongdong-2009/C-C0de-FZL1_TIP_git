/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/pages/iscs_page/src/AlarmsGuiDlg.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/21 11:36:18 $
  * Last modified by:  $Author: lu.xiangmei $
  *
  */
// AlarmsGuiDlg.h : header file
//

#if !defined(AFX_ALARMSGUIDLG_H__F9076B67_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_)
#define AFX_ALARMSGUIDLG_H__F9076B67_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

#include "app/system_control/banner/pages/iscs_page/src/IscsBannerPage_rc.h"

#include "bus/alarm/alarm_list_control/src/AlarmListCtrl.h"
#include "bus/alarm/alarm_list_control/src/IAlarmListObserver.h"
#include "bus/banner_framework/src/BannerPage.h"
#include "bus/banner_framework/src/SilenceButton.h"
#include "bus/generic_gui/src/ISystemStateUpdateObserver.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Bus
{
    // Forward declaration
    class SettingsMgr;
    class SoundManagerProxy;
    class SystemControllerGroupUpdateSubscriber;
}


/////////////////////////////////////////////////////////////////////////////
// CAlarmsGuiDlg dialog

namespace TA_Base_App
{
    class CAlarmsGuiDlg : public TA_Base_Bus::BannerPage,
                          public TA_Base_Bus::IAlarmListObserver, 
                          public TA_Base_Bus::ISystemStateUpdateObserver,
                          public TA_Base_Core::RunParamUser
    {
    public:
        // The name of the Alarm sound category.
        static const std::string ALARM_CATEGORY;
        
        /**
         * Constructor
         */
        CAlarmsGuiDlg::CAlarmsGuiDlg(const std::string& buttonName, int xPos, int yPos, int width, int height,
                                     TA_Base_Bus::BannerFrameworkDialog* parentDialog);

        /**
         * Destructor
         */
        CAlarmsGuiDlg::~CAlarmsGuiDlg();

        /**
         * onButtonDoubleClick
         *
         * This method handles the event when the ISCS button is double clicked. This will
         * launch the Alarm Manager.
         */
        virtual void onButtonDoubleClick();
        
        /**
         * addBannerPage
         *
         * Tells the banner page to add itself to the banner.
         */
        virtual void addBannerPage();

        void onRunParamChange(const std::string& name, const std::string& value);
        
        /**
         * notifySystemStateDegraded
         *
         * This is called when the system is in degraded mode.
         *
         */
        virtual void notifySystemStateDegraded();

        /**
         * notifySystemStateNormal
         *
         * This is called when the system has recovered from degraded mode 
         * (ie. system is running normally).
         *
         */
        virtual void notifySystemStateNormal();
        
        // Dialog Data
        //{{AFX_DATA(CAlarmsGuiDlg)
        enum { IDD = IDD_ISCS_ALARMS };
        TA_Base_Bus::SilenceButton	m_silenceButton;
        CString m_numNotAckedString;
        CString m_numTotalString;
        //}}AFX_DATA
        
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CAlarmsGuiDlg)
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

    protected:
        // Generated message map functions
        //{{AFX_MSG(CAlarmsGuiDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
        afx_msg void OnClose();
        afx_msg LRESULT OnUpdateAlarms(WPARAM, LPARAM);
        afx_msg void OnOK();
        afx_msg void OnCancel();
        afx_msg void OnDestroy();
        afx_msg void OnSilence();
        afx_msg LRESULT  OnSystemInDegradedMode(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT  OnSystemInNormalMode(WPARAM wParam, LPARAM lParam);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        /**
	     * initListControlActions
	     *
	     * Initialises the alarm list control actions.
	     *
	     * @return A vector of alarm actions.
	     */
	    std::vector<TA_Base_Bus::IAlarmAction*> initListControlActions();

	    /**
	     * initListControlColumns
	     *
	     * Initialises the alarm list control columns.
	     */
	    void initListControlColumns();

        /** 
          * initPreFiltering
          *
          * Creates a Filter object and a vector of locations based on
	      * the active session, so that they can be passed to the alarm
	      * list control.
          *
          * @param preFilter  The pre-filter to be applied.
          * @param locations  The locations we wish to display alarms from.
          *
          */
	    void initPreFiltering(TA_Base_Bus::Filter& preFilter, std::vector<unsigned long>& locations);

	    /**
         * updateAlarmCounts
         *
         * This method update the alarm counters on the container class.
	     * 
	     * @param The total number of alarms.
	     *
	     * @param The total number of unacknowledged alarms.
         */
	    virtual void updateAlarmCounts(unsigned int totAlarms, unsigned int totUnacknowledgedAlarms, bool exceededMaxDisplayAlarm, bool isLoadingAlarms);

        /**
         * updateAlarmSound
         * 
         * The virtual method that updates the container's alarm severity tones.
         *
         * @param The severity tone to update as an unsigned long.
         * @param True if new alarms were inserted, and false otherwise.
         */
	    virtual void updateAlarmSound(unsigned long severity, bool wereAlarmsAdded);

	    virtual CProgressCtrl* getProgressBar();

        virtual void finishedWithProgressBar();

        /**
	     * notifyNoMatchingAlarms
         * 
         * This will be called if no alarms match the current filter. 
         */
        virtual void notifyNoMatchingAlarms();

		/**
		 * notifyFilterIndexReset
		 *
		 * This will be called if alarmlist control is notified of by alarm store to unmap 
		 * so filter will be set to default
		 */
		virtual void notifyFilterReset();

        /**
         * updateAvailableAlarmActions
         *
         * This will pass in the current availability of each alarm action. This will be called whenever
         * the alarm selection changes or the current alarms change.
         *
         * @param ItemAvailability - This should pass the availability of every possible alarm action currently
         *                           being used.
         */
        virtual void updateAvailableAlarmActions(const ItemAvailability& availability);

	    /**
         * setCurrentDescriptionText
         *
         * This will pass some appropriate text to be shown by the observer in a status bar or the like.
         * This string will hold the description of the selected alarm. If multiple alarms are selected it
         * will indicate how many alarms are selected
         *
         * @param CString - The text to be displayed
         */
        virtual void setCurrentDescriptionText(const CString& description);

	    /**
         * setWindowTitle
         *
         * This method tells the alarm container class to update its window's title bar if any.
	     * 
	     * @param The text to be displayed.
         */
	    virtual void setWindowTitle(const std::string& windowText);

	    /**
  	     * getSessionID
	     * 
	     * The virtual method that retrieves the current session ID.
	     *
	     * @param The new session ID to return
	     */
	    virtual std::string getSessionID();

        /**
	     * updatePreFilterAccordingToSystemState
	     *
	     * Clears the pre-filter if the system is in degraded mode.
	     * 
	     * @param preFilter - Reference to the pre-filter that is to be changed.
	     */
        void updatePreFilterAccordingToSystemState(TA_Base_Bus::Filter& preFilter);

    private:	    
	    static const std::string RPARAM_USE_ALARM_ICON; //TD15156++

        bool m_unmuteOnNewAlarm;
        std::vector<TA_Base_Bus::IAlarmColumn*> m_alarmColumns;
        TA_Base_Bus::SettingsMgr* m_ptrSettingsMgr;
        TA_Base_Bus::SoundManagerProxy* m_soundInterface;
        int	m_numAlarmsNotAcked;
        int	m_numAlarmsTotal;
        bool m_exceededMaxDisplayAlarm;
        TA_Base_Bus::AlarmListCtrl* m_alarmList;
        TA_Base_Bus::SystemControllerGroupUpdateSubscriber* m_groupUpdateSubscriber;
        TA_Base_Bus::FixedHeaderCtrl m_headerCtrl;

    protected:
        HICON m_hIcon;
        HFONT m_boldFont;
        static const int ALARM_SOUND_TIMER;
        std::string m_sessionId;
        HACCEL  m_hAccel;
        CString m_SoundFilePath;
		CFont m_alarmfont;
    };
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMSGUIDLG_H__F9076B67_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_)
