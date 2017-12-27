/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/EventListComponent/src/Filter.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * Filter is a class that is used as the base class for each type of data filter that the list needs to show. It provides
 * standard filtering methods, and a number of variables to be used by specialised filter classes, as well as a uniform
 * filter interface.
 */

#pragma warning(disable:4503)   // decorated name length exceeded
#pragma warning(disable:4786)   // identifier truncated to 255 characters

#include <algorithm>

#include "app/soe/soe_event_viewer/EventListComponent/src/stdafx.h"
#include "app/soe/soe_event_viewer/EventListComponent/src/Filter.h"
#include "app/soe/soe_event_viewer/EventListComponent/src/EventListComponent.h"
#include "app/soe/soe_event_viewer/src/EventViewer.h"

#include "core/data_access_interface/src/NamedFilter.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const time_t Filter::CURRENT_TIME = 0;
const time_t Filter::PREVIOUS_DAY = 0;
const unsigned long Filter::MASK_PAGEUP = 0x00000001;
const unsigned long Filter::MASK_PAGEDOWN = 0x00000002;
const unsigned long Filter::MASK_REFRESH = 0x00000004;
const unsigned long Filter::MASK_LOADING = 0x00000008;
const unsigned long Filter::MASK_UPDATE_LIST = 0x00000010;
const unsigned long Filter::MASK_SORTING = 0x00000020;

const std::string Filter::FILTER_TOTIME("截止时间");
const std::string Filter::FILTER_FROMTIME("开始时间");

double Filter::m_firstrPKey = -1;
double Filter::m_lastrPKey = -1;

unsigned long Filter::m_maxEvents = 0;

//-------- Filter::Column --------//

Filter::Column::Column( const std::string& name, unsigned long width, unsigned long  action ) :
	m_name(name),
	m_width(width),
    m_action(action)
{
}

Filter::Column::Column( const Filter::Column& rhs ) :
	m_name(rhs.m_name),
	m_width(rhs.m_width),
    m_action(rhs.m_action)
{
}

Filter::Column::~Column()
{
}

void Filter::Column::operator=( const Filter::Column& rhs )
{
	m_name  = rhs.m_name;
	m_width = rhs.m_width;
    m_action = rhs.m_action;
}

//-------- Filter --------//

Filter::Filter() 
: m_filterID(0), m_currentPage(0), m_backgroundColour( RGB(255,255,255) ), m_foregroundColour( RGB(0,0,0) ),
  m_stopEventLoading(false),  m_pauseEventLoading(false), m_namedFilter(NULL), m_eventLevel(100), m_dutyEnabled(true), m_pauseRefresh(false)
{
   m_hwndDlgWnd = AfxGetMainWnd()->GetSafeHwnd();
}

Filter::~Filter()
{
    
}

void Filter::setNamedFilter( std::auto_ptr<TA_Base_Core::NamedFilter> namedFilter )
{
    m_namedFilter = namedFilter;
}

TA_Base_Core::NamedFilter* Filter::getNamedFilter()
{
    return m_namedFilter.get();
}

void Filter::setToTime( time_t toTime )
{
    // Clear the current to time filter.
    m_namedFilter->removeNumericFilter( FILTER_TOTIME );

    // Now add the new time.
    m_namedFilter->addNumericFilter( FILTER_TOTIME, static_cast<unsigned long>(toTime) );
}

time_t Filter::getToTime()
{
    // If the to time doesn't exist in the named filter this value will be used.
    time_t toTime = CURRENT_TIME;

    // Get the current to time.
    TA_Base_Core::NamedFilter::NumericList toTimeList = m_namedFilter->getNumericFilter( FILTER_TOTIME );
    if ( 0 < toTimeList.size() )
    {
        toTime = static_cast<time_t>(toTimeList[0]);
    }

    return toTime;
}

void Filter::setFromTime( time_t fromTime )
{
    // Clear the current from time filter.
    m_namedFilter->removeNumericFilter( FILTER_FROMTIME );

    // Now add the new time.
    m_namedFilter->addNumericFilter( FILTER_FROMTIME, static_cast<unsigned long>(fromTime) );
}

