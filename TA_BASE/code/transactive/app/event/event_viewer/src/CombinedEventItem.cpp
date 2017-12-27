/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/src/CombinedEventItem.cpp $
 * @author:  Bradley Cleaver
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2014/07/18 15:34:16 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * CombinedEventItem is an implementation of the DisplayItem abstract class. It contains the data for one row in the list, and 
 * defines the standard ordering operator (<) for list sorting.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/EventViewer.h"
#include "app/event/event_viewer/src/CombinedEventItem.h"
#include "app/event/event_viewer/src/CombinedEventFilter.h"
#include "app/event/event_viewer/src/DatabaseAccess.h"
#include "core/data_access_interface/src/AlarmData.h"
//#include "core/data_access_interface/src/ICombinedEventData.h"
//#include "core/data_access_interface/src/IAlarmData.h"
#include "core/utilities/src/RunParams.h"

// NOTE: This inclusion of the CombinedEventFilter is simply to gain access to the statically declared variables. While
// this does lead to a cross-dependency (as AuditEventFilter is dependant on this class), that is preferable to 
// hard coding string variables throughout the application
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#include <iomanip>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

enum TA_Base_Core::EventCategory;
namespace
{
    const std::string DEFAULT_VALUE("");
	// Alarm Comments identifiers
    const std::string COMMENT_IDENTIFIER("C");
	// Alarm DSS Plan identifiers
	const std::string AUTO_PLAN_NOT_RUN_ID("A");
	const std::string SEMI_AUTO_PLAN_NOT_RUN_ID("S");
	const std::string PLAN_RUN_ID("R");
	const std::string PLAN_POSTPONED_ID("X");
	const std::string NO_PLAN("");
	// Alarm Avalanche Status identifiers
	const std::string AVL_HEAD_ID("H");
	const std::string AVL_CHILD_ID("C");
	// Alarm Mms Status identifiers
	const std::string MMS_AUTO_ID("A");
	const std::string MMS_SEMI_ID("S");
	const std::string MMS_JR_SENT_ID("R");
	const std::string MMS_FAILED_ID("D"); 
	const std::string MMS_INHIBIT_ID("I");

}

bool CombinedEventItem::is_set_millisec = TA_Base_Core::RunParams::getInstance().isSet( EventConstants::RPARAM_MILLISECOND.c_str());
std::string CombinedEventItem::dateFormatStr = TA_Base_Core::DateFormat().getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CombinedEventItem::CombinedEventItem():DisplayItem()
{

}

//Finished //TODO: add c_str() to the temporary return string.
CombinedEventItem::CombinedEventItem( TA_Base_Core::ICombinedEventData* const eventData )
{
    TA_ASSERT( NULL != eventData, "CombinedEventItem constructor provided with NULL data." );

	m_eventData = eventData;
	if( eventData->getEventCategoryType() == TA_Base_Core::ATSCategory)
	{
		m_eventCategory = TA_Base_Core::ATSCategory;
	}else 
	{
		m_eventCategory = TA_Base_Core::ISCSCategory;
	}

	m_key = eventData->getKey();

	m_eventId = eventData->getEventID();
	
    m_createTime = eventData->getCreateTime();


	//this redundant column is just for sorting:
}


CombinedEventItem::~CombinedEventItem()
{
	if(m_eventData!=NULL)
	{
		delete m_eventData;
		m_eventData = NULL;
	}
}
void CombinedEventItem::setEventCategory(  TA_Base_Core::EventCategory ec )
{
	m_eventCategory= ec;
}

TA_Base_Core::EventCategory CombinedEventItem::getEventCategory()
{
	return m_eventCategory;
}

unsigned long CombinedEventItem::getKey()
{
	return m_key;
}





