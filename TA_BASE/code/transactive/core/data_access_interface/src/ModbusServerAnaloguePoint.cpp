/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/src/ModbusServerAnaloguePoint.cpp $
  * @author:  Dhanshri 
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/05/05 18:05:34 $
  * Last modified by:  $Author: dhanshri $
  * 
  * ModbusServerAnaloguePoint is an implementation of IModbusServerAnaloguePoint. It holds the data specific to an ModbusServerAnaloguePoint entry
  * in the database, and allows read-only access to that data.
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/ModbusServerAnaloguePoint.h"

namespace TA_Base_Core
{
	const std::string	ModbusServerAnaloguePoint::ENTITY_KEY			= "ENTITY_KEY";
	const std::string	ModbusServerAnaloguePoint::ENGINEERINGLIMITHIGH	= "ENGINEERINGLIMITHIGH";
	const std::string	ModbusServerAnaloguePoint::ENGINEERINGLIMITLOW	= "ENGINEERINGLIMITLOW";
	const std::string	ModbusServerAnaloguePoint::RAWLIMITHIGH			= "RAWLIMITHIGH";
	const std::string	ModbusServerAnaloguePoint::RAWLIMITLOW			= "RAWLIMITLOW";
	//const std::string	ModbusServerAnaloguePoint::ENGGCONSTANT			= "ENGGCONSTANT";
	const std::string	ModbusServerAnaloguePoint::PRECISION			= "PRECISION";
	
	ModbusServerAnaloguePoint::ModbusServerAnaloguePoint(const unsigned long row, TA_Base_Core::IData& data)
	{
        m_entityKey = data.getUnsignedLongData(row, ENTITY_KEY);
        m_EngineeringLimitHigh = data.getRealData( row, ENGINEERINGLIMITHIGH );
		m_EngineeringLimitLow = data.getRealData( row, ENGINEERINGLIMITLOW);
		m_RawLimitHigh = data.getRealData(row, RAWLIMITHIGH);
		m_RawLimitLow = data.getRealData( row, RAWLIMITLOW );
		m_Precision = data.getIntegerData( row, PRECISION );
		m_EnggConstant = (m_EngineeringLimitHigh - m_EngineeringLimitLow)/(m_RawLimitHigh - m_RawLimitLow);
	}

	ModbusServerAnaloguePoint::~ModbusServerAnaloguePoint()
	{
	}

    void ModbusServerAnaloguePoint::invalidate()
    {
    }

} // closes TA_Base_Core
