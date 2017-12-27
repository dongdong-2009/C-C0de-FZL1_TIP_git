/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Nick Jardine
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 * 
 * EventListComponent is the main list class. It is a subclass of CListCtrl, and is responsible for controlling the display
 * of list data, and providing functions to the main application such as print, sort, and export.
 */

//#include "app/alarms_events/event_viewer/src/stdafx.h"
#include <cctype>
#include <algorithm>
#include <fstream>

#include "app/event/event_viewer/EventListComponent/src/stdafx.h"
#include "bus/mfc_extensions/src/print_list_ctrl/ListCtrlPrint.h"
#include "app/event/event_viewer/EventListComponent/src/EventListComponent.h"
#include "app/event/event_viewer/EventListComponent/src/Filter.h"
/*#include "app/event/event_viewer/EventListComponent/src/EventLoader.h"*/
#include "app/event/event_viewer/EventListComponent/src/ActionThread.h"
#include "app/event/event_viewer/src/CombinedEventFilter.h"

#include "bus/resources/resource.h"
#include "bus/mfc_extensions/src/standard_dialogs_arial_bold/FileDialogArial.h"
//TD17888 marc_bugfix for save as dialog
#include "bus/mfc_extensions/src/file_dialog_no_folder_change/FileDialogArialDisableFolders.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IEventColourData.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DateFormat.h" // TD12474
#include "core/utilities/src/TA_MinMax.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

/////////////////////////////////////////////////////////////////////////////
// EventListComponent


const unsigned long EventListComponent::ONE_SECOND = 1000;

EventListComponent::EventListComponent()
: m_filter(NULL), m_refreshPeriodMs(0), m_elapsedTimeMs(0), m_currentSortColumn(-1),
m_sortAscending(false), m_isColourCoded(true), m_reapplyingSelection(false), m_bToolTipCtrlCustomizeDone(FALSE)
{
	m_printoutColWidths = NULL;
}

EventListComponent::~EventListComponent()
{
	// TD 12763 delete the col width array after use
	if (m_printoutColWidths != NULL)
	{
		delete m_printoutColWidths;
		m_printoutColWidths = NULL;
	}
}


BEGIN_MESSAGE_MAP(EventListComponent, CListCtrl)
	//{{AFX_MSG_MAP(EventListComponent)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
    ON_NOTIFY_REFLECT(NM_CLICK, OnItemSelected)
    ON_NOTIFY_REFLECT(NM_DBLCLK, onDblclk)
	//}}AFX_MSG_MAP
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdrawEventList)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EventListComponent message handlers


void EventListComponent::PreSubclassWindow() 
{
    EnableToolTips();
    CListCtrl::PreSubclassWindow();
}

int EventListComponent::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
    // Borrowed from: http://www.codeguru.com/Cpp/controls/listview/tooltiptitletip/article.php/c909/

    // Determine the coordinates of the cell.
    int row, col;
    RECT cellrect;
    row = CellRectFromPoint(point, &cellrect, &col );

    if ( row == -1 )
    {
        return -1;
    }

    // Encode the cell coordinates the ID.
    pTI->hwnd = m_hWnd;
    pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
    pTI->lpszText = LPSTR_TEXTCALLBACK;
    pTI->rect = cellrect;

    return pTI->uId;
}

int EventListComponent::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const
{
    // Borrowed from: http://www.codeguru.com/Cpp/controls/listview/tooltiptitletip/article.php/c909/

    int colnum;

    // Make sure that the ListView is in LVS_REPORT
    if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
    {
        return -1;
    }

    // Get the top and bottom row visible
    int row = GetTopIndex();
    int bottom = row + GetCountPerPage();
    if ( bottom > GetItemCount() )
    {
        bottom = GetItemCount();
    }

    // Get the number of columns
    CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();

    // Loop through the visible rows
    for ( ; row<=bottom; row++ )
    {
        // Get bounding rect of item and check whether point falls in it.
        CRect rect;
        GetItemRect( row, &rect, LVIR_BOUNDS );
        if ( rect.PtInRect(point) )
        {
            // Now find the column
            for ( colnum = 0; colnum < nColumnCount; colnum++ )
            {
                int colwidth = GetColumnWidth(colnum);
                if ( ( point.x >= rect.left ) && ( point.x <= (rect.left + colwidth ) ) )
                {
                    RECT rectClient;
                    GetClientRect( &rectClient );
                    if ( col ) *col = colnum;
                    rect.right = rect.left + colwidth;

                    // Make sure that the right extent does not exceed the client area
                    if( rect.right > rectClient.right )
                    {
                        rect.right = rectClient.right;
                    }
                    *cellrect = rect;
                    return row;
                }
                rect.left += colwidth;
            }
        }
    }
    return -1;
}

