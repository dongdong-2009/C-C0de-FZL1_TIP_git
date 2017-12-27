/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#if !defined(AFX_ALARMFILTERDLG_H__CCB479D0_5367_4905_B748_6C2429D62472__INCLUDED_)
#define AFX_ALARMFILTERDLG_H__CCB479D0_5367_4905_B748_6C2429D62472__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlarmFilterDlg.h : header file
//

#include "Afxdtctl.h"
#include "app/alarm/alarm_manager/src/MultipleSelectionDlg.h"
#include "app/alarm/alarm_manager/src/Resource.h"
#include "app/alarm/alarm_manager/src/ColourCheckListBox.h"

#include "bus/alarm/alarm_list_control/src/Filter.h"
#include "bus/alarm/alarm_list_control/src/AlarmListCtrl.h"

#include "bus/mfc_extensions/src/combo_temporary_item/ComboTempItem.h"

#include "core/data_access_interface/src/NamedFilter.h"

#include <string>
#include <vector>
#include <map>

/////////////////////////////////////////////////////////////////////////////
// AlarmFilterDlg dialog
namespace TA_Base_Core
{
	class IAlarmSeverityData;
}

namespace TA_Base_App
{

	class AlarmListComponent;

	class AlarmFilterDlg : public CDialog
	{
	public:

		// Static constants for identifying filters.
		static const std::string FILTER_DEFAULT;
		
		static const std::string FILTER_TOTIME;
		static const std::string FILTER_FROMTIME;
		static const std::string FILTER_SUBSYSTEM;
		static const std::string FILTER_LOCATION;
		static const std::string FILTER_OPERATOR;
		static const std::string FILTER_ASSET;
		static const std::string FILTER_DESCRIPTION;
		static const std::string FILTER_SEVERITY;
		static const std::string FILTER_ALARMTYPE;

		static const std::string FILTER_ACKSTATE;
		static const std::string FILTER_STATE;
		static const std::string FILTER_OPEN;
		static const std::string FILTER_CLOSE;
		static const std::string FILTER_ACKED;
		static const std::string FILTER_UNACKED;
		static const std::string FILTER_ALL;

		
		// Construction
		AlarmFilterDlg(TA_Base_Bus::AlarmListCtrl& alarmList, CWnd* pParent = NULL); 
		// Destruction
		~AlarmFilterDlg();

		enum FilterIndex
		{
			INDEX_DEFAULT,
			INDEX_USER_DEFINED,
			INDEX_LIST,
		};

		/**
		* getAllFilterSelections
		* 
		* This method retrieves the vector of alarm filters.
		*/
		// TA_Base_App::AlarmFilter getAllFilterSelections();

		TA_Base_Bus::AlarmListCtrl& m_alarmList;
		FilterIndex filterIndex;

		// This member is to set whether there is any change made to the filter dlg
		// True = changed, false = not changed
		bool m_updateNeeded;


		// font
		CFont m_font;


		// Dialog Data
		//{{AFX_DATA(AlarmFilterDlg)
		enum { IDD = IDD_ALARM_FILTER };
		CButton	m_okButton;
		CButton	m_applyFilterButton;
		CButton m_saveButton;
		CButton m_deleteButton;
		CButton m_cancelButton;
		CButton m_applyButton;
		CDateTimeCtrl	m_toTime;
		CDateTimeCtrl	m_toDate;
		CDateTimeCtrl	m_fromTime;
		CDateTimeCtrl	m_fromDate;
		CButton	m_toCheck;
		CButton	m_fromCheck;
		CEdit	m_assetEdit;
		CEdit	m_descEdit;
		CEdit	m_locationEdit;
		CEdit	m_subsystemEdit;
		CEdit	m_operatorEdit;
		CEdit	m_alarmTypeEdit;
		CColourCheckListBox	m_severityList;
		TA_Base_Bus::ComboTempItem	m_namedFilter;
        CComboBox m_comboFrom;
        CComboBox m_comboTo;
        CEdit   m_editTo;
        CEdit   m_editFrom;
        CSpinButtonCtrl m_spinFrom;
        CSpinButtonCtrl m_spinTo;
		//}}AFX_DATA


	  /**
		* setNamedFilter
		* 
		* This is to filter the alarm list control to the named filter passed in
		*
		* @param: A name string of the filter for filtering.
		*/
		void setNamedFilter(std::string filterName);
		
