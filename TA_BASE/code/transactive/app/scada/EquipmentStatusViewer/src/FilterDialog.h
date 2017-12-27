/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/FilterDialog.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class to perform filtering of the equipment status listCtrl based on provided filters
 *
 */

#if !defined(AFX_FILTERDIALOG_H__347AE16A_4147_4425_A044_19D6A0AC4134__INCLUDED_)
#define AFX_FILTERDIALOG_H__347AE16A_4147_4425_A044_19D6A0AC4134__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EquipmentStatusListCtrl.h"
#include "EquipmentStatusListItem.h"

namespace TA_Base_App
{

	class CFilterDialog : public CDialog
	{

	public:

		/**
		 *	SetFilterPointers()
		 *
		 *	Sets the pointers to the filter collection variables. Collection should be of size
		 *	TIMESTAMP_COLUMN
		 *
		 *  @param	aFilterFlags	bool flags of the filters to apply
		 *	@param	aFilterString	filter strings of the filters to apply
		 *
		 */

		/**
		 *	CFilterDialog()
		 *
		 *	Constructor
		 *
		 *  @param	aCanConfigure	true if filters are to be saved to database on exit, or 
		 *							false if filters are NOT to be saved to database but 
		 *							used as current filters
		 *
		 */
	    CFilterDialog( bool aCanConfigure,
            FilterFlagsAndStrings& sourceFilters, 
            std::string& reportId,
            CString const& reportName,
            CString const& locationName,
            EquipmentStatusListItemMap const& reportItems,
            CWnd* pParent = NULL);

	private:

		//{{AFX_DATA(CFilterDialog)
	    enum { IDD = IDD_FILTER };
	    CButton	m_Mms_Inhibit_Check;
	    CButton	m_Tagged_PTW_Check;
	    CButton	m_Tagged_Check;
	    CButton	m_Overridden_Check;
	    CButton	m_Alarm_Inhibit_Check;
	    CButton	m_Scan_Inhibit_Check;
	    CButton	m_Control_Inhibit_Check;
		CComboBox	m_ItemTypeFilterCombo;
		CButton	m_TimestampFilterCheck;
		CButton	m_StatusChangeByFilterCheck;
		CButton	m_StatusFilterCheck;
		CButton	m_DPValueFilterCheck;
		CButton	m_DPTypeFilterCheck;
		CButton	m_DPNameFilterCheck;
		CComboBox	m_TimestampFilterCombo;
		CComboBox	m_StatusChangeByFilterCombo;
		CComboBox	m_DPValueFilterCombo;
		CComboBox	m_DPTypeFilterCombo;
		CComboBox	m_DPNameFilterCombo;
		CButton	m_DescriptionFilterCheck;
		CComboBox	m_DescriptionFilterCombo;
		CButton	m_AssetFilterCheck;
		CComboBox	m_AssetFilterCombo;
	    //}}AFX_DATA


		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CFilterDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	protected:

		// Generated message map functions
		//{{AFX_MSG(CFilterDialog)
		virtual BOOL OnInitDialog();
        afx_msg void OnClearButton();
		afx_msg void OnAssetFilterEnabled();
		afx_msg void OnDescFilterEnabled();
		afx_msg void OnDpnameFilterEnabled();
		afx_msg void OnDptypeFilterEnabled();
		afx_msg void OnDpvalueFilterEnabled();
		afx_msg void OnStatusFilterEnabled();
		afx_msg void OnStatuschangebyFilterEnabled();
		afx_msg void OnTimestampFilterEnabled();
	    virtual void OnOK();
	    afx_msg void OnSelchangeItemtypeFilter();
	    //}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:

		/**
		 *	Flag to tell if filters should be saved to DB (true) or not saved and instead used
		 *	as current filters (false)
		 *
		 */
		bool m_canConfigure;

		/**
		 *	Primary key of the report
		 *
		 */
        std::string			m_ReportId;