BOOL EventListComponent::OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
        // Borrowed from: http://www.codeguru.com/Cpp/controls/listview/tooltiptitletip/article.php/c909/

    UINT nID = pNMHDR->idFrom;

    *pResult = 0;

    // We want to disable automatic tooltips completely.
    if( 0 == nID )
    {
        return TRUE;
    }

    // I want to implement this in PreSubclassWindow(), but it crashes.
    if(!m_bToolTipCtrlCustomizeDone)
    {
#if _MSC_VER >= 1300
		CToolTipCtrl* pToolTip = AfxGetModuleState()->m_thread.GetDataNA()->m_pToolTip;
#else
		_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
		CToolTipCtrl* pToolTip = pThreadState->m_pToolTip;
#endif

        // Set max tip width in pixel.
        // you can change delay time, tip text or background color also. enjoy yourself!
		//Shiva++
		//Change style to TTS_NOPREFIX - to support tab character and & characters in tooltip
		pToolTip->ModifyStyle(0, TTS_NOPREFIX);
        pToolTip->SetMaxTipWidth(1000); //TD14876
        m_bToolTipCtrlCustomizeDone = TRUE;
    }

    // need to handle both ANSI and UNICODE versions of the message
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

    // Extract the row and column numbers.
    int row = ((nID-1) >> 10) & 0x3fffff ;
    int col = (nID-1) & 0x3ff;
    CString strTip;
    strTip = GetItemText( row, col );
	
	//TD16138++ wangyn
	int columnLength = GetColumnWidth(col);
	
	int textWidth = GetStringWidth(strTip.GetBuffer(0)) + 12;
	
	if(columnLength >= textWidth)
	{
		return FALSE;
	}
	//++TD16138

    // Convert the text as required.
    if(pNMHDR->code == TTN_NEEDTEXTA)
    {
        m_pchTip = std::auto_ptr<TCHAR>( new TCHAR[strTip.GetLength()+1] );
        lstrcpyn( m_pchTip.get(), strTip, strTip.GetLength() );
        m_pchTip.get()[strTip.GetLength()] = NULL;
        pTTTW->lpszText = (WCHAR*)m_pchTip.get();
    }
    else
    {
        m_pwchTip = std::auto_ptr<WCHAR>( new WCHAR[strTip.GetLength()+1] );
        _mbstowcsz( m_pwchTip.get(), strTip, strTip.GetLength() );
        m_pwchTip.get()[strTip.GetLength()] = NULL;
        pTTTW->lpszText = (WCHAR*)m_pwchTip.get();
    }

    return TRUE;
}

void EventListComponent::setFilter(Filter* filter)
{
    m_filter = filter;

	if(m_printoutColWidths == NULL){

		// TD 12763 since this is the first time, establish the default printout width for each columns
		std::vector<Filter::Column> filterColumns = m_filter->getColumns();
		m_printoutColWidths = new int [filterColumns.size() + 1];
		for (unsigned int i=0 ; i<filterColumns.size(); ++i )
		{
			Filter::Column col = filterColumns[i];
			m_printoutColWidths[i+1] = col.m_width;
		}

	}

    // Create the columns.
    std::vector<Filter::Column> columns = m_filter->getColumns();	    
    for (unsigned int i = 0;i < columns.size();i++)
    {
        //InsertColumn(i,columns[i].c_str(),LVCFMT_LEFT,100);
		
		//TD14897 azenitha++
		CString columnName = columns[i].m_name.c_str();
		if (columnName.Compare("Sev") == 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD14897] Setting severity column to center justified");
			InsertColumn(i,columns[i].m_name.c_str(),LVCFMT_CENTER,columns[i].m_width);
		}
		else
		{
			InsertColumn(i,columns[i].m_name.c_str(),LVCFMT_LEFT,columns[i].m_width);
		}
		//TD14897 ++azenitha
    }

    // Set any filter dependent styles.
    SetBkColor( m_filter->getBackgroundColour() );	

}

