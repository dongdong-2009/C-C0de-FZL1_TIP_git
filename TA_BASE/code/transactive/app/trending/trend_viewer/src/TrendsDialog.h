/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendsDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_TRENDSDIALOG_H__0E35842D_9557_46F8_B8C0_F0F2936E0773__INCLUDED_)
#define AFX_TRENDSDIALOG_H__0E35842D_9557_46F8_B8C0_F0F2936E0773__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrendsDialog.h : header file
//

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 

#include "app/trending/trend_viewer/src/resource.h"
#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"
#include "app/trending/trend_viewer/src/IItemStore.h"
#include "app/trending/trend_viewer/src/IArchiveDataStore.h"

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h" //TD16561 

/////////////////////////////////////////////////////////////////////////////
// TrendsDialog dialog


namespace TA_Base_App
{
    class CTrendsDialog : public CDialog
    {
    // Construction
    public:
        CTrendsDialog(IItemStore& itemStore, IArchiveDataStore& archiveDataStore, bool loadingComplete, CWnd* pParent = NULL);   // standard constructor

        TrendProperties getTrendProperties()
        {
            return m_trendProperties;
        }

		void enableNewButton(); // TD12625 azenitha

		void setMaxTrendsToDisplay(const int newMaxTrendsDisplayed)  //TD17529
		{
			m_maxTrendsDisplay = newMaxTrendsDisplayed;
		}
        
    // Dialog Data
        //{{AFX_DATA(TrendsDialog)
        enum { IDD = IDD_TRENDS_DLG };
        TA_Base_Bus::ListCtrlSelNoFocus   m_trendsList; //TD16561 
		CButton m_newButton; //TD17812
            // NOTE: the ClassWizard will add data members here
        //}}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CTrendsDialog)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(CTrendsDialog)
        afx_msg void OnDestroy();
        virtual BOOL OnInitDialog();
        afx_msg void OnClickTrendsList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnOK();
        afx_msg void OnNew();
	    afx_msg void OnDelete();
        afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        CTrendsDialog(const CTrendsDialog&);
        CTrendsDialog& operator=(const CTrendsDialog&);

        void loadTrendsList();
        void setUpColumnHeadings();
        void checkArchivedDataNeeded(time_t startTime, long rangeSeconds);
        void setButtonAvailability(bool disable = true);

        typedef struct
        {
            TrendProperties trendProperties;
            std::string sortColumn;
            bool sortAscending;

        } TrendPropertiesSortDetails;

        static bool compare(TrendPropertiesSortDetails& arg1,TrendPropertiesSortDetails& arg2);

    private:

        TrendProperties m_trendProperties;                          // Properties of the selected template
        std::auto_ptr<TrendPropertiesStore> m_trendPropertiesStore; // Object that stores all the templates loaded from database
        IItemStore& m_itemStore;                                    // Object that stores all the trendable items
        IArchiveDataStore& m_archiveDataStore;                      // Object that stores all the restored data dates
        bool m_sortAscending;                                       // Flag indicating sort order of list
        std::string m_sortColumn;                                   // Column the current sort is on
        bool m_loadingComplete;
		int m_maxTrendsDisplay;										//TD17529

        static const char* NAME_COLUMN;
        static const char* DESCRIPTION_COLUMN;
        static const char* GRAPH_TYPE_COLUMN;

        static const int NAME_COL_WIDTH;
        static const int TYPE_COL_WIDTH;
        static const int DESC_COL_WIDTH;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}

#endif // !defined(AFX_TRENDSDIALOG_H__0E35842D_9557_46F8_B8C0_F0F2936E0773__INCLUDED_)
