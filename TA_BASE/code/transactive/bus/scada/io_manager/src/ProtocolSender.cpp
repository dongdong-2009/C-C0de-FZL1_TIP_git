/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolSender.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "ProtocolSender.h"

#include "core/data_access_interface/src/SessionAccessFactory.h"

using TA_Base_Core::DebugUtil;

namespace 
{
	//todo delete this function.
    bool isSystemSessionID( const std::string& sessionID )//limin++, SystemSession should not access db
    {
        bool ret = ( TA_Base_Core::SessionAccessFactory::getSuperSessionId() == sessionID ||
                     TA_Base_Bus::DPT_INTERNAL_SESSION_ID == sessionID ||
                     TA_Base_Bus::NotLoaded == sessionID );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[sessionID=%s][return=%d]", sessionID.c_str(), ret );
        return ret;
    }
}


namespace TA_Base_Bus
{
	ProtocolSender::ProtocolSender(std::string name, IProtocolDevice* defaultDevice)
		:
		ProtocolIOBase(name),
		m_defaultDevice(defaultDevice)
    {
        
    }

	ProtocolSender::~ProtocolSender()
	{
	}

	bool ProtocolSender::processWriteRequest ( TA_Base_Bus::DataPointWriteRequest & newWrite )
	{
		TA_Base_Bus::DataPoint * dp = newWrite.getDataPoint();
		TA_ASSERT( 0 != dp, "ModbusOutputSender::processWriteRequest: newWrite contains a NULL datapoint." );

		IDeviceCommand* cmd = constructDeviceCommand(newWrite);
		if (0 == cmd)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "sender %s failed to construct device command for datapoint:%s",
					m_name.c_str(), dp->getDataPointName().c_str());
			return false;
		}		

		bool sendResult = false;
		IProtocolDevice* device = m_defaultDevice;
		if(0 == device)
		{
			if (m_protocolManager)
			{
				device = m_protocolManager->getOutputDevice(m_name);
			}
		}
		
		bool isSystemSession = isSystemSessionID( newWrite.getSessionID() );//limin++, SystemSession should not access db
		if(0 != device)
		{
			dp->updateControlState ( TA_Base_Bus::pending, isSystemSession );

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "device %s start to sending command for datapoint:%s to value:%s",
					device->getEntityName().c_str(), dp->getDataPointName().c_str(), newWrite.getValue().getValueAsString().c_str());

			sendResult = device->sendCommand(cmd);
			if (sendResult)
			{
				dp->startABETimeout( newWrite.getSessionID() );
			}
			else 
			{
				dp->updateControlState ( TA_Base_Bus::comms_failed, isSystemSession );
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "sender %s can not find the device to send command for datapoint:%s",
					m_name.c_str(), dp->getDataPointName().c_str());
			dp->updateControlState ( TA_Base_Bus::comms_failed, isSystemSession );
		}
		delete cmd;
		cmd = 0;
        return sendResult;
    }
}
