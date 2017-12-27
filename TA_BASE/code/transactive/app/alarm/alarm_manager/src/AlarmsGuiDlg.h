/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/AlarmsGuiDlg.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/06/29 14:25:09 $
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


#include <set>

#include "app\alarm\alarm_manager\src\AlarmFilterDlg.h"
#include "bus\alarm\alarm_list_control\src\AlarmListCtrl.h"
#include "bus\alarm\alarm_list_control\src\ListHeaderCtrl.h"

#include "bus\generic_gui\src\IGUIAccess.h"
#include "bus\user_settings\src\SettingsMgr.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/generic_gui/src/ISystemStateUpdateObserver.h"

#include "core\utilities\src\RunParams.h"
#include "core\synchronisation\src\ReEntrantThreadLockable.h"

#include "core\alarm\IDL\src\AlarmAgentCorbaDef.h"
#include "core\alarm\IDL\src\AlarmMessageCorbaDef.h"

#include "bus\alarm\alarm_list_control\src\IAlarmListObserver.h"
#include "bus\alarm\alarm_list_control\src\Filter.h"

#include "bus/mfc_extensions/src/combo_temporary_item/ComboTempItem.h"


namespace TA_Base_Bus
{
    // Forward declaration
    class SystemControllerGroupUpdateSubscriber;
}


/////////////////////////////////////////////////////////////////////////////
// CAlarmsGuiDlg dialog

class CAlarmsGuiDlg :
    public TA_Base_Bus::TransActiveDialog,
	public TA_Base_Core::RunParamUser, 
    public TA_Base_Bus::IAlarmListObserver,
    public TA_Base_Bus::ISystemStateUpdateObserver
{
// Construction
public:
 
  /**
    * Constructor
    * 
    * This constructs an instance of CAlarmsGuiDlg.
    * Preconditions  - IGUIApplication pointer passed in is not null
    *
    * @param IGUIAccess* controlClass - This is a pointer to a constructed GUI Acess
    *                                   object for GenericGUI to call.
    *
    * @exception AlarmViewerException - Thrown if there are erros with the Sound Manager.
    */
	CAlarmsGuiDlg(TA_Base_Bus::IGUIAccess* controlClass);
	~CAlarmsGuiDlg();

	// Dialog Data
    //{{AFX_DATA(CAlarmsGuiDlg)
	enum { IDD = IDD_ALARMSGUI_DIALOG };
	CStatic	m_MenuLine;
	CStatic m_currentFilterText;
	TA_Base_Bus::ComboTempItem m_currentFilterListCtrl;
	CStatic m_totalAlarms;
	CStatic m_totalUnack;
	CEdit   m_totalAlarmsView;
	CEdit   m_totalUnackEdit;
    CProgressCtrl    m_progress;
    CStatic    m_statusBar;
    CWnd m_Pager;
	CString m_totAlarms;
	UINT	m_totIntAlarms;
	UINT	m_totUnacknowledged;
	//}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAlarmsGuiDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

public:
	//TD18095, jianghp
	void init();
	//TD18095, jianghp
  /**
	* serverDown
	*
	* This implements the virtual function from AlarmViewerGUI. It 
	* handles the situation in an event when the main server goes down.
    */
	void serverDown();

  /**
	* serverUp
	*
	* This implements the virtual function from AlarmViewerGUI. It 
	* handles the situation in an event when the main server 
	* reestablishes comms. 
    */
    void serverUp();

  /**
	* onRunParamChange
	*
	* This implements the virtual function from AlarmViewerGUI. It 
	* handles the situation in an event when the main server 
	* reestablishes comms. 
	*
	* @param The RunParam name.
	*
	* @param The new RunParam value.
    */
    void onRunParamChange(const std::string& name, const std::string& value);

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

	/**
	 * getControlGUIAccess
	 *
	 */
	const TA_Base_Bus::IGUIAccess* getControlGUIAccess() { return m_controlClass; };

protected:

    HICON m_hIcon;
	
	CFont m_alarmfont;
    // Generated message map functions
    //{{AFX_MSG(CAlarmsGuiDlg)
    virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y );
    afx_msg void OnClose();
    afx_msg LRESULT OnChangeMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPrint();
	afx_msg void OnExport();
	afx_msg void OnAppAbout();
