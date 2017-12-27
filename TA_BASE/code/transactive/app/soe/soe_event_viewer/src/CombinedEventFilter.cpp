/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/CombinedEventFilter.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2009/12/03 17:15:17 $
  * Last modified by:  $Author: miao.yu $
  *
  * CombinedEventFilter is an implementation of the abstract class Filter, from the
  * EventListComponent. It specifies the details of events from both the EVENT table
  * and the LO_DATAPT_STATE_UPDATE table, and includes the column names, and
  * filterable data items, in constant static variables.
  *
  */

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "stdafx.h"

#include <algorithm>
#include <functional>
#include <map>
#include <utility>


#include "app/soe/soe_event_viewer/EventListComponent/src/EventListComponent.h"	// ly++ for vs2005, WM_USER_LISTVIEW_UPDATE
 
#include "app/soe/soe_event_viewer/src/CombinedEventFilter.h"
#include "app/soe/soe_event_viewer/src/CombinedEventFilterDlg.h"
#include "app/soe/soe_event_viewer/src/CombinedEventItem.h"
#include "app/soe/soe_event_viewer/src/DatabaseAccess.h"
#include "app/soe/soe_event_viewer/src/CreateDefaultFilterThread.h" //zhiqiang++

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/CombinedEventAccessFactory.h"
#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/data_access_interface/soe/src/SOEEvent.h"
#include "core/data_access_interface/soe/src/SOEFilter.h"
#include "core/data_access_interface/soe/src/SOEAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"


using TA_Base_Core::CombinedEventAccessFactory;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ICombinedEventData;
using TA_Base_Core::IDatabaseCallbackUser;

using TA_Base_Core::SOEEvent;
using TA_Base_Core::SOEAccessFactory;

// Assign the required values to the column constants.
 
const std::string CombinedEventFilter::COLUMN_TRANSACTIVE_TIME("系统日期/时间");
const std::string CombinedEventFilter::COLUMN_SOURCE_DATETIME("SOE日期/时间");		// source_time 
const std::string CombinedEventFilter::COLUMN_ASSET("设备");
const std::string CombinedEventFilter::COLUMN_DESCRIPTION("描述"); 
const std::string CombinedEventFilter::COLUMN_DISPLAY_VALUE("值");
const std::string CombinedEventFilter::COLUMN_LOCATION_NAME("位置");

// Assign the required values to the hidden column constants.
 
const std::string CombinedEventFilter::FILTER_TRANSACTIVE_TIME("系统日期/时间");
const std::string CombinedEventFilter::FILTER_SOURCE_DATETIME("SOE日期/时间");	
const std::string CombinedEventFilter::FILTER_ASSET("设备");
const std::string CombinedEventFilter::FILTER_DESCRIPTION("描述");
const std::string CombinedEventFilter::FILTER_DISPLAY_VALUE("值");
const std::string CombinedEventFilter::FILTER_LOCATION("位置"); 


 

// The name of the default filter.
const std::string CombinedEventFilter::NAMEDFILTER_DEFAULT( "默认过滤器" );

const unsigned long CombinedEventFilter::MIN_DISPLAY_ITEM_NUM = 50;

const unsigned long CombinedEventFilter::REFRESH_PRECOUNT =1000;
namespace
{
    const unsigned int TWENTY_FOUR_HOURS = 24*60*60;
    const std::string EMPTY_STRING("");

	const COLORREF COLOUR_BLACK( RGB(0, 0, 0) );
    const COLORREF COLOUR_BEIGE( RGB( 230, 227, 194 ) );
    const COLORREF COLOUR_DARKBEIGE( RGB( 219, 186, 119 ) );

    const unsigned long DEFAULT_EVENT_LEVEL( 5 );

    //const std::string STATUS_QUERYING( "Executing database query..." );
    //const std::string STATUS_DUTY( "Retrieving duty..." );
    const std::string STATUS_QUERYING( "执行数据库查询..." );
    const std::string STATUS_DUTY( "获取职责..." );
}

// ly++ for vs2005 2009-06-15, override Filter::sendListViewUpdate method
// reason: LoadingThread::run() method can not use AfxGetMainWnd() get frame windows handle
void CombinedEventFilter::sendListViewUpdate(const unsigned long numElements, const unsigned long pageStatus)
{
	if ( NULL == m_hwndDlgWnd )
	{
		Filter::sendListViewUpdate( numElements, pageStatus );
	}
	else
	{
		PostMessage( m_hwndDlgWnd, WM_USER_LISTVIEW_UPDATE, numElements, pageStatus );
	}
}
// ++ly


CombinedEventFilter::CombinedEventFilter() : m_latestPkey(0), m_authenticationLib(NULL), m_pageStatus(0),
                                             m_pCreateDefaultFilterThread(NULL) //zhiqiang++
											 , m_hwndDlgWnd( NULL ) // ++ly
											 , m_nAuditPKey( 0 )
{
    // Initialise the default background colour to beige and foreground to black.
	m_foregroundColour = COLOUR_BLACK;
    m_backgroundColour = COLOUR_BEIGE;

    // Set up the actions.
    //m_actions.push_back( new ActionCreateIncidentReport() );
    //m_actions.push_back( new ActionShowComment() );


    m_columns.push_back( Filter::Column( COLUMN_TRANSACTIVE_TIME, 200, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_SOURCE_DATETIME, 200, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_ASSET, 210, ACTION_NO_ACTION ) );
	m_columns.push_back( Filter::Column( COLUMN_DESCRIPTION, 210, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_DISPLAY_VALUE, 170, ACTION_NO_ACTION ) );
    //m_columns.push_back( Filter::Column( COLUMN_LOCATION_NAME, 180, ACTION_NO_ACTION ) );

	CreateDefaultFilter();

	// ly++ vs2005 update
	// save main windows handle, PostMessage use
	m_hwndDlgWnd = AfxGetMainWnd()->GetSafeHwnd();
	// ++ly

	// Setup loading thread.
	//m_loadingThread = new LoadingThread(this);
 
	// Set default sorting.
	sort( getDefaultSortColumn(), false );


}

