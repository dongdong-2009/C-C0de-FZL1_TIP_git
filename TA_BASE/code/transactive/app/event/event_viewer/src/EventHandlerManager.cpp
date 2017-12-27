/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/EventHandlerManager.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * EventHandlerManager is ...
  *
  */

#include <algorithm>

#include "EventHandlerManager.h"
#include "FilterCriteria.h"
#include "ATSRealTimeObserverManager.h"
#include "ConstantItems.h"
#include "CombinedEventItem.h"
#include "CombinedEventFilter.h"

#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"


using namespace TA_Base_App;
using namespace TA_Base_Core;
using namespace EventViewerConstants;

//Luxm++ : for 955 event viewer:
#define RPARAM_ATSEVENTCOUNT "ATSEventCount"
#define RPARAM_ISCSEVENTCOUNT "ISCSEventCount"
#define RPARAM_ENABLEISCSEVENT  "EnableISCSEvent"
#define RPARAM_ENABLEATSEVENT  "EnableATSEvent"
#define RPARAM_SORTINGPRIORITY "SortingPriority"
//++Luxm

namespace TA_Base_App{

	unsigned long EventHandlerManager::s_maxEvents = 0;
	unsigned long EventHandlerManager::s_maxATSEvents = 0;
	unsigned long EventHandlerManager::s_maxISCSEvents = 0;
	unsigned long EventHandlerManager::s_refreshRateDB = 0; 
	unsigned long EventHandlerManager::s_refreshRateAgent =0;
	bool EventHandlerManager::s_isISCSSortingPriority = true;
	bool EventHandlerManager::s_rightsCheck = true;
	int EventHandlerManager::s_eventDetailDebugLevelInterval = 0; //0 means not log detail in DEBUG, 1 means write log for each event, 2 means write log for every other 2 events...  

	EventHandlerManager::EventHandlerManager(Filter * filter){
		m_curMode = TA_Base_App::RealTimeMode;
		m_taskSeqID = 0;
		m_historicalHandler = NULL;
		m_realtimeHandler = NULL;
		m_ATSHealthStatus = false;
		m_ISCSHealthStatus = false;
		initialize2Switch();
		initEventDetailLogLevel();
		initRightsCheck();

		m_msg_ISCS = "";
		m_msg_ATS =  "";
		m_msg="";	

		m_filter = filter;		
		
	}


	EventDisplayMode EventHandlerManager::getDisplayMode()
	{
		return m_curMode;
	}