//	afx_msg void OnTimer(UINT nIDEvent);  //TD17081
	afx_msg void OnViewShowFilters();
	afx_msg void OnAlarmAcknowledge();
	afx_msg void OnAlarmAllAcknowledge();
	afx_msg void OnAlarmClose();
	afx_msg void OnAlarmDisplayComment();
	afx_msg void OnAlarmSchematic();
	afx_msg void OnAlarmInspector();
	afx_msg void OnAlarmPlan();
	afx_msg void OnAlarmPlanInstance();
	//TD 13825 Add postpone dss menu command
	afx_msg void OnAlarmCancelPlan();
	afx_msg void OnAlarmTrend();
	afx_msg void OnRelatedAvalancheEvents();
	afx_msg void OnAlarmJobRequest();
	afx_msg void OnAlarmEventLog();
	afx_msg void OnAlarmEvents();
	//TD 11783 Event Viewer button is always enabled via toolbar, and will launch EventViewer
	afx_msg void OnEventsViewer();
	afx_msg void OnViewStatusBar();
	afx_msg void OnViewToolbar();
	afx_msg void OnDefaultSort();
    afx_msg void OnViewInhibitAlarmByDescription();
    afx_msg void OnViewInhibitAlarmByType();
    afx_msg void OnViewInhibitAlarms();
    afx_msg void OnViewInhibitions();
	afx_msg void OnSelchangeNamedFilter();
    afx_msg LRESULT  OnFilterApply(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT  OnFilterDelete(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT  OnSystemInDegradedMode(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT  OnSystemInNormalMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	 OnFilterUpdate(WPARAM wParam, LPARAM lParam);
	//TD18095,jianghp
	afx_msg void OnAppExit();
	//TD18095,jianghp
	//[[add by wuzhongyi for popup comment dialog 
	afx_msg void OnClickAlarmList(NMHDR* pNMHDR, LRESULT* pResult);
	//]]
	//}}AFX_MSG
    //afx_msg void OnItemSelectAlarmList( NMHDR* pNMHDR, LRESULT* pResult );
    //afx_msg void OnChkChangeAlarmsList();
    DECLARE_MESSAGE_MAP()

	
	typedef std::map<std::string, TA_Base_Core::AlarmMessageCorbaDef> t_alarmsMap;
    typedef std::map<unsigned long, CString> t_LUT;
	

    BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

    HFONT m_boldFont;

	static const int ALARM_DATA_INITIALIZATION_TIMER;  //TD17081
	
	static const int UPDATE_TIMER;


    static const int ALARM_SOUND_TIMER;
    static const int FLASH_ACK_TIMER; 
    static const CString ALARM_INHIBIT;
    bool m_startSound;

    CImageList        m_alarmsTbImageListEnabled;
    CImageList        m_alarmsTbImageListHot;
    CImageList        m_commentsImageList;
    CToolBarCtrl	  m_alarmsToolbar;
    CMenu             m_monitorMenu;
	TA_Base_Bus::ListHeaderCtrl   m_listHeader;
	std::string       m_sessionId;


	HACCEL  m_hAccel;

    CPoint m_selectedPoint;

	/*
     * PopulateAlarmToolbar
     *
     * Populates the Alarm tool bar with its associated button functionality. 
     * Disables all buttons and menus as SetButtonStates will enable these 
     * functions accordingly. 
     */
    void PopulateAlarmToolbar();

	/*
     * Print
     *
     * Enables the alarm list to be printed. This laucnhes a standard MS 
     * printing dialog. 
     */
    void Print();

	/*
     * Save
     *
     * Enables the alarm list to be saved as .CSV file. This laucnhes a standard MS 
     * Save As dialog.
     */
    void Save();

	/*
     * DisableButtons
     *
     * This method disables menus, buttons on the dialog. 
     */
    void DisableButtons();


private:
 
 	static DWORD WINAPI	ThreadFunction(LPVOID lpThreadParameter);
	DWORD m_thread_id;
	HANDLE m_thread_hd;//reduce init time

	//Disable the copy constructor and assignment operator so the 
	//compiler won't create its own version. 
	CAlarmsGuiDlg(const CAlarmsGuiDlg& theAlarmsGuiDlg);
	CAlarmsGuiDlg& operator=(const CAlarmsGuiDlg &);

	TA_Base_Bus::AlarmListCtrl m_alarmList;
	TA_Base_App::AlarmFilterDlg* m_alarmFilterDlg;
	std::vector<TA_Base_Bus::IAlarmColumn*> m_alarmColumns;

	TA_Base_Bus::IGUIAccess* m_controlClass;
	TA_Base_Bus::SettingsMgr* m_ptrSettingsMgr;

	bool m_isToolBarVisible;
	bool m_hasAlarmListInit;
	bool m_exceededMaxDisplayAlarm;
	bool m_isLoadingAlarms; // indicator for loading all alarms

    unsigned long m_profileId;

	static const std::string RPARAM_FILTERONASSET;
	static const std::string RPARAM_FILTERONENTITY;
	static const std::string RPARAM_XPOS;
	static const std::string RPARAM_YPOS;
	static const std::string RPARAM_HEIGHT;
	static const std::string RPARAM_WIDTH;

    static const std::string NO_MATCHING_ALARMS_MESSAGE;

    TA_Base_Bus::SystemControllerGroupUpdateSubscriber* m_groupUpdateSubscriber;

	typedef std::map<std::string, unsigned long> EntityNameToKeyMap;
	EntityNameToKeyMap	m_entityNameKeyMap;


	// TD 13994 Default column width arrays for appropriate printout
	int * m_printoutColWidths;


	// font
	CFont m_font;

	/*
	 * initListControlActions
	 *
	 * Initialises the alarm list control actions.
	 *
	 * @return A vector of alarm actions.
	 */
	std::vector<TA_Base_Bus::IAlarmAction*> initListControlActions();

	/*
	 * initListControlColumns
	 *
	 * Initialises the alarm list control columns.
	 */
	void initListControlColumns();

    /**
     * calculateWindowPlacements()
     *
     * Calculates and resizes (if necessary) the windowing components on the Alarms GUI user interface
     *
     * @param cx The size of the client area in the x-dimension (i.e. width)
     * @param cy The size of the client area in the y-dimension (i.e. height)
     */
    void calculateWindowPlacements(int cx, int cy);

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
	void initPreFiltering(TA_Base_Bus::Filter& preFilter, 
		std::vector<unsigned long>& locations);

    /** 
      * reverseSort
      *
      * Creates a new sort object that is the exact reverse of the supplied
	  * sort object.
      *
      * @param sort  The sort object to reverse.
      *
      * @return  The reversed sort object.
      */
	TA_Base_Bus::Sort reverseSort(const TA_Base_Bus::Sort& sort);

	/*
     * SetMenuItemCheck
     *
     * This method sets the menu item check state. 
	 *
	 * @param The menu identifier. 
	 *
	 * @param The menu item's index.
	 *
	 * @param the check state. 
     */
	void SetMenuItemCheck(const UINT& itemID, bool bChecked = true);

	/*
     * updateAlarmCounts
     *
     * This method is the concrete implementation for the IAlarmListComponentObserver. The
	 * method updates the total and unacknowledged alarm counters.
	 *
	 * @param The total alarms count.
	 *
	 * @param The total unacknowledged alarms count.
     */
	virtual void updateAlarmCounts(unsigned int totAlarms, unsigned int totUnacknowledgedAlarms, bool exceededMaxDisplayAlarm, bool isLoadingAlarms);

	/*
     * updateAlarmSound
     *
     * This method is the concrete implementation for the IAlarmListComponentObserver. The
	 * method updates the alarm severity sound to be played. 
	 *
     * @param The severity tone to update as an unsigned long.
     * @param True if new alarms were inserted, and false otherwise.
     */
    virtual void updateAlarmSound(unsigned long severity, bool wereAlarmsAdded);

	/*
     * setWindowTitle
     *
     * This method is the concrete implementation for the IAlarmListComponentObserver. The
	 * method sets the container's window's text.
     */
	virtual void setWindowTitle(const std::string& windowText);

    /** 
      * filterOnAsset
      *
      * Filters the alarm list on asset. Used when the Alarm Manager is launched
	  * with a --filter-on-asset command line argument.
      *
      * @param asset  The asset to filter on.
      */
	virtual void filterOnAsset(const std::string& asset);

    /** 
      * filterOnEntity
      *
      * Filters the alarm list on entity. Used when the Alarm Manager is launched
	  * with a --filter-on-entity command line argument.
      *
      * @param asset  The asset to filter on.
      */
	virtual void filterOnEntity(const std::string& entity);

    void setProfileId();

	/**
	 * populateNamedFilters
	 *
	 * Populates the named filter combo box with the current list of named filters.
	 * 
	 * @param string - The current filter name used in the alarm control list and to be displayed
	 */
	void populateNamedFilters(std::string curName);

    /**
	 * updatePreFilterAccordingToSystemState
	 *
	 * Clears the pre-filter if the system is in degraded mode.
	 * 
	 * @param preFilter - Reference to the pre-filter that is to be changed.
	 */
    void updatePreFilterAccordingToSystemState(TA_Base_Bus::Filter& preFilter);


	void initializeAlarmList();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMSGUIDLG_H__F9076B67_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_)