CombinedEventFilter::~CombinedEventFilter()
{
    FUNCTION_ENTRY("destructor");

    unsigned int i;
    TA_Base_Core::ThreadGuard guard(m_storedItemsLock);
	TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
    // Remove all columns.
    m_columns.clear();

    
	{
		// All event items not excluded by current filter are stored in client side.
		for( i=0; i<m_eventStoreListItems.size(); i++)
		{
			delete m_eventStoreListItems[i];
		}
		m_eventStoreListItems.clear();
	}

	m_displayItems.clear();


    // The filter owns any actions so we need to delete them.
    for ( i=0; i<m_actions.size(); i++ )
    {
        delete m_actions[i];
    }
    m_actions.clear();
	if( m_authenticationLib != NULL )
	{
		delete m_authenticationLib;
		m_authenticationLib = NULL;
	}
	//zhiqiang++begin
	if (m_pCreateDefaultFilterThread != NULL)
	{
		delete m_pCreateDefaultFilterThread;
		m_pCreateDefaultFilterThread = NULL;
	}
    //zhiqiang++end
    FUNCTION_EXIT;
}

CombinedEventFilter::LoadingThread::LoadingThread(CombinedEventFilter* filter) : m_isStopped (true) , m_filter(filter){}
CombinedEventFilter::LoadingThread::~LoadingThread() {}

bool CombinedEventFilter::LoadingThread::getStopFlag()
{
	return m_isStopped;
}

void CombinedEventFilter::LoadingThread::setColumnRange(std::string startTime, unsigned long totalRows)
{
	m_startTimeStr = startTime;
	m_totalRows = totalRows;
}

void CombinedEventFilter::LoadingThread::terminate() {}

void CombinedEventFilter::LoadingThread::run()
{
	 
	
}

void CombinedEventFilter::internalGetListData(std::string startTime, unsigned long totalRows)
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log: Entering Loading Thread. ");	 
	setPauseRefreshFlag(true);
	//CombinedEventAccessFactory::EventFilter filter;
	//populatePublicFilter(filter);
	

	try
	{
		CWnd* pWnd = AfxGetMainWnd();
		std::vector<SOEEvent*> events;
		TA_Base_Core::SOEEventFilter filter;
		populateSOEFilter( &filter );
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "populating filter finished."); 
	 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Begin calling loadDesignatedEvents() function.");
		events = SOEAccessFactory::getInstance().loadDesignatedEvents(filter, startTime, totalRows);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Stop calling loadDesignatedEvents() function.");

  		if(!events.empty())
		{
			std::vector<SOEEvent*>::iterator eventIt; 
			{
				TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
				for ( eventIt=events.begin(); eventIt!=events.end(); eventIt++ )
				{
					CombinedEventItem* newItem = new CombinedEventItem(*eventIt);
					m_eventStoreListItems.push_back(newItem);
// 					if ( COLUMN_DATETIME == DisplayItem::s_sortColumn )
// 						m_displayItems.push_back(newItem);
// 					else
// 						insertBinaryDisplayItem(newItem, filter.bSortAscend);
					delete *eventIt; //li zhongyan add delete
				}
				events.clear();
				
			}

			assignDisplayListItems();
			refreshListView();	 

			// retrieve more events
			events = SOEAccessFactory::getInstance().retrieveMoreSOEEvents();

			// Pause if required.
			while ( getPauseLoadingFlag() && !m_stopEventLoading )
			{
				Sleep( 250 );
			}
		    preloadListView(Filter::MASK_LOADING | Filter::MASK_UPDATE_LIST);
		 
		}
       
		TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
		unsigned long count=0;
		while ( !events.empty() && !getStopLoadingFlag() )
		{			 
            // Add the events to the store list.
			std::vector<SOEEvent*>::iterator eventIt;
			for ( eventIt=events.begin(); eventIt!=events.end(); eventIt++ )
			{
				//std::auto_ptr<CombinedEventItem> newItem;
				CombinedEventItem* newItem = new CombinedEventItem(*eventIt);
                count++;
				m_eventStoreListItems.push_back(newItem);
				if ((count%800)==0  && !getStopLoadingFlag() )
				{
					Sleep( 250 );
				}
// 				if ( COLUMN_DATETIME == DisplayItem::s_sortColumn )
// 				{
// 					m_filter->m_displayItems.push_back(newItem);
// 				}
// 				else
// 					m_filter->insertBinaryDisplayItem(newItem, filter.bSortAscend);
				delete *eventIt; //li zhongyan add delete
			}		 
			events.clear();			 

			// Get the next set of events.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:calling retrieveMoreSOEEvents() function.");

			events = SOEAccessFactory::getInstance().retrieveMoreSOEEvents();

			// Pause if required.
			while ( getPauseLoadingFlag() && getStopLoadingFlag() )
			{
				Sleep( 250 );
			}

		}

	}
	catch(...)
	{
		// If there's a problem (any problem), notify the user, then return 0.
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UE_040030);
		LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"Database load was not successful");
	}