	bool EventHandlerManager::isISCSSortingPriority()
	{
		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_SORTINGPRIORITY))
		{            
			std::string sortingPriority=TA_Base_Core::RunParams::getInstance().get(RPARAM_SORTINGPRIORITY);
			if (sortingPriority != "" )
			{
				//LocationKey=atob(enableISCSstr.c_str());
				transform (sortingPriority.begin(),sortingPriority.end(), sortingPriority.begin(), tolower); 
				if(sortingPriority.find("iscs") != std::string::npos)
				{
					return true;
				}
				if(sortingPriority.find("ats") != std::string::npos)
				{
					return false;
				}
			}
		}

		//otherwise:
		return true;
	}

	void EventHandlerManager::setIsISCSSortingPriority()
	{
		s_isISCSSortingPriority = isISCSSortingPriority();
	}

	bool EventHandlerManager::getIsISCSSortingPriority()
	{
		return s_isISCSSortingPriority;
	}

	void EventHandlerManager::initEventDetailLogLevel()
	{
		if(TA_Base_Core::RunParams::getInstance().isSet(EventViewerConstants::LOG_EVENT_DETAIL_INTERVAL.c_str()))
		{
			std::string eventLogIntervalStr=TA_Base_Core::RunParams::getInstance().get(LOG_EVENT_DETAIL_INTERVAL.c_str());
			if( 0<eventLogIntervalStr.size()){
				//convert value to unsing long
				std::stringstream eventLogIntervalStream( eventLogIntervalStr );
				unsigned long eventLogIntervalUL;
				if ( ! ( eventLogIntervalStream >> eventLogIntervalUL ).fail() )
				{
					s_eventDetailDebugLevelInterval = eventLogIntervalUL;
				}
			}
		}
		
	}

	int EventHandlerManager::getEventDetailInDebugLevel()
	{
		return s_eventDetailDebugLevelInterval;
	}
	// read 2 parameters from commandline ( EnableISCSEvent, EnableATSEvent)
	void EventHandlerManager::initialize2Switch()
	{
		m_ISCSEnabled = true;
		m_ATSEnabled = false;

		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_ENABLEISCSEVENT))
		{            
			std::string enableISCSstr=TA_Base_Core::RunParams::getInstance().get(RPARAM_ENABLEISCSEVENT);
			if (enableISCSstr != "")
			{
				//LocationKey=atob(enableISCSstr.c_str());
				transform (enableISCSstr.begin(),enableISCSstr.end(), enableISCSstr.begin(), tolower); 
				if(enableISCSstr.find("false") != std::string::npos)
				{
					m_ISCSEnabled = false;
				}
			}
		}
		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_ENABLEATSEVENT))
		{            
			std::string enableATSstr=TA_Base_Core::RunParams::getInstance().get(RPARAM_ENABLEATSEVENT);
			if (enableATSstr != "" )
			{
				//LocationKey=atob(enableISCSstr.c_str());
				transform (enableATSstr.begin(),enableATSstr.end(), enableATSstr.begin(), tolower); 
				if(enableATSstr.find("false") != std::string::npos)
				{
					m_ATSEnabled = false;
				}
			}
		}
	}

	void EventHandlerManager::setISCS_ATS_MaxByDefault()
	{
		s_maxATSEvents = EventViewerConstants::ATS_EVENT_COUNT_DEFAULT;
		s_maxISCSEvents = EventViewerConstants::ISCS_EVENT_COUNT_DEFAULT;
	}

	//init m_maxATSEvents, m_maxISCSEvents
	void EventHandlerManager::initialize_ATS_ISCS_MaxCount()
	{
		setISCS_ATS_MaxByDefault();

		//set m_ATSEventCount from commandline parameter
		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_ATSEVENTCOUNT)){
			std::string ATSEventCountStr=TA_Base_Core::RunParams::getInstance().get(RPARAM_ATSEVENTCOUNT);
			if( 0<ATSEventCountStr.size()){
				//convert value to unsing long
				std::stringstream ATSEventCountStream( ATSEventCountStr );
				unsigned long ATSEventCountUL;
				if ( ! ( ATSEventCountStream >> ATSEventCountUL ).fail() )
				{
					s_maxATSEvents = ATSEventCountUL;
				}
			}
		}

		//set m_ISCSEventCount from commandline parameter
		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_ISCSEVENTCOUNT)){
			std::string ISCSEventCountStr=TA_Base_Core::RunParams::getInstance().get(RPARAM_ISCSEVENTCOUNT);
			if( 0<ISCSEventCountStr.size()){
				//convert value to unsing long
				std::stringstream ISCSEventCountStream( ISCSEventCountStr );
				unsigned long ISCSEventCountUL;
				if ( ! ( ISCSEventCountStream >> ISCSEventCountUL ).fail() )
				{
					s_maxISCSEvents = ISCSEventCountUL;
				}
			}
		}

		if(s_maxATSEvents > s_maxEvents)
		{
			s_maxATSEvents = s_maxEvents;
		}
		if(s_maxISCSEvents > s_maxEvents)
		{
			s_maxISCSEvents = s_maxEvents;
		}


	}

	void EventHandlerManager::initialHandler()
	{
		m_realtimeHandler = new EventHandler(TA_Base_App::RealTimeMode, this);
		ATSRealTimeObserverManager::getInstance()->removeObserver();
		ATSRealTimeObserverManager::getInstance()->addObserver(m_realtimeHandler);

		m_historicalHandler = new EventHandler(TA_Base_App::HistoricalMode, this);	
	}

	EventHandlerManager::~EventHandlerManager(){

		if (m_historicalHandler != NULL){
			delete m_historicalHandler;
			m_historicalHandler = NULL;
		}

		ATSRealTimeObserverManager::freeInstance();

		if(m_realtimeHandler != NULL){
			delete m_realtimeHandler;
			m_realtimeHandler = NULL;
		}

		
	}


	// same filter and mode as last task, and in last task, the load step already finished.
	// ( "load step finished" means: for historical mode, total reseach result has been returned.
	//  and for real time, the backlog list has been returned.)
