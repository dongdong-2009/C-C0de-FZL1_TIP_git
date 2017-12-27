/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/IDeviceData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_I_DEVICE_DATA_H_
#define SCADA_I_DEVICE_DATA_H_

#include <string>

namespace TA_Base_Bus
{

	/** Interface for all kinds of device data types.
     *  
     */
	class IDeviceData
	{
	public:
		virtual ~IDeviceData(){};

		/**
         * get the device data type name.
         *
         * @return std::string: device data type name.
         */
		virtual std::string getStaticType() = 0;

	};

};

#endif
