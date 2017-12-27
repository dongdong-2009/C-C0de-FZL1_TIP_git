/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/src/CombinedEventFilterDlg.h $
 * @author:  Bradley Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2013/01/24 17:57:59 $
 * Last modified by:  $Author: CM $
 * 
 * CombinedEventFilterDlg is an MFC class deriving from the FilterDlg header (which is in turn a CDialog derived class).
 * This dialog provides filtering capabilities on the data items displayed in the combined event view.
 */

#if !defined(_COMBINED_EVENT_FILTER_DLG_H_)
#define _COMBINED_EVENT_FILTER_DLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CombinedEventFilterDlg.h : header file
//
#include "stdafx.h"

#include "app/event/event_viewer/src/DatabaseAccess.h"
#include "app/event/event_viewer/src/FilterDlg.h"
#include "app/event/event_viewer/src/MultipleLoadDlg.h"
#include "app/event/event_viewer/src/resource.h"
#include "FilterCriteria.h"

#include "bus/mfc_extensions/src/combo_temporary_item/ComboTempItem.h"

#include "core/data_access_interface/src/NamedFilter.h"

#include <string>
#include <vector>
#include <map>

namespace EventConstants
{
	static const std::string RPARAM_NOSEVERITY = "NoSeverityFilter";
};


/////////////////////////////////////////////////////////////////////////////
// CombinedEventFilterDlg dialog

class CombinedEventFilter; // Forward declaration

class CombinedEventFilterDlg : public FilterDlg
{
// Construction
public:
	CombinedEventFilterDlg(CombinedEventFilter* filter,CWnd* pParent = NULL);   // standard constructor
	virtual ~CombinedEventFilterDlg();

// Dialog Data
	//{{AFX_DATA(CombinedEventFilterDlg)
	enum { IDD = IDD_COMBINED_EVENT_FILTER };
	CEdit	m_alarmtypeEdit;
	CEdit	m_eventtypeEdit;
	CEdit	m_systemEdit;
	CEdit   m_alarmStateEdit;  //for "Alarm State"
	CEdit	m_operatorEdit;
	CButton	m_fromCheck;
	CCheckListBox	m_severityList;
	CButton	m_okButton;
	CButton	m_cancelButton;
	CButton	m_defaultButton;
	CButton	m_applyButton;
	CEdit	m_alarmIdEdit;
	CEdit	m_descriptionEdit;
	CEdit	m_assetEdit;
	CEdit	m_locationEdit;
	CEdit	m_subsystemEdit;
	CDateTimeCtrl	m_toTime;
	CDateTimeCtrl	m_toDate;
	CButton	m_toCheck;
	CDateTimeCtrl	m_fromTime;
	CDateTimeCtrl	m_fromDate;
	TA_Base_Bus::ComboTempItem	m_namedFilterCombo;
    CButton m_saveFilterButton;
    CButton m_deleteFilterButton;

	CButton m_radio1;
	CButton m_radio2;
	CButton m_radio3;
	CButton m_radio4;


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CombinedEventFilterDlg)
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTo();
	afx_msg void OnSubsystemExt();
	afx_msg void OnLocationExt();
	afx_msg void OnApply();
	afx_msg void OnDefaultFilter();
	afx_msg void OnChangeAsset();
	afx_msg void OnChangeDescription();
	afx_msg void OnDatetimechangeToDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeToTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeFromDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeFromTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusSeverityList();
	afx_msg void OnChkChange();
	afx_msg void OnFrom();
	afx_msg void OnEventtypeExt();
	afx_msg void OnSystemExt();
	afx_msg void OnAlarmtypeExt();
	afx_msg void OnOperatorExt();
    afx_msg void OnSaveFilter();
    afx_msg void OnDeleteFilter();
	afx_msg void OnSelchangeComboNamedFilter();
	afx_msg void OnTextchangeNamedFilter();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

    /**
     * updateFilter
     *
     * Notifies the filter dialog that the filter object has been changed. When this method is called, 
     * the filter object is re-read and the filter dialog updated to reflect its contents.
     */
    void updateFilter();

	/**
     * getOldFilterName
     *
     * Returns the old_filter name if the combolist already has a name that is the same as the input one(case-insensitive).
     *
     * @param new_filterName The newly input filtername.
     *
     * @return std::string The old filterName. If not exist, then return ""
     */
	std::string CombinedEventFilterDlg::getOldFilterName (const std::string& new_filterName );

	/**
     * populateFilterCriteria
     *
     * set the whole FilterCiteria by the current filter.
	 * 
	 * @param filter: the filterCriteria which will be set.
     */
	void populateFilterCriteria(TA_Base_App::FilterCriteria & filterCriteria);

    // Used to distinguish the event filter from the severities filter
    static const int EVENT_ONLY_KEY;

