/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmSortByField.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmSortByField.cpp
//  Implementation of the Class AlarmSortByField
//  Created on:      24-Jan-2004 04:19:57 PM
///////////////////////////////////////////////////////////
#include <algorithm>
#include "bus/alarm/alarm_list_control/src/StdAfx.h"

#include "AlarmSortByField.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/alarm/src/Utilities.h"

//marvin++
#include "core/message/src/MessageConfig.h"
#include "bus/alarm/alarm_common/src/AlarmConstants.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Bus;

AlarmSortByField::AlarmSortByField(ESortField sortField,ESortDirection sortDirection)
				 :  m_sortField(sortField),
					m_sortDirection(sortDirection)

{
	FUNCTION_ENTRY("Constructor");

	m_isUsingSourceTime = TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_USESOURCETIME.c_str());

	FUNCTION_EXIT;
}



AlarmSortByField::~AlarmSortByField()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmSortByField::operator == (const IAlarmSort& sortItem) const
{
	FUNCTION_ENTRY("Operator==");

	// See if it is the same sort type
	const AlarmSortByField* fieldSort = dynamic_cast<const AlarmSortByField*>(&sortItem);
	if (fieldSort!=NULL)
	{
		// Check parameters are the same
		if ((m_sortField==fieldSort->m_sortField)&&
			(m_sortDirection==fieldSort->m_sortDirection))
		{
			return true;
		}
	}
	FUNCTION_EXIT;
	return false;
}