void EventListComponent::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem=&(pDispInfo)->item;	

	int iItemIndx= pItem->iItem;

	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		CHeaderCtrl* header = GetHeaderCtrl();
		HDITEM headerItem;
		char headerName[1000];
		headerItem.mask = HDI_TEXT;
		headerItem.pszText = headerName;
		headerItem.cchTextMax = 1000;
		header->GetItem(pItem->iSubItem,&headerItem);

		//TD14876++
		// Pattern code from AlarmListCtrl

		// Remove any non-printable charcters and copy to control
		//lstrcpy(pItem->pszText, replaceNonPrintableCharactersWithSpace(m_filter->getItemData(iItemIndx,headerName)).c_str());

		std::string text;
		text = m_filter->getItemData(iItemIndx,headerName);
		//std::string text = replaceNonPrintableCharactersWithSpace(m_filter->getItemData(iItemIndx,headerName));

		int maxLen = pItem->cchTextMax;

		int	text_length	= text.size();
		if (text_length > maxLen)
		{
			text = text.substr(0, maxLen);
			text_length = maxLen;
		}
		text_length = ta_min(text_length, pItem->cchTextMax);

		memset(pItem->pszText, 0, pItem->cchTextMax);
		memcpy(pItem->pszText, text.c_str(), text_length);	
		//++TD14876
	}    

	*pResult = 0;
}



void EventListComponent::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;


    // Want to get the header item text (to get the column name)
    HDITEM headerItem;
    char headerName[1000];

    headerItem.mask = HDI_TEXT;
    headerItem.pszText = headerName;
    headerItem.cchTextMax = 1000;
	CHeaderCtrl* headerCtrl = GetHeaderCtrl(); 
	headerCtrl->GetItem(pNMListView->iSubItem, &headerItem); 

	// Toggle order or change to new column
    bool newSortAscending = true;
    if(m_currentSortColumn == pNMListView->iSubItem)
	{
		newSortAscending = !m_sortAscending;
	}
		
	sort(headerName,pNMListView->iSubItem,newSortAscending);
	*pResult = 0;
}


void EventListComponent::OnItemSelected(NMHDR* pNMHDR, LRESULT* pResult)
{
	//save new selected positions&eventID to filter.
	std::vector<unsigned long> selectedItemsTest;
	POSITION pos = GetFirstSelectedItemPosition();
	while ( pos!=NULL )
	{
		selectedItemsTest.push_back(  GetNextSelectedItem(pos) );
	}
	m_filter->setSelectedItemIDs(selectedItemsTest);


    //bool enableButton = true;
    unsigned int numSelectedItems = 0;
    int index = 0;
    
    if(m_filter != NULL)
    {
        // First test if only one event is selected
        const std::vector<unsigned long>& selectedItems = m_filter->getSelectedItems();
        numSelectedItems = selectedItems.size();

        if(selectedItems.size() > 0)
        {
            index = selectedItems[0];
        }
    }
    
	// Redraw to get rid of the blue line on previously selected items
	RedrawWindow();
	UpdateWindow();
    // Post the message to the Event Viewer
    ::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_USER_LIST_SELECTION_UPDATE, numSelectedItems, index);
}

void EventListComponent::clearSortMark()
{
	if (m_currentSortColumn != -1) 
	{  
		HDITEM headerItem;
		CHeaderCtrl* headerCtrl = GetHeaderCtrl(); 
		// Remove image from original sort column
		headerCtrl->GetItem(m_currentSortColumn, &headerItem); 
		headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 
		if (headerItem.hbm != 0) 
		{ 
			DeleteObject(headerItem.hbm); 
			headerItem.hbm = 0; 
		} 
		headerCtrl->SetItem(m_currentSortColumn, &headerItem);
		m_currentSortColumn = -1;
		m_sortAscending = false;   //
	}
}

BOOL EventListComponent::clearList()
{
    // Before actually deleting all items, need to remove the sort icons from any currently sorted headers
    
	clearSortMark();

    return DeleteAllItems();
}


