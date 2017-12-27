/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/IDeviceCommand.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_I_DEVICE_COMMAND_H_
#define SCADA_I_DEVICE_COMMAND_H_

#include <string>

namespace TA_Base_Bus
{

	/** Interface for sending command.
	 * all protocol device write commands should derive from this interface.
     *  
     */
	class IDeviceCommand
	{
	public:
		virtual ~IDeviceCommand(){};
		
		/**
         * get the command type name.
         *
         * @return std::string: command type name.
         */
		virtual std::string getStaticType() = 0;

	};

};

#endif
