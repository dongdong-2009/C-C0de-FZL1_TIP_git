/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendsDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// TrendsDialog.cpp : implementation file
//

#include <map>

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/TrendsDialog.h"
#include "app/trending/trend_viewer/src/TrendPropertiesDialog.h"
#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/Utilities.h"

#include "core/utilities/src/FunctionEntry.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/TrendViewerException.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "bus/resources/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using TA_Base_Core::DebugUtil;
using TA_Base_Core::TrendViewerException;

namespace TA_Base_App
{
    const char* CTrendsDialog::NAME_COLUMN = "Name";
    const char* CTrendsDialog::DESCRIPTION_COLUMN = "Description";
    const char* CTrendsDialog::GRAPH_TYPE_COLUMN = "Type";

    const int CTrendsDialog::NAME_COL_WIDTH = 150;
    const int CTrendsDialog::TYPE_COL_WIDTH = 100;
    const int CTrendsDialog::DESC_COL_WIDTH = 403; //TD17933

    /////////////////////////////////////////////////////////////////////////////
    // TrendsDialog dialog

    CTrendsDialog::CTrendsDialog(IItemStore& itemStore, IArchiveDataStore& archiveDataStore, bool loadingComplete, CWnd* pParent /*=NULL*/)
        : CDialog(CTrendsDialog::IDD, pParent),
          m_trendPropertiesStore(NULL),
          m_itemStore(itemStore),
          m_archiveDataStore(archiveDataStore),
          m_sortColumn(NAME_COLUMN),
          m_sortAscending(true),
          m_loadingComplete(loadingComplete),
		  m_maxTrendsDisplay(MAX_ITEMS_DEFAULT) //TD17529

    {
        FUNCTION_ENTRY("CTrendsDialog");

        //{{AFX_DATA_INIT(CTrendsDialog)
            // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT

        // Initialise Trend Properties member variable.
        m_trendProperties.name = "";
        m_trendProperties.description = "";
        m_trendProperties.graphType = LINE;
        m_trendProperties.endDateTime = 3600;
        m_trendProperties.startDateTime = 0;
        m_trendProperties.samplePeriod = DEFAULT_SAMPLE_PERIOD; //TD17529
        m_trendProperties.live = 1;
        m_trendProperties.items.clear();
        m_trendProperties.extraInfo = "";

        FUNCTION_EXIT;
    }