// 	if(m_filter->m_eventStoreListItems.size() > 0)
// 	{
// 		std::vector<DisplayItem*>::iterator iterend = m_filter->m_eventStoreListItems.end() - 1;
// 		pageEventIDbound = ((CombinedEventItem*)*iterend)->getrPKey();
// 	}
// 
// 	m_filter->m_pagebottomEventIDMap[m_filter->m_currentPage+1] = pageEventIDbound;
// 
// 	if ( COLUMN_DATETIME == DisplayItem::s_sortColumn && DisplayItem::s_sortAscending == true){
// 		std::reverse(m_filter->m_displayItems.begin(), m_filter->m_displayItems.end());
// 	}

	assignDisplayListItems();
	refreshListView();	 
	setPauseRefreshFlag(false);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log: Loading Thread stops.");
}



void CombinedEventFilter::getListData(bool clearPage /*=true*/)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "New EV Log:Entering getListData() function. ");

	//Wait the loading thread to stop.
	m_stopEventLoading = true;
// 	while(!m_loadingThread->getStopFlag())
// 	{
// 		TA_Base_Core::Thread::sleep(200);
// 	}
	//m_loadingThread->terminateAndWait();
	unsigned long eventCount;

    // Clear all selections when loading from scratch.
    m_selectedItems.clear();

    // Querying the duty agent.
    sendStatusBarUpdate( 0, STATUS_QUERYING.c_str() );

	// Determine the page status
	if (clearPage)
	{
		m_currentPage = 0;
		m_pageTimeMap.clear();
		m_pageTimeMap.insert(PageTimeMap::value_type(m_currentPage, EMPTY_STRING));
		    LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:current Page %d. ", m_currentPage);
	}
	bool hasNextPage = false;
	//populateFilter cannt be called until createDefaultFilter thread finish all work
	//m_pCreateDefaultFilterThread->terminateAndWait(); //zhiqiang++

    CombinedEventAccessFactory::EventFilter filter;

    // Notify that the list control contains no elements.
    sendListUpdate( 0 );

	m_stopEventLoading = false;
	
	sendListViewUpdate(0,0);

	// Clean up all of the old items.
	// wenguang++ TD14188	
	std::vector<DisplayItem*>::iterator itemIt;
	{  
		TA_Base_Core::ThreadGuard guard(m_storedItemsLock);		
		for ( itemIt=m_eventStoreListItems.begin(); itemIt!=m_eventStoreListItems.end(); itemIt++ )
		{
			delete *itemIt;
		}
		m_eventStoreListItems.clear();
	}
	{ 
		TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
		// Clean up all of the old items.
		// wenguang++ TD14188
		m_displayItems.clear();
		m_displayItems.reserve(m_maxEvents);
	}
	

	//Starting the loading thread

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:Start the loading thread.");
	m_stopEventLoading = false;
	PageTimeMap::iterator pageIt = m_pageTimeMap.find(m_currentPage);
	if (pageIt != m_pageTimeMap.end())
	{
		//m_loadingThread->setColumnRange(pageIt->second, m_maxEvents+1);
		internalGetListData(pageIt->second, m_maxEvents+1);
	}else{
		//m_loadingThread->setColumnRange("", m_maxEvents+1);
		internalGetListData("", m_maxEvents+1);
	}
	sendStatusBarUpdate(0,NULL);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:Leaving getListData() function.");
}

bool CombinedEventFilter::refreshListData()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering refreshListData() function. ");
	if ( m_pauseRefresh ) return false;//changxing
	if (m_currentPage>0) return false;//chenlei
