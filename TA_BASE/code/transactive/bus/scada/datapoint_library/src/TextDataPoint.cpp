/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/TextDataPoint.cpp $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  * A DataPoint that represents a series of ASCII 
  * characters. No alarms can be configured for this
  * DataPoints. No engineering conversion is 
  * available, though the DataPoint may be associated 
  * with some engineering units.
  *
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/TextDataPoint.h"

#include "core/exceptions/src/GenericAgentException.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	TextDataPoint::TextDataPoint( TA_Base_Core::DataPointEntityDataPtr  			dataPointEntityAccess,
								  TA_Base_Bus::EDataPointType		                dpType,
								  TA_Base_Bus::EDataPointDataType	                dpDataType,
                                  ScadaPersistencePtr                               persistence,
                                  TA_Base_Core::ScadaRootEntityData&                scadaRootEntityData,
                                  MmsScadaUtility&                                  mmsScadaUtility,
                                  CommandProcessor&                                 commandProcessor )
	: 
	DataPoint( dataPointEntityAccess, dpType, dpDataType, persistence, scadaRootEntityData, mmsScadaUtility, commandProcessor ),
	m_maxTextLength( 0 )
	{
		// 
		// Check Datapoint Data Type
		//
		if (dpDataType != TA_Base_Bus::DPT_TEXT_TYPE)
		{
            std::ostringstream desc;
            desc << "Invalid Configuration; TextDataPoint [" << m_name << "] data type (" << dpDataType << ") invalid.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}

		//
		// setup the max text length for this Text DataPoint if necessary
		//

		setMaxTextLength( dataPointEntityAccess->getMaxTextLength() );

		//
		// Configure engineering units
		//

		if ( true == dataPointEntityAccess->getEngineeringUnitsEnabled() )
		{
    		setEngineeringUnits( dataPointEntityAccess->getTextEngineeringUnit() );
		}

		//
		// Set up the forced value
		//
		if ( m_forcedState )
        {
			std::string tempString;

            DpValue forcedValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
            m_persistence->readPersistedValue( DP_MANUALLY_DRESSED_VALUE, m_entityKey, tempString );
            
			forcedValue.setFromPersist( tempString );
			
            convertToRaw( forcedValue, m_forcedValue );
            m_forcedEngValue = forcedValue;
        }
	}

	TextDataPoint::~TextDataPoint()
	{

	}

	int TextDataPoint::getMaxTextLength() const
	{
		return m_maxTextLength;
	}


	void TextDataPoint::setMaxTextLength ( int newMax )
	{
		// setup the max text length for this text data point
		if ( 0 >= newMax )
		{
			// use the default size if not a valid number
			m_maxTextLength = DPT_MAX_TEXT_LENGTH;
		}
		else
		{
			m_maxTextLength = newMax;
		}
	}


	void TextDataPoint::setEngineeringUnits( const std::string & engUnits )
	{
		if ( engUnits.length() > 0 )
		{
			m_engineeringUnits.assign( engUnits );
			setEngUnitsEnabled( true );
		}
		else
		{
			m_engineeringUnits.assign( "" );
			setEngUnitsEnabled( false );
		}

		// update eng unit for alarm of this data point, if there is any
		updateEngUnitForAlarms ( engUnits );
	}


	void TextDataPoint::getEngineeringUnits(
							TA_Base_Bus::DataPointEngineeringUnits & eng )
	{
		// there is no engineering unit for a text data point
		eng.EngineeringUnitsEnabled = false;
		eng.LowLimit = 0;
		eng.HighLimit = 0;
		eng.Units = "";
	}





//	void TextDataPoint::getAlarmProperties(
//						TA_Base_Bus::DataPointCorbaDef::DataPointAlarmTypes & almType,
//						TA_Base_Bus::DataPointCorbaDef::AlarmProperties  & almProps )
//	{
//		throw TA_Base_Core::BadParameterException( "Alarms not configured for Text DataPoints" );
//	}


	void TextDataPoint::updateSpecificConfiguration ( const std::vector< std::string > * configChanges )
	{
		// for each of the changed parameters
		for ( unsigned int i = 0; i < configChanges->size(); i++ )
		{
			std::string paramName = (*configChanges)[i];

			if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_UNITS_ENABLED )
			{
				// Engineering Units Enabled
				//
				// Only interested if this disables existing 
				// eng units
				//
				if ( false == m_dataPointEntityAccess->getEngineeringUnitsEnabled() )
				{
					setEngineeringUnits( "" );
				}
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_TEXT_ENGINEERING_UNIT )
			{
				// Text Engineering Units	
				setEngineeringUnits( m_dataPointEntityAccess->getTextEngineeringUnit() );
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_MAX_TEXT_LENGTH )
			{
				// max text length
				setMaxTextLength ( m_dataPointEntityAccess->getMaxTextLength() );
			}
			else 
			{
				// do nothing
			}
		}
	}


	void TextDataPoint::setToControlMode()
	{
		// call the base class
		DataPoint::setToControlMode();
	}


	void TextDataPoint::setToMonitorMode()
	{
		// call the base class
		DataPoint::setToMonitorMode();
	}


    std::string TextDataPoint::getDataPointValueAsString( const DpValue & dpValue )
    {
        FUNCTION_ENTRY("getDataPointValueAsString");

        TA_Base_Bus::ByteSequence byteSeq = dpValue.getByteSequence();
    
        std::ostringstream valueStr;
        for ( unsigned int i=0; i < byteSeq.size(); ++i )
        {
            valueStr << (char)byteSeq[i];
        }

        FUNCTION_EXIT;
        return valueStr.str();
    }
    
    boost::shared_ptr<DatapointCompleteStaticConfig> TextDataPoint::getCompleteStaticConfig()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<DatapointCompleteStaticConfig> toReturn = DataPoint::getCompleteStaticConfig();
        getEngineeringUnits(toReturn->engUnits);
        toReturn->alarmPropertyVect = getAlarmPropertiesVect();
        return toReturn;
    }

    boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> TextDataPoint::getCompleteDynamicUpdate()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> completeudpate = DataPoint::getCompleteDynamicUpdate();
        //set the current alarm properties
        completeudpate->alarmProperties = getAlarmPropertiesVect();
        return completeudpate;
    }

    TA_Base_Bus::AlarmPropertyVect TextDataPoint::getAlarmPropertiesVect()
    {
		//
		// Alarm Properties sequence
		//
		// set the size of the sequence
		 TA_Base_Bus::AlarmPropertyVect alarmProperties;
        //the return sequence should be empty
        return alarmProperties;
    }

    void TextDataPoint::updateSynState(const TA_Base_Bus::DpDynamicSyncUpdate& toupdate)
    {
        FUNCTION_ENTRY("TextDataPoint::updateSynState");
        //go to the base class to update the rest
        DataPoint::updateSynState(toupdate);
        FUNCTION_EXIT;
    }

    void TextDataPoint::updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
        //the alarm properties is not required to be synchronized
		throw ( TA_Base_Core::NotAvailableException() );        
    }

    void TextDataPoint::setAlarmProperties ( const std::string& sessionID, const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
		throw ( TA_Base_Core::NotAvailableException() );        
    }
}
