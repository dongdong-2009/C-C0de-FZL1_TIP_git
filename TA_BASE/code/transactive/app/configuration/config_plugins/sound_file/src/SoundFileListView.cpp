/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/sound_file/src/SoundFileListView.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all sound files that are currently configured. This view must communicate
  * with the SoundFileDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/sound_file/src/StdAfx.h"
#include "app/configuration/config_plugins/sound_file/src/SoundFileListView.h"
#include "app/configuration/config_plugins/sound_file/src/SoundFileComponent.h"
#include "app/configuration/config_plugins/sound_file/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigItem.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
typedef TA_Base_App::SoundFileDatabaseAccessor::SoundFile SoundFile;

using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    const int SoundFileListView::APPLIED_COLUMN = 0;
    const int SoundFileListView::CATEGORY_COLUMN = APPLIED_COLUMN + 1;
    const int SoundFileListView::PRIORITY_COLUMN = APPLIED_COLUMN + 2;
    const int SoundFileListView::DESCRIPTION_COLUMN = APPLIED_COLUMN + 3;
    const int SoundFileListView::SOUNDFILE_COLUMN =APPLIED_COLUMN + 4;

    const CString SoundFileListView::APPLIED_COLUMN_NAME = "Status";
    const CString SoundFileListView::CATEGORY_COLUMN_NAME = "Sound Category";
    const CString SoundFileListView::PRIORITY_COLUMN_NAME = "Priority";
    const CString SoundFileListView::DESCRIPTION_COLUMN_NAME = "Description";
    const CString SoundFileListView::SOUNDFILE_COLUMN_NAME = "Sound File";

    const int SoundFileListView::APPLIED_COLUMN_WIDTH = 75;
    const int SoundFileListView::CATEGORY_COLUMN_WIDTH = 150;
    const int SoundFileListView::PRIORITY_COLUMN_WIDTH = 75;
    const int SoundFileListView::DESCRIPTION_COLUMN_WIDTH = 400;
    const int SoundFileListView::SOUNDFILE_COLUMN_WIDTH = 400;

    // These are the possible values for the status of each alarm plan association
    const CString SoundFileListView::MODIFIED = "Modified";
    const CString SoundFileListView::UPTODATE = "";

    const bool SoundFileListView::ASCENDING = true;
    const bool SoundFileListView::DESCENDING = false;

    IMPLEMENT_DYNCREATE(SoundFileListView, CListView)


    SoundFileListView::SoundFileListView()
    :     m_sortOrder(DESCENDING),        // Set to descending so the first time we sort it switches to ASCENDING
          m_sortColumn(CATEGORY_COLUMN)  // This is the column to sort on when the list control gets created
    {
        m_componentIcon = LoadIcon(GetModuleHandle(SoundFileComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_SOUND_FILE) );
    }


    SoundFileListView::~SoundFileListView()
    {
    }


