/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByDescription.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByDescription.cpp
//  Implementation of the Class AlarmFilterByDescription
//  Created on:      26-Aug-2004
///////////////////////////////////////////////////////////

#include "AlarmFilterByDescription.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterByDescription::AlarmFilterByDescription(EComparitor compareOp,const std::string& compareDescription) 
                  : IAlarmFilter(compareOp),
					m_compareDescription(compareDescription.c_str())
{
	FUNCTION_ENTRY("Constructor");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for filter.");
	FUNCTION_EXIT;
}



AlarmFilterByDescription::~AlarmFilterByDescription()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByDescription::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterByDescription* entityNameFilter = dynamic_cast<const AlarmFilterByDescription*>(&filter);
	if (entityNameFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==entityNameFilter->m_compareOp) &&
		    (m_compareDescription==entityNameFilter->m_compareDescription))
		{
			FUNCTION_EXIT;
			return true;
		}
	}
	FUNCTION_EXIT;
	return false;
}

/**
 * Check if the specified alarm meets the filter requirements
 * @param alarm
 * 
 */
bool AlarmFilterByDescription::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return strcmp(alarm.alarmDescription,m_compareDescription.c_str())==0;
		case COMPARE_NOT_EQUAL:			return strcmp(alarm.alarmDescription,m_compareDescription.c_str())!=0;
		case COMPARE_LESS_THAN:			return strcmp(alarm.alarmDescription,m_compareDescription.c_str())<0;
		case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm.alarmDescription,m_compareDescription.c_str())<=0;
		case COMPARE_GREATER_THAN:		return strcmp(alarm.alarmDescription,m_compareDescription.c_str())>0;
		case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm.alarmDescription,m_compareDescription.c_str())>=0;
		case COMPARE_CONTAINS:			//return strstr(alarm.alarmDescription,m_compareDescription.c_str())!=NULL;
			//TD15029 azenitha++
			//Must be case insensitive
			{
				//Place in temporary holders, or else the m_compareDescription will be in lower case
				std::string alarmDescriptionStrTmp = alarm.alarmDescription.in();
				std::string compareDescriptionStrTmp = m_compareDescription;

				//Convert first to lower before searching substring
				const char* alarmDescriptionStr = strlwr((char*)alarmDescriptionStrTmp.c_str());
				const char* compareDescriptionStr = strlwr((char*)compareDescriptionStrTmp.c_str());

				return strstr(alarmDescriptionStr,compareDescriptionStr)!=NULL;
			}
			//TD15029 ++azenitha
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
	}

	FUNCTION_EXIT;
    return false;
}


/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterByDescription::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->alarmDescription,m_compareDescription.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->alarmDescription,m_compareDescription.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->alarmDescription,m_compareDescription.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->alarmDescription,m_compareDescription.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->alarmDescription,m_compareDescription.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->alarmDescription,m_compareDescription.c_str())>=0;
	case COMPARE_CONTAINS:			//return strstr(alarm.alarmDescription,m_compareDescription.c_str())!=NULL;
		//TD15029 azenitha++
		//Must be case insensitive
		{
			//Place in temporary holders, or else the m_compareDescription will be in lower case
			std::string alarmDescriptionStrTmp = alarm->alarmDescription;
			std::string compareDescriptionStrTmp = m_compareDescription;
			
			//Convert first to lower before searching substring
			const char* alarmDescriptionStr = strlwr((char*)alarmDescriptionStrTmp.c_str());
			const char* compareDescriptionStr = strlwr((char*)compareDescriptionStrTmp.c_str());
			
			return strstr(alarmDescriptionStr,compareDescriptionStr)!=NULL;
		}
		//TD15029 ++azenitha
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	
	FUNCTION_EXIT;
    return false;
}
/*-----------------------------------------------------------------------------------------------------------*/

//++Noel
bool AlarmFilterByDescription::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->alarmDescription.c_str(),m_compareDescription.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->alarmDescription.c_str(),m_compareDescription.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->alarmDescription.c_str(),m_compareDescription.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->alarmDescription.c_str(),m_compareDescription.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->alarmDescription.c_str(),m_compareDescription.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->alarmDescription.c_str(),m_compareDescription.c_str())>=0;
	case COMPARE_CONTAINS:			//return strstr(alarm.alarmDescription,m_compareDescription.c_str())!=NULL;
		//TD15029 azenitha++
		//Must be case insensitive
		{
			//Place in temporary holders, or else the m_compareDescription will be in lower case
			std::string alarmDescriptionStrTmp = alarm->alarmDescription;
			std::string compareDescriptionStrTmp = m_compareDescription;
			
			//Convert first to lower before searching substring
			const char* alarmDescriptionStr = strlwr((char*)alarmDescriptionStrTmp.c_str());
			const char* compareDescriptionStr = strlwr((char*)compareDescriptionStrTmp.c_str());
			
			return strstr(alarmDescriptionStr,compareDescriptionStr)!=NULL;
		}
		//TD15029 ++azenitha
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	
	FUNCTION_EXIT;
    return false;
}