//	if (!m_loadingThread->getStopFlag()) return false; //chenlei
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Refreshing event data...");

    bool changeMade = false;


	try
	{
		// update GUI display
		preloadListView( m_pageStatus | Filter::MASK_REFRESH );
		
		// Load new events since the last load.
		std::vector<SOEEvent*> events;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering getDesignatedEventsSinceLastLoad.");
		
		// get subsequent events
		TA_Base_Core::SOEEventFilter filter;
		populateSOEFilter( &filter );

		events = SOEAccessFactory::getInstance().getDesignatedEventsSinceLastLoad();
		//events = SOEAccessFactory::getInstance().retrieveSOEEvents(  &m_nAuditPKey, &filter );
		
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "REFRESH: Found %d new elements.", events.size() );

		TA_Base_Core::ThreadGuard guard(m_storedItemsLock);
		TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
		
		std::vector<DisplayItem*>::iterator iterend = m_eventStoreListItems.end();

		if ( 0 < events.size() )
		{
			changeMade = true;
			std::reverse(m_eventStoreListItems.begin(), m_eventStoreListItems.end());
			int iterdistance = m_eventStoreListItems.end() - m_eventStoreListItems.begin();

			while( events.size() )
			{

				// Allocate required space in the display list.
				m_eventStoreListItems.reserve( m_eventStoreListItems.size() + events.size() );
				
				// Add the events to the display list in the correct position.
				std::vector<SOEEvent*>::iterator eventIt = events.begin();
				//std::vector<DisplayItem*>::iterator dispiter = m_eventStoreListItems.begin();
				for ( ; eventIt!=events.end(); eventIt++ )
				{
					//std::auto_ptr<CombinedEventItem> newItem;
					CombinedEventItem* newItem;
					newItem =  new CombinedEventItem( *eventIt);			 
					m_eventStoreListItems.push_back(newItem);
					delete *eventIt;
				}

				// retrive more
				events = SOEAccessFactory::getInstance().retrieveMoreSOEEvents();
			}
			//std::reverse(m_eventStoreListItems.begin()+iterdistance, m_eventStoreListItems.end());
			//std::reverse(m_eventStoreListItems.begin(), m_eventStoreListItems.end());
			std::sort(m_eventStoreListItems.begin(), m_eventStoreListItems.end(), compare_events());

			LPARAM pageStatus = 0;
			if(m_eventStoreListItems.size() > m_maxEvents)
			{
				pageStatus |= MASK_PAGEDOWN;
				std::vector<DisplayItem*>::iterator toDelete;
				for ( toDelete=m_eventStoreListItems.begin()+m_maxEvents; toDelete!=m_eventStoreListItems.end(); toDelete++ )
				{
					delete *toDelete;
				}
				m_eventStoreListItems.erase(m_eventStoreListItems.begin()+m_maxEvents, m_eventStoreListItems.end());
				if (m_eventStoreListItems.size() > 0)
				{
					int lastitem = m_eventStoreListItems.size() - 1;
					std::string  str= static_cast<CombinedEventItem*>(m_eventStoreListItems[lastitem])->getCreateTimeStr();
					
					//li zhongyan add
					double r_PKey = static_cast<CombinedEventItem*>(m_eventStoreListItems[lastitem])->getrPKey();

					int pagenum = m_currentPage+1;
					m_pageTimeMap.insert( PageTimeMap::value_type(pagenum, str) );
					
					//li zhongyan add insert into map for paging
					//if(DisplayItem::s_sortAscending == false)
					//m_pagebottomEventIDMap.insert(PagebottomEventIDnMap::value_type(pagenum, r_PKey));
					std::map<unsigned int, double>::iterator it;
					/*it = m_pagebottomEventIDMap.find(m_currentPage+1);
					if(it != m_pagebottomEventIDMap.end())
					{
						//(*it)
						double secondvalue = it->second;
						if(secondvalue > r_PKey)
						{
							m_pagebottomEventIDMap.insert(PagebottomEventIDnMap::value_type(m_currentPage+1, r_PKey ));
						}
					}*/
					double pageEventIDbound = 0;
					if(m_eventStoreListItems.size() > 0)
					{
						std::vector<DisplayItem*>::iterator iterend = m_eventStoreListItems.end() - 1;
						pageEventIDbound = ((CombinedEventItem*)*iterend)->getrPKey();
					}
					//m_pagebottomEventIDMap.insert(PagebottomEventIDnMap::value_type(m_currentPage+1,pageEventIDbound ));
					m_pagebottomEventIDMap[m_currentPage+1] = pageEventIDbound;

				}
					//m_pageTimeMap.insert(PageTimeMap::value_type(m_currentPage+1, static_cast<CombinedEventItem*>(m_eventStoreListItems[m_maxEvents])->getCreateTimeStr()));
			}
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "New EV Log:current Page %d. ", m_currentPage);
			m_pageStatus = pageStatus;
			// Copy the list items to display list.
			m_displayItems.clear();

			if( COLUMN_SOURCE_DATETIME == DisplayItem::s_sortColumn )
			{
				for(std::vector<DisplayItem*>::iterator it = m_eventStoreListItems.begin(); it != m_eventStoreListItems.end(); it++)
					m_displayItems.push_back(*it);
				if(DisplayItem::s_sortAscending == true)
					std::reverse(m_displayItems.begin(), m_displayItems.end());
			}
			else
				for(std::vector<DisplayItem*>::iterator it = m_eventStoreListItems.begin(); it != m_eventStoreListItems.end(); it++)
					insertBinaryDisplayItem(*it, DisplayItem::s_sortAscending);

			//m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());

			// update list
			pageStatus |= Filter::MASK_UPDATE_LIST;

			sendListViewUpdate(m_displayItems.size(), pageStatus);
		}
// 		else
// 		{
// 			// inform GUI load complete
// 			//preloadListView( m_pageStatus & ( Filter::MASK_PAGEUP | Filter::MASK_PAGEDOWN ) );
// 		}
	}
	catch(...)
	{
		// If there's a problem (any problem), notify the user, then return 0.
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UE_040030);
		LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"Database load was not successful");
	}
    sendStatusBarUpdate(0,NULL);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving refreshListData() function. ");
    return changeMade;
}

const std::vector<Filter::Column>& CombinedEventFilter::getColumns() const
{
    return m_columns;
}

const std::vector<std::string> CombinedEventFilter::getFilters() const
{
    // Filters will be returned 
    std::vector<std::string> filters;

    // Add all the filters to the list.
    filters.push_back(FILTER_SOURCE_DATETIME);
	filters.push_back(FILTER_TRANSACTIVE_TIME);
	filters.push_back(FILTER_ASSET);
	filters.push_back(FILTER_DESCRIPTION);
    filters.push_back(FILTER_DISPLAY_VALUE);
 

 
    return filters;
}

const std::string& CombinedEventFilter::getItemData(const unsigned long index,const std::string& columnName,const bool summarise/*=true*/)
{
    // This array index operation has a small window where it can fail - which is the time between the data vector
    // being updated, but before the "SetItemCount" method can be called on the list. For this time (possilby only two-to
    // three calls of this method, and only when the vector is being shrunk, while the end of the list is still visible)
    // there needs to be a backup solution - so instead of crashing the application, we just return an empty string. It 
    // does nothing, and will leave an empty cell (if an update ever has time to complete its paint - which is unlikely),
    // but it is the best solution.

	int i=0;
	size_t nItemsCount = m_displayItems.size();
	LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log: Getting Item No. %d.", index);
	TA_Base_Core::ThreadGuard guard(m_displayItemsLock);
	if (index >= nItemsCount)
	{
		return EMPTY_STRING;
	}

    try
    {
        if ( summarise )
        {
			return m_displayItems[index]->getColumnData(columnName);
        }
        else
        {
            return m_displayItems[index]->getCompleteColumnData(columnName);
        }
    }
    catch(...)
    {
        return EMPTY_STRING;
    }
}

COLORREF CombinedEventFilter::getRowColour( const unsigned long row )
{
    // Make sure the row isn't out of range.
	COLORREF foreground = m_foregroundColour;
	COLORREF background = m_backgroundColour;	 
	return foreground;
}