BEGIN_MESSAGE_MAP(SoundFileListView, AbstractListView)
	//{{AFX_MSG_MAP(SoundFileListView)
	ON_WM_CREATE()
    ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, onColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    int SoundFileListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
        // Set the styles for our list control before it is created
        lpCreateStruct->style = LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER;

	    if (CListView::OnCreate(lpCreateStruct) == -1)
		    return -1;
	    
	    return 0;
    }


    void SoundFileListView::OnInitialUpdate() 
    {
	    CListView::OnInitialUpdate();
	    
        GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES );

        // Insert the columns to display the data
        GetListCtrl().InsertColumn(APPLIED_COLUMN,     APPLIED_COLUMN_NAME,     LVCFMT_LEFT, APPLIED_COLUMN_WIDTH     );
        GetListCtrl().InsertColumn(CATEGORY_COLUMN,    CATEGORY_COLUMN_NAME,    LVCFMT_LEFT, CATEGORY_COLUMN_WIDTH    );
        GetListCtrl().InsertColumn(PRIORITY_COLUMN,    PRIORITY_COLUMN_NAME,    LVCFMT_LEFT, PRIORITY_COLUMN_WIDTH    );
        GetListCtrl().InsertColumn(DESCRIPTION_COLUMN, DESCRIPTION_COLUMN_NAME, LVCFMT_LEFT, DESCRIPTION_COLUMN_WIDTH );
        GetListCtrl().InsertColumn(SOUNDFILE_COLUMN,   SOUNDFILE_COLUMN_NAME,   LVCFMT_LEFT, SOUNDFILE_COLUMN_WIDTH   );
   }


    void SoundFileListView::OnSize(UINT nType, int cx, int cy) 
    {
        CListView::OnSize(nType, cx, cy);
        // Call the CListView::ONSize instead of the one in the abstract class above
    }


    void SoundFileListView::loadAllItems()
    {
        FUNCTION_ENTRY("loadAllItems");

        TA_ASSERT(getDatabaseAccessor() != NULL, "setDatabaseAccessor() must be called before this method");
        TA_ASSERT(getDetailView() != NULL, "The detail view has not been set yet");
        TA_ASSERT(getStatusBar() != NULL, "The status bar has not been set yet");

        // Retrieve the progress bar and display it on the status bar
        CProgressCtrl& progress = getStatusBar()->displayProgressBar();
        progress.SetStep(1);
        progress.SetPos(3); // Indicate that we have started loading and use the wait cursor to do the rest.

        SoundFileDatabaseAccessor* dbAccessor = dynamic_cast<SoundFileDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to a Sound File one");

        m_currentDisplayedItems.clear();

        try
        {
            AfxGetApp()->DoWaitCursor(1);

            // Tell the database accessor to load all items from the database
            dbAccessor->loadItems();
            
            // Now get a list of item names and ids so we can populate our list
            // The progress bar will start moving in this method. This method also sets the range
            // on the status bar.
            m_currentDisplayedItems = dbAccessor->getAllItemDetails(progress);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            getStatusBar()->hideProgressBar();
            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            getStatusBar()->hideProgressBar();
            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return;
        }

        if (m_currentDisplayedItems.empty())
        {
            getStatusBar()->hideProgressBar();

            FUNCTION_EXIT;
            return;
        }


        // Set the range to be the number of entities we have then we can just step by one each time.

        // Now add all the items into our list and set their data to be their ids
        for ( std::map<unsigned long,SoundFile>::iterator iter = m_currentDisplayedItems.begin(); 
              iter != m_currentDisplayedItems.end();
              iter++ )
        {
            if ( dbAccessor->getItem(iter->first)->hasChanged() )
            {
                iter->second.status = MODIFIED;
            }
            else
            {
                iter->second.status = UPTODATE;
            }

            int position = GetListCtrl().InsertItem( 0, "", 0 );
            GetListCtrl().SetItemText( position, APPLIED_COLUMN, iter->second.status );
            GetListCtrl().SetItemText( position, CATEGORY_COLUMN, iter->second.category );
            GetListCtrl().SetItemText( position, PRIORITY_COLUMN, iter->second.priority );
            GetListCtrl().SetItemText( position, DESCRIPTION_COLUMN, iter->second.description );
            GetListCtrl().SetItemText( position, SOUNDFILE_COLUMN, iter->second.soundfile );
            
            GetListCtrl().SetItemData( position, iter->first );
            progress.StepIt();
        }

        // Now sort the columns
        updateHeaderSort( m_sortColumn );
        GetListCtrl().SortItems( compareItems, reinterpret_cast<unsigned long>(this) );

        getStatusBar()->hideProgressBar();
        AfxGetApp()->DoWaitCursor( -1 );

        FUNCTION_EXIT;
    }

    
    void SoundFileListView::itemDetailsChanged(unsigned long id, const CString& name)
    {
        FUNCTION_ENTRY("itemChanged");

        // We must find the item in our list with it's data set to be the same as the id
        // passed in.

        // Search all lParams stored with each list item for the id we have.
        LVFINDINFO info;
        info.flags = LVFI_PARAM;
        info.lParam = id;

        SoundFileDatabaseAccessor* dbAccessor = dynamic_cast<SoundFileDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to a Sound File one");

        SoundFileDatabaseAccessor::SoundFile details = dbAccessor->getItemDetails( id );

        // If the item is found then update it with the new details

        int foundItemIndex = GetListCtrl().FindItem( &info );
        if ( foundItemIndex != -1 )
        {
            if ( dbAccessor->getItem(id)->hasChanged() )
            {
                details.status = MODIFIED;
            }
            else
            {
                details.status = UPTODATE;
            }

            GetListCtrl().SetItemText(foundItemIndex, APPLIED_COLUMN, details.status);
            GetListCtrl().SetItemText(foundItemIndex, CATEGORY_COLUMN, details.category);
            GetListCtrl().SetItemText(foundItemIndex, PRIORITY_COLUMN, details.priority);
            GetListCtrl().SetItemText(foundItemIndex, DESCRIPTION_COLUMN, details.description);
            GetListCtrl().SetItemText(foundItemIndex, SOUNDFILE_COLUMN, details.soundfile);
        }
        else
        {
            // What to do here? This should never occur.
        }

        // Keep our stored map up-to-date so we can perform accurate and quick sorting
        std::map<unsigned long,SoundFile>::iterator matching;
        matching = m_currentDisplayedItems.find( id );
        if ( matching != m_currentDisplayedItems.end() )
        {
            matching->second = details;
        }
        else
        {
            m_currentDisplayedItems.insert( std::map<unsigned long,SoundFile>::value_type(id,details) );
        }

        // Now sort the columns again
        m_sortOrder = !m_sortOrder; // Make sure we sort in the same order. To do this we must change the
                                    // sort order as the sorting method will change it again
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));

        FUNCTION_EXIT;
    }
    

    void SoundFileListView::onColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        updateHeaderSort(pNMListView->iSubItem);

        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));
	    
	    *pResult = 0;
    }


	void SoundFileListView::updateHeaderSort(const unsigned int column)
	{
		FUNCTION_ENTRY("updateHeaderSort");

		TA_ASSERT(column >= 0, "Column was -ve!");

		//First find if the selected column is currently sorted on. If so, toggle 
		//the sorting arrangment. 
		CHeaderCtrl* headerCtrl = GetListCtrl().GetHeaderCtrl(); 

        HDITEM headerItem; 
		headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
		headerCtrl->GetItem(column, &headerItem); 

		if (headerItem.hbm != 0) 
		{ 
			DeleteObject(headerItem.hbm); 
			headerItem.hbm = 0; 
		}
        
        // If this is the same column we sorted on last time then now we want to sort in the opposite order
		if(m_sortColumn == column)
		{
			m_sortOrder = !m_sortOrder;
		}
        else
        {
            // If this is a different column reset the sort order to be ascending first.
            m_sortOrder = ASCENDING;
        }

        // Set up ready to add our bitmap
		headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 

        /// Load the bitmap based on whether we are sorting ascending or descending
        int resourceToLoad = IDB_DESCENDING;
        if (m_sortOrder == ASCENDING)
        {
            resourceToLoad = IDB_ASCENDING;
        }

        headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle(SoundFileComponent::DLL_NAME), MAKEINTRESOURCE(resourceToLoad), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		headerCtrl->SetItem(column, &headerItem); 

        // If the column we are sorting on has changed then we must remove the arrow bitmap from the old
        // column header
		if(m_sortColumn != column) 
		{ 
			headerCtrl->GetItem(m_sortColumn, &headerItem); 
			headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 

			if(headerItem.hbm != 0) 
			{ 
				DeleteObject(headerItem.hbm); 
				headerItem.hbm = 0; 
			} 
			headerCtrl->SetItem(m_sortColumn, &headerItem); 

            // Now set our new sort column
            m_sortColumn = column;
		}

		FUNCTION_EXIT;
	}


    int CALLBACK SoundFileListView::compareItems(LPARAM index1, LPARAM index2, LPARAM listCtrl)
    {
        // Typecast from unsigned long to SoundFileListView*
        SoundFileListView* list = reinterpret_cast<SoundFileListView*>(listCtrl);

        // Make sure the list is indeed a SoundFileListView.
        list = dynamic_cast<SoundFileListView*>(list);

        TA_ASSERT(list != NULL, "The wrong type was passed as the third parameter to this method");

        // Now retrieve the data for these items so we can start comparing it
        std::map<unsigned long,SoundFile>::iterator item1 = list->m_currentDisplayedItems.find( index1 );
        std::map<unsigned long,SoundFile>::iterator item2 = list->m_currentDisplayedItems.find( index2 );

        if (item1 == list->m_currentDisplayedItems.end() || 
            item2 == list->m_currentDisplayedItems.end())
        {
            // Cannot compare
            return 0;
        }

        CString string1("");
        CString string2("");

        switch(list->m_sortColumn)
        {
            case(APPLIED_COLUMN):
                    string1 = item1->second.status;
                    string2 = item2->second.status;
                    break;

            case(CATEGORY_COLUMN):
                    string1 = item1->second.category;
                    string2 = item2->second.category;
                    break;

            case(PRIORITY_COLUMN):
                    string1 = item1->second.priority;
                    string2 = item2->second.priority;
                    break;

            case(DESCRIPTION_COLUMN):
                    string1 = item1->second.description;
                    string2 = item2->second.description;
                    break;

            case(SOUNDFILE_COLUMN):
                    string1 = item1->second.soundfile;
                    string2 = item2->second.soundfile;
                    break;
        }
        
        int returnVal = string1.CompareNoCase(string2);

        // If we are sorting descending then switch the sign of the returnVal
        if (list->m_sortOrder == DESCENDING)
        {
            returnVal = -returnVal;
        }

        return returnVal;
    }

    
    void SoundFileListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // SoundFileListView diagnostics

    #ifdef _DEBUG
    void SoundFileListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void SoundFileListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

