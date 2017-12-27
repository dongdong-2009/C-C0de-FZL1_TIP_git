/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPOutputSender.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "SNMPOutputSender.h"
#include "SNMPCommand.h"
#include "DataTranslater.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_Bus
{
	SNMPOutputSender::SNMPOutputSender (std::string name, IProtocolDevice* defaultDevice)
	:
	ProtocolSender(name, defaultDevice)
	{
	}


	SNMPOutputSender::~SNMPOutputSender()
	{
	}

	bool SNMPOutputSender::isDataPointMatched(TA_Base_Bus::DataPoint * dp )
	{
		bool ret = SNMPGlobalHelper::isNameMatchedDeviceName(m_name, dp->getDataPointName());
		return ret;
    }

	IDeviceCommand* SNMPOutputSender::constructDeviceCommand ( TA_Base_Bus::DataPointWriteRequest & newWrite )
	{
		SNMPCommand* cmd = new SNMPCommand(); 

		TA_Base_Bus::DataPoint * dp = newWrite.getDataPoint();
		DpValue	value = newWrite.getValue();
        
		cmd->oid = dp->getDataPointAddress();
		cmd->vb.set_oid(dp->getDataPointAddress().c_str());
		DataTranslater::translateToSNMPVb(value, cmd->vb);

        return cmd;
    }

}
