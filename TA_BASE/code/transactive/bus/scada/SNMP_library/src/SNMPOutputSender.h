/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPOutputSender.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__SNMP_OUTPUT_SENDER_H_
#define SNMP__SNMP_OUTPUT_SENDER_H_


#include "bus/scada/io_manager/src/ProtocolSender.h"

#include "SNMPCommand.h"

namespace TA_Base_Bus
{
    class DataPointWriteRequest;
	class DataPoint;	
}

namespace TA_Base_Bus
{
	/** this class construct IDeviceCommand.
	 * derived from ProtocolSender.
     *  
     */
	class SNMPOutputSender: public ProtocolSender
	{
    public:

		/** constructor.
         *
         * @param name: object name.
		 * @param defaultDevice: if there is a ProtocolDevice to sender IDeviceCommand, then set it, otherwise NULL.
         */
		SNMPOutputSender (std::string name, IProtocolDevice* defaultDevice = 0);

		virtual ~SNMPOutputSender();

		virtual bool isDataPointMatched(DataPoint * dp );

		IDeviceCommand* constructDeviceCommand (DataPointWriteRequest & newWrite );


	private:
        // disabled constructor
		SNMPOutputSender();
		SNMPOutputSender ( const SNMPOutputSender & obj );
		SNMPOutputSender & operator= ( const SNMPOutputSender & rhs );

    // attributes
	protected:
    };
}

#endif
