/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/EventListComponent/src/Filter.cpp $
* @author:  Nick Jardine
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2014/07/18 15:34:16 $
* Last modified by:  $Author: huirong.luo $
* 
* Filter is a class that is used as the base class for each type of data filter that the list needs to show. It provides
* standard filtering methods, and a number of variables to be used by specialised filter classes, as well as a uniform
* filter interface.
*/

#pragma warning(disable:4503)   // decorated name length exceeded
#pragma warning(disable:4786)   // identifier truncated to 255 characters

#include <algorithm>

#include "app/event/event_viewer/EventListComponent/src/stdafx.h"
#include "app/event/event_viewer/EventListComponent/src/Filter.h"
#include "app/event/event_viewer/EventListComponent/src/EventListComponent.h"
#include "app/event/event_viewer/src/EventViewer.h"
#include "app/event/event_viewer/src/EventHandlerManager.h"

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

const __int64 Filter::CURRENT_TIME = 0;
const __int64 Filter::PREVIOUS_DAY = 0;
const __int64 Filter::UNDEFINED_TIME = 0;

const std::string Filter::FILTER_TOTIME("结束时间");
const std::string Filter::FILTER_FROMTIME("开始时间");
const unsigned long Filter::MASK_PAGEUP = 0x00000001;
const unsigned long Filter::MASK_PAGEDOWN = 0x00000002;
const unsigned long Filter::MASK_REFRESH = 0x00000004;



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
:m_filterID(0),m_stopEventLoading(false),  
m_pauseEventLoading(false), m_namedFilter(NULL), m_eventLevel(100), m_rightEnabled(true), m_pauseRefresh(false),
m_currentPage(0), m_pageStatus(0),m_backgroundColour( RGB(255,255,255) ), m_foregroundColour( RGB(0,0,0) )
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

void Filter::setToTime( __int64 toTime )
{
	// Clear the current to time filter.
	m_namedFilter->removeNumericFilter( FILTER_TOTIME );

	// Now add the new time.
	m_namedFilter->addNumericFilter( FILTER_TOTIME, static_cast<unsigned long>(toTime) );
}

__int64 Filter::getToTime()
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

void Filter::setFromTime( __int64 fromTime )
{
	// Clear the current from time filter.
	m_namedFilter->removeNumericFilter( FILTER_FROMTIME );

	// Now add the new time.
	m_namedFilter->addNumericFilter( FILTER_FROMTIME, static_cast<unsigned long>(fromTime) );
}

__int64 Filter::getFromTime()
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



void Filter::setFilterID(const int filterID)
{
	m_filterID = filterID;
}


int Filter::getFilterID()
{
	return m_filterID;
}


void Filter::setEarliestTime(time_t earliestTime)
{
	m_earliestTime = earliestTime;
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
	m_rightEnabled = enabled;
}

bool Filter::isDutyEnabled()
{
	return m_rightEnabled;
}
COLORREF Filter::getForegroundColour( )
{
	return m_foregroundColour;
}

const COLORREF Filter::getItemColour(const unsigned long index, const std::string& columnName)
{
	COLORREF foreground = getRowColour(index);
	COLORREF background = getBackgroundColour();

	determineRowColours(index, foreground, background);

	return background; // For print the foreground is always black while the background varies in colour
}

COLORREF Filter::getBackgroundColour()
{
	return m_backgroundColour;
}

COLORREF Filter::getRowColourDefault( TA_Base_Core::IEventColourData::EEventColourType colourType  )
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


const std::vector<unsigned long>& Filter::getSelectedItems() const
{
	return m_selectedItems;
}


