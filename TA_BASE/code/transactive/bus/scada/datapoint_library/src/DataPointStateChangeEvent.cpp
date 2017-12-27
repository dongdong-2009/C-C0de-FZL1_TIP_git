/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointStateChangeEvent.cpp $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class implements the IEvent interface. It is responsible for writing datapoint state changes
 * to the database and providing summary information that can be used for event processing.
 */

#if defined(__WIN32__)
#pragma warning(disable:4786)
#endif // defined(__WIN32__)

#include <ctime>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/DataPointStateChangeEvent.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/data_access_interface/src/DataPointStateChangeAccessFactory.h"
#include "core/data_access_interface/src/IDataPointStateChangeData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/message/types/DataPointAudit_MessageTypes.h"
#include "core/message/types/MessageTypes.h"
#include "core/utilities/src/TAAssert.h"

namespace
{
    static const int TEXT_FIELD_SIZE = 100;
}

namespace TA_Base_Bus
{    
    DataPointStateChangeEvent::DataPointStateChangeEvent( boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> stateChange,
                                                          DataPoint& dp,
                                                          const TA_Base_Core::MessageType& messageType )
        : 
		m_entityKey(dp.getEntityKey()),
		m_entityTypeKey(dp.getTypeKey()),
        m_eventTypeKey(TA_Base_Core::EventConstants::INVALID_KEY),
		m_subSystemKey(dp.getSubsystemKey()),
		m_physicalSubsystemKey(dp.getPhysicalSubsystemKey()),
		m_locationKey(dp.getLocationKey()),
		m_assetName(dp.getAssetName())
    {
        std::stringstream typeKey;
        typeKey << messageType.getTypeKey();
        typeKey >> m_eventTypeKey;

        int dpValueType = DPT_NULL_TYPE;
        DpValue* dpValue = NULL;
        int updatetype = stateChange->updateType;
        time_t updateTimeStamp; 
        //only the value update, the update data contain the datapoint type
        if (TA_Base_Bus::ValueUpdate == updatetype)
        {
            dpValueType = stateChange->value.dpType;
		    dpValue = new DpValue(stateChange->value, dp.getDataPointDataType(), dp.getEnumLabelsMap(), dp.getBooleanLabels(), dp.getPrecision());
        }
        else
        {
            dpValueType = dp.getDataPointDataType();
		    dpValue = new DpValue(dp.getCurrentValue());
        }

        //make sure the dpvalue is correct
        TA_ASSERT(dpValueType != DPT_NULL_TYPE, "unknown datapoint data type!");
        //make sure the dpvalue is constructed
        TA_ASSERT(dpValue != NULL, "failed to create dp value!");

        int alarmstate;
        if (updatetype == TA_Base_Bus::AlarmStateUpdate)
        {
            alarmstate = stateChange->alarmState;
        }
        else
        {
            alarmstate = dp.getAlarmState();
        }
        
        if (updatetype == LastOperatorUpdate)
        {
            updateTimeStamp = stateChange->lastOperatorInfo.timeStamp.time;
        }
        else
        {
            updateTimeStamp = dp.getTimestamp();
        }

        std::stringstream value;
        std::string alarmMessageType("");
        long generalMessage(-1);

        switch ( dpValueType )
        {
        case TA_Base_Bus::DPT_INT16_TYPE:		
        case TA_Base_Bus::DPT_INT32_TYPE:	
        case TA_Base_Bus::DPT_INT16S_TYPE:
        case TA_Base_Bus::DPT_INT32S_TYPE:
        case TA_Base_Bus::DPT_UINT16_TYPE:
        case TA_Base_Bus::DPT_UINT32_TYPE:
        case TA_Base_Bus::DPT_IEEE32_TYPE:	
            switch(alarmstate)
            {
            case TA_Base_Bus::DptOverScaleHi:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_MESSAGE;
                value << DPTALM_OH_SHORT << " ";
                break;

            case TA_Base_Bus::DptOverScaleLo:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_MESSAGE;
                value << DPTALM_OL_SHORT << " ";
                break;

            case TA_Base_Bus::DptHiHi:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_MESSAGE;
                value << DPTALM_HIHI_SHORT << " ";
                break;
                
            case TA_Base_Bus::DptHi:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_MESSAGE;
                value << DPTALM_HI_SHORT << " ";
                break;
            
            case TA_Base_Bus::DptLo:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_MESSAGE;
                value << DPTALM_LO_SHORT << " ";
                break;
                
            case TA_Base_Bus::DptLoLo:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_MESSAGE;
                value << DPTALM_LOLO_SHORT << " ";
                break;
                
            case TA_Base_Bus::DptGeneral:
            case TA_Base_Bus::DptNoAlarm:
            case TA_Base_Bus::DptBoolTrue:
            case TA_Base_Bus::DptBoolFalse:
            default:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_EVENT_MESSAGE;
                break;
            }
            
			{
				value << dpValue->getValueAsString();

				// Append units here...
                // (TD 15181: but only if the EngUnits are enabled and not blank)
                if ( true == dp.isEngUnitsEnabled() )
                {
				    TA_Base_Bus::DataPointEngineeringUnits dpEngUnit;
				    dp.getEngineeringUnits( dpEngUnit );

                    std::string engUnits = dpEngUnit.Units;
                    if ( !engUnits.empty() )
        	            value << " (" << engUnits << ")"; // wenching++ (TD14422)
                }

			}
			break;

		case TA_Base_Bus::DPT_ENUM_TYPE:
            
            switch(stateChange->alarmState)
            {    
            case TA_Base_Bus::DptGeneral:
                if (TA_Base_Bus::ValueUpdate == updatetype)
                {
                    generalMessage = stateChange->value.enumValue;
                }
                else
                {
                    generalMessage = dp.getCurrentValue().getEnum();
                }
                break;
            
            case TA_Base_Bus::DptNoAlarm:
            case TA_Base_Bus::DptBoolTrue:
            case TA_Base_Bus::DptBoolFalse:
			case TA_Base_Bus::DptOverScaleHi:
            case TA_Base_Bus::DptOverScaleLo:
            case TA_Base_Bus::DptHiHi:
            case TA_Base_Bus::DptHi:
            case TA_Base_Bus::DptLo:
            case TA_Base_Bus::DptLoLo:
            default:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_EVENT_MESSAGE;
                break;
            }
            
            value << dpValue->getValueAsString();
            break;
            
        case TA_Base_Bus::DPT_BOOL_TYPE:
            switch(stateChange->alarmState)
            {
            case TA_Base_Bus::DptBoolTrue:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_TRUE_ALARM_MESSAGE;
                break;

            case TA_Base_Bus::DptBoolFalse:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_FALSE_ALARM_MESSAGE;
                break;

            case TA_Base_Bus::DptNoAlarm:
            case TA_Base_Bus::DptGeneral:
			case TA_Base_Bus::DptOverScaleHi:
            case TA_Base_Bus::DptOverScaleLo:
            case TA_Base_Bus::DptHiHi:
            case TA_Base_Bus::DptHi:
            case TA_Base_Bus::DptLo:
            case TA_Base_Bus::DptLoLo:
            default:
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_EVENT_MESSAGE;
                break;
            }

            value << dpValue->getValueAsString();
            break;
        
        case TA_Base_Bus::DPT_TEXT_TYPE:
            {
                value << dpValue->getValueAsString();
            
                alarmMessageType = TA_Base_Core::DataPointEntityData::DATAPOINT_EVENT_MESSAGE;
            }
            break;

        default:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown DataPointState.value");
            break;
        }

        if ( value.str().empty() )
        {
            value << " ";
        }

        // And convert the status to a string - this should be replaced with a human readable string in the future.
        std::ostringstream status;
        status << dp.getStatus();

        // Now need to "resize" all the strings to the maximum length their database fields will take - this will
        // truncate any strings at this size.
        if ( TEXT_FIELD_SIZE < value.str().size() )
        {
            std::string valueStr( value.str() );
            valueStr.resize( TEXT_FIELD_SIZE );
            value.str( valueStr );
        }
		m_value = value.str();

		m_createTime.time = updateTimeStamp;

        // Create the data point state update object
        m_stateChange = std::auto_ptr<TA_Base_Core::IDataPointStateChangeData>( TA_Base_Core::DataPointStateChangeAccessFactory::getInstance().createDataPointStateChange() );

        // And now set all the fields
        m_stateChange->setEntityKey( m_entityKey );
        m_stateChange->setLogtime( time(NULL) );
        m_stateChange->setStatus( status.str() );
        m_stateChange->setTimestamp( updateTimeStamp );
        m_stateChange->setValue( value.str() );
        m_stateChange->setEventTypeKey( m_eventTypeKey );

        TA_ASSERT (alarmMessageType != "" || generalMessage >= 0, "Invalid alarmMessageType || generalMessage");

        if (alarmMessageType != "")
        {
            m_stateChange->setMessage(alarmMessageType);
        }
        else
        {
            m_stateChange->setMessage(generalMessage);
        }

        //clean the dpValue
        delete dpValue;
        dpValue = NULL;

		generateDescription(stateChange, dp, alarmMessageType, generalMessage);

    }

