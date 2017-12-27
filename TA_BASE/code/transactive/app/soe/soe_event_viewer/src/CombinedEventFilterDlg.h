/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/CombinedEventFilterDlg.h $
 * @author:  Bradley Cleaver
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
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

#include "app/soe/soe_event_viewer/src/DatabaseAccess.h"
#include "app/soe/soe_event_viewer/src/FilterDlg.h"
#include "app/soe/soe_event_viewer/src/MultipleLoadDlg.h"
#include "app/soe/soe_event_viewer/src/resource.h"

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

class Filter; // Forward declaration

class CombinedEventFilterDlg : public FilterDlg
{
// Construction
public:
	CombinedEventFilterDlg(Filter* filter,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CombinedEventFilterDlg)
	enum { IDD = IDD_COMBINED_EVENT_FILTER };
 
	CButton	m_fromCheck;
	 
	CButton	m_okButton;
	CButton	m_cancelButton;
	CButton	m_defaultButton;
	CButton	m_applyButton;
	CEdit	m_alarmIdEdit;
	CEdit	m_txtDisplayValue;
	CEdit	m_txtDescription;
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
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CombinedEventFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

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
	afx_msg void OnChangeDisplayValue();
	afx_msg void OnDatetimechangeToDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeToTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeFromDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeFromTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusSeverityList();
	afx_msg void OnChkChange();
	afx_msg void OnFrom();
	afx_msg void OnEventtypeExt();
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

    /**
     * loadListData
     *
     * Loads all data required for the multiple selection lists.
     */
    void loadListData();

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
    void populateNamedFilters();

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
    static const std::string ALARM_FILTER_MESSAGE;  // String to display if there is currently no alarm filter
    static const int COMBO_LIMIT;
    static const char* EVENT_ONLY_STRING;
    
    // The filter object that contains the data represented by this dialog.
    Filter* m_filter;
    TA_Base_Core::NamedFilter m_namedFilter;

    // List of all data that can be selected.
    ListItems m_locations;
 

    // The data access object.
    DatabaseAccess m_dbAccess;

    // This member is used to disable events until after the dialog has been initialised.
    bool m_disableEvents;

	//Begin--  TD14346

	//store the old filter data
	struct FilterData {
		 
		CString m_strLocation;
		CString m_strOperator;
		 
		CString m_strAsset;
		CString m_strDescription;
	 
		bool	m_bFromTimeChecked;
		bool	m_bToTimeChecked;
		CTime	m_ctFromDate;
		CTime	m_ctFromTime;
		CTime	m_ctToDate;
		CTime	m_ctToTime;
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
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_COMBINED_EVENT_FILTER_DLG_H_)
