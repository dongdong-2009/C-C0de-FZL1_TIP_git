#if !defined(AFX_TRENDPROPERTIES_H__3127E3B3_AB03_413C_9A4E_883BE22240CC__INCLUDED_)
#define AFX_TRENDPROPERTIES_H__3127E3B3_AB03_413C_9A4E_883BE22240CC__INCLUDED_

/**
  * TrendProperties.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendPropertiesDialog.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * The Trend Properties dialog class. This dialog displays the trend properties for one trend.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"
#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/IArchiveDataStore.h"
#include "app/trending/trend_viewer/src/resource.h"

#include "core/data_access_interface/trends/src/ItemInfoDetails.h"

#include "boost/smart_ptr.hpp"

namespace TA_Base_App
{
    enum ETimeUnits
    {
        MINUTES,
        HOURS,
        DAYS,
        WEEKS,
        MONTHS,
        YEARS
    };

    /////////////////////////////////////////////////////////////////////////////
    // CTrendProperties dialog

    class CTrendPropertiesDialog : public CDialog
    {
    // Construction
    public:
        CTrendPropertiesDialog(IItemStore& itemStore, IArchiveDataStore& archiveDataStore, CWnd* pParent = NULL);   // standard constructor

		~CTrendPropertiesDialog() {} //TD17529

        TrendProperties getTrendProperties()
        {
            return m_trendProperties;
        }

        void setTrendProperties(const TrendProperties& trendProperties);

        bool addNewTemplate();
        
        bool modifyExistingTemplate(time_t lastModified);
        
		void setMaxTrendsToDisplay(int newMaxTrendsDisplayed); //TD17529
                
    // Dialog Data
        //{{AFX_DATA(CTrendPropertiesDialog)
        enum { IDD = IDD_TREND_PROPERTIES };
        CComboBox       m_offsetCombo;
        CComboBox       m_samplePeriodCtrl;
        CComboBox       m_graphTypeCtrl;
        CDateTimeCtrl   m_startTimeCtrl;
        CDateTimeCtrl	m_startDateCtrl;
        CDateTimeCtrl   m_endTimeCtrl;
        CDateTimeCtrl	m_endDateCtrl;
        CString         m_name;
        CString         m_description;
        BOOL            m_isNowChecked;
        CString         m_offsetEdit;
        //}}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CTrendPropertiesDialog)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(CTrendPropertiesDialog)
        afx_msg void OnDestroy();
        virtual BOOL OnInitDialog();
        afx_msg void OnNowCheck();
        afx_msg void OnColourButton1();
        afx_msg void OnColourButton2();
        afx_msg void OnColourButton3();
        afx_msg void OnColourButton4();
        afx_msg void OnColourButton5();
        afx_msg void OnColourButton6();
        afx_msg void OnColourButton7();
        afx_msg void OnColourButton8();
        afx_msg void OnItemButton1();
        afx_msg void OnItemButton2();
        afx_msg void OnItemButton3();
        afx_msg void OnItemButton4();
        afx_msg void OnItemButton5();
        afx_msg void OnItemButton6();
        afx_msg void OnItemButton7();
        afx_msg void OnItemButton8();
        afx_msg void OnOK();
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
        afx_msg void OnSelchangeSampleCombo();
        afx_msg void OnStartTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnEndTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnStartDateChange(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnEndDateChange(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnSelchangeTypeCombo();
        afx_msg void OnSelchangeOffsetCombo();
        afx_msg void OnChangeOffsetEdit();
        afx_msg void OnDisplayNameChange1();
        afx_msg void OnDisplayNameChange2();
        afx_msg void OnDisplayNameChange3();
        afx_msg void OnDisplayNameChange4();
        afx_msg void OnDisplayNameChange5();
        afx_msg void OnDisplayNameChange6();
        afx_msg void OnDisplayNameChange7();
        afx_msg void OnDisplayNameChange8();
	    afx_msg void OnSave();
	    afx_msg void OnChangeDescEdit();
		afx_msg void OnDateChange(NMHDR* pNMHDR, LRESULT* pResult); //TD18018

 	//}}AFX_MSG
        afx_msg LRESULT OnLargeRange(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

    private:

        void initialiseItemList();
        void initialiseComboBoxes();
        void initialiseColourList();
        void populateSampleComboBox(long rangeSeconds);
        void setDateTimePickers();
        void loadTrendProperties();
        void setColourEditBackground(int index, COLORREF colour);
        void setNewColour(int index);
        void setItem(int index);
        void populateTrendPropertyItem(std::map<std::string, TA_Base_Core::ItemInfo>& items, const EGraphType& graphType);
        void populateExtraInfo(const EGraphType& graphType);
        void setExtraInfo(const EGraphType& graphType);
        void onRangeChange(int days);
        void checkArchivedDataNeeded(time_t startTime, long rangeSeconds);
        // void setControlsForXY(bool isXY); //TD17529 
		void setControlsForTrends(const EGraphType& graphType); //TD17529 
        bool hasCorrectItemCountIfXY();
        void setSamplePeriod();
        void adjustSamplePeriod();
        void loadSamplePeriod(long currentSamplePeriod, bool initialLoad = false);
        void onDisplayNameChange(int index);
        void retrieveLastModifiedTime();
        void updateCurrentTrendTemplate();
        void modificationsMade();
        void modificationsSaved();
        long getCurrentSamplePeriod();
        void populateSamplePeriods(time_t startTime, time_t endTime, long samplePeriod, bool initialLoad = false);
        void setLiveMode(bool isLive);
        COleDateTime getCombinedTime(const COleDateTime& date,const COleDateTime& time);
		void revertSavedTimeProperties(); //TD18018

    private:

        TrendProperties m_trendProperties;          // Properties for template displayed in dialog
        std::map<int, int> m_itemNamesId;           // Resource Ids for each item edit box
        std::map<int, int> m_colBoxId;              // Resource Ids for each item colour selector button
        std::map<int, int> m_displayNamesId;        // Resource Ids for each display name edit box
		//TD17529++ - add additional maps for the rest of the configurable resources
		std::map<int, int> m_staticItemsId;			// Resource Ids for each the static identifiers
		std::map<int, int> m_itemButtonsId;         // Resource Ids for each item selector button
		std::map<int, int> m_colButtonId;		    // Resource Ids for each colour selector button
		//++TD17529
        
        // The brushes used to paint the Colour edit controls. Four CBrush objects are
        // needed, one for each edit box. They are keyed on the edit boxes' index.
        typedef std::map< int, boost::shared_ptr<CBrush> > ResourceToBrushMap;
        ResourceToBrushMap m_itemColours;

        IItemStore& m_itemStore;                    // Object that stores all trendable items
        IArchiveDataStore& m_archiveDataStore;      // Object that stores all restored data dates

        bool m_changeSamplePeriod;                  // This flag indicates whether the sample period needs to
                                                    // be saved as a different value to what was retrieved
                                                    // from the sample period combo box. This is the case if '0'
                                                    // is selected.

        CString m_defaultOffsetValueStr;            // Default value for offset units displayed in dialog.

        bool m_disableNameField;                    // This flag indicates whether the name edit box should be disabled.

        bool m_initialising;

        static const COleDateTime EARLIEST_START_TIME;

        COleDateTime m_previousStartTime;
        COleDateTime m_previousEndTime;

        bool m_rangeTooLarge;

		int m_maxTrendsDisplayed; //TD17529

		long m_rangeInSeconds; //TD18018
		bool m_dateTimeChanged; //TD18018

    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}

#endif // !defined(AFX_TRENDPROPERTIES_H__3127E3B3_AB03_413C_9A4E_883BE22240CC__INCLUDED_)