		/**
		 *	Name of the chosen location
		 *
		 */
		CString					m_LocationName;

		/**
		 *	Name of the chosen report
		 *
		 */
		CString					m_ReportName;

		/**
		 *	Reference to the original filters in the calling dialog
		 *
		 */
        FilterFlagsAndStrings& m_sourceFilters;

		/**
		 *	Local working copy of the filters (these will be copied
		 *  back to the original when OK button is clicked).
		 *
		 */
        FilterFlagsAndStrings  m_currentFilters;

		/**
		 *	GetCurrentFilters()
		 *
		 *	Populates the filter collections with the currently set filter values from the
		 *	dialog box.
		 *
		 *	@return			Returns true if all input is OK or false if something is missing
		 *
		 */
		bool GetCurrentFilters();

		/**
		 *	typedef: collection of unique strings
		 */
        typedef std::set< CString> FilterStringSet;

        /** 
          * AddToFilterStringSet
          *
          * Add this string to the collection of unique non-blank strings
          *
          * @param fss          The collection to add to
          * @param newString    The string to add (can be blank)
          *
          */
		static void AddToFilterStringSet( FilterStringSet& fss, CString newString);

        /** 
          * PopulateComboBoxWithStringSet
          *
          * Fill the combo box "in bulk" with the collection of unique non-blank strings
          *
          * @param comboBox         The combobox to fill
          * @param filterStrings    The collection to fill the combobox with
          *
          */
		void PopulateComboBoxWithStringSet( CComboBox& comboBox, FilterStringSet const& filterStrings);

        /** 
          * SetFilterFlagAndString
          *
          * Updates the GUI controls using the inUse parameter.
          *
          * @param inUse        Whether this filter item is to be used
          *                     True: the checkbox will be ticked and the combobox enabled
          *                     False: the checkbox will be empty and the combobox greyed out
          * @param checkBox     Checkbox to update
          * @param comboBox     Associated combobox to enable or disable
          * @param filterIndex  Filter item to use (currently just uses this item's string
          *                     as the default text if enabling the combobox).
          */
    	void SetFilterFlagAndString( bool inUse, CButton& checkBox, CComboBox& comboBox, int filterIndex);

        /** 
          * GetFilterFlagAndString
          *
          * Gets the filter string if the associated checkbox is ticked and updates the relevant
          * filter flag and string.
          *
          * @param checkBox     Checkbox to evaluate (if ticked then set inUse flag to true)
          * @param comboBox     If inUse then use the current text from this combobox to filter with
          * @param filterIndex  Update this filter item with the new values.
          *
          * @return bool        True: input is valid
          *                     False: input is not valid (checkbox is ticked but string is blank)
          */
        bool GetFilterFlagAndString( CButton const& checkBox, CComboBox const& comboBox, int filterIndex );

        /** 
          * GetFilterFlagAndPredefinedString
          *
          * Builds the filter string using the predefined filter text if the related checkbox
          * is ticked. This is used for the Status checkboxes which do not have associated 
          * comboboxes of filter text.
          *
          * @param checkBox     Checkbox to evaluate (if ticked then set inUse flag to true)
          * @param filterIndex  Update this filter item with the new values.
          * @param pfsIndex     Which predefined filter string to use for this filter item.
          */
        void GetFilterFlagAndPredefinedString( CButton const& checkBox, int filterIndex, PredefinedFilterStrings pfsIndex );

		/**
		 *	Collection of datanode/datapoint proxies that are being used in the current
         *  report (all items regardless of filtering). These are used to build the droplists
         *  for the comboboxes.
		 */
        EquipmentStatusListItemMap const& m_reportItems;

		// TD16914:Wenching
		std::map<std::string, FilterStringSet> m_equipmentAssetComboMap;
		std::map<std::string, FilterStringSet> m_equipmentDescriptionComboMap;
	};

}	// TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERDIALOG_H__347AE16A_4147_4425_A044_19D6A0AC4134__INCLUDED_)
