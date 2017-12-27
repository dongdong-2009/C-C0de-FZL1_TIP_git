/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DataPointEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2016/12/30 17:34:07 $
  * Last modified by: $Author: wang.wang $
  *
  * DataPointEntityData provides read-only access to DataPoint entity defined in
  * the database.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4723)	// potential divide by 0
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/IData.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	// Common/Miscellaneous
    const std::string DataPointEntityData::ENTITY_TYPE                          = "DataPoint";
	const std::string DataPointEntityData::DATAPOINT_TYPE = "DataPointType";
	const std::string DataPointEntityData::DATAPOINT_TYPE_SHORT_NAME = "DataTypeShortName";
	const std::string DataPointEntityData::DATAPOINT_DATA_TYPE = "DataType";
	const std::string DataPointEntityData::DATAPOINT_HELP_URL = "HelpURL";
	const std::string DataPointEntityData::DATAPOINT_VIEW_ASSOCIATION = "ViewAssociation";
	const std::string DataPointEntityData::DATAPOINT_ENTITY_ASSOCIATION = "EntityAssociation";
	const std::string DataPointEntityData::DATAPOINT_LAUNCH_CONDITION = "LaunchCondition";
	const std::string DataPointEntityData::DATAPOINT_CALCULATED_EXPRESSION = "CalculatedExpression";
	const std::string DataPointEntityData::DATAPOINT_ABE_EXPRESSION = "ReturnCondition";
	const std::string DataPointEntityData::DATAPOINT_ABE_TIMEOUT_PERIOD_SECONDS = "ReturnTimeOutInSec";
	const std::string DataPointEntityData::DATAPOINT_SCAN_TIME_FACTOR = "ScanTimeFactor";
	const std::string DataPointEntityData::DATAPOINT_ENABLE_HELP_URL = "EnableHelpURL";
	const std::string DataPointEntityData::DATAPOINT_ENABLE_ENGINEERING_NOTE = "EnableEngineeringNote";
	const std::string DataPointEntityData::DATAPOINT_ENGINEERING_NOTE = "EngineeringNote";
	const std::string DataPointEntityData::DATAPOINT_ENABLE_TRENDING = "EnableTrending";
	const std::string DataPointEntityData::DATAPOINT_ALLOW_WRITE = "AllowWrite";
	const std::string DataPointEntityData::DATAPOINT_SERVICE_STATE = "ServiceState";
	const std::string DataPointEntityData::DATAPOINT_DISPLAY_ORDER = "DisplayOrder";
	const std::string DataPointEntityData::DATAPOINT_UPDATE_PRIORITY = "UpdatePriority";
	const std::string DataPointEntityData::DATAPOINT_LOG_CHANGES = "LogChanges";
	const std::string DataPointEntityData::DATAPOINT_LAST_OPERATOR_SESSION_ID = "LastOperatorSessionID";
	const std::string DataPointEntityData::DATAPOINT_MAX_TEXT_LENGTH = "MaxTextLength";
	const std::string DataPointEntityData::DATAPOINT_OMFLAG = "OMFlag";
    const std::string DataPointEntityData::DATAPOINT_WORD_SWAP = "WordSwap";
    const std::string DataPointEntityData::DATAPOINT_BYTE_SWAP = "ByteSwap";
	const std::string DataPointEntityData::DATAPOINT_BIT_WIDTH = "BitWidth";
	const std::string DataPointEntityData::DATAPOINT_BIT_SEQUENCE = "BitSequence";

	// Engineering Conversion
	const std::string DataPointEntityData::DATAPOINT_ENGINEERING_UNITS_ENABLED = "ConversionEnabled";
	const std::string DataPointEntityData::DATAPOINT_RAW_LIMIT_HIGH = "RawLimitHigh";
	const std::string DataPointEntityData::DATAPOINT_RAW_LIMIT_LOW = "RawLimitLow";
	const std::string DataPointEntityData::DATAPOINT_ENGINEERING_LIMIT_HIGH = "EngineeringLimitHigh";
	const std::string DataPointEntityData::DATAPOINT_ENGINEERING_LIMIT_LOW = "EngineeringLimitLow";
	const std::string DataPointEntityData::DATAPOINT_UPDATE_TIME_LIMIT = "UpdateTimeLimit";
	const std::string DataPointEntityData::DATAPOINT_ZERO_VALUE_THRESHOLD = "ZeroValueThreshold";
	const std::string DataPointEntityData::DATAPOINT_UPDATE_THRESHOLD = "UpdateThreshold";
	const std::string DataPointEntityData::DATAPOINT_ALARM_THRESHOLD = "AlarmThreshold";
	const std::string DataPointEntityData::DATAPOINT_PRECISION = "Precision";
	const std::string DataPointEntityData::DATAPOINT_ENGINEERING_UNITS = "UnitLabel";
	const std::string DataPointEntityData::DATAPOINT_TEXT_ENGINEERING_UNIT = "TextEngineeringUnit";
	const std::string DataPointEntityData::DATAPOINT_FALSE_ENGINEERING_UNIT = "FalseLabel";
	const std::string DataPointEntityData::DATAPOINT_TRUE_ENGINEERING_UNIT = "TrueLabel";

	// DpOverScaleHi
	const std::string DataPointEntityData::DATAPOINT_OH_ALARM_VALUE = "OverScaleHiAlarmValue";
	const std::string DataPointEntityData::DATAPOINT_OH_ALARM_DELAY = "OverScaleHiAlarmDelay";
	const std::string DataPointEntityData::DATAPOINT_OH_ALARM_SEVERITY = "OverScaleHiSeverity"; //TD15102
	const std::string DataPointEntityData::DATAPOINT_OH_ALARM_MESSAGE  = "OverScaleHiAlarmMessage";
	const std::string DataPointEntityData::DATAPOINT_OH_ALARM_MMS = "OverScaleHiAlarmMMS";

	// DpOverScaleLo
	const std::string DataPointEntityData::DATAPOINT_OL_ALARM_VALUE = "OverScaleLoAlarmValue";
	const std::string DataPointEntityData::DATAPOINT_OL_ALARM_DELAY = "OverScaleLoAlarmDelay";
	const std::string DataPointEntityData::DATAPOINT_OL_ALARM_SEVERITY = "OverScaleLoSeverity"; //TD15102
	const std::string DataPointEntityData::DATAPOINT_OL_ALARM_MESSAGE = "OverScaleLoAlarmMessage";
	const std::string DataPointEntityData::DATAPOINT_OL_ALARM_MMS = "OverScaleLoAlarmMMS";

	// DpHiHi
	const double DataPointEntityData::FAULTY_VALUE = 999999999.99;
	const std::string DataPointEntityData::DATAPOINT_HIHI_ALARM_VALUE = "HiHiAlarmValue";
	const std::string DataPointEntityData::DATAPOINT_HIHI_ALARM_DELAY = "HiHiAlarmDelay";
	const std::string DataPointEntityData::DATAPOINT_HIHI_ALARM_SEVERITY = "HiHiSeverity";
	const std::string DataPointEntityData::DATAPOINT_HIHI_ALARM_MESSAGE = "HiHiAlarmMessage";
	const std::string DataPointEntityData::DATAPOINT_HIHI_ALARM_MMS = "HiHiAlarmMMS";

	// DpHi
	const std::string DataPointEntityData::DATAPOINT_HI_ALARM_VALUE = "HiAlarmValue";
	const std::string DataPointEntityData::DATAPOINT_HI_ALARM_DELAY = "HiAlarmDelay";
	const std::string DataPointEntityData::DATAPOINT_HI_ALARM_SEVERITY = "HiSeverity";
	const std::string DataPointEntityData::DATAPOINT_HI_ALARM_MESSAGE = "HiAlarmMessage";
	const std::string DataPointEntityData::DATAPOINT_HI_ALARM_MMS = "HiAlarmMMS";

	// DpLoLo
	const std::string DataPointEntityData::DATAPOINT_LOLO_ALARM_VALUE = "LoLoAlarmValue";
	const std::string DataPointEntityData::DATAPOINT_LOLO_ALARM_DELAY = "LoLoAlarmDelay";
	const std::string DataPointEntityData::DATAPOINT_LOLO_ALARM_SEVERITY = "LoLoSeverity";
	const std::string DataPointEntityData::DATAPOINT_LOLO_ALARM_MESSAGE = "LoLoAlarmMessage";
	const std::string DataPointEntityData::DATAPOINT_LOLO_ALARM_MMS = "LoLoAlarmMMS";

	// DpLo
	const std::string DataPointEntityData::DATAPOINT_LO_ALARM_VALUE = "LoAlarmValue";
	const std::string DataPointEntityData::DATAPOINT_LO_ALARM_DELAY = "LoAlarmDelay";
	const std::string DataPointEntityData::DATAPOINT_LO_ALARM_SEVERITY = "LoSeverity";
	const std::string DataPointEntityData::DATAPOINT_LO_ALARM_MESSAGE = "LoAlarmMessage";
	const std::string DataPointEntityData::DATAPOINT_LO_ALARM_MMS = "LoAlarmMMS";

	// DpTrue
	const std::string DataPointEntityData::DATAPOINT_TRUE_ALARM_DELAY = "TrueAlarmDelay";
	const std::string DataPointEntityData::DATAPOINT_TRUE_ALARM_SEVERITY = "TrueAlarmSeverity";
	const std::string DataPointEntityData::DATAPOINT_TRUE_ALARM_MESSAGE = "TrueAlarmMessage";
	const std::string DataPointEntityData::DATAPOINT_TRUE_ALARM_MMS = "TrueAlarmMMS";

	// DpFalse
	const std::string DataPointEntityData::DATAPOINT_FALSE_ALARM_DELAY = "FalseAlarmDelay";
	const std::string DataPointEntityData::DATAPOINT_FALSE_ALARM_SEVERITY = "FalseAlarmSeverity";
	const std::string DataPointEntityData::DATAPOINT_FALSE_ALARM_MESSAGE = "FalseAlarmMessage";
	const std::string DataPointEntityData::DATAPOINT_FALSE_ALARM_MMS = "FalseAlarmMMS";

	//TD15446 add more columns from DR_DATAPOINT_STATE
	const std::string DataPointEntityData::DATAPOINT_ENTITYKEY = "EntityKey";
	const std::string DataPointEntityData::DATAPOINT_HI_ALARM_ENABLED = "HiAlarmEnabled";
	const std::string DataPointEntityData::DATAPOINT_HIHI_ALARM_ENABLED = "HiHiAlarmEnabled";
	const std::string DataPointEntityData::DATAPOINT_OH_ALARM_ENABLED = "OverScaleHiAlarmEnabled";
	const std::string DataPointEntityData::DATAPOINT_LO_ALARM_ENABLED = "LoAlarmEnabled";
	const std::string DataPointEntityData::DATAPOINT_LOLO_ALARM_ENABLED = "LoLoAlarmEnabled";
	const std::string DataPointEntityData::DATAPOINT_OL_ALARM_ENABLED = "OverScaleLoAlarmEnabled";
	const std::string DataPointEntityData::DATAPOINT_TRUE_ALARM_ENABLED = "TrueAlarmEnable";
	const std::string DataPointEntityData::DATAPOINT_FALSE_ALARM_ENABLED = "FalseAlarmEnable";
	
	// Metering
	const std::string DataPointEntityData::DATAPOINT_MI_DATAPOINT_PKEY = "InputDependency";
	const std::string DataPointEntityData::DATAPOINT_ROLLOVER_CONSTANT = "RolloverConstant";
	const std::string DataPointEntityData::DATAPOINT_METER_FACTOR = "MeterFactor";

	// MMS
	const std::string DataPointEntityData::DATAPOINT_MMS_REPORT_TYPE = "MMSReportType";
	const std::string DataPointEntityData::DATAPOINT_MMS_REPORT_PERIOD = "MMSReportPeriod";
	const std::string DataPointEntityData::DATAPOINT_MMS_REPORT_TIME = "MMSReportTime";
	const std::string DataPointEntityData::DATAPOINT_MMS_NONE_STRING = "";
	const std::string DataPointEntityData::DATAPOINT_MMS_AUTO_STRING = "A";
	const std::string DataPointEntityData::DATAPOINT_MMS_SEMI_STRING = "S";

    // Global MMS Inhibit Datapoint for agent
    const std::string DataPointEntityData::GLOBAL_MMS_INHIBITED_DATAPOINT = "GlobalMmsInhibitedDatapoint";

    const std::string DataPointEntityData::DATAPOINT_EVENT_MESSAGE = "DataPointEventMessage";

	//Using for IEC 104 Protocol
	const std::string DataPointEntityData::DATAPOINT_PSCADA_FLAG = "PScadaFlag";
	const std::string DataPointEntityData::DATAPOINT_RELATED_INPUT_DATAPOINT  = "RelatedInputDataPoint";
	const std::string DataPointEntityData::DATAPOINT_TRUE_RCC  = "True_Rcc";
	const std::string DataPointEntityData::DATAPOINT_FALSE_RCC  = "False_Rcc";
    
    const std::string DataPointEntityData::DATAPOINT_RELATED_WF_DATAPOINT = "RelatedWfDataPoint"; // Using for WF

	//Using for Special Alarm, such as FireAlarm
	const std::string DataPointEntityData::DATAPOINT_SPECIAL_ALARM_TYPE = "SpecialAlarmType";

	DataPointEntityData::DataPointEntityData()
	:
	m_firstTimeReload ( true )
    {

    }


    DataPointEntityData::DataPointEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() ),
	m_firstTimeReload ( true )
    {

    }

    DataPointEntityData::~DataPointEntityData()
    {

    }

	std::string DataPointEntityData::getType()
	{
        return getStaticType();
	}

    std::string DataPointEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    IEntityData* DataPointEntityData::clone ( unsigned long key )
    {
        return new DataPointEntityData ( key );
    }

    void DataPointEntityData::invalidate()
    {
        getHelper()->invalidate();

        //m_dataPointStateValues.clear();
    }

	bool DataPointEntityData::setParameterValues ( std::map < std::string, std::string > & parameters )
	{
		bool result = false;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s: Set parameter value %s = %s", getName().c_str(), parameters.begin()->first.c_str(), parameters.begin()->second.c_str());

		// just in case if an exception (don't care what type) is thrown
		try
		{
			// update this DataPoint entity with the new parameter value
			getHelper()->writeParameters ( parameters );
			
			// reset the valid data flag
			invalidate();

			result = true;
		}
		catch ( ... )
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"%s: failed to set parameter %s = %s", 
				getName().c_str(), parameters.begin()->first.c_str(), parameters.begin()->second.c_str());
		}

		return result;
	}

	const std::string DataPointEntityData::getDataPointType()
	{
        return getStringData ( DATAPOINT_TYPE );
	}

	bool DataPointEntityData::setDataPointType ( const std::string & newType )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_TYPE ] = newType;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getDataPointTypeShortName()
	{
        return getStringData ( DATAPOINT_TYPE_SHORT_NAME );		
	}

	bool DataPointEntityData::setDataPointTypeShortName ( const std::string & newShortName )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_TYPE_SHORT_NAME ] = newShortName;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getDataType()
	{
        return  getStringData ( DATAPOINT_DATA_TYPE );
	}

	bool DataPointEntityData::setDataType ( const std::string & newType )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_DATA_TYPE ] = newType;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getHelpURL()
	{
        return  getStringData ( DATAPOINT_HELP_URL );
	}

	bool DataPointEntityData::setHelpURL ( const std::string & newURL )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_HELP_URL ] = newURL;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getEngineeringNote()
	{
        return  getStringData ( DATAPOINT_ENGINEERING_NOTE );
	}

	bool DataPointEntityData::setEngineeringNote ( const std::string & newNote )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_ENGINEERING_NOTE ] = newNote;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getFalseAlarmMessage()
	{
        return  getStringData ( DATAPOINT_FALSE_ALARM_MESSAGE );
	}

	bool DataPointEntityData::setFalseAlarmMessage ( const std::string & newMsg )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_FALSE_ALARM_MESSAGE ] = newMsg;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getTrueAlarmMessage()
	{
		return  getStringData ( DATAPOINT_TRUE_ALARM_MESSAGE );
	}

	bool DataPointEntityData::setTrueAlarmMessage ( const std::string & newMsg )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_TRUE_ALARM_MESSAGE ] = newMsg;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getLoLoAlarmMessage()
	{
		return getStringData ( DATAPOINT_LOLO_ALARM_MESSAGE );
	}

	bool DataPointEntityData::setLoLoAlarmMessage ( const std::string & newMsg )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_LOLO_ALARM_MESSAGE ] = newMsg;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getLoAlarmMessage()
	{
		return getStringData ( DATAPOINT_LO_ALARM_MESSAGE );
	}

	bool DataPointEntityData::setLoAlarmMessage ( const std::string & newMsg )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_LO_ALARM_MESSAGE ] = newMsg;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getHiAlarmMessage()
	{
		return getStringData ( DATAPOINT_HI_ALARM_MESSAGE );
	}

	bool DataPointEntityData::setHiAlarmMessage ( const std::string & newMsg )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_HI_ALARM_MESSAGE ] = newMsg;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getHiHiAlarmMessage()
	{
		return getStringData ( DATAPOINT_HIHI_ALARM_MESSAGE );
	}

	bool DataPointEntityData::setHiHiAlarmMessage ( const std::string & newMsg )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_HIHI_ALARM_MESSAGE ] = newMsg;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getEngineeringUnits()
	{
		return getStringData ( DATAPOINT_ENGINEERING_UNITS );
	}

	bool DataPointEntityData::setEngineeringUnits ( const std::string & newEngUnit )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_ENGINEERING_UNITS ] = newEngUnit;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getTextEngineeringUnit()
	{
		return getStringData ( DATAPOINT_TEXT_ENGINEERING_UNIT );
	}

	bool DataPointEntityData::setTextEngineeringUnit ( const std::string & newEngUnit )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_TEXT_ENGINEERING_UNIT ] = newEngUnit;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getFalseLabel()
	{
		return getStringData ( DATAPOINT_FALSE_ENGINEERING_UNIT );
	}

	bool DataPointEntityData::setFalseLabel ( const std::string & newEngUnit )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_FALSE_ENGINEERING_UNIT ] = newEngUnit;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getTrueLabel()
	{
		return getStringData ( DATAPOINT_TRUE_ENGINEERING_UNIT );
	}

	const std::string DataPointEntityData::getSpecialAlarmType()
	{
		return getStringData ( DATAPOINT_SPECIAL_ALARM_TYPE );
	}
	bool DataPointEntityData::setTrueLabel ( const std::string & newEngUnit )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_TRUE_ENGINEERING_UNIT ] = newEngUnit;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getViewAssociation()
	{
		return getStringData ( DATAPOINT_VIEW_ASSOCIATION );
	}

	bool DataPointEntityData::setViewAssociation ( const std::string & newViewAssociation )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_VIEW_ASSOCIATION ] = newViewAssociation;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getEntityAssociation()
	{
		return getStringData ( DATAPOINT_ENTITY_ASSOCIATION );
	}

	bool DataPointEntityData::setEntityAssociation ( const std::string & newEntityAssociation )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_ENTITY_ASSOCIATION ] = newEntityAssociation;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getLaunchExpression()
	{
		return getStringData ( DATAPOINT_LAUNCH_CONDITION );
	}

	bool DataPointEntityData::setLaunchExpression ( const std::string & newExpression )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_LAUNCH_CONDITION ] = newExpression;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getCalculatedExpression()
	{
		return getStringData ( DATAPOINT_CALCULATED_EXPRESSION );
	}

	bool DataPointEntityData::setCalculatedExpression ( const std::string & newExpression )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_CALCULATED_EXPRESSION ] = newExpression;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getABEExpression()
	{
		return getStringData ( DATAPOINT_ABE_EXPRESSION );
	}

	const std::string DataPointEntityData::getTrueRccExpression()
	{
		return getStringData ( DATAPOINT_TRUE_RCC );
	}

	const std::string DataPointEntityData::getFalseRccExpression()
	{
		return getStringData ( DATAPOINT_FALSE_RCC );
	}


	bool DataPointEntityData::setABEExpression ( const std::string & newExpression )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_ABE_EXPRESSION ] = newExpression;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getTrueAlarmMMS()
	{
		return getStringData ( DATAPOINT_TRUE_ALARM_MMS );
	}

	bool DataPointEntityData::setTrueAlarmMMS ( const std::string & newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_TRUE_ALARM_MMS ] = newValue;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getFalseAlarmMMS()
	{
		return getStringData ( DATAPOINT_FALSE_ALARM_MMS );
	}

	bool DataPointEntityData::setFalseAlarmMMS ( const std::string & newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_FALSE_ALARM_MMS ] = newValue;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getLoAlarmMMS()
	{
		return  getStringData ( DATAPOINT_LO_ALARM_MMS );
	}

	bool DataPointEntityData::setLoAlarmMMS ( const std::string & newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_LO_ALARM_MMS ] = newValue;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getLoLoAlarmMMS()
	{
		return getStringData ( DATAPOINT_LOLO_ALARM_MMS );
	}

	bool DataPointEntityData::setLoLoAlarmMMS ( const std::string & newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_LOLO_ALARM_MMS ] = newValue;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getHiAlarmMMS()
	{
		return getStringData ( DATAPOINT_HI_ALARM_MMS );
	}

	bool DataPointEntityData::setHiAlarmMMS ( const std::string & newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_HI_ALARM_MMS ] = newValue;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getHiHiAlarmMMS()
	{
		return getStringData ( DATAPOINT_HIHI_ALARM_MMS );
	}

	bool DataPointEntityData::setHiHiAlarmMMS ( const std::string & newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_HIHI_ALARM_MMS ] = newValue;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getPrecision()
	{
		return getIntegerData ( DATAPOINT_PRECISION );
	}

	bool DataPointEntityData::setPrecision ( int newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_PRECISION ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getUpdateTimeLimit()
	{
		return getIntegerData ( DATAPOINT_UPDATE_TIME_LIMIT );
	}

	bool DataPointEntityData::setUpdateTimeLimit ( int newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_UPDATE_TIME_LIMIT ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getABETimeoutPeriodSeconds()
	{
		return getIntegerData ( DATAPOINT_ABE_TIMEOUT_PERIOD_SECONDS );
	}

	bool DataPointEntityData::setABETimeoutPeriodSeconds ( int newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_ABE_TIMEOUT_PERIOD_SECONDS ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getMaxTextLength()
	{
		return getIntegerData ( DATAPOINT_MAX_TEXT_LENGTH );
	}

	bool DataPointEntityData::setMaxTextLength ( int newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_MAX_TEXT_LENGTH ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned int DataPointEntityData::getScanTimeFactor()
	{
		return getIntegerData ( DATAPOINT_SCAN_TIME_FACTOR );
	}

	bool DataPointEntityData::setScanTimeFactor ( unsigned int newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%u", newValue );
		parameters [ DATAPOINT_SCAN_TIME_FACTOR ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned long DataPointEntityData::getFalseAlarmDelay()
	{
		return getUnsignedLongData ( DATAPOINT_FALSE_ALARM_DELAY );
	}

	bool DataPointEntityData::setFalseAlarmDelay ( unsigned long newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%lu", newValue );
		parameters [ DATAPOINT_FALSE_ALARM_DELAY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned long DataPointEntityData::getTrueAlarmDelay()
	{
		return getUnsignedLongData ( DATAPOINT_TRUE_ALARM_DELAY );
	}

	bool DataPointEntityData::setTrueAlarmDelay ( unsigned long newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%lu", newValue );
		parameters [ DATAPOINT_TRUE_ALARM_DELAY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned long DataPointEntityData::getLoLoAlarmDelay()
	{
		return getUnsignedLongData ( DATAPOINT_LOLO_ALARM_DELAY );
	}

	bool DataPointEntityData::setLoLoAlarmDelay ( unsigned long newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%lu", newValue );
		parameters [ DATAPOINT_LOLO_ALARM_DELAY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned long DataPointEntityData::getLoAlarmDelay()
	{
		return getUnsignedLongData ( DATAPOINT_LO_ALARM_DELAY );
	}

	bool DataPointEntityData::setLoAlarmDelay ( unsigned long newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%lu", newValue );
		parameters [ DATAPOINT_LO_ALARM_DELAY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned long DataPointEntityData::getHiAlarmDelay()
	{
		return getUnsignedLongData ( DATAPOINT_HI_ALARM_DELAY );
	}

	bool DataPointEntityData::setHiAlarmDelay ( unsigned long newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%lu", newValue );
		parameters [ DATAPOINT_HI_ALARM_DELAY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned long DataPointEntityData::getHiHiAlarmDelay()
	{
		return getUnsignedLongData ( DATAPOINT_HIHI_ALARM_DELAY );
	}

	bool DataPointEntityData::setHiHiAlarmDelay ( unsigned long newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%lu", newValue );
		parameters [ DATAPOINT_HIHI_ALARM_DELAY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getFalseAlarmSeverity()
	{
		return getIntegerData ( DATAPOINT_FALSE_ALARM_SEVERITY );
	}

	bool DataPointEntityData::setFalseAlarmSeverity ( int newValue )
	{
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_FALSE_ALARM_SEVERITY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getTrueAlarmSeverity()
	{
		return getIntegerData ( DATAPOINT_TRUE_ALARM_SEVERITY );
	}

	bool DataPointEntityData::setTrueAlarmSeverity ( int newValue )
	{
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_TRUE_ALARM_SEVERITY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getLoLoAlarmSeverity()
	{
		return getIntegerData ( DATAPOINT_LOLO_ALARM_SEVERITY );
	}

	bool DataPointEntityData::setLoLoAlarmSeverity ( int newValue )
	{
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_LOLO_ALARM_SEVERITY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getLoAlarmSeverity()
	{
		return getIntegerData ( DATAPOINT_LO_ALARM_SEVERITY );
	}

	bool DataPointEntityData::setLoAlarmSeverity ( int newValue )
	{
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_LO_ALARM_SEVERITY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getHiAlarmSeverity()
	{
		return getIntegerData ( DATAPOINT_HI_ALARM_SEVERITY );
	}

	bool DataPointEntityData::setHiAlarmSeverity ( int newValue )
	{
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_HI_ALARM_SEVERITY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getHiHiAlarmSeverity()
	{
		return getIntegerData ( DATAPOINT_HIHI_ALARM_SEVERITY );
	}

	bool DataPointEntityData::setHiHiAlarmSeverity ( int newValue )
	{
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_HIHI_ALARM_SEVERITY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	double DataPointEntityData::getEngineeringLimitHigh()
	{
		return getRealData ( DATAPOINT_ENGINEERING_LIMIT_HIGH );
	}

	bool DataPointEntityData::setEngineeringLimitHigh ( double newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%f", newValue );
		parameters [ DATAPOINT_ENGINEERING_LIMIT_HIGH ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	double DataPointEntityData::getEngineeringLimitLow()
	{
        return getRealData ( DATAPOINT_ENGINEERING_LIMIT_LOW );
	}

	bool DataPointEntityData::setEngineeringLimitLow ( double newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%f", newValue );
		parameters [ DATAPOINT_ENGINEERING_LIMIT_LOW ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	double DataPointEntityData::getRawLimitHigh()
	{
        return getRealData ( DATAPOINT_RAW_LIMIT_HIGH );
	}

	bool DataPointEntityData::setRawLimitHigh ( double newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%f", newValue );
		parameters [ DATAPOINT_RAW_LIMIT_HIGH ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	double DataPointEntityData::getRawLimitLow()
	{
        return getRealData ( DATAPOINT_RAW_LIMIT_LOW );
	}

	bool DataPointEntityData::setRawLimitLow ( double newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%f", newValue );
		parameters [ DATAPOINT_RAW_LIMIT_LOW ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	double DataPointEntityData::getZeroValueThreshold()
	{
        return getRealData ( DATAPOINT_ZERO_VALUE_THRESHOLD );
	}

	bool DataPointEntityData::setZeroValueThreshold ( double newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%f", newValue );
		parameters [ DATAPOINT_ZERO_VALUE_THRESHOLD ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	double DataPointEntityData::getUpdateThreshold()
	{
        return getRealData ( DATAPOINT_UPDATE_THRESHOLD );
	}

	bool DataPointEntityData::setUpdateThreshold ( double newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%f", newValue );
		parameters [ DATAPOINT_UPDATE_THRESHOLD ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	double DataPointEntityData::getAlarmThreshold()
	{
        return getRealData ( DATAPOINT_ALARM_THRESHOLD );
	}

	bool DataPointEntityData::setAlarmThreshold ( double newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%f", newValue );
		parameters [ DATAPOINT_ALARM_THRESHOLD ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	bool DataPointEntityData::getEnableHelpURL()
	{
        return getBooleanData ( DATAPOINT_ENABLE_HELP_URL );
	}

	bool DataPointEntityData::setEnableHelpURL ( bool newIndication )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_ENABLE_HELP_URL ] = "0";

		if ( true == newIndication )
		{
			parameters [ DATAPOINT_ENABLE_HELP_URL ] = "1";
		}

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	bool DataPointEntityData::getEnableEngineeringNote()
	{
        return getBooleanData ( DATAPOINT_ENABLE_ENGINEERING_NOTE );
	}

	bool DataPointEntityData::setEnableEngineeringNote ( bool newIndication )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_ENABLE_ENGINEERING_NOTE ] = "0";

		if ( true == newIndication )
		{
			parameters [ DATAPOINT_ENABLE_ENGINEERING_NOTE ] = "1";
		}

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	bool DataPointEntityData::getEnableTrending()
	{
        return getBooleanData ( DATAPOINT_ENABLE_TRENDING );
	}

	bool DataPointEntityData::setEnableTrending ( bool newIndication )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_ENABLE_TRENDING ] = "0";

		if ( true == newIndication )
		{
			parameters [ DATAPOINT_ENABLE_TRENDING ] = "1";
		}

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	bool DataPointEntityData::getEngineeringUnitsEnabled()
	{
        return getBooleanData ( DATAPOINT_ENGINEERING_UNITS_ENABLED );
	}

	bool DataPointEntityData::setEngineeringUnitsEnabled ( bool newIndication )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_ENGINEERING_UNITS_ENABLED ] = "0";

		if ( true == newIndication )
		{
			parameters [ DATAPOINT_ENGINEERING_UNITS_ENABLED ] = "1";
		}

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	bool DataPointEntityData::getAllowWrite()
	{
        return getBooleanData ( DATAPOINT_ALLOW_WRITE );
	}

	bool DataPointEntityData::setAllowWrite ( bool newIndication )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_ALLOW_WRITE ] = "0";

		if ( true == newIndication )
		{
			parameters [ DATAPOINT_ALLOW_WRITE ] = "1";
		}

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	long DataPointEntityData::getDisplayOrder()
	{
        return getIntegerData ( DATAPOINT_DISPLAY_ORDER );
	}

	bool DataPointEntityData::setDisplayOrder ( long newOrder )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%li", newOrder );
		parameters [ DATAPOINT_DISPLAY_ORDER ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	long DataPointEntityData::getBitWidth()
	{
		return getIntegerData ( DATAPOINT_BIT_WIDTH );
	}

	bool DataPointEntityData::setBitWidth ( long bitWidth )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = "";
		sprintf ( temp, "%li", bitWidth );
		parameters [ DATAPOINT_BIT_WIDTH ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	bool DataPointEntityData::getIsBigEndian()
	{
		std::string paraValue = getStringData ( DATAPOINT_BIT_SEQUENCE );
		 if ( ( "BigEndian" == paraValue ) || ( "BIGENDIAN" == paraValue)) 
		 {
			 return true;
		 }
		 return false;
	}
	
	bool DataPointEntityData::setIsBigEndian(bool isBigEndian)
	{
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_BIT_SEQUENCE ] = "BigEndian";

		if ( false == isBigEndian )
		{
			parameters [ DATAPOINT_BIT_SEQUENCE ] = "LittleEndian";
		}
		return setParameterValues ( parameters );
	}
	long DataPointEntityData::getUpdatePriorityLevel()
	{
        if ( ( "HIGH" == getStringData ( DATAPOINT_UPDATE_PRIORITY ) ) ||
			 ( "High" == getStringData ( DATAPOINT_UPDATE_PRIORITY ) ) )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	bool DataPointEntityData::setUpdatePriorityLevel ( long newPriority )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_UPDATE_PRIORITY ] = "LOW";

		if ( 1 == newPriority )
		{
			parameters [ DATAPOINT_UPDATE_PRIORITY ] = "HIGH";
		}
		// else do nothing

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	bool DataPointEntityData::getLogChanges()
	{
        return getBooleanData ( DATAPOINT_LOG_CHANGES );
	}

	bool DataPointEntityData::setLogChanges ( bool newIndication )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_LOG_CHANGES ] = "0";

		if ( true == newIndication )
		{
			parameters [ DATAPOINT_LOG_CHANGES ] = "1";
		}

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	//
    bool DataPointEntityData::getWordSwapIndication()
    {
        return getBooleanData ( DATAPOINT_WORD_SWAP );
    }
    
    bool DataPointEntityData::getByteSwapIndication()
    {
        return getBooleanData ( DATAPOINT_BYTE_SWAP );
    }
    
	bool DataPointEntityData::setWordSwapIndication ( bool newIndication )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_WORD_SWAP ] = "0";

		if ( true == newIndication )
		{
			parameters [ DATAPOINT_WORD_SWAP ] = "1";
		}

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

    bool DataPointEntityData::setByteSwapIndication ( bool newIndication )
    {
        // set up the new parameter value to be written
        std::map < std::string, std::string > parameters;
        parameters [ DATAPOINT_BYTE_SWAP ] = "0";
        
        if ( true == newIndication )
        {
            parameters [ DATAPOINT_BYTE_SWAP ] = "1";
        }
        
        // update this DataPoint entity with the new parameter value
        return setParameterValues ( parameters );
    }
    
	double DataPointEntityData::getRolloverConstant()
	{
        return getRealData ( DATAPOINT_ROLLOVER_CONSTANT );
	}

	bool DataPointEntityData::setRolloverConstant ( double newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%f", newValue );
		parameters [ DATAPOINT_ROLLOVER_CONSTANT ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	double DataPointEntityData::getMeterFactor()
	{
        return getRealData ( DATAPOINT_METER_FACTOR );
	}

	bool DataPointEntityData::setMeterFactor ( double newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%f", newValue );
		parameters [ DATAPOINT_METER_FACTOR ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned long DataPointEntityData::getMIDataPointPKey()
	{
       	return getUnsignedLongData ( DATAPOINT_MI_DATAPOINT_PKEY );
	}

	bool DataPointEntityData::setMIDataPointPKey ( unsigned long newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%lu", newValue );
		parameters [ DATAPOINT_MI_DATAPOINT_PKEY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getOMFlag()
	{
		std::string OMFlag = getStringData ( DATAPOINT_OMFLAG );

		// if not defined or has been set to empty string
		if ( true == OMFlag.empty() )
		{
			// default to "N", meaning "none" for OMF flag
			OMFlag = "N";
		}

		return OMFlag;
	}


	bool DataPointEntityData::setOMFlag ( const std::string & newName )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_OMFLAG ] = newName;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	bool DataPointEntityData::isNumericalString ( const std::string & str )
	{
		bool isNumericalstring = true;

		std::string compactedStr = removeSpaces ( str );

		// for each of the character in the specified string
		for ( unsigned int currentPos = 0; currentPos < compactedStr.length(); currentPos++ )
		{
			// if the character is within the range A-Z or a-z
			if ( 0 != isalpha ( compactedStr [ currentPos ] ) )
			{
				// modify the flag and exit the loop.  No need to go further
				isNumericalstring = false;
				break;
			}
		}

		return isNumericalstring;
	}


	std::string DataPointEntityData::removeSpaces ( const std::string & originalStr )
	{
		std::string modifiedStr = "";

		// for each of the character in the specified string
		for ( unsigned int currentPos = 0; currentPos < originalStr.length(); currentPos++ )
		{
			// if the character is not a white-space character (0x09 - 0x0D or 0x20)
			if ( 0 == isspace ( originalStr [ currentPos ] ) )
			{
				// append the character to the end of the modified string
				modifiedStr += originalStr [ currentPos ];
			}
		}

		return modifiedStr;
	}

	std::string DataPointEntityData::getStringData ( const std::string & parameterName )
	{
		std::string data ( "" );

		try
		{
			data = getHelper()->getParameter( parameterName );
		}
		catch ( ... )
		{
			// do nothing
		}

		return data;
	}


	bool DataPointEntityData::getBooleanData ( const std::string & parameterName )
	{
		bool data = false;

        try
		{
    		data = EntityHelper::getBooleanData ( getHelper()->getParameter( parameterName ) );
		}
        catch( const DataException& e )
		{
            // quieten down the logs

            if (e.getFailType() == DataException::NO_VALUE)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                            "%s: failed to retrieve the value of missing parameter %s", 
                            getName().c_str(), parameterName.c_str());
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                            "%s: failed to retrieve the value of parameter %s. %s", 
                            getName().c_str(), parameterName.c_str(), e.what());
            }
		}
		catch ( ... )
		{
			// do nothing
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "%s: failed to retrieve the value of parameter %s", 
                        getName().c_str(), parameterName.c_str());
		}

		return data;
	}


	int DataPointEntityData::getIntegerData ( const std::string & parameterName )
	{
		int data = 0;

		try
		{
			data = EntityHelper::getIntegerData ( getHelper()->getParameter( parameterName ) );
		}
		catch( const DataException& e )
		{
            // quieten down the logs

            if (e.getFailType() == DataException::NO_VALUE)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                            "%s: failed to retrieve the value of missing parameter %s", 
                            getName().c_str(), parameterName.c_str());
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                            "%s: failed to retrieve the value of parameter %s. %s", 
                            getName().c_str(), parameterName.c_str(), e.what());
            }
		}
		catch ( ... )
		{
			// do nothing
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "%s: failed to retrieve the value of parameter %s", 
                        getName().c_str(), parameterName.c_str());
		}

		return data;
	}


	unsigned long DataPointEntityData::getUnsignedLongData ( const std::string & parameterName )
	{
		unsigned long data = 0;

		try
		{
			data = EntityHelper::getUnsignedLongData ( getHelper()->getParameter( parameterName ) );
		}
        catch( const DataException& e )
		{
            // quieten down the logs

            if (e.getFailType() == DataException::NO_VALUE)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                            "%s: failed to retrieve the value of missing parameter %s", 
                            getName().c_str(), parameterName.c_str());
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                            "%s: failed to retrieve the value of parameter %s. %s", 
                            getName().c_str(), parameterName.c_str(), e.what());
            }
		}
		catch ( ... )
		{
			// do nothing
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "%s: failed to retrieve the value of parameter %s", 
                        getName().c_str(), parameterName.c_str());
		}

		return data;
	}


	double DataPointEntityData::getRealData ( const std::string & parameterName )
	{
		double data = 0;

    	try
		{
            data = EntityHelper::getRealData ( getHelper()->getParameter( parameterName ) );
		}
        catch( const DataException& e )
		{
            // quieten down the logs

            if (e.getFailType() == DataException::NO_VALUE)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                            "%s: failed to retrieve the value of missing parameter %s", 
                            getName().c_str(), parameterName.c_str());
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                            "%s: failed to retrieve the value of parameter %s. %s", 
                            getName().c_str(), parameterName.c_str(), e.what());
            }
		}
		catch ( ... )
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "%s: failed to retrieve the value of parameter %s", 
                        getName().c_str(), parameterName.c_str());
		}

		return data;
	}

	unsigned long DataPointEntityData::getOverScaleHiAlarmDelay()
	{
		return getUnsignedLongData ( DATAPOINT_OH_ALARM_DELAY );
	}

	bool DataPointEntityData::setOverScaleHiAlarmDelay ( unsigned long newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%lu", newValue );
		parameters [ DATAPOINT_OH_ALARM_DELAY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getOverScaleHiAlarmSeverity()
	{
		return getIntegerData ( DATAPOINT_OH_ALARM_SEVERITY );
	}

	bool DataPointEntityData::setOverScaleHiAlarmSeverity ( int newValue )
	{
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_OH_ALARM_SEVERITY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getOverScaleHiAlarmMessage()
	{
		return getStringData ( DATAPOINT_OH_ALARM_MESSAGE );
	}

	bool DataPointEntityData::setOverScaleHiAlarmMessage ( const std::string & newMsg )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_OH_ALARM_MESSAGE ] = newMsg;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getOverScaleHiAlarmMMS()
	{
		return getStringData ( DATAPOINT_OH_ALARM_MMS );
	}

	bool DataPointEntityData::setOverScaleHiAlarmMMS ( const std::string & newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_OH_ALARM_MMS ] = newValue;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned long DataPointEntityData::getOverScaleLoAlarmDelay()
	{
		return getUnsignedLongData ( DATAPOINT_OL_ALARM_DELAY );
	}

	bool DataPointEntityData::setOverScaleLoAlarmDelay ( unsigned long newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%lu", newValue );
		parameters [ DATAPOINT_OL_ALARM_DELAY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataPointEntityData::getOverScaleLoAlarmSeverity()
	{
		return getIntegerData ( DATAPOINT_OL_ALARM_SEVERITY );
	}

	bool DataPointEntityData::setOverScaleLoAlarmSeverity ( int newValue )
	{
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", newValue );
		parameters [ DATAPOINT_OL_ALARM_SEVERITY ] = temp;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getOverScaleLoAlarmMessage()
	{
		return getStringData ( DATAPOINT_OL_ALARM_MESSAGE );		
	}

	bool DataPointEntityData::setOverScaleLoAlarmMessage ( const std::string & newMsg )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_OL_ALARM_MESSAGE ] = newMsg;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string DataPointEntityData::getOverScaleLoAlarmMMS()
	{
		return getStringData ( DATAPOINT_OL_ALARM_MMS );
	}

	bool DataPointEntityData::setOverScaleLoAlarmMMS ( const std::string & newValue )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_OL_ALARM_MMS ] = newValue;

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}


	bool DataPointEntityData::getIsDpParameterCreatedInDb ( const std::string & parameterName ) 
	{
		bool ret  = false;

		std::string temp = getHelper()->getParameter( parameterName );
		if( temp != "" )
		{
			ret = true;
		}
		return ret;
	}
	
	bool DataPointEntityData::getPScadaFlag()
	{
		return getBooleanData ( DATAPOINT_PSCADA_FLAG );
	}


	bool DataPointEntityData::setPScadaFlag ( bool newPScadaFlag )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATAPOINT_PSCADA_FLAG ] = "0";

		if ( true == newPScadaFlag )
		{
			parameters [ DATAPOINT_PSCADA_FLAG ] = "1";
		}

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}

	unsigned long DataPointEntityData::getRelatedInputPoint()
	{
		return getUnsignedLongData ( DATAPOINT_RELATED_INPUT_DATAPOINT );
	}

	std::string DataPointEntityData::getEventMessage()
	{
		return getStringData(DATAPOINT_EVENT_MESSAGE);
	}
    
    // Get Wu Fang data point
	unsigned long DataPointEntityData::getRelatedWfPoint()
	{
		return getUnsignedLongData( DATAPOINT_RELATED_WF_DATAPOINT );
	}
} //close namespace TA_Base_Core