void EventListComponent::OnCustomdrawEventList ( NMHDR* pNMHDR, LRESULT* pResult )
{

	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	//unsigned long index = pLVCD->nmcd.dwItemSpec;
	*pResult = CDRF_DODEFAULT;
	bool isSelected = false;
	switch(pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// First thing - check the draw stage. If it's the control's prepaint
		// stage, then tell Windows we want messages for every item.
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_POSTPAINT:
		break;
	case CDDS_POSTERASE:
		break;
	case CDDS_ITEMPREPAINT:
		// This is the prepaint stage for an item. Here's where we set the
		// item's text color. Our return value will tell Windows to draw the
		// item itself, but it will use the new color we set here.
		// We'll cycle the colors through red, green, and light blue.
		// Get the index for the item
		{

			unsigned long index= pLVCD->nmcd.dwItemSpec;

			if ( m_isColourCoded )
			{
				// If alarm severities are colour coded pass the event onto the subitems.
				*pResult = CDRF_NOTIFYSUBITEMDRAW; 
			}
			else
			{
				// Otherwise just draw with row foreground and background colour.
				m_filter->getRowColour( index, pLVCD->clrText, pLVCD->clrTextBk );

				*pResult = CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;
			}
		}
		break;
	case (CDDS_ITEMPREPAINT | CDDS_SUBITEM):
		// This is the prepaint stage for a subitem. Here's where we set the
		// item's text and background colors. Our return value will tell 
		// Windows to draw the subitem itself, but it will use the new colors
		// we set here.
		{
			CHeaderCtrl* header = GetHeaderCtrl();
			HDITEM headerItem;
			char headerName[1000];
			headerItem.mask = HDI_TEXT;
			headerItem.pszText = headerName;
			headerItem.cchTextMax = 1000;
			header->GetItem(pLVCD->iSubItem,&headerItem);
			pLVCD->clrTextBk = m_filter->getItemColour(pLVCD->nmcd.dwItemSpec,headerName);
			*pResult = CDRF_DODEFAULT;
		}
		break;
	case CDDS_ITEMPOSTPAINT:
		unsigned long index= pLVCD->nmcd.dwItemSpec;

		if(m_filter->isRowSeleted(index) )	  
		{
			isSelected = true;
		}
		if(isSelected)
		{
			int index = pLVCD->nmcd.dwItemSpec;
			CRect rect;
			GetItemRect(index, &rect,LVIR_BOUNDS);
			CDC* newDC = CDC::FromHandle(pLVCD->nmcd.hdc);

			// Blue Border selection highlight
			CPen   newPen  (PS_SOLID, 3, RGB(0,0,255));
			CPen* oldPen = newDC->SelectObject(&newPen);

			newDC->SelectStockObject(NULL_BRUSH);
			newDC->Rectangle(&rect);

			newDC->SelectObject(oldPen);


			// Validate this item so it is not redrawn again
			ValidateRect(&rect);
			*pResult = CDRF_SKIPDEFAULT;//CDRF_DODEFAULT;
		}
	}
	
}

void EventListComponent::print()
{
	CListCtrlPrint cJob;

    // Set the header information.

	// TD12474 ++
	//CString header = "Event Viewer : " + CTime::GetCurrentTime().Format("%a %#d/%#m/%Y %#H:%M:%S");

	TA_Base_Core::DateFormat dateFormat; 
	std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat3);

	CString header = "ÊÂ¼þä¯ÀÀÆ÷ : " + CTime::GetCurrentTime().Format(dateFormatStr.c_str()); 
	// ++ TD12474

	cJob.csPageHead = header;
	cJob.csPage = "Page %d of %d";

    // Get some information required to populate the colour map.
    const std::vector<Filter::Column>& columns = m_filter->getColumns();
    unsigned int numRows = this->GetItemCount();

    // Generate the CellColourMap required for printing the events in colour.
    TA_Base_Bus::CellColourMap colourMap( columns.size(), numRows, 0, GetBkColor() );
    for ( unsigned int row=0; row<numRows; row++ )
    {
        if ( ! m_isColourCoded )
        {
            // If individual cells aren't colour coded apply row colouring.
            colourMap.setRowBkColour( row, m_filter->getBackgroundColour() );
        }
        else
        {
            // Otherwise add the individual cell colours.
            unsigned int column = 0;
            std::vector<Filter::Column>::const_iterator it;
            for ( it=columns.begin(); it!=columns.end(); it++,column++ )
            {
                colourMap.setCellBkColour( row, column, m_filter->getItemColour( row, it->m_name ) );
            }
        }
    }
    cJob.pColourMap = &colourMap;

	// TD 12763 Determine the column width and adjust where appropriate
	if (m_printoutColWidths != NULL)
	{
		// create a new col width array for cJob to process the width, if there are changes
		cJob.pColWidthArray = m_printoutColWidths;
	}
    // Finally, add the list of data to print and then print it.
	cJob.pList = this;
	cJob.OnFilePrint();	
}