    void CTrendsDialog::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CTrendsDialog)
        DDX_Control(pDX, IDC_TREND_LIST, m_trendsList);
		DDX_Control(pDX, IDC_NEW, m_newButton); //TD17812
            // NOTE: the ClassWizard will add DDX and DDV calls here
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CTrendsDialog, CDialog)
        //{{AFX_MSG_MAP(CTrendsDialog)
            ON_NOTIFY(NM_CLICK, IDC_TREND_LIST, OnClickTrendsList)
            ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_TREND_LIST, OnColumnClick)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // TrendsDialog message handlers


    //
    // OnInitDialog
    //
    BOOL CTrendsDialog::OnInitDialog()
    {
        FUNCTION_ENTRY("OnInitDialog");

        CDialog::OnInitDialog();

        m_trendsList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
       
        setUpColumnHeadings();
        loadTrendsList();

        // Disable the Display and Delete buttons until an item has been selected
        GetDlgItem(IDOK)->EnableWindow(false);
        GetDlgItem(IDC_DELETE)->EnableWindow(false);
		//TD17812
        // GetDlgItem(IDC_NEW)->EnableWindow(m_loadingComplete); 
		m_newButton.EnableWindow(m_loadingComplete); 
		//++TD17812

        return TRUE;
    }


    //
    // OnOK
    //
    void CTrendsDialog::OnOK()
    {
        FUNCTION_ENTRY("OnOK");

        CWaitCursor waitCursor;

        // Display button has been pressed
        POSITION pos = m_trendsList.GetFirstSelectedItemPosition();

        TA_ASSERT(pos != NULL, "No items have been selected");

        int nItem = m_trendsList.GetNextSelectedItem(pos);
        CString itemName = m_trendsList.GetItemText(nItem, 0);

        try
        {
            m_trendProperties = m_trendPropertiesStore->getTrendProperties(itemName.GetBuffer(0));
        }
        catch(const TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            if(ex.getType() == TrendViewerException::TEMPLATE_NOT_EXIST)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << itemName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120038);

                // Remove the item from the list
                m_trendsList.DeleteItem(nItem);
                setButtonAvailability();

                FUNCTION_EXIT;
                return;
            }

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120028);

            FUNCTION_EXIT;
            return;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120028);

            FUNCTION_EXIT;
            return;
        }

        // Launch a warning dialog if archive data needs to be restored to display all points
        long rangeSeconds = m_trendProperties.endDateTime - m_trendProperties.startDateTime;
        checkArchivedDataNeeded(m_trendProperties.startDateTime, rangeSeconds);

        CDialog::OnOK();
        FUNCTION_EXIT;
    }


    //
    // OnNew
    //
    void CTrendsDialog::OnNew() 
    {
        FUNCTION_ENTRY("OnNew");

        // Launch Trend Properties dialog with blank fields
        CTrendPropertiesDialog trendPropertiesDlg(m_itemStore, m_archiveDataStore);
		trendPropertiesDlg.setMaxTrendsToDisplay(m_maxTrendsDisplay); //TD17529

        try
        {
            UINT result = trendPropertiesDlg.DoModal();
            if(result == IDOK)
            {
                // Display has been pressed
                m_trendProperties = trendPropertiesDlg.getTrendProperties();

                CDialog::OnOK();
            }
            else if(result == IDCANCEL)
            {
                // Reload the list in case a new one was added
                m_trendsList.DeleteAllItems();
                loadTrendsList();
            }
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            std::string msg("");
            if(ex.getType() == TA_Base_Core::TrendViewerException::DIALOG_INIT_ERROR)
            {
                msg = "Error launching dialog: ";
                msg += ex.what();
            }
            else
            {
                msg = "Unknown error occured.";
            }

            msg += " Cannot launch TrendProperties dialog.";
            
			// TD14164 azenitha++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = msg.c_str();
			userMsg << reason;
			CString errMsg = userMsg.constructMessage(IDS_UE_020071);
            /*AfxMessageBox(msg.c_str(), MB_ICONERROR);*/
            AfxMessageBox(errMsg, MB_ICONERROR);
			// TD14164 ++azenitha
        }

        FUNCTION_EXIT;
    }


    //
    // OnDelete
    //
    void CTrendsDialog::OnDelete() 
    {
	    FUNCTION_ENTRY("OnDelete");

        POSITION pos = m_trendsList.GetFirstSelectedItemPosition();

        TA_ASSERT(pos != NULL, "No items have been selected");

        int nItem = m_trendsList.GetNextSelectedItem(pos);
        CString itemName = m_trendsList.GetItemText(nItem, 0);

        try
        {
            TrendProperties trendProperties = m_trendPropertiesStore->getTrendProperties(itemName.GetBuffer(0));
            time_t lastModified = trendProperties.lastModified;

            m_trendPropertiesStore->removeTrendProperties(itemName.GetBuffer(0), lastModified);
            m_trendsList.DeleteItem(nItem);
            setButtonAvailability();

            FUNCTION_EXIT;
            return;
        }
        catch(const TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            if(ex.getType() == TrendViewerException::TEMPLATE_MODIFIED)
            {
                // Template has been modified since user has viewed it. Give warning and 
                // allow them to cancel if desired.
                std::string action("delete");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_trendProperties.name << action;
            
                if(userMsg.showMsgBox(IDS_UE_120039) == IDOK)
                {
                    try
                    {
                        m_trendPropertiesStore->forceRemoveTrendProperties(m_trendProperties.name);
                        m_trendsList.DeleteItem(nItem);
                        setButtonAvailability();

                        FUNCTION_EXIT;
                        return;
                    }
                    catch(const TrendViewerException& ex)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

                        if(ex.getType() == TrendViewerException::DATABASE_ERROR)
                        {
                            TA_Base_Bus::TransActiveMessage userMsg;
                            userMsg << ex.what();
                            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120037);
                        }
                        else
                        {
                            std::string reason("Unknown");
                            TA_Base_Bus::TransActiveMessage userMsg;
                            userMsg << reason;
                            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120037);
                        }
                    }
                    catch(...)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

                        std::string action("delete");
                        std::string reason("an unexpected error occurred.");
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << action << reason;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);
                    }
                }

                FUNCTION_EXIT;
                return;
            }
            
            if(ex.getType() == TrendViewerException::TEMPLATE_NOT_EXIST)
            {
                // Template has already been deleted. Just remove it from the list.
                m_trendsList.DeleteItem(nItem);
                setButtonAvailability();

                FUNCTION_EXIT;
                return;
            }
        }
        catch(...)
        {
        }
        
        std::string action("delete");
        std::string reason("an unexpected error occurred.");
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << action << reason;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);

	    FUNCTION_EXIT;    
    }


    //
    // OnDestroy
    //
    void CTrendsDialog::OnDestroy() 
    {
        FUNCTION_ENTRY("OnDestroy");

        FUNCTION_EXIT;
    }


    //
    // OnClickTrendsList
    //
    void CTrendsDialog::OnClickTrendsList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        POSITION pos = m_trendsList.GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            GetDlgItem(IDOK)->EnableWindow(false);
            GetDlgItem(IDC_DELETE)->EnableWindow(false);
        }
        else
        {
            GetDlgItem(IDOK)->EnableWindow(true);
            GetDlgItem(IDC_DELETE)->EnableWindow(true);
        }
    }


    //
    // OnColumnClick
    //
    void CTrendsDialog::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        static int oldSortColumn = 0;

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Want to get the header item text (to get the column name)
        HDITEM headerItem;
        char headerName[1000];

        headerItem.mask = HDI_TEXT | HDI_FORMAT | HDI_BITMAP;;
        headerItem.pszText = headerName;
        headerItem.cchTextMax = 1000;
        CHeaderCtrl* headerCtrl = m_trendsList.GetHeaderCtrl(); 
        headerCtrl->GetItem(pNMListView->iSubItem, &headerItem); 

        // Toggle order or change to new column
        if(m_sortColumn == headerName)
        {
	        m_sortAscending = !m_sortAscending;
        }
        else
        {
            m_sortColumn = headerName;
            m_sortAscending = true;
        }


        // If the header currently has a bitmap set, delete it.
	    if (headerItem.hbm != 0) 
	    { 
		    DeleteObject(headerItem.hbm); 
		    headerItem.hbm = 0; 
	    }

        // Add the bitmap and on right formats to the header item
	    headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 
    
	    // Load the required image
	    headerItem.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), 
            MAKEINTRESOURCE((m_sortAscending) ? IDB_ASCENDING : IDB_DESCENDING), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS); 

        // Set the header item with the new image
	    headerCtrl->SetItem(pNMListView->iSubItem, &headerItem); 

        // If the previous sort was not on this column
	    if (oldSortColumn != pNMListView->iSubItem) 
	    {
		    // Remove image from original sort column
		    headerCtrl->GetItem(oldSortColumn, &headerItem); 
		    headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 
		    if (headerItem.hbm != 0) 
		    { 
			    DeleteObject(headerItem.hbm); 
			    headerItem.hbm = 0; 
		    } 
		    headerCtrl->SetItem(oldSortColumn, &headerItem);
        }

        oldSortColumn = pNMListView->iSubItem;

        
        // Resort the list
        m_trendsList.DeleteAllItems();
        loadTrendsList();

        *pResult = 0;
    }

    //
    // setUpColumnHeadings
    //
    void CTrendsDialog::setUpColumnHeadings()
    {
        FUNCTION_ENTRY("setUpColumnHeadings"); // //TD17770 - correct function name entry

        m_trendsList.InsertColumn(0, NAME_COLUMN, LVCFMT_LEFT, NAME_COL_WIDTH);
        m_trendsList.InsertColumn(1, GRAPH_TYPE_COLUMN, LVCFMT_LEFT, TYPE_COL_WIDTH);
        m_trendsList.InsertColumn(2, DESCRIPTION_COLUMN, LVCFMT_LEFT, DESC_COL_WIDTH);            

        FUNCTION_EXIT;
    }


    //
    // loadTrendsList
    //
    void CTrendsDialog::loadTrendsList()
    {
        FUNCTION_ENTRY("loadTrendsList");

        std::map<std::string, TrendProperties> trends;
        std::vector<TrendPropertiesSortDetails> sortedTrends;		

        try
        {
            m_trendPropertiesStore = std::auto_ptr<TrendPropertiesStore> (new TrendPropertiesStore);
            trends = m_trendPropertiesStore->getAllTrendProperties();

            std::map<std::string, TrendProperties>::iterator iter = trends.begin();
            for(; iter != trends.end(); ++iter)
            {
                TrendPropertiesSortDetails sortDetails;
                sortDetails.trendProperties = iter->second;
                sortDetails.sortColumn = m_sortColumn;
                sortDetails.sortAscending = m_sortAscending;

                sortedTrends.push_back(sortDetails);
            }

            // Sort the list
            std::vector<TrendPropertiesSortDetails>::iterator iter1 = sortedTrends.begin();
            std::sort(iter1, (sortedTrends.end()), compare);
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());

            if(ex.getType() == TA_Base_Core::TrendViewerException::TEMPLATE_LOADING_ERROR)
            {
                std::string msg("Error ocurred while retrieving trend templates: ");
                msg += ex.what();

				// TD14164 azenitha++
                /*AfxMessageBox(msg.c_str(), MB_ICONERROR);*/
				TA_Base_Bus::TransActiveMessage userMsg;
				CString temp = ex.what();
				CString description = "retrieving trend templates" + temp;
				userMsg << description;
				UINT selectedButton = userMsg.showMsgBox(IDS_UW_632015);
				// TD14164 ++azenitha

                FUNCTION_EXIT;
                return;
            }

			// TD14164 azenitha++
            /*AfxMessageBox("Unknown error ocurred while retrieving trend templates", MB_ICONERROR);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString description = "retrieving trend templates";
			userMsg << description;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_120043);
			// TD14164 ++azenitha

            FUNCTION_EXIT;
            return;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

			// TD14164 azenitha++
            /*AfxMessageBox("Unknown error ocurred while retrieving trend templates", MB_ICONERROR);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString description = "retrieving trend templates";
			userMsg << description;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_120043);
			// TD14164 ++azenitha

            FUNCTION_EXIT;
            return;
        }

        int itemCnt = 0;
        std::vector<TrendPropertiesSortDetails>::iterator it = sortedTrends.begin();
        for(; it != sortedTrends.end(); ++it)
        {
            TrendProperties trendProperties = it->trendProperties;
            
            std::string graphType("");
            try
            {
                 graphType = TrendPropertiesStore::getGraphTypeAsString(trendProperties.graphType);
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid graph type retrieved. Not loading template: %s", trendProperties.name.c_str());
                continue;
            }
            
            m_trendsList.InsertItem(itemCnt, trendProperties.name.c_str());
            m_trendsList.SetItemText(itemCnt, 1, graphType.c_str());
            m_trendsList.SetItemText(itemCnt, 2, trendProperties.description.c_str());

            ++itemCnt;
        }

        FUNCTION_EXIT;
    }


    //
    // checkArchivedDataNeeded
    //
    //void CTrendsDialog::checkArchivedDataNeeded(time_t startTime, int rangeDays, long rangeTime)
    void CTrendsDialog::checkArchivedDataNeeded(time_t startTime, long rangeSeconds)
    {
        FUNCTION_ENTRY("checkArchivedDataNeeded");

        CTime nowTime = CTime::GetCurrentTime();
        time_t earliestDate = nowTime.GetTime() - MAX_DATABASE_STORAGE_DAYS * 24 * 60 * 60;
        
        if(startTime < earliestDate)
        {
            time_t endTime = startTime + rangeSeconds;
            if(!m_archiveDataStore.isAllDataRestored(startTime, endTime))
            {
                // Need to restore archived data before all points can be displayed
                std::string unrestoredDates = m_archiveDataStore.getUnrestoredDates(startTime, endTime);

                std::string warning("Archive data needs to be restored before all points can be displayed. The following dates need to be restored: ");
                warning += unrestoredDates;

				// TD14164 azenitha++
                /*AfxMessageBox(warning.c_str());*/
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << unrestoredDates.c_str();
				UINT selectedButton = userMsg.showMsgBox(IDS_UE_120034);
				// TD14164 ++azenitha
            }
        }

        FUNCTION_EXIT;
    }


    //
    // setButtonAvailability
    //
    void CTrendsDialog::setButtonAvailability(bool disable /* = true */)
    {
        FUNCTION_ENTRY("setButtonAvailability");

        // Disable the Display and Delete button if there are no items in the list. These 
        // buttons will be enabled when an item is selected.
        if(m_trendsList.GetItemCount() == 0)
        {
            GetDlgItem(IDOK)->EnableWindow(false);
            GetDlgItem(IDC_DELETE)->EnableWindow(false);
        }
        
        if(disable)
        {
            GetDlgItem(IDOK)->EnableWindow(false);
            GetDlgItem(IDC_DELETE)->EnableWindow(false);
        }

        FUNCTION_EXIT;
    }


    bool CTrendsDialog::compare(TrendPropertiesSortDetails& arg1,TrendPropertiesSortDetails& arg2)
    {
        std::string sortColumn = arg1.sortColumn;
        bool sortAscending = arg1.sortAscending;

        if(sortColumn == NAME_COLUMN)
        {
            if(sortAscending)
            {
				//TD17770 - use insensitive compare
                return (stricmp(arg1.trendProperties.name.c_str(), arg2.trendProperties.name.c_str()) < 0);
            }
            else
            {
				//TD17770 - use insensitive compare
                return (stricmp(arg1.trendProperties.name.c_str(), arg2.trendProperties.name.c_str()) >= 0);
            }
        }
        else if(sortColumn == GRAPH_TYPE_COLUMN)
        {
            if(sortAscending)
            {
				//TD18016 - modified compare operator to avoid hang
                return (strcmp(TrendPropertiesStore::getGraphTypeAsString(arg1.trendProperties.graphType).c_str(), 
                    TrendPropertiesStore::getGraphTypeAsString(arg2.trendProperties.graphType).c_str()) < 0);
            }
            else
            {
				//TD18016 - modified compare operator to avoid hang
                return (strcmp(TrendPropertiesStore::getGraphTypeAsString(arg1.trendProperties.graphType).c_str(), 
                    TrendPropertiesStore::getGraphTypeAsString(arg2.trendProperties.graphType).c_str()) > 0);
            }
        }
        else if(sortColumn == DESCRIPTION_COLUMN)
        {
            if(sortAscending)
            {
				//TD17770 - use insensitive compare
                return (stricmp(arg1.trendProperties.description.c_str(), arg2.trendProperties.description.c_str()) < 0);
            }
            else
            {
				//TD17770 - use insensitive compare
                return (stricmp(arg1.trendProperties.description.c_str(), arg2.trendProperties.description.c_str()) >= 0);
            }
        }
        else
        {
            TA_ASSERT(false, "Invalid sort column");
            return false;
        }
    }

	void CTrendsDialog::enableNewButton()
    {
		//TD17812++
		//GetDlgItem(IDC_NEW)->EnableWindow(true);
		m_newButton.EnableWindow(true);
		//++TD17812
	}
}