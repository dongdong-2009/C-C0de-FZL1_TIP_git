/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolDevice.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "ProtocolDevice.h"

namespace TA_Base_Bus
{
    ProtocolDevice::ProtocolDevice(TA_Base_Core::IEntityData* pData)
    {
		if (pData)
		{
			m_entityKey			= pData->getKey();
			m_entityName		= pData->getName();
			m_entityAddress		= pData->getAddress();
			m_entityType		= pData->getType();
			m_entityDescription = pData->getDescription();
			m_entityParentName	= pData->getParentName();
			m_entityAgentName	= pData->getAgentName();
			m_entityLocationName = pData->getLocationName();
			m_entitySubsystemName = pData->getSubsystemName();
		}
    }

	ProtocolDevice::~ProtocolDevice()
	{
	}

}