const std::string CombinedEventItem::getColumnData(const std::string& columnName)
{
        if ( CombinedEventFilter::COLUMN_COMMENT == columnName   )
        {
            return m_eventData->getAlarmComment().c_str();
        }
		else if ( CombinedEventFilter::COLUMN_MMS == columnName  )
		{
			// this is the MMS status column
			 return getMmsStatusIdentifier(m_eventData->getAlarmMmsState().c_str());
		}
		else if (  CombinedEventFilter::COLUMN_DSS == columnName  )
		{
			// this is the DSS Plan column
			return getDssPlanStatusIdentifier(m_eventData->getAlarmDssState().c_str());
		}
		else if ( CombinedEventFilter::COLUMN_AVL == columnName  )
		{
			// this is the AVL status column
			return getAvlStatusIdentifier(m_eventData->getAlarmAvlState().c_str());
		}
		else if (  CombinedEventFilter::COLUMN_SEVERITY == columnName  )
		{
			// this is the severity column
			return getSevIdentifier(m_eventData->getAlarmSeverityName().c_str());
		}
		else if( CombinedEventFilter::COLUMN_DATETIME == columnName )
		{
			std::string timePart(CTime(m_eventData->getCreateTime().time).Format(dateFormatStr.c_str()));
			std::stringstream fullString;
			fullString << timePart;

			// wenguang++
			// Change the millisecond setting to the RUNPARAM
			/*if (TA_Base_Core::RunParams::getInstance().isSet( EventConstants::RPARAM_MILLISECOND.c_str()))*/
			if(is_set_millisec)
			{
				fullString << "." << std::setfill('0') << std::setw(3) << m_eventData->getCreateTime().millitm;
			}
			return fullString.str();
		}
		else if( CombinedEventFilter::COLUMN_ASSET == columnName )
		{
			return m_eventData->getAssetName().c_str();
		}
		else if( CombinedEventFilter::COLUMN_DESCRIPTION == columnName || CombinedEventFilter::COLUMN_VALUE == columnName)
		{
			//init m_desc and m_val
			std::string strTem = m_eventData->getDescription().c_str();
			std::string strDescription("JobRequestSubmitted from ");
			CString cstrTem("JobRequestSubmitted");
			if (cstrTem == strTem.substr(0,19).c_str())
			{
				strDescription += m_eventData->getAssetName().c_str();
				if( CombinedEventFilter::COLUMN_DESCRIPTION == columnName)
				{
					return strDescription;
				}else
				{
					return  "";
				}
			}
			else
			{
				if( CombinedEventFilter::COLUMN_DESCRIPTION == columnName)
				{
					return strTem;
				}
				else
				{
					// Insert the value.
					std::string valueStr(m_eventData->getValue().c_str());
					//TD12415
					// Change the format 20.0(0C) to correct format of degree Celsius
					// using ALT+0176
					CString degreeString = valueStr.c_str();

					if (degreeString.Find("?C", 0) >= 0)
					{
						degreeString.Replace(_T('?'), _T('?'));
					}
					valueStr.assign(degreeString);
					//TD12415
					return valueStr;
				}

			}
		}

		else if( CombinedEventFilter::COLUMN_ALARM_STATUS == columnName)
		{
			std::string eventType=m_eventData->getEventTypeName().c_str();
			if(DatabaseAccess::getInstance().findAlarmStateName(eventType))
			{
				return eventType;
			}else
			{
				return "";
			}
		}
		else if( CombinedEventFilter::COLUMN_T == columnName)
		{
			return m_eventData->getOperationMode().c_str();
		}
		else if( CombinedEventFilter::COLUMN_OPERATOR == columnName)
		{
			return m_eventData->getOperatorName().c_str() ;
		}
		else if(CombinedEventFilter::HIDDEN_COLUMN_ALARMID == columnName)
		{
			return m_eventData->getAlarmId().c_str();
		}
		else if(CombinedEventFilter::HIDDEN_COLUMN_EVENTID == columnName)
		{
			return m_eventData->getEventID().c_str() ;
		}
		else if(CombinedEventFilter::HIDDEN_COLUMN_ACKED == columnName)
		{
			return m_eventData->getAlarmAck().c_str() ;
		}
		else if(CombinedEventFilter::HIDDEN_COLUMN_STATUS == columnName)
		{
			return m_eventData->getAlarmStatus().c_str() ;
		}

		return DEFAULT_VALUE;
}

