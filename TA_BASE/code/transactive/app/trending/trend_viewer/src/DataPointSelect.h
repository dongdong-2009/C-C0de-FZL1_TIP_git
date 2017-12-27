#if !defined(AFX_ItemSelectDialog_H__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_ItemSelectDialog_H__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * ItemSelectDialog.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/DataPointSelect.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This dialog is launched by the Trend Properties dialog. It lists the items
  * that can be added for trending.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    //
    // CItemSelectDialog
    //
    class CItemSelectDialog : public CDialog
    {
    public:

        /**
		  * CItemSelectDialog
		  * 
		  * Constructor.
		  *
		  */
        CItemSelectDialog(const std::string& itemName, CWnd* pParent = NULL);

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CItemSelectDialog)
    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(CItemSelectDialog)
        virtual BOOL OnInitDialog();
        virtual void OnOK();
	    afx_msg void OnFindButton();
	    afx_msg void OnClearButton();
	    //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        
        void InitialiseDataPointList();
        void SortDatapointList();
        void LoadDataPointList();
        static int CALLBACK CompareEvents(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    public:

        const std::vector <unsigned long>& GetDataPointKeyList() 
        {
            return m_DataPointKeyList;
        }

    public:

        // Dialog Data
        //{{AFX_DATA(CItemSelectDialog)
	    enum { IDD = IDD_DATAPOINT_SELECT };
        CListCtrl	m_DataPointList;
	    CString	m_searchString;
	    //}}AFX_DATA

    private:

        bool m_IsDelete;                                // True if deleting, False if adding
        DataPointMap m_DPMap;                           // Datapoint list
        CString m_DialogTitle;                          // Title for the dialog box
        std::vector <unsigned long> m_DataPointKeyList; // Array of the selected datapoint keys
        CHistoryViewerDlg* m_pParent;   
        int m_maxTrendsDisplayed;
    };
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ItemSelectDialog_H__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)




