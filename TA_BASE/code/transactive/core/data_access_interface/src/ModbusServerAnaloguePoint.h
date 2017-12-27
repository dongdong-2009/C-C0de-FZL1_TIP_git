
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/src/ModbusServerAnaloguePoint.h $
  * @author:  dhanshri
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/05/05 18:05:34 $
  * Last modified by:  $Author: dhanshri $
  * 
  * Table - ANALOGUE_DATAPOINT_CONFIG
  * Columns - ENTITY_KEY,EngineeringLimitHigh,EngineeringLimitLow,RawLimitHigh,RawLimitLow,Precision
  *
  */
#if !defined(DAI_MODBUSSERVERANALOGUEPOINT_H)
#define DAI_MODBUSSERVERANALOGUEPOINT_H

#include "core/data_access_interface/src/IModbusServerAnaloguePoint.h"

namespace TA_Base_Core
{
	class IData;

    class ModbusServerAnaloguePoint : public IModbusServerAnaloguePoint
    {

    public:
		ModbusServerAnaloguePoint(const unsigned long row, TA_Base_Core::IData& data);
		~ModbusServerAnaloguePoint();
        virtual void invalidate();
        /**
         * Refer to IModbusServerAnaloguePoint.h for a description of this method.
         */
		double getEngineeringLimitHigh() {return m_EngineeringLimitHigh;};
		double getEngineeringLimitLow()  {return m_EngineeringLimitLow;};
		double getRawLimitHigh()  {return m_RawLimitHigh;};
		double getRawLimitLow() {return m_RawLimitLow;};
		double getEnggConstant() {return m_EnggConstant;};
		int getPrecision()  {return m_Precision;};

		unsigned long getKey() {return m_entityKey;};
		std::string getName(){return 	m_name;};

	public:
		static const std::string	ENTITY_KEY;
		static const std::string	ENGINEERINGLIMITHIGH;
		static const std::string	ENGINEERINGLIMITLOW;
		static const std::string	RAWLIMITHIGH;
		static const std::string	RAWLIMITLOW;
		static const std::string	PRECISION;
		//static const std::string	ENGGCONSTANT;

	private:

        // Assignment operator not used so it is made private
        ModbusServerAnaloguePoint( const ModbusServerAnaloguePoint&);            
		ModbusServerAnaloguePoint& operator=(const ModbusServerAnaloguePoint&);

        unsigned long	m_entityKey;
		double	m_EngineeringLimitHigh, m_EngineeringLimitLow, m_RawLimitHigh, m_RawLimitLow, m_EnggConstant;
		int m_Precision;
		std::string		m_name;

    };
} // closes TA_Base_Core

#endif // !defined(DAI_MODBUSSERVERANALOGUEPOINT_H)