IAlarmSort::ECompareResult AlarmSortByField::compareAlarms(const TA_Base_Core::AlarmDetailCorbaDef& alarm1,
															const TA_Base_Core::AlarmDetailCorbaDef& alarm2) const
{
	FUNCTION_ENTRY("compareAlarms");

	IAlarmSort::ECompareResult  result;

	try
	{
		
	switch (m_sortField)
	{
		case SORT_OPEN_CLOSE:   {	
                                    bool alarm1Open = alarm1.state == TA_Base_Core::AlarmOpen;
                                    bool alarm2Open = alarm2.state == TA_Base_Core::AlarmOpen;
                                    result = compare(alarm2Open,alarm1Open);
								    break;
                                }

		case SORT_ACK_NOT_ACK:	{
									std::string alarm1AckBy(alarm1.alarmAcknowledgedBy.in());
									bool alarm1Ack = (alarm1AckBy.length()>0); 
									std::string alarm2AckBy(alarm2.alarmAcknowledgedBy.in());
									bool alarm2Ack = (alarm2AckBy.length()>0); 
									result = compare(alarm1Ack,alarm2Ack);
								}
								break;

		case SORT_ALARM_TYPE:	{
										const unsigned long alarm1type = alarm1.messageTypeKey;
										const unsigned long alarm2type = alarm2.messageTypeKey;
									result = compare(alarm1type,alarm2type);
								}
								break;

			case SORT_LOCATION:		{
										const unsigned long alarm1loc = static_cast <unsigned long> (alarm1.locationKey);
										const unsigned long alarm2loc = static_cast <unsigned long> (alarm2.locationKey);
										result = compare(alarm1loc,alarm2loc);
									}
								break;

			case SORT_TIME:	{
										if ( m_isUsingSourceTime )
										{
											result = compare(alarm1.sourceTime,alarm2.sourceTime);
										}
										else
										{
											result = compare(alarm1.displayTime,alarm2.displayTime);
										}
										
									}
								break;

		case SORT_DESCRIPTION:	result = compare(alarm1.alarmDescription.in(), alarm2.alarmDescription.in());
								break;

		case SORT_ENTITY_NAME:	result = compare(alarm1.entityName.in(), alarm2.entityName.in());  // ASSET
								break;

			case SORT_SUB_SYSTEM_KEY: {
										const unsigned long alarm1SubSysKey = static_cast <unsigned long> (alarm1.subsystemKey);
										const unsigned long alarm2SubSysKey = static_cast <unsigned long> (alarm2.subsystemKey);
										result = compare(alarm1SubSysKey,alarm2SubSysKey);
									  }
								break;

			case SORT_ACK_TIME:		{
										const TA_Base_Core::DateTime alarm1AckTime = static_cast <TA_Base_Core::DateTime> (alarm1.ackTime);
										const TA_Base_Core::DateTime alarm2AckTime = static_cast <TA_Base_Core::DateTime> (alarm2.ackTime);
										result = compare(alarm1AckTime,alarm2AckTime);
									}
								break;

		case SORT_ACK_BY:		result = compare(alarm1.alarmAcknowledgedBy.in(), alarm2.alarmAcknowledgedBy.in());
								break;

			case SORT_SEVERITY:		{
										const unsigned long alarm1Sevr = static_cast <unsigned long> (alarm1.alarmSeverity);
										const unsigned long alarm2Sevr = static_cast <unsigned long> (alarm2.alarmSeverity);
										result = compare(alarm1Sevr,alarm2Sevr);
									}
								break;	
								
		case SORT_ASSET_NAME:	result = compare(alarm1.assetName.in(), alarm2.assetName.in());
								break;	
                                
        case SORT_VALUE:	    //result = compare(alarm1.messageTypeKey.in(),alarm2.messageTypeKey.in()); // For some reason, messageTypeKey contains the type name
								//marvin++
								result = compare(alarm1.alarmValue.in(), alarm2.alarmValue.in() );
								//++marvin
								break;	

        case SORT_MMS:	        {
									std::string mms1 = TA_Base_Core::Utilities::getInstance().convertMMS2String(alarm1.mmsState); 
                                    std::string mms2 = TA_Base_Core::Utilities::getInstance().convertMMS2String(alarm2.mmsState);

                                    result = compare(mms1.c_str(),mms2.c_str());
								}
                                break;	
                                                                
        case SORT_DSS:	        {
                                    std::string dss1 = TA_Base_Core::Utilities::getInstance().convertDSS2String(alarm1.decisionSupportState);
                                    std::string dss2 = TA_Base_Core::Utilities::getInstance().convertDSS2String(alarm2.decisionSupportState);

                                    result = compare(dss1.c_str(),dss2.c_str());
								}
                                break;	
                                
        case SORT_AVALANCHE:	{
                                    std::string avalanche1 = TA_Base_Core::Utilities::getInstance().convertAvalanche2String(alarm1.isHeadOfAvalanche, alarm1.isChildOfAvalanche);
                                    std::string avalanche2 = TA_Base_Core::Utilities::getInstance().convertAvalanche2String(alarm2.isHeadOfAvalanche, alarm2.isChildOfAvalanche);

                                    result = compare(avalanche1.c_str(),avalanche2.c_str());
								}
                                break;	
		//TD #12719++
		case SORT_COMMENT:		result = compare(alarm1.alarmComments.in(), alarm2.alarmComments.in());
								break;
		//TD ++#12719

		default:
			TA_ASSERT(false,"Unhandled sorting field.");
	}

	// Correct result for sort direction
	if (m_sortDirection==SORT_DESCENDING)
	{
		if (result==IAlarmSort::SORT_BEFORE)
		{
			result=IAlarmSort::SORT_AFTER;
		}
		else if (result==IAlarmSort::SORT_AFTER)
		{
			result=IAlarmSort::SORT_BEFORE;
		}
	}
	}

	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"compareAlarms: Problem encountered in comparing alarm1 id: %s and alarm2 id: %s for sort field: %d", alarm1.alarmID.in(), alarm2.alarmID.in(), m_sortField );
		TA_ASSERT(false, "compareAlarms failure");
	}

	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
	//	"compareAlarms: Completed comparing alarm1 id: %s and alarm2 id: %s of result: %d", alarm1.alarmID.in(), alarm2.alarmID.in(), result );

	FUNCTION_EXIT;
	return result;
}


//*******************
//* Private methods *
//*******************

IAlarmSort::ECompareResult AlarmSortByField::compare(const unsigned long key1,const unsigned long key2) const
{
	if (key1 == key2)
	{
		return IAlarmSort::SORT_EQUAL;
	}

	if (key1 < key2)
	{
		return IAlarmSort::SORT_BEFORE;
	}

	return IAlarmSort::SORT_AFTER;
}