// These functions are to convert the various mms, dss and avl status from string stored in database
// to defined identifier (ie. letter) for display
const std::string CombinedEventItem::getMmsStatusIdentifier(const std::string mmsStatus)
{
	//check the given string with the static const used in the alarm data
	//as mms status is a number under alarm data, there is need to convert the string to number
	int status = atoi(mmsStatus.c_str());
	
	switch (status)
	{
		case TA_Base_Core::IAlarmData::TYPE_SEMI:
			return MMS_SEMI_ID.c_str();
		case TA_Base_Core::IAlarmData::TYPE_AUTO:
			return MMS_AUTO_ID.c_str();
		case TA_Base_Core::IAlarmData::TYPE_ALM_SENT:
			return MMS_AUTO_ID.c_str();
		case TA_Base_Core::IAlarmData::TYPE_JR_SENT:
			return MMS_JR_SENT_ID.c_str();
		case TA_Base_Core::IAlarmData::TYPE_INHIBIT:
			return MMS_INHIBIT_ID.c_str();
		case TA_Base_Core::IAlarmData::TYPE_FAILED:
			return MMS_FAILED_ID.c_str();
		default:
			return DEFAULT_VALUE.c_str();
	}
}

const std::string CombinedEventItem::getDssPlanStatusIdentifier(const std::string dssStatus)
{
	if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::AUTO_PLAN_NOT_RUN) )
	{
		return AUTO_PLAN_NOT_RUN_ID.c_str();
	}
	else if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::SEMIAUTO_PLAN_NOT_RUN) )
	{
		return SEMI_AUTO_PLAN_NOT_RUN_ID.c_str();
	}
	else if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::PLAN_RUN) )
	{
		return PLAN_RUN_ID.c_str();
	}
	else if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::PLAN_POSTPONED) )
	{
		return PLAN_POSTPONED_ID.c_str();
	}
	else
	{
		return NO_PLAN.c_str();
	}
}

const std::string CombinedEventItem::getAvlStatusIdentifier(const std::string avlStatus)
{
	//check the given string with the static const used in the alarm data
	//as avl status is a number under alarm data, there is need to convert the string to number
	int status = atoi(avlStatus.c_str());
	
	switch (status)
	{
		case TA_Base_Core::IAlarmData::HEAD_AVL:
			return AVL_HEAD_ID.c_str();
		case TA_Base_Core::IAlarmData::CHILD_AVL:
			return AVL_CHILD_ID.c_str();
		default:
			return DEFAULT_VALUE.c_str();
	}
}

const std::string CombinedEventItem::getSevIdentifier(const std::string severityName)
{
	//check the given string with the static const used in the alarm data
	//as avl status is a number under alarm data, there is need to convert the string to number
	int sev = atoi(severityName.c_str());

	if (sev == 0)
	{
		return DEFAULT_VALUE.c_str();
	}else
	{
		return severityName.c_str();
	}
}


//Finished //TODO: delete "&" from return type
const std::string CombinedEventItem::getCompleteColumnData( const std::string& columnName )
{

	if ( CombinedEventFilter::COLUMN_MMS == columnName  )
	{
		// this is the MMS status column
		return m_eventData->getAlarmMmsState().c_str();
	}
	else if (  CombinedEventFilter::COLUMN_DSS == columnName  )
	{
		// this is the DSS Plan column
		return m_eventData->getAlarmDssState().c_str();
	}
	else if ( CombinedEventFilter::COLUMN_AVL == columnName  )
	{
		// this is the AVL status column
		return m_eventData->getAlarmAvlState().c_str();
	}
	else if (  CombinedEventFilter::COLUMN_SEVERITY == columnName  )
	{
		// this is the severity column
		return m_eventData->getAlarmSeverityName().c_str();
	}
	else return getColumnData(columnName);
}

