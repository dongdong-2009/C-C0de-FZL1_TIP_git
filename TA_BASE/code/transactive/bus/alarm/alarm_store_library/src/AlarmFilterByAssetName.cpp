/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByAssetName.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByAssetName.cpp
//  Implementation of the Class AlarmFilterByAssetName
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterByAssetName.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterByAssetName::AlarmFilterByAssetName(EComparitor compareOp,const std::string& compareAssetName) 
                  : IAlarmFilter(compareOp),
					m_compareAssetName(compareAssetName.c_str())
{
	FUNCTION_ENTRY("Constructor");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for filter.");
	FUNCTION_EXIT;
}



AlarmFilterByAssetName::~AlarmFilterByAssetName()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByAssetName::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterByAssetName* entityNameFilter = dynamic_cast<const AlarmFilterByAssetName*>(&filter);
	if (entityNameFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==entityNameFilter->m_compareOp) &&
		    (m_compareAssetName==entityNameFilter->m_compareAssetName))
		{
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

bool AlarmFilterByAssetName::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
//bool includeAlarm(const AlarmItem& alarm)
{
	FUNCTION_ENTRY("includeAlarm");

	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return strcmp(alarm.assetName.in(), m_compareAssetName.c_str())==0;
		case COMPARE_NOT_EQUAL:			return strcmp(alarm.assetName.in(), m_compareAssetName.c_str())!=0;
		case COMPARE_LESS_THAN:			return strcmp(alarm.assetName.in(), m_compareAssetName.c_str())<0;
		case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm.assetName.in(), m_compareAssetName.c_str())<=0;
		case COMPARE_GREATER_THAN:		return strcmp(alarm.assetName.in(), m_compareAssetName.c_str())>0;
		case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm.assetName.in(), m_compareAssetName.c_str())>=0;
		case COMPARE_CONTAINS:			//return strstr(alarm.assetName,m_compareAssetName.c_str())!=NULL;
			//TD15029 azenitha++
			//Must be case insensitive
			{
				//Place in temporary holders, or else the m_compareAssetName will be in lower case
				std::string assetNameStrTmp = alarm.assetName.in();
				std::string compareAssetNameStrTmp = m_compareAssetName;

				//Convert first to lower before searching substring
				const char* assetNameStr = strlwr((char*)assetNameStrTmp.c_str() ) ;
				const char* compareAssetNameStr = strlwr((char*)compareAssetNameStrTmp.c_str());

				return strstr(assetNameStr,compareAssetNameStr)!=NULL;
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

/*-----------------------------------------------New Alarm Store--------------------------------------------------------*/
bool AlarmFilterByAssetName::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->assetName, m_compareAssetName.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->assetName, m_compareAssetName.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->assetName, m_compareAssetName.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->assetName, m_compareAssetName.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->assetName, m_compareAssetName.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->assetName, m_compareAssetName.c_str())>=0;
	case COMPARE_CONTAINS:			//return strstr(alarm.assetName,m_compareAssetName.c_str())!=NULL;
		//TD15029 azenitha++
		//Must be case insensitive
		{
			//Place in temporary holders, or else the m_compareAssetName will be in lower case
			std::string assetNameStrTmp = alarm->assetName;
			std::string compareAssetNameStrTmp = m_compareAssetName;
			
			//Convert first to lower before searching substring
			const char* assetNameStr = strlwr((char*)assetNameStrTmp.c_str() ) ;
			const char* compareAssetNameStr = strlwr((char*)compareAssetNameStrTmp.c_str());
			
			return strstr(assetNameStr,compareAssetNameStr)!=NULL;
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
/*----------------------------------------------------------------------------------------------------------------------*/

//++Noel 
bool AlarmFilterByAssetName::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->assetName.c_str(), m_compareAssetName.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->assetName.c_str(), m_compareAssetName.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->assetName.c_str(), m_compareAssetName.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->assetName.c_str(), m_compareAssetName.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->assetName.c_str(), m_compareAssetName.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->assetName.c_str(), m_compareAssetName.c_str())>=0;
	case COMPARE_CONTAINS:			//return strstr(alarm.assetName,m_compareAssetName.c_str())!=NULL;
		//TD15029 azenitha++
		//Must be case insensitive
		{
			//Place in temporary holders, or else the m_compareAssetName will be in lower case
			std::string assetNameStrTmp = alarm->assetName;
			std::string compareAssetNameStrTmp = m_compareAssetName;
			
			//Convert first to lower before searching substring
			const char* assetNameStr = strlwr((char*)assetNameStrTmp.c_str() ) ;
			const char* compareAssetNameStr = strlwr((char*)compareAssetNameStrTmp.c_str());
			
			return strstr(assetNameStr,compareAssetNameStr)!=NULL;
		}
		//TD15029 ++azenitha
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnormal case" );
		}
	}
	
	
	FUNCTION_EXIT;
    return false;
}