IAlarmSort::ECompareResult AlarmSortByField::compare(const bool key1,const bool key2) const
{
	if (key1 == key2)
	{
		return IAlarmSort::SORT_EQUAL;
	}

	if (key2)
	{
		return IAlarmSort::SORT_BEFORE;
	}

	return IAlarmSort::SORT_AFTER;
}

IAlarmSort::ECompareResult AlarmSortByField::compare(const char* key1,const char* key2) const
{
	int result = _stricmp(key1,key2);
	
	if (result==0)
	{
		return IAlarmSort::SORT_EQUAL;
	}

	if (result<0)
	{
		return IAlarmSort::SORT_BEFORE;
	}

	return IAlarmSort::SORT_AFTER;
}

IAlarmSort::ECompareResult AlarmSortByField::compare(const TA_Base_Core::MillisecondTime& key1, 
                                                     const TA_Base_Core::MillisecondTime& key2) const
{
	if (key1.time == key2.time)
	{
        if (key1.millitm == key2.millitm)
        {
            return IAlarmSort::SORT_EQUAL;
        }
        else if (key1.millitm < key2.millitm)
        {
            return IAlarmSort::SORT_BEFORE;
        }
        else
        {
            return IAlarmSort::SORT_AFTER;
        }
	}

	if (key1.time < key2.time)
	{
		return IAlarmSort::SORT_BEFORE;
	}

	return IAlarmSort::SORT_AFTER;
}

/*
//marvin++
//TD14023
std::string AlarmSortByField::getItemText(const TA_Base_Core::AlarmDetailCorbaDef& alarm) const
{
	// The value will be: "AlarmType Value"
	CString text = alarm.messageTypeKey;

    // The value is stored in one of the description parameters
    std::string alarmParameters(alarm.alarmParameterList);
    
    TA_Base_Core::DescriptionParameters parameters;
    TA_Base_Core::gGetParametersFromString(alarmParameters, parameters);
    
    TA_Base_Core::DescriptionParameters::iterator valueParam = std::find_if(parameters.begin(),
                                                                       parameters.end(),
                                                                       HasName("DataPointValue"));

	TA_Base_Core::DescriptionParameters::iterator messageParam = std::find_if(parameters.begin(),
																		parameters.end(),
																		HasName("AlarmMessage"));

	// TD 12785 Only display the value without the alarm type if there is parameter value
	// if not, use the alarm type description
    if(valueParam != parameters.end())
    {
		bool isAck = false;
		bool isBoolDatapt = false;

		// TD 12785 check whether it is acknowledged if yes, add "ACK" to the front
		// and add the param value
		std::string ackedBy = alarm.alarmAcknowledgedBy;
		if ( !(ackedBy.empty()) ) isAck = true;
		// Check if the text contains DpGeneral, DpFalse and DpTrue
		// Take them out // this is a quick fix for TD 12785
		if (text.Find("DpGeneral", 0) >= 0 || text.Find("DpFalse", 0) >=0 || text.Find("DpTrue", 0) >= 0)
			isBoolDatapt = true;
		

		if (isAck)
		{
			text = "ACK ";
			
			if (isBoolDatapt)
			{
				text += (*valueParam)->value.c_str();
			}
			else
			{
				text += alarm.messageTypeKey;
				text += (*valueParam)->value.c_str();
			}
		}
		else
		{
			if (isBoolDatapt)
			{
				text = (*valueParam)->value.c_str();
			}
			else
			{
				text += (*valueParam)->value.c_str();
			}
		}
    }

	//TD 13146 Check for the temperature and '?' and replace '?' with '?
	CString alarmDesc = alarm.alarmDescription;
	alarmDesc.MakeUpper();
	if (text.Find("?C", 0) >= 0 && alarmDesc.Find("TEMPERATURE", 0) >= 0)
	{
		text.Replace(_T('?'), _T('?));
	}

    for (int i = 0;i < static_cast<int>(parameters.size());i++)
    {
        delete parameters[i];
        parameters[i] = NULL;
    }

    return text;
}
//++marvin
//TD14023 */

