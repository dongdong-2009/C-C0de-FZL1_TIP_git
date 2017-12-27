#if !defined(AFX_ItemSelectDialog_CPP__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_ItemSelectDialog_CPP__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * ItemSelectDialog.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/DataPointSelect.cpp $
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

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/ItemSelectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{
    //
    // CItemSelectDialog
    //
    CItemSelectDialog::CItemSelectDialog(const std::string& itemName, CWnd* pParent /*=NULL*/)
        : m_IsDelete( isDelete ),
          CDialog(CItemSelectDialog::IDD, pParent)
    {
        //{{AFX_DATA_INIT(CItemSelectDialog)
	    m_searchString = _T("<sub-string>");
	    //}}AFX_DATA_INIT

        m_pParent = dynamic_cast<CHistoryViewerDlg*>(pParent);

        // Set to either add or delete
        if( m_IsDelete )
        {
	        m_DPMap = CDataPointInfo::getInstance().GetGraphingDataPointMap();
	        m_DialogTitle = "Select Datapoint To Delete";
        }
        else
        {
	        m_DPMap = CDataPointInfo::getInstance().GetDataPointMap();
	        m_DialogTitle = "Select Datapoint To Add";
        }

        std::string maxTrends = TA_Base_Core::RunParams::getInstance().get(RPARAM_MAX_TRENDS_DISPLAYED);
        if(maxTrends.empty())
        {
            m_maxTrendsDisplayed = CFG_MAX_TRENDS_DISPLAYED_DEFAULT;
        }
        else
        {
            m_maxTrendsDisplayed = atoi(maxTrends.c_str());
        }
    }


    //
    // DoDataExchange
    //
    void CItemSelectDialog::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CItemSelectDialog)
        DDX_Control(pDX, IDC_DATAPOINT_LIST, m_DataPointList);
	    DDX_Text(pDX, IDC_SEARCH_EDIT, m_searchString);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CItemSelectDialog, CDialog)
        //{{AFX_MSG_MAP(CItemSelectDialog)
	ON_BN_CLICKED(IDC_FIND_BUTTON, OnFindButton)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, OnClearButton)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CItemSelectDialog message handlers


    //
    // OnInitDialog
    //
    BOOL CItemSelectDialog::OnInitDialog() 
    {
        CDialog::OnInitDialog();

        // Set the dialog title
        SetWindowText( m_DialogTitle );
        
        // Configure the list and load the datapoints
        m_DataPointList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
        InitialiseDataPointList();

        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    //
    // InitialiseDataPointList
    //
    void CItemSelectDialog::InitialiseDataPointList()
    {
        // Insert columns
        LV_COLUMN lvc;

        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.fmt = LVCFMT_LEFT;

        // Insert Name column
        lvc.iSubItem = 0;
        lvc.pszText = "Name";
        lvc.cx = 250;
        m_DataPointList.InsertColumn( 0, &lvc );

        // Insert Description column
        lvc.iSubItem = 1;
        lvc.pszText = "Description";
        lvc.cx = 250;
        m_DataPointList.InsertColumn( 1, &lvc );

        // Insert Trending Enabled column
        lvc.iSubItem = 2;
        lvc.pszText = "Trending";
        lvc.cx = 150;
        m_DataPointList.InsertColumn( 2, &lvc );

        // Go through the map and put the data into the list
        LoadDataPointList();
    }


    //
    // OnOK
    //
    void CItemSelectDialog::OnOK() 
    {
        int iItem = -1;
        unsigned long dataPointKey;
        int selCount = m_DataPointList.GetSelectedCount();

        int currentDisplayCnt = m_pParent->getCurrentGraphCount();
        if(m_IsDelete)
        {
            //
            // If we're deleting, then set this back to zero because
            // we don't care how many are displayed
            //
            currentDisplayCnt = 0;
        }

        //
        // Limit the maximum number of trends to be displayed
        // If more than the maximum allowed amount is selected, then
        // display a warning dialog and only display the maximum
        // allowed amount.
        //
        if(((selCount + currentDisplayCnt) > m_maxTrendsDisplayed) && !m_IsDelete)
        {
            char buffer[256];
            sprintf(buffer, "%d datapoints selected. A maximum of %d trends allowed displayed. "
                "Not all selected datapoints will be displayed.", 
                selCount, m_maxTrendsDisplayed);
            if(MessageBox(buffer, "Trend Viewer", MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
            {
                return;
            }
        }

        bool trendDisabledSel(false);
        m_DataPointKeyList.clear();
        if( selCount > 0 )
        {
	        for( int i=0; i<selCount && i<m_maxTrendsDisplayed-currentDisplayCnt; i++ )
	        {
		        iItem = m_DataPointList.GetNextItem( iItem, LVNI_SELECTED );
		        TA_ASSERT(iItem != -1,  "Item selected does not have valid index");
		        dataPointKey = m_DataPointList.GetItemData( iItem );

                if(!m_IsDelete)
                {
                    char buffer[100];
                    m_DataPointList.GetItemText(iItem, 2, buffer, 100);
                    if(strcmp(buffer, TRENDING_DISABLED) == 0)
                    {
                        trendDisabledSel = true;
                    }
                }

		        m_DataPointKeyList.push_back( dataPointKey );
	        }
            
        }

        if(trendDisabledSel && !m_pParent->m_NowCheck && !m_IsDelete)
        {
            MessageBox("Datapoints without trending data have been selected. These points "
                "will not be displayed until graph is in 'Live' mode.", "Warning", MB_ICONWARNING);
        }
        
        CDialog::OnOK();
    }


    //
    // OnFindButton
    //
    void CItemSelectDialog::OnFindButton() 
    {
	    UpdateData(TRUE);
        m_DataPointList.DeleteAllItems();

        // Convert whole search string to upper case as we want the find to be
        // context insensitive.
        m_searchString.MakeUpper();

        // Go through the map and put the data into the list
        DataPointMap::iterator it;
        int i, insertPoint;
        for ( i=0, it = m_DPMap.begin(); it != m_DPMap.end(); it++, i++ )
        {
            CString name = it->second.entityName.c_str();

            // Convert whole string to upper case as we want the find to be
            // case insensitive.
            name.MakeUpper();
            
            if(name.Find(m_searchString) == -1)
            {
                continue;
            }

	        insertPoint = m_DataPointList.InsertItem( i, it->second.entityName.c_str() );
	        m_DataPointList.SetItem( insertPoint, 1, LVIF_TEXT, it->second.description.c_str(), 0, 0, 0, 0 );
            m_DataPointList.SetItem( insertPoint, 2, LVIF_TEXT, it->second.trendingEnabled ? TRENDING_ENABLED : TRENDING_DISABLED, 0, 0, 0, 0 );
            m_DataPointList.SetItemData( insertPoint, it->second.entityKey );
        }

        // Sort list by Enabled...
        SortDatapointList();
    }
	    
    
    //
    // OnClearButton
    //
    void CItemSelectDialog::OnClearButton() 
    {
	    m_searchString = _T("<sub-string>");
        UpdateData(FALSE);
        m_DataPointList.DeleteAllItems();

        // 
        // Reload the list
        //
        LoadDataPointList();
    }


    //
    // SortDatapointList
    //
    void CItemSelectDialog::SortDatapointList()
    {
        m_DataPointList.SortItems(CompareEvents, (LPARAM) this);
    }


    //
    // CompareEvents
    //
    int CALLBACK CItemSelectDialog::CompareEvents(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
	    CItemSelectDialog* pDlg = (CItemSelectDialog*) lParamSort;

	    LVFINDINFO info1;
	    LVFINDINFO info2;

	    info1.flags = LVFI_PARAM;
	    info2.flags = LVFI_PARAM;
	    info1.lParam = lParam1;
	    info2.lParam = lParam2;

	    int index1 = pDlg->m_DataPointList.FindItem(&info1);
	    int index2 = pDlg->m_DataPointList.FindItem(&info2);

	    int returnVal = 0; 
	    CString string1;
	    CString string2;
	    int shortestString(0);

		string1 = pDlg->m_DataPointList.GetItemText(index1,2);
		string2 = pDlg->m_DataPointList.GetItemText(index2,2);
		returnVal = -(string1.CompareNoCase(string2));

        if (returnVal == 0) // Then we have two items them same so now sort by the name.
	    {
		    string1 = pDlg->m_DataPointList.GetItemText(index1,0);
		    string2 = pDlg->m_DataPointList.GetItemText(index2,0);
		    returnVal = string1.CompareNoCase(string2);
	    }

	    return returnVal;
    }


    //
    // LoadDataPointList
    //
    void CItemSelectDialog::LoadDataPointList()
    {
        // Go through the map and put the data into the list
        DataPointMap::iterator it;
        int i, insertPoint;
        for ( i=0, it = m_DPMap.begin(); it != m_DPMap.end(); it++, i++ )
        {
	        insertPoint = m_DataPointList.InsertItem( i, it->second.entityName.c_str() );
	        m_DataPointList.SetItem( insertPoint, 1, LVIF_TEXT, it->second.description.c_str(), 0, 0, 0, 0 );
            m_DataPointList.SetItem( insertPoint, 2, LVIF_TEXT, it->second.trendingEnabled ? TRENDING_ENABLED : TRENDING_DISABLED, 0, 0, 0, 0 );
            m_DataPointList.SetItemData( insertPoint, it->second.entityKey );
        }

        // Sort list by Enabled...
        SortDatapointList();
    }
}


#endif // !defined(AFX_ItemSelectDialog_CPP__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)