private:
    typedef enum
    {
        NF_USERDEFINED,
        NF_UNSAVED,
        NF_READONLY,
        NF_READWRITE
    } ENamedFilterState;

	void CombinedEventFilterDlg::loadListDataFromCache();

	void loadEventTypeListData();
	void loadLocationKeyDisplayNameListData();
	void loadAlarmStateListData();
	void loadOperatorListData();
	void loadAlarmTypeListData();
	void retrieveSubsystemsBySeletedSystem (SelectedItems selectedSystemKeys);
	void loadSubsystemsListDataAll ();
	void loadSystemsListData ();

	/*Returns the main window of the application*/
	CWnd* GetMainWindow();


	SelectedItems getATSSystemPkeysByNames(TA_Base_Core::NamedFilter::StringList atsSystemsStrList);
	SelectedItems getATSSubsystemPkeysByNames(TA_Base_Core::NamedFilter::StringList atsSubsystemsStrList);
	TA_Base_Core::NamedFilter::StringList getATSSubsystemNamesByPKeys(SelectedItems atsSubsystemPkeys);
	TA_Base_Core::NamedFilter::StringList getATSSystemNamesByPKeys(SelectedItems atsSystemPkeys);
	TA_Base_Core::NamedFilter::StringList getStrVectorFromStrList(list<std::string> strList);
	ListItems getListItmesFromVector(std::vector<std::pair<unsigned long,std::string> > keyNamePairs);

	void getAllSubsystemKeys(SelectedItems &selection);
	// reset m_ISCSsubsystems/ m_ATSsubsystems/m_subsystems, reset m_substemEdit to "ALL", reset 3 NumericFilters
	void setSubsystemsBySystemkeys ( SelectedItems ISCSselection);
	

	void reChooseSubsystemsListByFilterOnSystem ();
	void loadATSsubsystemsBySystemKeys( SelectedItems ISCSselection );


	/**
     * setStringlistToFilterCriteria
     *
     * set the list<string> in FilterCiteria by the current filter.
	 * 
	 * @param listToBeSet: the list to be set.	
	 * @param columnName: the numeric filter name used for retrieving the corresponding data from namedFilter
	 * @param items: the key&value pair list of the whole list for the specified column.
     */
	void setStringlistFromNumericNamedFilter(list<std::string> & listToBeSet, const std::string columnName, ListItems& items);


    /**
     * populateTMTypeFromFilter
     *
     * populate the operator/maintainence radio box from the m_namedFilter
     */
	void populateTMTypeFromFilter (); 

	/**
     * resetStartEndTimeDefined
     *
     * set the StartTimePeriodDefined and endTimePeriodDefined in FilterCriteria, by the check box status on the GUI . 
	 * 
	 * @param filter: the filterCriteria which will be set.
     */
	void resetStartEndTimeDefined( TA_Base_App::FilterCriteria & filter);

    /**
     * populateEditFromFilter
     *
     * Places the selected items into the provided edit control as a comma separated list.
     *
     * @param edit The edit control to place the list into.
     * @param columnId The column whose selection has changed.
     * @param items The list of items that will map the key to the text.
     */
    void populateEditFromFilter( CEdit& edit, const std::string& columnId, ListItems& items );

    /**
     * populateEditFromSelection
     *
     * Places the selected items into the provided edit control as a comma separated list.
     *
     * @param edit The edit control to place the list into.
     * @param selection The selected items to display in the edit control.
     * @param items The list ofitems that will map the key to the text.
     */
    void populateEditFromSelection( CEdit& edit, const SelectedItems& selection, ListItems& items );

	/*void populateSubsystemEdit();*/

    /**
     * setUpSeverities
     *
     * Sets up the severity list control.
     *
     * @param dataItems The severties to add to the list control.
     */
    void setUpSeverities(std::vector<std::pair<unsigned long,std::string> > dataItems);

    /**
     * loadFilter
     *
     * Reads data from the filter object and popultes the dialogs controls with the information.
     */
    void loadFilter( bool importFromFilter );

    /**
     * updateTimes
     *
     * Updates the times in filter buffer.
     */
    void updateTimes();

    /**
     * enableButtons
     *
     * Enables all buttons that can be greyed out. This is used when an object in the filter manipulated.
     */
    void enableButtons();

    /**
     * getCombinedTime
     *
     * Calculates and returns the "combined" time from the date and time components passed in.
     */
    CTime getCombinedTime(const CTime& date,const CTime& time);

    /**
     * populateNamedFilters
     *
     * Populates the named filter combo box with the current list of named filters.
     */
    void populateNamedFilters(bool reload=false);

    /**
     * selectUserFilter
     *
     * Selects the 'User Defined' filter.
     */
    void selectUserFilter();

    /**
     * getTimeFromNamedFilter
     *
     * Extracts a time value from the named filter.
     *
     * @param timeIdentifier The time being requested.
     *
     * @return time_t The time value.
     */
    time_t getTimeFromNamedFilter( const std::string& timeIdentifier );

    /**
     * getNamedFilterState
     *
     * Returns the state of the specified named filter.
     *
     * @param namedFilter The filter to check.
     *
     * @return NamedFilterState The state of the specified named filter.
     */
    ENamedFilterState getNamedFilterState( const std::string& namedFilter );

    /**
     * enableNamedFilterButtons
     *
     * Enables the 'Save' and 'Delete' buttons associated with the specified named filter as required.
     *
     * @param filterName The name of the filter to set the buttons for.
     */
    void enableNamedFilterButtons( const std::string& filterName );

	void offsetToTimeTakeRefFromTime(unsigned long hours);
	void offsetFromTimeTakeRefToTime(unsigned long hours);

    // Variable definitions to remove the use of literals
    static const int UNCHECKED; // Value returned by MFC when a checkbox is not checked
    static const int CHECKED;   // When it is checked
    //static const std::string ALARM_FILTER_MESSAGE;  // String to display if there is currently no alarm filter
    static const int COMBO_LIMIT;
    //static const char* EVENT_ONLY_STRING;
    
    // The filter object that contains the data represented by this dialog.
    CombinedEventFilter* m_filter;
    TA_Base_Core::NamedFilter m_namedFilter;