void EventListComponent::exportEvents(const CString& exportLocation)
{
    // Show print dialog for user to choose a file name
/*
	CFileDialogArial dlg(FALSE,            // We want a Save dialog not an Open one
					".csv",           // The default file extension
					"Untitled",       // This is the initial file name
					OFN_HIDEREADONLY| // Hide the readonly check box
					OFN_OVERWRITEPROMPT| // Prompt to ask the user if they want to overwrite existing files
					OFN_PATHMUSTEXIST,	 // Check that the path entered is correct and warn if it is not
					"CSV (Comma delimited) (*.csv)|*.csv||",	// Only allow them to save in csv format
					this);			  // Parent window is the actual event viewer
*/
	//TD17888 marc_bugfix
	TA_Base_Bus::FileDialogArialDisableFolders dlg(FALSE,            // We want a Save dialog not an Open one
					".csv",           // The default file extension
					"Untitled",       // This is the initial file name
					OFN_HIDEREADONLY| // Hide the readonly check box
					OFN_OVERWRITEPROMPT| // Prompt to ask the user if they want to overwrite existing files
					OFN_PATHMUSTEXIST |	 // Check that the path entered is correct and warn if it is not
					OFN_NOCHANGEDIR,
					"CSV (Comma delimited) (*.csv)|*.csv||",	// Only allow them to save in csv format
					this);			  // Parent window is the actual event viewer

    dlg.m_ofn.lpstrInitialDir = exportLocation;

	if (dlg.DoModal() != IDOK)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Export was cancelled");
		return;
	}

	try
	{
		// TD11124: Use an output file stream to write the events to file.
        std::ofstream outFile( dlg.GetPathName() );
        if ( !outFile.is_open() )
        {
            throw std::exception( "Couldn't open file for writing." );
        }

		// Output the date as a heading
		COleDateTime currTime = COleDateTime::GetCurrentTime();

		// TD12474 ++
		//outFile << std::string(currTime.Format("%d %B %Y %H:%M")) << "\n\n";

		TA_Base_Core::DateFormat dateFormat; 
		std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 

		outFile << std::string(currTime.Format(dateFormatStr.c_str())) << "\n\n";
		// ++ TD12474

		// Output the column headings to the file
        std::vector<Filter::Column> columns = m_filter->getColumns();
        unsigned int i = 0;
        for (i = 0;i < columns.size();i++)
        {
            outFile << columns[i].m_name;

            if (i < columns.size() - 1)
            {
                outFile << ",";
            }
        }
		outFile << "\n";

        CHeaderCtrl* header = GetHeaderCtrl();

		std::string text;
		for (int index = 0; index <  GetItemCount(); ++index)
		{
            for (i = 0;i < columns.size();i++)
            {
                text = m_filter->getItemData(index,columns[i].m_name);
				// TD 14418 Export the comment if there is comment in the item
				if (columns[i].m_name.compare("C") == 0 && text.compare("C") == 0)
				{
					text = m_filter->getItemData( index, columns[i].m_name, false );
				}
                convertText(text);

                outFile << text;
                if (i < columns.size() - 1)
                {
                    outFile << ",";
                }
            }
			outFile << "\n";
		}
		outFile.flush();
		outFile.close();
	
	}
	catch ( ... )
	{
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << std::string(dlg.GetPathName());
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_040018);
		return;
	}

    TA_Base_Bus::TransActiveMessage userMsg;
    UINT selectedButton = userMsg.showMsgBox(IDS_UI_040018);
}


void EventListComponent::convertText(std::string& text)
{
    // replace all carriage returns with spaces
    //text = replaceNonPrintableCharactersWithSpace(text);

	// First check for comma. If there is none then the string can remain the same
	if (std::string::npos == text.find(','))
	{
		return;
	}

	// Ok now we have established there is a comma we must change any existing double quotes to "".
	std::string::size_type pos = text.find('\"');
	while (pos != std::string::npos)
	{
		text.insert(pos,"\"");
		pos = text.find('\"',pos+2);
	}

	// Now add quotes at either end
	//text.insert(0,'\"');
	text.insert(0,"\"");
	text.insert(text.size(),"\"");
}