    DataPointStateChangeEvent::~DataPointStateChangeEvent()
    {
		for (TA_Base_Core::DescriptionParametersIterator itPara = m_description.begin(); itPara != m_description.end(); ++itPara)
		{
			delete *itPara;
			*itPara = NULL;
		}
		m_description.clear();
    }


    unsigned long DataPointStateChangeEvent::getEntityKey()
    {
        return m_entityKey;
    }

	std::string DataPointStateChangeEvent::getEventId()
	{
		return m_stateChange->getKey();
	}

    unsigned long DataPointStateChangeEvent::getEntityTypeKey()
    {
        return m_entityTypeKey;
    }


    unsigned long DataPointStateChangeEvent::getEventTypeKey()
    {
        return m_eventTypeKey;
    }


    void DataPointStateChangeEvent::writeToDatabase()
    {
        m_stateChange->applyChanges();
    }

	
	unsigned long DataPointStateChangeEvent::getSubsystemKey()
	{
		return m_subSystemKey;
	}

	unsigned long DataPointStateChangeEvent::getPhysicalSubsystemKey()
	{
		return m_physicalSubsystemKey;
	}

	unsigned long DataPointStateChangeEvent::getEntityLocationKey()
	{
		return m_locationKey;
	}

	std::string DataPointStateChangeEvent::getAssetName()
	{
		return m_assetName;
	}
	