COLORREF CombinedEventFilter::getRowColourDefault( TA_Base_Core::IEventColourData::EEventColourType colourType  )
{
	COLORREF groundColor = m_foregroundColour;
    
	switch (colourType)
	{
		//foreground
		case TA_Base_Core::IEventColourData::OPEN_UNACKED_FG1:
		case TA_Base_Core::IEventColourData::CLOSED_UNACKED_FG1:
		case TA_Base_Core::IEventColourData::OPEN_ACKED_FG1:
		case TA_Base_Core::IEventColourData::CLOSED_ACKED_FG1:
		{
			groundColor = m_foregroundColour;
			break;
		}
		//background
		case TA_Base_Core::IEventColourData::OPEN_UNACKED_BG1:
		case TA_Base_Core::IEventColourData::CLOSED_UNACKED_BG1:
		case TA_Base_Core::IEventColourData::OPEN_ACKED_BG1:
		case TA_Base_Core::IEventColourData::CLOSED_ACKED_BG1:
		{
			groundColor = m_backgroundColour;
			break;
		}
		default:
		{
			groundColor = m_foregroundColour;
			break;
		}
	}
	
	return groundColor;
}

void CombinedEventFilter::determineRowColours( const unsigned long row, unsigned long& foregroundColour, unsigned long& backgroundColour )
{	 
	foregroundColour = m_foregroundColour;
	backgroundColour = m_backgroundColour;	 	 
}

const COLORREF CombinedEventFilter::getColourReference(const unsigned long row, TA_Base_Core::IEventColourData::EEventColourType colourType)
{
	return getRowColourDefault(colourType); //Use other getRowColour function to avoid infinite loop
}

const COLORREF CombinedEventFilter::getItemColour(const unsigned long index, const std::string& columnName)
{
	COLORREF foreground = getRowColour(index);
	COLORREF background = getBackgroundColour();

	determineRowColours(index, foreground, background);

	return background; // For print the foreground is always black while the background varies in colour
}

// yanrong++ CL-15055
std::string CombinedEventFilter::privGetCurrentFilterString()
{
    FUNCTION_ENTRY("CombinedEventFilter::privGetCurrentFilterString");

    // Filter data is contained in two maps - a string-string and a string-long map, so create iterators for both.
    TA_Base_Core::NamedFilter::StringList stringFilterList = m_namedFilter->getStringFilterList();
    TA_Base_Core::NamedFilter::StringList numericFilterList = m_namedFilter->getNumericFilterList();

    TA_Base_Core::NamedFilter::StringList::iterator stringIter;

    // Iterate through each map, adding any filters to the string..
    std::string filterString("");
    for ( stringIter=stringFilterList.begin(); stringIter!=stringFilterList.end(); stringIter++ )
    {
        filterString += " [" + *stringIter + "]";
    }

	//to make sure from time display in front of to time on caption
	bool hasFilterToTime = false;
	bool hasFilterFromeTime = false;
    for ( stringIter=numericFilterList.begin(); stringIter!=numericFilterList.end(); stringIter++ )
    {
		if( *stringIter == SOEEvent::COL_NAME_LOCATION_NAME )
		{
			filterString += " [位置]";
		}
		else
		{
			if( *stringIter == Filter::FILTER_TOTIME )
			{
				if(hasFilterFromeTime)
				{
					filterString += " [" + *stringIter + "]";
				}
				else
				{
					hasFilterToTime = true;
				}
			}
			else
			{
				filterString += " [" + *stringIter + "]";
				if( *stringIter == Filter::FILTER_FROMTIME && hasFilterToTime)
				{
					hasFilterFromeTime = true;
					hasFilterToTime = false;
					filterString += " [" + Filter::FILTER_TOTIME + "]";
				}
			}
		}
    }
	if(hasFilterToTime)
	{
		filterString += " [" + Filter::FILTER_TOTIME + "]";
	}

    FUNCTION_EXIT;
    return filterString;
}


void CombinedEventFilter::saveAsDefaultFilter(const TA_Base_Core::NamedFilter& filter)
{
	m_defaultFilterString = privGetCurrentFilterString();
}
// ++yanrong CL-15055



std::string CombinedEventFilter::getCurrentFilterString()
{
    FUNCTION_ENTRY("CombinedEventFilter::getCurrentFilterString");

    std::string filterString = privGetCurrentFilterString();
	if(filterString.compare(m_defaultFilterString) == 0)
		filterString = "";

    FUNCTION_EXIT;
    return filterString;
}

const std::string& CombinedEventFilter::getDefaultSortColumn()
{
    // Default sort will always apply to time column.
    return COLUMN_SOURCE_DATETIME;
}

void CombinedEventFilter::resetListLoadFlag()
{
    // Reset load flag on the DAI.
    CombinedEventAccessFactory::getInstance().resetStopFlag();
}