	  /**
		* showFilter
		* 
		* This is to display the filter parameters to the named filter passed in
		*
		* @param: A name string of the filter for display
		*/
		void showFilter (std::string filterName);

	  /**
		* getCurrentFilterString
		* 
		* This is to return string of the named filter parameters for display on caption, etc
		*
		* @return: A param string of the named filter for display
		*/
		std::string getCurrentFilterString ();

		//TD15028++ wangyn
		void setFromDateTime(TA_Base_Bus::AlarmListCtrl& listCtrl);
		void setToDateTime(TA_Base_Bus::AlarmListCtrl& listCtrl);
		//++TD15028

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(AlarmFilterDlg)
		public:
			virtual int DoModal();

		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL


	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(AlarmFilterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDelete();
	afx_msg void OnApply();
	afx_msg void OnClearFiltersButton();
	afx_msg void OnChangeEditAsset();
	afx_msg void OnChangeEditDescription();
	afx_msg void OnDatetimechangeDatetimepickerFromdate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerFromtime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerTodate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerTotime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSubsystemButton();
	afx_msg void OnLocationButton();
	afx_msg void OnOperatorButton();
	afx_msg void OnAlarmTypeButton();
	virtual void OnCancel();
	afx_msg void OnSave();
	afx_msg void OnFromCheckBoxClicked();
	afx_msg void OnToCheckBoxClicked();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnStateRadioButton(UINT nId);
	afx_msg void OnAckRadioButton(UINT nId);
	afx_msg void OnKillfocusSeverityList();
	afx_msg void OnChkChange();
	afx_msg void OnSelchangeNamedFilter();
	afx_msg void OnTextchangeNamedFilter();
//	afx_msg void OnTimer(UINT nIDEvent); //TD17081
    //}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:

		static const int ALARM_FILTER_DATA_INITIALIZE_TIMER; //TD17081
        // member variables
		// These ListItems variables are for the FULL list of items, not the selected ones
		ListItems m_operators;
		ListItems m_alarmTypes;
		ListItems m_locations;
		ListItems m_subsystems;
		
		// The following variables are for the CURRENTLY SELECTED items in the control only
		ListItems m_selectedSubsystems;
		ListItems m_selectedLocations;
		ListItems m_selectedOperators;
		ListItems m_selectedAlarmTypes;

		MultipleSelectionDlg* m_subsystemDlg;
		MultipleSelectionDlg* m_locationDlg;
		MultipleSelectionDlg* m_operatorDlg;
		MultipleSelectionDlg* m_alarmTypeDlg;

		// This member is used to access and store the filter names
		// It is the current Named Filter selected and applied
		TA_Base_Core::NamedFilter m_nmFilter;

		// This member is used to store the filter for alarm list control.
		// It is the current filter for the alarm list
		TA_Base_Bus::Filter m_filter;

		// This member is to set whether user has saved the filter
		// True = saved, false = not saved
		bool m_savedFilter;

		// This member is used for checking if the current filter was deleted
		// 0 = no changes
		// 1 = updated
		// -1 = deleted
		// OnCancel we check if this variable and set the to default filter if 1 or -1
		int m_nCurFilterState;

		// This member is used to store state of the filters
		// true if a filter is added, updated, or removed
		bool m_bNMFilterUpdated;

		// This member is used to store the current filter used in the main dialog.
		std::string m_strCurDlgFilter;

		// This will hold the userDefine filter
		TA_Base_Bus::Filter m_filterUserDef;	

	  /**
		* createFilter
		* 
		* This method creates a Filter object based on the user's selections.
		*
		* @return A Filter object containing all information required for filtering.
		*/
		TA_Base_Bus::Filter createFilter();

		void addTimeFilters(TA_Base_Bus::Filter& filter);
		void addStateFilters(TA_Base_Bus::Filter& filter);

	  /**
		* setUpdateNeeded
		* 
	    * Call this method when the controls are changed or a filter applied
		*
		* @param TRUE => enable 'Apply' button. FALSE => disable 'Apply' button.
		*/
		void setUpdateNeeded(bool updateNeeded);

	  /**
		* retrieveOperators
		* 
	    * This method retrieves all operators from the database and stores it in 
		* a map..
		*/
		void retrieveOperators();

	  /**
		* retrieveSeverities
		* 
	    * This method retrieves all alarm severities from the database and stores it in 
		* a map.
		*/
		void retrieveSeverities();

	  /**
		* retrieveAlarmTypes
		* 
	    * This method retrieves all alarm types from the database and stores it in 
		* a vector.
		*/
		void retrieveAlarmTypes();

		void retrieveLocations();
		void retrieveSubsystems();

		void populateLocationsEditCtrl();
		void populateSubsystemsEditCtrl();
		void populateOperatorsEditCtrl();
		void populateAlarmTypesEditCtrl();
		void populateState(const TA_Base_Bus::Filter::FilterDetails& setting);

		void setAckRadioButton(int resourceID);
		void setStateRadioButton(int resourceID);

		
	  /**
		* clearAllDisplays
		* 
	    * This method is to clear or reset all displays and controls in the dialog 
		* 
		*/
		void clearAllDisplays();

	  /**
		* deleteNamedFilter
		* 
	    * Delete the named filter from the database.
		*
		* @param NamedFilter - The NamedFilter obj or data to be deleted
		* @return bool - Return the success of delete false = not sucessful, otherwise true
		*
		*/
		bool deleteNamedFilter (TA_Base_Core::NamedFilter& nmFilter);

	  /**
		* SetAllSeverityFilterControls
		* 
	    * Set or Clear all the severity checkbox controls.
		*
		* @param TRUE => set severity filters. FALSE => clear them.
		*
		*/
		void SetAllSeverityFilterControls(BOOL doSet);

	
	  /**
		* populateFilterSelections
		* 
	    * Set the filter controls based on the currently applied filter.
		*/
		void populateFilterSelections(TA_Base_Bus::Filter& filter);

		//TD17081++ - modified argument type from severityKey to the severityData 
		// to limit DB access
		/**
		* getSeverityColour
		* 
	    * Retrieve the severity color of the given IAlarmSeverityData
		*/

        //COLORREF getSeverityColour(unsigned long severityKey);
		COLORREF getSeverityColour(TA_Base_Core::IAlarmSeverityData* severityData);
		//++TD17081 

		/**
		 * enableButtons or disableButtons
		 *
		 * Enables or disable "OK" and "APPLY" buttons that can be greyed out.
		 * This is used when an object in the filter manipulated or updated.
		 * "CANCEL" button is always enabled.
		 *
		 */
		void enableButtons();
		void disableButtons();

		/**
		 * selectUserFilter
		 *
		 * Selects the 'User Defined' filter.
		 */
		void selectUserFilter();

		/**
		 * populateNamedFilters
		 *
		 * Populates the named filter combo box with the current list of named filters.
		 * 
		 * @param string - The current filter name used in the alarm control list and to be displayed
		 */
		void populateNamedFilters(std::string curName);

		/**
		 * convertNamedFilterToFilter
		 * 
		 * Convert the NamedFilter object to Filter object for use in alarm list control
		 *
		 * @param NamedFilter - The NamedFilter reference from which the Filter obj will be constructed
		 * @return Filter - The Filter copy contructed with the input NamedFilter
		 *
		 */
		TA_Base_Bus::Filter convertNamedFilterToFilter (TA_Base_Core::NamedFilter& nmFilter);

		/**
		 * convertFilterToNamedFilter
		 * 
		 * Convert the Filter object to NamedFilter object for use in this dialog
		 *
		 * @param Filter - The Filter reference from which the NamedFilter obj will be constructed
		 * @return NamedFilter - The NamedFilter copy contructed with the input Filter
         *
		 */
		TA_Base_Core::NamedFilter convertFilterToNamedFilter (TA_Base_Bus::Filter& filter);

		/**
		 * checkInNamedFilterDatabase
		 * 
		 * Convert the Filter object to NamedFilter object for use in this dialog
		 *
		 * @param Name string - Name of the named filter to be checked
		 * @return bool - Determine the presence of the filter. True if present, otherwise false
         *
		 */
		bool checkInNamedFilterDatabase(std::string curName);

        void fromCheckBoxUpdated(bool isTicked);

        void toCheckBoxUpdated(bool isTicked);

	};
} //TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMFILTERDLG_H__CCB479D0_5367_4905_B748_6C2429D62472__INCLUDED_)
