/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/src/IModbusServerAnaloguePoint.h $
  * @author:  dhanshri
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/05/05 18:05:34 $
  * Last modified by:  $Author: dhanshri $
  * 
  * IModbusServerAnaloguePoint is an interface to a ModbusServerAnaloguePoint object.
  */

#if !defined(DAI_IMODBUSSERVERANALOGUEPOINT_H)
#define DAI_IMODBUSSERVERANALOGUEPOINT_H

#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IModbusServerAnaloguePoint : public virtual IItem
    {

    public:
        virtual ~IModbusServerAnaloguePoint() { };
        virtual void invalidate() =0;

		virtual unsigned long getKey() = 0;
		virtual std::string getName() = 0;
		virtual double getEngineeringLimitHigh() = 0;
		virtual double getEngineeringLimitLow() = 0;
		virtual double getRawLimitHigh() = 0;
		virtual double getRawLimitLow() = 0;
		virtual double getEnggConstant() = 0;
		virtual int getPrecision() = 0;
	};
} //close namespace TA_Base_Core

#endif // !defined(DAI_IMODBUSSERVERANALOGUEPOINT_H)
