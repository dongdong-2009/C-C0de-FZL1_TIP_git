/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmFilterBySubsystemType.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: grace.koh $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterBySubsystemType.cpp
//  Implementation of the Class AlarmFilterBySubsystemType
//  Created on:      26-Aug-2004
///////////////////////////////////////////////////////////

#include "AlarmFilterBySubsystemType.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterBySubsystemType::AlarmFilterBySubsystemType(EComparitor compareOp,const std::string& compareSubsytemType) 
                  : IAlarmFilter(compareOp),
					m_compareSubsytemType(compareSubsytemType)
{
	FUNCTION_ENTRY("Constructor");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for filter.");
	FUNCTION_EXIT;
}



AlarmFilterBySubsystemType::~AlarmFilterBySubsystemType()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterBySubsystemType::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterBySubsystemType* entityNameFilter = dynamic_cast<const AlarmFilterBySubsystemType*>(&filter);
	if (entityNameFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==entityNameFilter->m_compareOp) &&
		    (m_compareSubsytemType==entityNameFilter->m_compareSubsytemType))
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
bool AlarmFilterBySubsystemType::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	return true;
}
/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterBySubsystemType::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->subsytemType,m_compareSubsytemType.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->subsytemType,m_compareSubsytemType.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->subsytemType,m_compareSubsytemType.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->subsytemType,m_compareSubsytemType.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->subsytemType,m_compareSubsytemType.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->subsytemType,m_compareSubsytemType.c_str())>=0;
	case COMPARE_CONTAINS:			//return strstr(alarm.subsytemType,m_compareSubsytemType.c_str())!=NULL;
		//TD15029 azenitha++
		//Must be case insensitive
		{
			//Place in temporary holders, or else the m_compareSubsytemType will be in lower case
			std::string subsytemTypeStrTmp = alarm->subsytemType;
			std::string compareSubsytemTypeStrTmp = m_compareSubsytemType;
			
			//Convert first to lower before searching substring
			const char* subsytemTypeStr = strlwr((char*)subsytemTypeStrTmp.c_str());
			const char* compareSubsytemTypeStr = strlwr((char*)compareSubsytemTypeStrTmp.c_str());
			
			return strstr(subsytemTypeStr,compareSubsytemTypeStr)!=NULL;
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
bool AlarmFilterBySubsystemType::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->subSystemType.c_str(),m_compareSubsytemType.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->subSystemType.c_str(),m_compareSubsytemType.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->subSystemType.c_str(),m_compareSubsytemType.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->subSystemType.c_str(),m_compareSubsytemType.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->subSystemType.c_str(),m_compareSubsytemType.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->subSystemType.c_str(),m_compareSubsytemType.c_str())>=0;
	case COMPARE_CONTAINS:			//return strstr(alarm.subsytemType,m_compareSubsytemType.c_str())!=NULL;
		//TD15029 azenitha++
		//Must be case insensitive
		{
			//Place in temporary holders, or else the m_compareSubsytemType will be in lower case
			std::string subsytemTypeStrTmp = alarm->subSystemType.c_str();
			std::string compareSubsytemTypeStrTmp = m_compareSubsytemType;

			//Convert first to lower before searching substring
			const char* subsytemTypeStr = strlwr((char*)subsytemTypeStrTmp.c_str());
			const char* compareSubsytemTypeStr = strlwr((char*)compareSubsytemTypeStrTmp.c_str());

			return strstr(subsytemTypeStr,compareSubsytemTypeStr)!=NULL;
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