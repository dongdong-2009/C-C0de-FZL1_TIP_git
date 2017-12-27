/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/EventViewerDlg.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * EventViewerDlg is the main dialog class. This is the main window of the application, and is the container that holds the 
 * Event List Component.
 */

#if !defined(AFX_EVENTVIEWERDLG_H__482E140A_F7F4_40DF_8499_9F47F3114230__INCLUDED_)
#define AFX_EVENTVIEWERDLG_H__482E140A_F7F4_40DF_8499_9F47F3114230__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/soe/soe_event_viewer/src/StatusProgressBar.h"
#include "app/soe/soe_event_viewer/EventListComponent/src/EventListComponent.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/mfc_extensions/src/combo_temporary_item/ComboTempItem.h"

#include "core/utilities/src/RunParams.h"

#include <map>

/////////////////////////////////////////////////////////////////////////////
// EventViewerDlg dialog

class FilterDlg;    //forward declarations

//*zhiqiang++begin
class CPopulateEventLevelsThread;
class CApplyRightsThread;
//*/ //zhiqiang++end
namespace TA_Base_Bus
{
    class RightsLibrary;
}

class EventViewerDlg : public TA_Base_Bus::TransActiveDialog,
                       public TA_Base_Core::RunParamUser
{
// Construction
public:
	EventViewerDlg(TA_Base_Bus::IGUIAccess& controlClass);	// standard constructor
	virtual ~EventViewerDlg(); //zhiqiang++

// Dialog Data
	//{{AFX_DATA(EventViewerDlg)
	enum { IDD = IDD_EVENTVIEWER_DIALOG };
	CButton	m_pageDown;
	CButton	m_pageUp;
	EventListComponent	m_eventList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EventViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(EventViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnFileExit();
	afx_msg void OnViewFilters();
	afx_msg void OnStatusBar();
	afx_msg void OnToolBar();
    afx_msg void OnShowAll();
	afx_msg void OnAppAbout();
    afx_msg void OnHelp();
	afx_msg void OnPageUp();
	afx_msg void OnPageDown();	
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus); //TD17143
	//}}AFX_MSG
	//zhiqiang++begin
	afx_msg LRESULT OnApplyRights(WPARAM,LPARAM);
	//zhiqiang++end
    afx_msg LRESULT  OnListUpdate(WPARAM,LPARAM);
    afx_msg LRESULT  OnLoadListCount(WPARAM,LPARAM);
    afx_msg LRESULT  OnLoadListUpdate(WPARAM,LPARAM);
	afx_msg LRESULT  OnListViewUpdate(WPARAM,LPARAM);
    afx_msg LRESULT  OnFilterDlgClose(WPARAM,LPARAM);
    afx_msg LRESULT  OnFilterApply(WPARAM,LPARAM);
    afx_msg LRESULT  OnFilterDlgCancel(WPARAM,LPARAM);
    afx_msg LRESULT  OnNamedFilterChange(WPARAM,LPARAM);
    afx_msg LRESULT  OnListItemSelect(WPARAM wParam,LPARAM lParam);
    afx_msg LRESULT  OnCloseMainWindow(WPARAM,LPARAM);
	afx_msg LRESULT OnSetupEventFiltersAndRights(WPARAM,LPARAM); //TD17143
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
    afx_msg void OnNext();
    afx_msg void OnPrevious();
    afx_msg void OnPrint();
    afx_msg void OnExport();
    afx_msg void OnAlarmManager();
    afx_msg void OnIncidentReport();
	afx_msg void OnSelchangeComboNamedFilter();
    DECLARE_MESSAGE_MAP()

public:

	//TD18095,jianghp
	virtual void init();
    /**
     * setFilter
     *
     * Sets the filter member variable.
     *
     * @param filter A pointer to the filter object that will be used to create the initial event load.
     * NOTE: This pointer can be NULL, in which case this class will create a default filter.
     */
    void setFilter(Filter* filter);

    /**
     * isReadyToClose
     *
     * Determines if the event list is in a state that is ready to close. 
     *
     * @return true If the event list is in a "closeable" state
     */
    bool isReadyToClose();

    /**
     * initialiseWindowOffsets
     *
     * Initialises the offsets that are used when the dialog is resized. 
     *
     */
    void initialiseWindowOffsets();

    //---------- Methods Inherited from RunParamUser --------//

    /**
     * onRunParamChange
     *
     * Called whenever a RunParam is updated during run-time.
     *
     * @param name The name of the RunParam that has changed.
     * @param value The new value of the RunParam.
     */
    void onRunParamChange(const std::string& name, const std::string& value);

private:

    /**
     * loadEvents
     *
     * Initiates the loading of events from the database
     */
    void loadEvents(bool enableRefresh);
	void reloadEvents();

    /**
     * enableDisableColumn
     *
     * If the specified column is currently of width:
     *  0 - expands the column to be visible, and checks the menu item associated with the column
     * >0 - sets the column width to 0, and unchecks the menu item associated with the column
     *
     * @param columnName The name of the column to hide or show
     * @param menuId The ID of the menu item associated with the column.
     */
    void enableDisableColumn(const std::string& columnName,const unsigned long menuId);

    /**
     * findMenuItem
     *
     * Retrieves the numerical position of a sub-menu based on the display string. This is needed because some CMenu commands
     * only take a numerical position.
     *
     * @param Menu A pointer to the menu object that contains the sub-menu being searched for
     * @param MenuString A pointer to a string equivalent to the name of the menu that is to be found. This string must include
     * any control characters (e.g. &)
     *
     * @return The numerical position of the sub-menu named by the string. 
     * -1 if not found.
     */
    int findMenuItem(CMenu* Menu, LPCTSTR MenuString);

    /**
     * setUpStatusBar
     *
     * Creates and positions the status bar, and re-adjusts the main window dimensions to account for it.
     */
    void setUpStatusBar();

    /**
     * setUpToolBar
     *
     * Creates and positions the tool bar and pager control that contains it.
     */
    void setUpToolBar();

    /**
     * setUpMenu
     *
     * Sets default menu checks, and adds the default columns menu
     */
    void setUpMenu();

    /**
     * setWindowCaption
     *
     * Sets the window caption depending on the current filter.
     */
    void setWindowCaption();

    /**
     * setFilterFromCommandLine
     *
     * Populates the filter with data provided on the command line.
     */
    void setFilterFromCommandLine();

    /**
     * tokenizeKeyList
     *
     * Splits the provided list of comma separated keys into a vector of the keys.
     *
     * @param param The parameter to convert.
     * @param keys The list of keys.
     */
    void tokenizeKeyList( const std::string& param, std::vector<unsigned long>& keys );

    /**
     * populateNamedFilters
     *
     * Populates the named filter combo box with the current list of named filters.
     */
    void populateNamedFilters();

    /**
     * populateEventLevels
     *
     * Populates the event levels combo box with the allowed range of levels.
     */
    void populateEventLevels();

    /**
     * applyRights
     *
     * Applys the current rights settings on any controls that a right dependant.
     */
    void applyRights();

    std::string getCurrentDatabaseString();

    // Constants to control the minimum size and list view offsets
    static const int STANDARD_MIN_HEIGHT;
    static const int STANDARD_MIN_WIDTH;
    static const int OFFSET_NO_STATUSBAR;
    static const int OFFSET_NO_TOOLBAR;

    // Constant for what to write in the status bar when nothing else suits
    static const CString IDLE_TEXT;

    // Constant for which status bar pane will be set to "stretch"
    static const int STRETCH_PANE;

    // The names of the menu options
    static const CString VIEW_MENU;
    static const CString COLUMNS_MENU;

    // The specifics of the font to set for sub-components
    static const CString FONT_NAME;
    static const int FONT_SIZE;

    // IDs and items used in defining the named filter combo box on the toolbar
    static const CString NAMED_FILTER_LABEL_TEXT;
    static const long    NAMED_FILTER_LABEL_SEPARATOR_ID;
    static const int     NAMED_FILTER_LABEL_SEPARATOR_WIDTH;
    static const int     NAMED_FILTER_LABEL_TOP;
    static const int     NAMED_FILTER_LABEL_OFFSET;
    static const long    NAMED_FILTER_COMBO_SEPARATOR_ID;
    static const int     NAMED_FILTER_COMBO_SEPARATOR_WIDTH;
    static const int     NAMED_FILTER_COMBO_TOP;
    static const int     NAMED_FILTER_COMBO_OFFSET;
    static const int     NAMED_FILTER_COMBO_DROP_HEIGHT;

    // IDs and items used in defining the event level combo box on the toolbar
    static const int     EVENT_LEVEL_SEPARATOR_ID;
    static const CString EVENT_LEVEL_LABEL_TEXT;
    static const long    EVENT_LEVEL_LABEL_SEPARATOR_ID;
    static const int     EVENT_LEVEL_LABEL_SEPARATOR_WIDTH;
    static const int     EVENT_LEVEL_LABEL_TOP;
    static const int     EVENT_LEVEL_LABEL_OFFSET;
    static const long    EVENT_LEVEL_COMBO_SEPARATOR_ID;
    static const int     EVENT_LEVEL_COMBO_SEPARATOR_WIDTH;
    static const int     EVENT_LEVEL_COMBO_TOP;
    static const int     EVENT_LEVEL_COMBO_OFFSET;
    static const int     EVENT_LEVEL_COMBO_DROP_HEIGHT;


    // Enumeration that lets the event viewer know whether a list update was from a load or a refresh
    enum
    {
        UPDATE_LOAD,
        UPDATE_REFRESH
    } m_updateType;

    // Required by GenericGUI
	TA_Base_Bus::IGUIAccess& m_controlClass;

    // The status bar member variable
    StatusProgressBar m_statusBar;

    // The image lists for the toolbar - enabled and "hot" (mouse over)
    CImageList m_eventsTbImageListEnabled;
    CImageList m_eventsTbImageListHot;

    // The toolbar and the pager that contains it
    CToolBarCtrl m_eventsToolbar;
    CWnd m_Pager;

    // Timer ID used with the standard application timer
    unsigned int m_nIDTimer;

    // Offests of the list from the top bottom and sides of the main window
    unsigned int m_listWidthOffset;
    unsigned int m_listTopOffset;
    unsigned int m_listBottomOffset;

    // The filter object
    Filter* m_filter;

    // Map of filters that are not currently active
    std::map<int,Filter*> m_savedFilters;

    // The filter dialog. Will either be NULL or have a valid filter dialog
    FilterDlg* m_filterDlg;

    bool m_progressBarExists;

    // The time to wait between event refreshes.
    unsigned long m_refreshTime;

    // The combo box for storing named filters.
    TA_Base_Bus::ComboTempItem m_namedFiltersComboBox;

    // The rights library.
    std::auto_ptr<TA_Base_Bus::RightsLibrary> m_rightsLibrary;

    // The current db connection string
    std::string m_currentDbString;
	//*zhiqiang++begin
	CPopulateEventLevelsThread *m_pPopulateEventLevelsThread;
	CApplyRightsThread  *m_pApplyRightsThread;
	//zhiqinag++end

	// font fix
	CFont m_font;
	CFont m_listfont;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTVIEWERDLG_H__482E140A_F7F4_40DF_8499_9F47F3114230__INCLUDED_)