// 	bool EventHandlerManager::isSameTaskAsLastAndLoadReady(FilterCriteria searchfilter, EventDisplayMode mode)
// 	{
// 		if(mode == RealTimeMode){
// 			if ( m_realtimeHandler->m_curFilterCriteria.equalTo(searchfilter) 
// 				&& m_realtimeHandler->IsAllTheWorkerFinishedLoading())
// 			{
// 				return true;
// 			}
// 		}
// 		else
// 		{
// 			if( m_historicalHandler->m_curFilterCriteria.equalTo(searchfilter)
// 				&& m_historicalHandler->IsAllTheWorkerFinishedLoading())
// 			{
// 				return true;
// 			}
// 		}
// 		return false;
// 
// 	}

	void EventHandlerManager::statusBarUpdate (const unsigned long eventCount,TA_Base_Core::EventCategory eventCategory , const string status )
	{
		if( TA_Base_Core::ISCSCategory == eventCategory)
		{
			m_msg_ISCS = status;
		}
		else
		{
			m_msg_ATS = status;
		}
		m_msg= m_msg_ISCS;
		//m_msg= m_msg_ISCS + STATUS_SEPERATE + m_msg_ATS;
        sendStatusBarUpdate( 0, m_msg.c_str() );

	}

	void EventHandlerManager::statusBarUpdateWhenStartQuery(){

		if(!m_ISCSEnabled)
		{
			m_msg_ISCS = STATUS_ICSC_DISABLED;
		}else
		{
			m_msg_ISCS = STATUS_ISCS_QUERYING;
		}

		if(!m_ATSEnabled)
		{
			m_msg_ATS = STATUS_ATS_DISABLED;
		}
		else
		{
			m_msg_ATS = STATUS_ATS_QUERYING;
		}
		//m_msg= m_msg_ISCS + STATUS_SEPERATE + m_msg_ATS;
		m_msg= m_msg_ISCS;
		sendStatusBarUpdate( 0, m_msg.c_str() );


	}

	bool EventHandlerManager::isStartNewTask(FilterCriteria searchfilter,EventDisplayMode mode)
	{
		if(m_taskSeqID != 0 && mode == m_curMode )
		{
			if( mode==TA_Base_App::HistoricalMode)// for realtime , always do reload
			{
				if( m_historicalHandler!=NULL && m_historicalHandler->m_curFilterCriteria.equalTo(searchfilter) )
				{
					return false;
				}
			}

		}
		return true;
	}

	void EventHandlerManager::startNewTask(FilterCriteria searchfilter, EventDisplayMode mode){

		//save current FilterCriteria.
		//setFilterCriteria(searchfilter);

		//if the task is just the same as last time. Then do nothing.

		/*if(m_taskSeqID != 0 && mode == m_curMode )
		{
			if( mode==EventDisplayMode::HistoricalMode)// for realtime , always do reload
			{
				if( m_historicalHandler!=NULL && m_historicalHandler->m_curFilterCriteria.equalTo(searchfilter) )
					return;
			}

		}*/

		// can't use old result, must start a new task now:

		{
			/*m_selectedItems.clear();

			statusBarUpdateWhenStartQuery();

			if( !m_ATSEnabled && !m_ISCSEnabled)
			{
				return;
			}

			//clear page:
			m_currentPage = 0;
			m_pageTimeMap.clear();
			m_pageTimeMap.insert(PageTimeMap::value_type(m_currentPage, EventViewerConstants::EMPTY_STRING));
  			LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:current Page %d. ", m_currentPage);*/
		   
			/*GUI event list not cleared here as it should be only cleared after recovering successfully from database call*/
			//sendListUpdate( 0 );
			//sendListViewUpdate(0,0);

			m_taskSeqID++;
			m_curMode = mode;

			if (mode == TA_Base_App::HistoricalMode){
				m_realtimeHandler -> stopCurrentTask();
				m_historicalHandler->startParameter(searchfilter,m_taskSeqID,m_ISCSEnabled,m_ATSEnabled);

			}else if( mode == TA_Base_App::RealTimeMode){
				m_historicalHandler->stopCurrentTask();
				m_realtimeHandler->startParameter(searchfilter,m_taskSeqID,m_ISCSEnabled,m_ATSEnabled);
			}
		}

	}


	void EventHandlerManager::refreshListView()
	{
		if( RealTimeMode == getDisplayMode() && NULL != m_realtimeHandler)
		{
			{  
				LPARAM pageStatus = 0;
				int currentPage =  m_filter->getCurrentPage();
				if ( 0 < currentPage )
				{
					// If we aren't on the first page, we can move up.
					pageStatus |= Filter::MASK_PAGEUP;
				}
				unsigned long dispSize = m_realtimeHandler->getDislplaySize();
				if ( dispSize > s_maxEvents )
				{
					// If we are now displaying more than a full page assume
					// we can move down..
					pageStatus |= Filter::MASK_PAGEDOWN;
					m_filter->insertPageTimeMap(currentPage+1, m_realtimeHandler->getCreateTimeStrAt(s_maxEvents));
					LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:current Page %d. ", currentPage);
				}
				m_filter->setPageStatus(pageStatus);
				sendListViewUpdate( (dispSize > s_maxEvents)?s_maxEvents:dispSize, m_filter->getPageStatus());
				LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log: updating list view with total number %d", ( dispSize > s_maxEvents)?s_maxEvents:dispSize);
			}
		}
		else if (HistoricalMode == getDisplayMode() && NULL != m_historicalHandler)
		{
			{
				LPARAM pageStatus = 0;
				int m_currentPage =  m_filter->getCurrentPage();
				if ( 0 < m_currentPage )
				{
					// If we aren't on the first page, we can move up.
					pageStatus |= Filter::MASK_PAGEUP;
				}
				unsigned long dispSize = m_historicalHandler->getDislplaySize();
				if ( dispSize > s_maxEvents )
				{
					// If we are now displaying more than a full page assume
					// we can move down..
					pageStatus |= Filter::MASK_PAGEDOWN;
					m_filter->insertPageTimeMap(m_currentPage+1, m_historicalHandler->getCreateTimeStrAt(s_maxEvents));
					LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:current Page %d. ", m_currentPage);
				}
				m_filter->setPageStatus(pageStatus);
				sendListViewUpdate( (dispSize > s_maxEvents)?s_maxEvents:dispSize, m_filter->getPageStatus());
				/*m_eventViewerDlg->OnListViewUpdate((dispSize > maxEvents)?maxEvents:dispSize, m_pageStatus);*/
				LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log: updating list view with total number %d", ( dispSize > s_maxEvents)?s_maxEvents:dispSize);
			}
		}
		
	}


	void EventHandlerManager::setMaxEvents(const unsigned long max)
	{
		s_maxEvents = max;
	}

	unsigned long EventHandlerManager::getMaxEvents()
	{
		return s_maxEvents;
	}

	void EventHandlerManager::setMaxISCSEvents( unsigned long maxISCS)
	{
		s_maxISCSEvents = maxISCS;
	}

	void EventHandlerManager::setMaxATSEvents( unsigned long maxATS)
	{
		s_maxATSEvents = maxATS;
	}

	unsigned long EventHandlerManager::getMaxISCSEvents()
	{
		return s_maxISCSEvents;
	}

	unsigned long EventHandlerManager::getMaxATSEvents()
	{
		return s_maxATSEvents;
	}


	unsigned long EventHandlerManager::getRefreshRateDB()
  	{
		return s_refreshRateDB;
	}

	void EventHandlerManager::setRefreshRateDB(const unsigned long refresh)
	{
		s_refreshRateDB = refresh;
	}

	bool EventHandlerManager::getRightsCheck()
	{
		return s_rightsCheck;
	}
	void EventHandlerManager::initRightsCheck()
	{
		s_rightsCheck= TA_Base_Core::RunParams::getInstance().isSet(EventViewerConstants::RPARAM_NO_VISIBILITY_RULE.c_str()) ? false : true;
	}

	unsigned long EventHandlerManager::getRefreshRateAgent()
	{
		return s_refreshRateAgent;
	}

	void EventHandlerManager::setRefreshRateAgent(unsigned long refresh)
	{
		s_refreshRateAgent = refresh;
	}

	void EventHandlerManager::reloadEvents()
	{
		m_filter->reloadEvents();
	}

};