void EventListComponent::setColourCoding(const bool isEnabled)
{
    m_isColourCoded = isEnabled;
}


void EventListComponent::changeDisplay(Filter* filter)
{
    m_filter = filter;
    //m_eventLoader->changeFilter(filter);
    CHeaderCtrl* header = GetHeaderCtrl();

    for (int i = header->GetItemCount() - 1; i >= 0;i--)
    {
        DeleteColumn(i);
    }
    
    // And now add in the new column details
    std::vector<Filter::Column> columns = m_filter->getColumns();	

    for (unsigned int j = 0;j < columns.size();j++)
    {
        //InsertColumn(j,columns[j].c_str(),LVCFMT_LEFT,100);
		//TD14897 azenitha++
		CString columnName = columns[j].m_name.c_str();
		if (columnName.Compare("Sev") == 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD14897] Setting severity column to center justified");
			InsertColumn(j,columns[j].m_name.c_str(),LVCFMT_CENTER,columns[j].m_width);
		}
		else
		{
			InsertColumn(j,columns[j].m_name.c_str(),LVCFMT_LEFT,columns[j].m_width);
		}
		//TD14897 ++azenitha
    }
}




void EventListComponent::sort()
{
    sort(m_filter->getDefaultSortColumn(), m_sortAscending);
}


void EventListComponent::sort(const std::string& columnName,const bool sortAscending)
{
	CHeaderCtrl* headerCtrl = GetHeaderCtrl();
    HDITEM headerItem;
    char text[1000];
    headerItem.mask = HDI_TEXT;
    headerItem.pszText = text;
    headerItem.cchTextMax = 1000;
    for (int i = 0;i < headerCtrl->GetItemCount();i++)
    {
	    headerCtrl->GetItem(i, &headerItem); 

		if (0 == strcmp(text,columnName.c_str())) 
		{ 
            sort(columnName,i,sortAscending);
            break;
		} 
    }
}


void EventListComponent::sort(const std::string& columnName, const int columnId, const bool sortAscending)
{
 
    // Now that we know the sort can be applied set the member variable for sort direction.
    m_sortAscending = sortAscending;

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	// Add sort arrows to list columns
    HDITEM headerItem;

    // Want to get the header item text (to get the column name), the format (to position the bitmap), and the bitmap handle
    headerItem.mask = HDI_FORMAT | HDI_BITMAP;
	CHeaderCtrl* headerCtrl = GetHeaderCtrl(); 
	headerCtrl->GetItem(columnId, &headerItem);

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
	headerCtrl->SetItem(columnId, &headerItem); 

    // If the previous sort was not on this column
	if (m_currentSortColumn != columnId && m_currentSortColumn != -1) 
	{
		// Remove image from original sort column
		headerCtrl->GetItem(m_currentSortColumn, &headerItem); 
		headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 
		if (headerItem.hbm != 0) 
		{ 
			DeleteObject(headerItem.hbm); 
			headerItem.hbm = 0; 
		} 
		headerCtrl->SetItem(m_currentSortColumn, &headerItem);
    }

	{
		if( m_currentSortColumn == columnId )
		{
			m_filter->reverse(columnName, m_sortAscending );
		}
		else
		{
			// Save the column ID for processing future sorting.
			m_currentSortColumn = columnId;

			// Need to use the "headerNameStr" here, as the headerName char array has been replaced with the last one selected.
			m_filter->sort(columnName,m_sortAscending);
		}

	}

}


void EventListComponent::reapplyCurrentSort()
{
    if(-1 != m_currentSortColumn) 
    {
        CHeaderCtrl* headerCtrl = GetHeaderCtrl();
        HDITEM headerItem;
        char text[1000];
        headerItem.mask = HDI_TEXT;
        headerItem.pszText = text;
        headerItem.cchTextMax = 1000;
    
        // Get the column name of the current sort column
        headerCtrl->GetItem(m_currentSortColumn, &headerItem);

        sort(text, m_currentSortColumn, m_sortAscending);
    }
    else
    {
        // Sort on the default column
        sort();
    }
}