time_t Filter::getFromTime()
{
    // If the from time doesn't exist in the named filter this value will be used.
    time_t fromTime = PREVIOUS_DAY;

    // Get the current to time.
    TA_Base_Core::NamedFilter::NumericList fromTimeList = m_namedFilter->getNumericFilter( FILTER_FROMTIME );
    if ( 0 < fromTimeList.size() )
    {
        fromTime = static_cast<time_t>(fromTimeList[0]);
    }

    return fromTime;
}

COLORREF Filter::getBackgroundColour()
{
    return m_backgroundColour;
}

COLORREF Filter::getRowColour( const unsigned long row )
{
    return m_foregroundColour;
}
void Filter::getRowColour( const unsigned long row, COLORREF& foreground, COLORREF& background )
{
	determineRowColours(row, foreground, background);

	if (foreground < 0) foreground = getRowColour(row);
	if (background < 0) background = getBackgroundColour();
}

void Filter::setFilterID(const int filterID)
{
    m_filterID = filterID;
}


int Filter::getFilterID()
{
    return m_filterID;
}

void Filter::setMaxEvents(const unsigned long maxEvents)
{
    m_maxEvents = maxEvents;
}

void Filter::setfirstrPKey(double firstrPKey)
{
	m_firstrPKey = firstrPKey;
}

double Filter::getfirstrPKey(){
	return m_firstrPKey;
}

void Filter::setlastrPKey(double lastrPKey)
{
	m_lastrPKey = lastrPKey;
}

double Filter::getlastrPKey(){
	return m_lastrPKey;
}

unsigned long Filter::getMaxEvents()
{
    return m_maxEvents;
}

void Filter::setEarliestTime(time_t earliestTime)
{
    m_earliestTime = earliestTime;
}

void Filter::clearList()
{
	 
	{
		for (unsigned int i = 0;i < m_eventStoreListItems.size();i++)
		{
			if (NULL != m_eventStoreListItems[i])
			{
				delete m_eventStoreListItems[i];
				m_eventStoreListItems[i] = NULL;
			}
		}
	}
}

void Filter::loadList()
{
    // Reset stop and pause flags on a new load.
    m_stopEventLoading = false;
    m_pauseEventLoading = false;

    // Load the data.
    getListData();

    // Determine the page button status.
    LPARAM pageStatus = MASK_LOADING | MASK_UPDATE_LIST;
    //if ( 0 < m_currentPage )
    //{
    //    // If we aren't on the first page, we can move up.
    //    pageStatus |= MASK_PAGEUP;
    //}
    //if ( m_displayItems.size() >= m_maxEvents )
    //{
    //    // If we retrieved a full page of events assume we can move down.
    //    pageStatus |= MASK_PAGEDOWN;
    //}
	
    // Send the message to make sure page navigation buttons have the correct status.
	//PostMessage(m_hwndDlgWnd, WM_USER_LISTVIEW_UPDATE, m_displayItems.size(), pageStatus );
	//PostMessage(m_hwndDlgWnd, WM_USER_LISTVIEW_UPDATE, 0, pageStatus );

	// Disable the progress bar.
    //PostMessage(m_hwndDlgWnd, WM_USER_LOAD_COUNT, 0, 0 );
}

void Filter::refreshList()
{
    // If the list didn't change in size don't bother sending any updates.
    if ( refreshListData() )
    {
		 
		{
			// Determine the page button status.
			LPARAM pageStatus = 0;
			if ( 0 < m_currentPage )
			{
				// If we aren't on the first page, we can move up.
				pageStatus |= MASK_PAGEUP;
			}
			if ( m_displayItems.size() >= m_maxEvents )
			{
				// If we are now displaying more than a full page assume
				// we can move down..
				pageStatus |= MASK_PAGEDOWN;
			}

			// Send the message.
			PostMessage(m_hwndDlgWnd, WM_USER_LISTVIEW_UPDATE, m_displayItems.size(), pageStatus );
		}
    }
}

