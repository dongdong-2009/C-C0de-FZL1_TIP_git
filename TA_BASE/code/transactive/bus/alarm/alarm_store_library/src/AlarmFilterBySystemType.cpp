/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmFilterBySystemType.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/7/19 09:44:45 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterBySystemType.cpp
//  Implementation of the Class AlarmFilterBySystemType
//  Created on:      26-Aug-2004
///////////////////////////////////////////////////////////

#include "AlarmFilterBySystemType.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterBySystemType::AlarmFilterBySystemType(EComparitor compareOp,const std::string& compareSytemType) 
                  : IAlarmFilter(compareOp),
					m_compareSystemType(compareSytemType)
{
	FUNCTION_ENTRY("Constructor");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for filter.");
	FUNCTION_EXIT;
}



AlarmFilterBySystemType::~AlarmFilterBySystemType()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterBySystemType::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterBySystemType* entityNameFilter = dynamic_cast<const AlarmFilterBySystemType*>(&filter);
	if (entityNameFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==entityNameFilter->m_compareOp) &&
		    (m_compareSystemType==entityNameFilter->m_compareSystemType))
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
bool AlarmFilterBySystemType::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	return true;
}
/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterBySystemType::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->systemType,m_compareSystemType.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->systemType,m_compareSystemType.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->systemType,m_compareSystemType.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->systemType,m_compareSystemType.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->systemType,m_compareSystemType.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->systemType,m_compareSystemType.c_str())>=0;
	case COMPARE_CONTAINS:			//return strstr(alarm.subsytemType,m_compareSubsytemType.c_str())!=NULL;
		//TD15029 azenitha++
		//Must be case insensitive
		{
			//Place in temporary holders, or else the m_compareSubsytemType will be in lower case
			std::string sytemTypeStrTmp = alarm->systemType;
			std::string compareSytemTypeStrTmp = m_compareSystemType;
			
			//Convert first to lower before searching substring
			const char* sytemTypeStr = strlwr((char*)sytemTypeStrTmp.c_str());
			const char* compareSytemTypeStr = strlwr((char*)compareSytemTypeStrTmp.c_str());
			
			return strstr(sytemTypeStr,compareSytemTypeStr)!=NULL;
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

//++Noel
bool AlarmFilterBySystemType::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->systemType.c_str(),m_compareSystemType.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->systemType.c_str(),m_compareSystemType.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->systemType.c_str(),m_compareSystemType.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->systemType.c_str(),m_compareSystemType.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->systemType.c_str(),m_compareSystemType.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->systemType.c_str(),m_compareSystemType.c_str())>=0;
	case COMPARE_CONTAINS:			//return strstr(alarm.subsytemType,m_compareSubsytemType.c_str())!=NULL;
		//TD15029 azenitha++
		//Must be case insensitive
		{
			//Place in temporary holders, or else the m_compareSubsytemType will be in lower case
			std::string sytemTypeStrTmp = alarm->systemType.c_str();
			std::string compareSytemTypeStrTmp = m_compareSystemType;

			//Convert first to lower before searching substring
			const char* sytemTypeStr = strlwr((char*)sytemTypeStrTmp.c_str());
			const char* compareSytemTypeStr = strlwr((char*)compareSytemTypeStrTmp.c_str());

			return strstr(sytemTypeStr,compareSytemTypeStr)!=NULL;
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