/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/SuppressionRulesDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_SUPPRESSIONRULESDIALOG_H__84766AF0_C06B_44B7_9983_B23FECF8539A__INCLUDED_)
#define AFX_SUPPRESSIONRULESDIALOG_H__84766AF0_C06B_44B7_9983_B23FECF8539A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SuppressionRulesDialog.h : header file
//
#include <map>
#include "app/alarm/alarm_manager/src/resource.h"
#include "core\message\src\ConfigUpdateMessageSender.h"

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef;
	class IAlarmRuleData;
    class SuppressionRuleData;
}


namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CSuppressionRulesDialog dialog

    class CSuppressionRulesDialog : public CDialog
    {
    // Construction
    public:
	    CSuppressionRulesDialog(unsigned long profileId, CWnd* pParent = NULL);   // standard constructor
        
    // Dialog Data
	    //{{AFX_DATA(CSuppressionRulesDialog)
	enum { IDD = IDD_RULES };
	CComboBox	m_locationCombo;
	CListCtrl	m_rulesList;
	//}}AFX_DATA

        
    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSuppressionRulesDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(CSuppressionRulesDialog)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnRemoveButton();
	    afx_msg void OnRadioFilterLocation();
	    afx_msg void OnRadioShowAll();
	    afx_msg void OnClickRulesList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnClose();
		afx_msg void OnDestroy();
	    afx_msg void OnRefresh();
	    afx_msg void OnExport();
	    afx_msg void OnPrint();
	    afx_msg void OnTimer(UINT nIDEvent);
		afx_msg void OnSelchangeComboLocations();
		//void CALLBACK EXPORT TimerCallback (HWND hWnd, UINT nMsg, UINT nTimerID, DWORD dwTime);
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:

        void setUpHeadings();
		void populateLocationCombo();
        void loadAllRules();
        void loadRulesByLocation(unsigned long locationKey);
        void sendConfigUpdate(TA_Base_Core::IAlarmRuleData* suppressionRule);
        void displayErrorMessage();
        bool hasPermissionToRemoveRules();
		bool hasPermissionToViewAllRules();
        void populateList(std::vector<TA_Base_Core::SuppressionRuleData*>& suppressionRules);
        bool filterByRegionAssignment(std::vector<TA_Base_Core::SuppressionRuleData*>& suppressionRules);
        void clearList();
        bool getCurrentOperatorsRegionAssignment(std::vector<unsigned long>& regionKeys);
        bool getRegionKeyByEntity(unsigned long entityKey, unsigned long& regionKey);
        void sortList(std::vector<TA_Base_Core::SuppressionRuleData*>& suppressionRules);
       
    private:

        typedef struct
        {
            bool isCreatedByCurrentProfile;
            TA_Base_Core::SuppressionRuleData* suppressionRule;
        } RuleInfo;

		// refresh timer
		static const int INHIBITION_DLG_REFRESH_CALLBACK;
		static const std::string REFRESH_TIME_PARAM;

		// Default refresh time for list
		static const int DEFAULT_REFRESH_TIME;

        unsigned long m_profileId;
		unsigned long m_currentLocationKey;

		// Column 0
        static const int REMOVABLE_COLUMN;
        static const char* REMOVABLE_COLUMN_NAME;
        static const int REMOVABLE_COLUMN_WIDTH;

		// Column 1
        static const int DATE_TIME_COLUMN;
        static const char* DATE_TIME_COLUMN_NAME;
        static const int DATE_TIME_COLUMN_WIDTH;

		// Column 2
        static const int ASSET_NAME_COLUMN;
        static const char* ASSET_NAME_COLUMN_NAME;
        static const int ASSET_NAME_COLUMN_WIDTH;

		// Column 3
        static const int ALARM_TYPE_COLUMN;
        static const char* ALARM_TYPE_COLUMN_NAME;
        static const int ALARM_TYPE_COLUMN_WIDTH;

		// Column 4
        static const int DESCRIPTION_COLUMN;
        static const char* DESCRIPTION_COLUMN_NAME;
        static const int DESCRIPTION_COLUMN_WIDTH;

		// Column 5
        static const int VALUE_COLUMN;
        static const char* VALUE_COLUMN_NAME;
        static const int VALUE_COLUMN_WIDTH;

		// Column 6
        static const int OPERATOR_COLUMN;
        static const char* OPERATOR_COLUMN_NAME;
        static const int OPERATOR_COLUMN_WIDTH;

        static const int REMOVABLE_IMAGE;
        static const int BLANK_IMAGE;

        typedef enum
        {
            REMOVABLE = 0,
            DATE_TIME,
            ASSET_NAME,
			ALARM_TYPE,
			DESCRIPTION,
			VALUE,
			OPERATOR
        } EColumnType;
        
		CWnd* m_parent;
        bool m_sortAscending;
		int m_refreshTime;
        EColumnType m_sortColumn;
		std::map<std::string, unsigned long> m_locationNameKeyMap;

        CImageList     m_imageList;
        HICON          m_removableIcon;
        HICON          m_blankIcon;
		
		CFont			m_font;

		// Vector of image/icon legend/explanation string/text for printing purposes
		std::vector<CString> m_imageStringVector; 
	    
    };

} // end TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPPRESSIONRULESDIALOG_H__84766AF0_C06B_44B7_9983_B23FECF8539A__INCLUDED_)