void Filter::sendStatusBarUpdate( const unsigned long eventCount, const char* text )
{
    // Send the message.
	 
    PostMessage( m_hwndDlgWnd, WM_USER_LOAD_COUNT, eventCount, reinterpret_cast<unsigned long>(text) );
}

void Filter::sendListUpdate( const unsigned long numElements )
{
    // Send the message.
    PostMessage(m_hwndDlgWnd, WM_USER_LIST_UPDATE, numElements, 0 );
}

void Filter::sendListViewUpdate( const unsigned long numElements, const unsigned long pageStatus )
{
    // Send the message.
    PostMessage(m_hwndDlgWnd, WM_USER_LISTVIEW_UPDATE, numElements, pageStatus );
}
void Filter::stopCurrentListDataLoad()
{
    m_stopEventLoading = true;
}

void Filter::pauseCurrentListDataLoad()
{
    m_pauseEventLoading = true;
}

void Filter::resumeCurrentListDataLoad()
{
    m_pauseEventLoading = false;
}
void Filter::pauseRefresh()
{
    m_pauseRefresh = true;
}

void Filter::resumeRefresh()
{
    m_pauseRefresh = false;
}
void Filter::sort(const std::string& columnName,const bool sortAscending)
{
    // Sorting is now performed in the SQL query, however we still need to set this
    // to correctly insert new events when refreshing.
    DisplayItem::setSortInfo(columnName,sortAscending);


	// wenguang++ TD14188
	sortListItems();
	LPARAM pageStatus = 0;
	if ( m_displayItems.size() >= m_maxEvents )
	{
		// If we retrieved a full page of events assume we can move down.
		pageStatus |= MASK_PAGEDOWN;
	}
	// Send the message to make sure page navigation buttons have the correct status.
	//PostMessage(m_hwndDlgWnd, WM_USER_LIST_UPDATE, m_displayItems.size(), pageStatus );
	//PostMessage(m_hwndDlgWnd, WM_USER_LISTVIEW_UPDATE, m_displayItems.size(), pageStatus );

	// ++wenguang TD14188
}

void Filter::reverse(const std::string& columnName,const bool sortAscending)
{
    DisplayItem::setSortInfo(columnName,sortAscending);
    // Move back to the first page.
    

	//m_currentPage = 0; //li zhongyan change, it'll not move back to the first page

	reverseListItems();
	
	//li zhongyan change
	//LPARAM pageStatus = 0;
    /*if ( m_displayItems.size() >= m_maxEvents )
    {
        // If we retrieved a full page of events assume we can move down.
        pageStatus |= MASK_PAGEDOWN;
    }*/

	//LPARAM pageStatus = m_pageStatus

    // Send the message to make sure page navigation buttons have the correct status.
    //PostMessage(m_hwndDlgWnd, WM_USER_LIST_UPDATE, m_displayItems.size(), pageStatus );
    //PostMessage(m_hwndDlgWnd, WM_USER_LISTVIEW_UPDATE, m_displayItems.size(), pageStatus );


}
void Filter::setSelectedItems( std::vector<unsigned long>& selectedItems )
{
    m_selectedItems = selectedItems;
}

const std::vector<unsigned long>& Filter::getSelectedItems() const
{
    return m_selectedItems;
}

IEventAction* Filter::getActionWithId( const unsigned int actionId )
{
    std::vector<IEventAction*>::iterator it;
    for ( it=m_actions.begin(); it!=m_actions.end(); it++ )
    {
        if ( (*it)->getActionId() == actionId )
        {
            return *it;
        }
    }

    // If the action doesn't exist return NULL.
    return NULL;
}

void Filter::setEventLevel( const unsigned long eventLevel )
{
    m_eventLevel = eventLevel;
}

unsigned long Filter::getEventLevel()
{
    return m_eventLevel;
}

void Filter::setDutyEnabled( const bool enabled )
{
    m_dutyEnabled = enabled;
}

bool Filter::isDutyEnabled()
{
    return m_dutyEnabled;
}