// must be "strict weak ordering"!
// Strict: pred (X, X) is always false.
// Weak: If ! pred (X, Y) && !pred (Y, X), X==Y.
// Ordering: If pred (X, Y) && pred (Y, Z), then pred (X, Z).
bool CombinedEventItem::compareTo(DisplayItem* rhs, string& sortColumn, bool sortAscending)
{
	bool ISCSSortingPriority = EventHandlerManager::getIsISCSSortingPriority();
	// Note: shouldn't add any log in this function, otherwise will make CPU usage very high! Because this func will be called VERY frequently.
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Come into CombinedEventItem::compareTo()");

	// These are both string values, so use std::string.compare(), and store the result, where
	// result > 0 --> this item is lexicographically greater than the rhs item
	// result = 0 --> this item is lexicographically equal to the rhs item
	// result < 0 --> this item is lexicographically less than the rhs item
	int sortValue = 0;
	if ( CombinedEventFilter::COLUMN_DATETIME == sortColumn )
	{

		sortValue = m_createTime.time - (rhs->getCreateTime()).time;


		if (0 == sortValue)
		{
			// If the sort value was 0, it means the two times were the same, so want to sort on the milliseconds.
			sortValue = m_createTime.millitm - (rhs->getCreateTime()).millitm;
		}
		if (0== sortValue)
		{
			// if the time is totally equal, compair the key ==> when using RealTime Event Agent, we can't use pkey to sort because pkey may not exist.
			if ( m_eventCategory == rhs->getEventCategory() )
			{
				//sortValue = m_key - rhs->getKey();				
				sortValue = m_eventId.compare( rhs->getEventID());
			}
			else // the EventCategory is different
			{
				if(m_eventCategory == TA_Base_Core::ATSCategory)
				{
					if (ISCSSortingPriority)
					{
						sortValue = -1;
					}
					else
					{
						sortValue = 1;
					}
				}else
				{
					if (ISCSSortingPriority)
					{
						sortValue = 1;
					}
					else
					{
						sortValue = -1;
					}
				}

			}
		} 
	}
	else
	{
		std::map<std::string,std::string>::iterator iter;

		if(( CombinedEventFilter::COLUMN_DESCRIPTION == sortColumn)||( CombinedEventFilter::COLUMN_ASSET == sortColumn)
		 ||( CombinedEventFilter::COLUMN_OPERATOR == sortColumn)||( CombinedEventFilter::COLUMN_VALUE == sortColumn)) 
		{
			//ignore case:
			sortValue=_strcmpi(getColumnData(sortColumn).c_str(),rhs->getColumnData(sortColumn).c_str() ) ;
		}
		else
		{
			sortValue=getColumnData(sortColumn).compare(rhs->getColumnData(sortColumn));
		}
		

		if( 0 == sortValue ){
			//if the compare result is equal, go on comparing based on timestamp, and by the same sort direction. Added by xiangmei.
			sortValue = m_createTime.time - (rhs->getCreateTime()).time;
			if (0 == sortValue)
			{
				// If the sort value was 0, it means the two times were the same, so want to sort on the milliseconds.
				sortValue = m_createTime.millitm - (rhs->getCreateTime()).millitm;
			}
			if (0== sortValue)
			{
				// if the time is totally equal, compair the key
				if ( m_eventCategory == rhs->getEventCategory() )
				{
					//sortValue = m_key - rhs->getKey();
					sortValue = m_eventId.compare( rhs->getEventID());
				}
				else // the EventCategory is different
				{
					if(m_eventCategory == TA_Base_Core::ATSCategory)
					{
						if (ISCSSortingPriority)
						{
							sortValue = -1;
						}
						else
						{
							sortValue = 1;
						}
					}else
					{
						if (ISCSSortingPriority)
						{
							sortValue = 1;
						}
						else
						{
							sortValue = -1;
						}
					}
				}
			} 
		}
	}

	
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sortValue = %d, going to Come Out from compareTo.", sortValue);
	// Now compare the sort values to determine what we're doing.
	if (sortAscending)
	{
		// If the sort method is ascending, a result less than 0 is true (i.e. lower item first)
		return 0 > sortValue ? true : false;

	}
	else
	{
		// If the sort method is decending, a result greater than 0 is true (i.e. lower item last).
		return 0 < sortValue ? true : false;
	}
}



std::string CombinedEventItem::getCreateTimeStr()
{
	//return m_strCreateTime;
	return getColumnData(CombinedEventFilter::COLUMN_DATETIME);
}

timeb CombinedEventItem::getCreateTime()
{
	return m_createTime;
}

std::string CombinedEventItem::getEventID()
{
	return m_eventId;
}