	std::string DataPointStateChangeEvent::getValue()
	{
		return m_value;
	}

	timeb DataPointStateChangeEvent::getCreateTime()
	{
		return m_createTime;
	}

	void DataPointStateChangeEvent::generateDescription(
		boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> stateChange,
		DataPoint& dp, std::string& alarmMessageType, unsigned long generalMessageKey)
	{
		std::string description("");
		TA_Base_Bus::EDataPointAlarms alarmState = TA_Base_Bus::DptNoAlarm;
		int updatetype = stateChange->updateType;

		if (TA_Base_Bus::AlarmStateUpdate == updatetype)
		{
			alarmState = stateChange->alarmState;
		}
		else
		{
			alarmState = dp.getAlarmState();
		}

		if (TA_Base_Bus::DptNoAlarm == alarmState)
		{
			description = dp.getEventDescription();
		}
		else if (TA_Base_Bus::ValueUpdate == updatetype)
		{
			DpValue dpValue(stateChange->value, dp.getDataPointDataType(), dp.getEnumLabelsMap(), dp.getBooleanLabels(), dp.getPrecision());
			description = dp.getAlarmDescription(alarmState, dpValue);
		}
		else
		{
			DpValue dpValue(dp.getCurrentValue());
			description = dp.getAlarmDescription(alarmState, dpValue);
		}

		if ( description.empty() )
		{
			std::stringstream eventDescription;
			if( DPT_ENUM_TYPE == dp.getDataPointDataType() )
			{
				eventDescription << "Description not set for derived datapoint ";
				eventDescription << dp.getDataPointName();
				eventDescription << "(" << m_entityKey << ") state:";
				eventDescription << generalMessageKey;
			}
			else 
			{
				eventDescription << "Description not set for datapoint ";
				eventDescription << dp.getDataPointName();
				eventDescription << "(" << m_entityKey << ") state:";
				eventDescription << alarmMessageType;
			}
			description = eventDescription.str();
		}

		m_description.clear();
		TA_Base_Core::NameValuePair* pPair = new TA_Base_Core::NameValuePair( "EventMessage", description);
		m_description.push_back(pPair);
	}

	TA_Base_Core::DescriptionParameters DataPointStateChangeEvent::getDescriptionParameters()
	{
		return m_description; 
	}
}