void CombinedEventFilter::populateFilter( TA_Base_Core::CombinedEventAccessFactory::EventFilter& filter, bool refresh /*=false*/ )
{
    FUNCTION_ENTRY("CombinedEventFilter::populateFilter");

    // Define two vectors for retrieveing filter data
    std::vector<std::string> stringVector;
    std::vector<unsigned long> ulVector;

    filter.dutyEnabled = false;
     
    
     

    // Location Ids
    ulVector = m_namedFilter->getNumericFilter(FILTER_LOCATION);
    if ( !ulVector.empty() )
    {
        filter.locationIds = ulVector;
    }

   
    
    // Asset Name
    stringVector = m_namedFilter->getStringFilter(FILTER_ASSET);
    if ( !stringVector.empty() )
    {
        filter.assetName = "%" + stringVector[0] + "%";
    }
    
    // Start with description
    stringVector = m_namedFilter->getStringFilter(FILTER_DESCRIPTION);
    if ( !stringVector.empty() )
    {
        filter.description = "%" + stringVector[0] + "%";
    }

    // Set the time range.
    filter.fromTime = getFromTime();
    filter.toTime = getToTime();

    // Don't show non-alarm events by default.
    filter.showNonAlarmEvents = false;

    

    // Event level.
    filter.eventLevel = m_eventLevel;

    // Column to sort by.
    if ( COLUMN_SOURCE_DATETIME == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_CREATETIME;
    }
     
    else if ( COLUMN_ASSET == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_ASSET;
    }
    else if ( COLUMN_DESCRIPTION == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_DESCRIPTION;
    }
    else if ( COLUMN_DISPLAY_VALUE == DisplayItem::s_sortColumn )
    {
		//filter.sortColumn = CombinedEventAccessFactory::sort_d;
    }
     
     
    else
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_CREATETIME;
    }

    // Order in which the sort should be performed.
    if ( DisplayItem::s_sortAscending )
    {
        filter.sortOrder = CombinedEventAccessFactory::SORT_ASCENDING;
    }
    else
    {
        filter.sortOrder = CombinedEventAccessFactory::SORT_DESCENDING;
    }

    FUNCTION_EXIT;
}



void CombinedEventFilter::nextPage()
{
	FUNCTION_ENTRY("nextPage");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering nextPage() function. ");
	m_currentPage++;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:current page %d. ", m_currentPage);
	//getListData(false);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving nextPage() function. ");
	FUNCTION_EXIT;
}

void CombinedEventFilter::previousPage()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering previousPage() function. ");
	FUNCTION_ENTRY("previousPage");
	if( m_currentPage > 0 )
	{
		m_currentPage--;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:current page %d. ", m_currentPage);
	//getListData(false);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving previousPage() function. ");
	FUNCTION_EXIT;
}


void CombinedEventFilter::sortListItems()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:Entering sortListItems() function. ");
	FUNCTION_ENTRY("sortListItems");

	pauseRefresh();

	//while (!m_loadingThread->getStopFlag())
	//{
	//	TA_Base_Core::Thread::sleep(200);
	//}

    //sendListUpdate( 0 );
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	//li zhongyan change
	/*m_displayItems.clear();
	for(std::vector<DisplayItem*>::iterator it = m_eventStoreListItems.begin(); it != m_eventStoreListItems.end(); it++)
		insertBinaryDisplayItem(*it, DisplayItem::s_sortAscending);*/
	TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
	std::sort(m_displayItems.begin(), m_displayItems.end(), compare_events());
	//m_eventStoreListItems.assign(m_displayItems.begin(), m_displayItems.end());
	//std::stable_sort(m_displayItems.begin(), m_displayItems.end(), compare_events());
	//m_displayItems.clear();
	//m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());


	sendListViewUpdate(m_displayItems.size(), m_pageStatus);
	resumeRefresh();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:Leaving sortListItems() function. ");
	FUNCTION_EXIT;
}


void CombinedEventFilter::reverseListItems()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:Entering reverseListItems() function. ");
	FUNCTION_ENTRY("reverseListItems");

// 	while (!m_loadingThread->getStopFlag())
// 	{
// 		TA_Base_Core::Thread::sleep(200);
// 	}
    sendListUpdate( 0 );
    TA_Base_Core::ThreadGuard guard (m_storedItemsLock);

//	std::reverse(m_eventStoreListItems.begin(), m_eventStoreListItems.end());

//	m_displayItems.clear();

	std::reverse(m_displayItems.begin(), m_displayItems.end());
//	m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());

	sendListViewUpdate(m_displayItems.size(), m_pageStatus);

	PostMessage( AfxGetMainWnd()->GetSafeHwnd(), WM_USER_LIST_UPDATE, m_displayItems.size(), m_pageStatus ); // li zhongyan add

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:Leaving reverseListItems() function. ");
	FUNCTION_EXIT;
}

void CombinedEventFilter::preloadListView( unsigned long nStatus /* = 0 */ )
{
	TA_Base_Core::ThreadGuard storeGuard(m_storedItemsLock);
	sendListViewUpdate(static_cast<unsigned long>(m_displayItems.size()), nStatus);
	LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log: updating list view with total number %d", m_maxEvents);	
}

void CombinedEventFilter::sessionChanged()
{
	TA_Base_Core::ThreadGuard storeGuard(m_sessionMapLock);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sessionChanged");
	m_SessionMap.clear();
}