void EventListComponent::nextPage()
{
    // Move to the next page.
    m_filter->nextPage();
	if( TA_Base_Core::RunParams::getInstance().isSet( EventConstants::RPARAM_NO_VISIBILITY_RULE.c_str()))
	{
		//m_eventLoader->setLoad(false);
	}
}

void EventListComponent::previousPage()
{
    // Move to the previous page.
    m_filter->previousPage();

}

std::string EventListComponent::replaceNonPrintableCharactersWithSpace(const std::string& inputString)
{
	std::string adjustedString = inputString;

// 	if(adjustedString.length()>0)
// 	{
// 		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Begin EventListComponent::replaceNonPrintableCharactersWithSpace,  %s", adjustedString.c_str());
// 	}

	// Loop through checking for non-printable characters
	for (unsigned int i = 0; i < adjustedString.length(); ++i)
	{	
		char c = adjustedString[i];

		if( (unsigned)(c + 1) > 256 || isprint(adjustedString[i]) == 0 )
		{
			// Replace the non-printable character with a space
			std::replace(adjustedString.begin(), adjustedString.end(), adjustedString[i], ' ');
		}
	}


	return adjustedString.c_str();
}

void EventListComponent::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
//    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // We will also be notified of selection changes when we do it ourselves, so
    // make sure we ignore expected events.
    // NOTE: The AlarmListCtrl keeps track of selection by storing the selection
    //       state for every alarm. While this is possible for the EventListComponent
    //       the large number of events means linear searching of all events should
    //       be avoided. Instead we'll just keep a vector of selected indices, which
    //       will be dynamically modified on refresh to ensure correct items remain
    //       selected.

}

BOOL EventListComponent::SetItemCountEx( int size, DWORD flags /*=LVSICF_NOINVALIDATEALL*/ )
{
    // Let the parent class handle the request.
    BOOL returnValue = CListCtrl::SetItemCountEx( size, flags );

    // Make sure the following select changes to not affect current selections.
    m_reapplyingSelection = true;

    // Unselect all currently selected items.
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos!=NULL)
	{
        SetItemState( GetNextSelectedItem(pos), 0, LVIS_SELECTED );
        pos = GetFirstSelectedItemPosition();
    }

    m_reapplyingSelection = false;

    return returnValue;
}

void EventListComponent::onDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
    FUNCTION_ENTRY("onDblclk");

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // Get the row number and validate it.
    int item = pNMListView->iItem;
    if ( item < 0 || item >= GetItemCount() )
    {
        *pResult = 0;
        FUNCTION_EXIT;
        return;
    }

    // Retrieve the list of columns from the filter.
    const std::vector<Filter::Column>& columns = m_filter->getColumns();

    // Get the column number and validate it.
    int subItem = pNMListView->iSubItem;
    if ( subItem < 0 || subItem >= static_cast<int>(columns.size()) )
    {
        *pResult = 0;
        FUNCTION_EXIT;
        return;
    }

    // Extract the of the column from the header.
    CHeaderCtrl* header = GetHeaderCtrl();
    HDITEM headerItem;
    char headerName[1000];
    headerItem.mask = HDI_TEXT;
    headerItem.pszText = headerName;
    headerItem.cchTextMax = 1000;
    header->GetItem(pNMListView->iSubItem,&headerItem);

    // Now find its associated Column object.
    std::vector<Filter::Column>::const_iterator it;
    for ( it=columns.begin(); it!=columns.end(); it++ )
    {
        // Execute the action if there is an associated action and the name of
        // the column is correct.
        if ( ( 0 != it->m_action ) && ( it->m_name == headerName ) )
        {
            executeAction( it->m_action );
            break;
        }
    }

    *pResult = 0;
}

void EventListComponent::executeAction( const unsigned int actionId )
{
    // Find the associated action.
    IEventAction* action = m_filter->getActionWithId( actionId );
    if ( NULL != action )
    {
        // Execute it.
        CWinThread* thread = AfxBeginThread(RUNTIME_CLASS(ActionThread));
        thread->PostThreadMessage(ActionThread::PERFORM_ACTION,
                                  reinterpret_cast<WPARAM>(action),
                                  reinterpret_cast<LPARAM>(m_filter) );
    }
    else
    {
		LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to execute action %i. No matching action found.", actionId);
    }
}