/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolSender.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_PROTOCOL_SENDER_H_
#define SCADA_PROTOCOL_SENDER_H_

#include "ProtocolIOBase.h"
#include "IProtocolDevice.h"

#include "IDeviceCommand.h"

namespace TA_Base_Bus
{
	/** this class use ProtocolDevice to send DataPointWriteRequest and update DataPoint control state.
	 * derived from ProtocolIOBase.
     *  
     */
	class ProtocolSender : public ProtocolIOBase
	{
	public:
		/** constructor.
         *
         * @param name: object name.
		 * @param defaultDevice: if there is a ProtocolDevice to sender IDeviceCommand, then set it, otherwise NULL.
         */
		ProtocolSender(std::string name, IProtocolDevice* defaultDevice = 0);
		virtual ~ProtocolSender();

		/** construct the specialised device command for ProtocolDevice writing value out.
         *
         * @param writeRequest: DataPointWriteRequest, construct IDeviceCommand from this request.
		 * @return IDeviceCommand*: new IDeviceCommand* or NULL.
         */
		virtual IDeviceCommand* constructDeviceCommand(DataPointWriteRequest& writeRequest) = 0;

		virtual bool isDataPointMatched(DataPoint*) = 0;
		virtual void finishedAddDataPoint(){};

		virtual void setOperationMode(TA_Base_Core::EOperationMode mode){};


		virtual bool processWriteRequest(DataPointWriteRequest&);

	protected:

		IProtocolDevice*	m_defaultDevice;
		///< if this value is not NULL, then using this ProtocolDevice to send write command,
		///< else get ProtocolDevice from ProtocolManager.
	};
	
};

#endif