void CombinedEventFilter::refreshListView()
{
	// std::copy( m_eventStoreListItems.begin()+oneHourEventsCount, m_eventStoreListItems.end(), m_displayItems.begin()+oneHourEventsCount);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:call refreshListView() function. ");
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
	//m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());
	LPARAM pageStatus = Filter::MASK_UPDATE_LIST;
	
	// page up/down is only available when sorting on DateTime
	if( DisplayItem::s_sortColumn == CombinedEventFilter::COLUMN_SOURCE_DATETIME )
	{
		if ( 0 < m_currentPage )
		{
			// If we aren't on the first page, we can move up.
			pageStatus |= MASK_PAGEUP;
		}
		if ( m_displayItems.size() > m_maxEvents )
		{
			// If we are now displaying more than a full page assume
			// we can move down..
			pageStatus |= MASK_PAGEDOWN;
			//m_pageTimeMap.insert(PageTimeMap::value_type(m_currentPage+1, static_cast<CombinedEventItem*>(m_displayItems[m_maxEvents])->getCreateTimeStr()));
			//double r_PKey = static_cast<CombinedEventItem*>(m_displayItems[m_maxEvents])->getrPKey();
			m_pageTimeMap.insert(PageTimeMap::value_type(m_currentPage+1, m_displayItems[m_maxEvents]->getColumnData(CombinedEventFilter::COLUMN_SOURCE_DATETIME)));
			
			//li zhongyan add
			//if(DisplayItem::s_sortAscending == false)
			//	m_pagebottomEventIDMap.insert(PagebottomEventIDnMap::value_type(m_currentPage+1, r_PKey ));

			LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:current Page %d. ", m_currentPage);
			//li zhongyan
			double r_PKey = static_cast<CombinedEventItem*>(m_displayItems[m_maxEvents])->getrPKey();
			std::map<unsigned int, double>::iterator it;
			it = m_pagebottomEventIDMap.find(m_currentPage+1);
			if(it != m_pagebottomEventIDMap.end())
			{
				//(*it)
				double secondvalue = it->second;
				if(secondvalue > r_PKey)
				{
					m_pagebottomEventIDMap.insert(PagebottomEventIDnMap::value_type(m_currentPage+1, r_PKey ));
				}
			}
			//m_pagebottomEventIDMap.insert(PagebottomEventIDnMap::value_type(m_currentPage+1, r_PKey ));
		}
	}
	m_pageStatus = pageStatus;
	//sendListViewUpdate( (m_displayItems.size() > m_maxEvents)?m_maxEvents:m_displayItems.size(), m_pageStatus);
	sendListViewUpdate( m_displayItems.size(), m_pageStatus);
	LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log: updating list view with total number %d", (m_displayItems.size() > m_maxEvents)?m_maxEvents:m_displayItems.size());
}

void CombinedEventFilter::reserveStoredListItemsSize(unsigned long newSize)
{
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	m_eventStoreListItems.reserve(newSize);
}

void CombinedEventFilter::pushIntoStoredListItems(CombinedEventItem* eventItem)
{
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	m_eventStoreListItems.push_back(eventItem);
}

unsigned long CombinedEventFilter::getStoredListItemsSize()
{
	return m_eventStoreListItems.size();
}

void CombinedEventFilter::populatePublicFilter(TA_Base_Core::CombinedEventAccessFactory::EventFilter &filter, bool refresh /* = false */)
{
	populateFilter(filter, refresh);
}

void CombinedEventFilter::assignDisplayListItems()
{
	//to do the lock
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);	
	m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());
}

//*zhiqiang++begin
void CombinedEventFilter::CreateDefaultFilter()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "void CombinedEventFilter::InitialFilterSetupInfo [enter initial filter info] ");
	
	m_earliestTime = 0;
    
	// Create the default filter.
    std::auto_ptr<TA_Base_Core::NamedFilter> defaultFilter( new TA_Base_Core::NamedFilter() );
    //defaultFilter->addNumericFilter( Filter::FILTER_TOTIME, Filter::CURRENT_TIME );
    //defaultFilter->addNumericFilter( Filter::FILTER_FROMTIME, Filter::PREVIOUS_DAY );
	
    	
    // Set the severities for both filters.
    DatabaseAccess data;
     
	
    // Give the filters appropriate names.
    defaultFilter->setName( NAMEDFILTER_DEFAULT );
     
	
    // Use the default filter and event level.
    m_namedFilter = std::auto_ptr<TA_Base_Core::NamedFilter>( defaultFilter->clone() );
    

	// yanrong++ CL-15055
	saveAsDefaultFilter(*defaultFilter.get());
	// ++yanrong CL-15055
	
	// Initialise and obtain the event colour maps
	m_eventColoursMap = data.getEventColours();
	
	// obtain the default foreground and background colours from the map
	std::map<std::string, TA_Base_Core::IEventColourData*>::iterator colourItr = m_eventColoursMap.find("DEFAULT");
	if (colourItr != m_eventColoursMap.end() )
	{
		TA_Base_Core::IEventColourData* eventColData = colourItr->second;
		m_foregroundColour = eventColData->getEventColourKey(TA_Base_Core::IEventColourData::OPEN_ACKED_FG1);
		m_backgroundColour = eventColData->getEventColourKey(TA_Base_Core::IEventColourData::OPEN_ACKED_BG1);
	}
    // Register the static filters with the access factory.
    TA_Base_Core::NamedFilterAccessFactory::getInstance().registerStaticNamedFilter( defaultFilter );
    //TA_Base_Core::NamedFilterAccessFactory::getInstance().registerStaticNamedFilter( avalancheFilter );
	
	// Load the authentication agent library
    try
    {
        m_authenticationLib = new TA_Base_Bus::AuthenticationLibrary(true);
    }
    catch(const TA_Base_Core::DataException& )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException","Cannot create the authentication library. This is because the authentication agent configuration has not yet been set up so we go straight to the database login instead");
    }
    catch (const TA_Base_Core::DatabaseException&)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException","Cannot create the authentication library. This is because the authentication agent configuration has not yet been set up so we go straight to the database login instead");
    }
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "void CombinedEventFilter::InitialFilterSetupInfo [exit initial filter info] ");
}
//*/ //zhiqiang++end

void CombinedEventFilter::terminateThread()
{
	//m_loadingThread->terminateAndWait();
}

void CombinedEventFilter::setNamedFilter( std::auto_ptr<TA_Base_Core::NamedFilter> namedFilter )
{
	Filter::setNamedFilter(namedFilter);
}