// 	// These 3 list are the related subystem selected by system(in filter), which will show on the filter

	ListItems m_subsystems; //( m_subsystems = m_ISCSsubsystems + m_ATSsubsystems)
	ListItems m_ISCSsubsystems;
	ListItems m_ATSsubsystems;

	ListItems m_systems;   // m_system = ISCS's systems + m_atsSystems
	ListItems m_atsSystems; 
	ListItems m_locationKeyDisplayNameList;
	ListItems m_operators;
    ListItems m_eventtypes;
	ListItems m_alarmtypes;
	ListItems m_alarmState;
	std::vector<std::pair<unsigned long,std::string> > m_alarmSeverities;

    // The data access object.
    DatabaseAccess& m_dbAccess;

    // This member is used to disable events until after the dialog has been initialised.
    bool m_disableEvents;

	bool m_someFilterChanged; //this will be set to true when some namedfilter is saved or deleted.

	bool m_originalFilterChanged;

	//Begin--  TD14346

	//store the old filter data
	struct FilterData {
		CString m_strSubsystem;
		CString m_strLocation;
		CString m_strOperator;
		CString m_strSystem;
		CString m_strAsset;
		CString m_strDescription;
		CString m_strAlarmType;
		CString m_strAlarmID;
		bool	m_bFromTimeChecked;
		bool	m_bToTimeChecked;
		CTime	m_ctFromDate;
		CTime	m_ctFromTime;
		CTime	m_ctToDate;
		CTime	m_ctToTime;
		std::string    m_TMType;   //added
	};
	FilterData m_OldFilterData;
	/*--------------------------------------------------
	Function:Judge if the filter data has been changed
	return:
		true  -- Filter data are changed
		false -- Filter data aren't changed
	---------------------------------------------------*/
	bool IsFilterDataChanged();

	//Get the old filter data
	void GetFilterData();

	//End-- TD14346
    // This string is used to determine if the filter in use has changed.
    std::string m_originalFilterName;

	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	void OnTMTypeRadioBoxChanged(std::string tmtype);
	afx_msg void OnAlarmStateExt();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_COMBINED_EVENT_FILTER_DLG_H_)
