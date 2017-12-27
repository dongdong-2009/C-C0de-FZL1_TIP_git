/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPCommand.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__SNMP_COMMAND_H_
#define SNMP__SNMP_COMMAND_H_


#include <string>
#include <vector>

#include "SNMPData.h"
#include "bus/scada/io_manager/src/IDeviceCommand.h"

namespace TA_Base_Bus
{
	class DataPoint;
	class DataPointWriteRequest;
    class IModbus;
}

namespace TA_Base_Core
{
    class AuditMessageSender;
    class AlarmHelper;
    class MessageType;
}

namespace TA_Base_Bus
{
	/** define the SNMP protocol Command.
     *  implement the IDeviceCommand interface.
     */
	class SNMPCommand : public TA_Base_Bus::IDeviceCommand
	{
	public:

		SNMPCommand (){};
		virtual ~SNMPCommand(){};
		virtual std::string getStaticType(){return "SNMPCommand";};
	
		// operations
	private:
		SNMPCommand( const SNMPCommand & obj );
		void operator= ( const SNMPCommand & rhs );
		// attributes
	public:
		std::string oid;
		Vb			vb;

	};
}
#endif