void CombinedEventFilter::populateSOEFilter( TA_Base_Core::SOEEventFilter* filter)
{
	using namespace TA_Base_Core;
	NamedFilter::StringList lstStr;
	
	// init
	filter->isEnabled = false;
	
	filter->tmFrom = getFromTime();
	filter->tmTo = getToTime();

	// check
	if( filter->tmFrom || filter->tmTo )
	{
		filter->isEnabled = true;
	}

	lstStr = m_namedFilter->getStringFilter(SOEEventFilter::FILTER_ASSET_NAME);
	if( lstStr.empty() )
	{
		filter->strAssetName = "";
	}
	else
	{
		filter->strAssetName = lstStr.front();
		filter->isEnabled = true;
	}
	lstStr = m_namedFilter->getStringFilter(SOEEventFilter::FILTER_DESCRIPTION);
	if( lstStr.empty() )
	{
		filter->strDescription = "";
	}
	else
	{
		filter->strDescription = lstStr.front();
		filter->isEnabled = true;
	}

	lstStr = m_namedFilter->getStringFilter(SOEEventFilter::FILTER_DISPLAY_VALUE);
	if( lstStr.empty() )
	{
		filter->strDisplayValue = "";
	}
	else
	{
		filter->strDisplayValue = lstStr.front();
		filter->isEnabled = true;
	}
	NamedFilter::NumericList locs = m_namedFilter->getNumericFilter(SOEEventFilter::FILTER_LOCATION_KEY);
	filter->locations.assign( locs.begin(), locs.end() );
	
	// check
	if( !filter->locations.empty() )
	{
		filter->isEnabled = true;
	}
	
	// page info
	unsigned int nPage = getCurrentPage();
	filter->strCurrPageTime = m_pageTimeMap[nPage];

	//Li Zhongyan add; for page change info
	filter->currPageEventID = m_pagebottomEventIDMap[nPage];
	
	// sort order
	filter->bSortAscend = DisplayItem::s_sortAscending;

	// sort column
	if( DisplayItem::s_sortColumn == CombinedEventFilter::COLUMN_SOURCE_DATETIME )
	{
		filter->strSortColumn = SOEEvent::COL_NAME_SOURCE_TIME;
	}
	else if( DisplayItem::s_sortColumn == CombinedEventFilter::COLUMN_TRANSACTIVE_TIME )
	{
		filter->strSortColumn = SOEEvent::COL_NAME_TRANSACTIVE_TIME;
	}
	else if( DisplayItem::s_sortColumn == CombinedEventFilter::COLUMN_ASSET )
	{
		filter->strSortColumn = SOEEvent::COL_NAME_ASSET_NAME;
	}
	else if( DisplayItem::s_sortColumn == CombinedEventFilter::COLUMN_DISPLAY_VALUE )
	{
		filter->strSortColumn = SOEEvent::COL_NAME_DISPLAY_VALUE;
	}
	else if( DisplayItem::s_sortColumn == CombinedEventFilter::COLUMN_LOCATION_NAME )
	{
		filter->strSortColumn = SOEEvent::COL_NAME_LOCATION_NAME;
	}
	else if( DisplayItem::s_sortColumn == CombinedEventFilter::COLUMN_DESCRIPTION )
	{
		filter->strSortColumn = SOEEvent::COL_NAME_DESCRIPTION;
	}
	else
	{
		filter->strSortColumn = "";
	}

	filter->currentPage = m_currentPage;
	
}



bool CombinedEventFilter::isLoading()
{
	//return !(m_loadingThread->getStopFlag());
	return false;
}

void CombinedEventFilter::insertBinaryDisplayItem(DisplayItem* displayitem, bool bSortAscend){
	std::vector<DisplayItem*>::iterator leftitem = m_displayItems.begin();
	std::vector<DisplayItem*>::iterator rightitem = m_displayItems.end();

	std::vector<DisplayItem*>::iterator middleitem;
	int middlevalue;
	
	if(leftitem == m_displayItems.end())
	{	
		m_displayItems.push_back(displayitem);
		return;
	}
	else{
		rightitem = rightitem - 1;
	}

	if(*displayitem < *(*leftitem))
	{
		m_displayItems.insert(leftitem, displayitem);
		return;
	}
	else if( *(*rightitem) < *displayitem)
	{
		m_displayItems.push_back(displayitem);
		return;
	}

	while(leftitem <= rightitem)
	{
		middlevalue = (rightitem - leftitem) / 2 ;
		middleitem = leftitem + middlevalue;
		if(middleitem == leftitem || middleitem == rightitem )
			break;

		if(*displayitem < *(*middleitem))
		{
			rightitem = middleitem;
		}
		else
		{
			leftitem = middleitem;
		}
	}
	
	m_displayItems.insert(middleitem+1, displayitem);
		
}


void CombinedEventFilter::sort(const std::string& columnName,const bool sortAscending)
{
	Filter::sort( columnName, sortAscending );
}

void CombinedEventFilter::initPageMap()
{
	// reset page time
	//std::string str = (*(m_displayItems.rbegin()))->getColumnData( CombinedEventFilter::COLUMN_DATETIME );
	m_currentPage = 0;
	m_pageTimeMap.clear();
	m_pageTimeMap.insert( PageTimeMap::value_type(m_currentPage, EMPTY_STRING) );
	//m_pageTimeMap.insert( PageTimeMap::value_type(m_currentPage+1,str) );
	
	//li zhongyan add
	m_pagebottomEventIDMap.clear();
	m_pagebottomEventIDMap.insert(PagebottomEventIDnMap::value_type(m_currentPage, 0));
}


void CombinedEventFilter::adjustNextPageTime()
{
	m_pageTimeMap[m_currentPage+1] = (*(m_displayItems.rbegin()))->getColumnData( CombinedEventFilter::COLUMN_SOURCE_DATETIME );
	//m_pagebottomEventIDMap[m_currentPage + 1] = static_cast<CombinedEventItem*>(*m_displayItems.rbegin())->getrPKey();
}