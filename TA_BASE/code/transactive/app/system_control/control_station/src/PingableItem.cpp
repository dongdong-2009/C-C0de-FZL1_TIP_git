/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/PingableItem.cpp $
  * @author:   Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * PingableItem class implimentation. See header file for class descrption
  */
//we need to pick up the sockets stuff


#include "app/system_control/control_station/src/PingableItem.h"
#include "app/system_control/control_station/src/ConsecutiveNetworkFailureAlarmPolicy.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

PingableItem::PingableItem
( 
	const std::string ipAddress, 
	FailurePolicyEnum policyType /*= CONSECUTIVE*/
)
	: m_ipAddress(ipAddress),
	  m_hasBeenProcessed(false),
	  m_alarmPolicy(0)
{
	FUNCTION_ENTRY("PingableItem");

	switch(policyType)
	{
		case CONSECUTIVE :
		{
			//unsigned int consecutiveFails will be reset to the configured
			//value during init - see PingableItem::setMaxConsecutiveFails()
			m_alarmPolicy = new ConsecutiveNetworkFailureAlarmPolicy(1);
			break;
		}
		case PERCENT :
		{
			TA_ASSERT(false, "PercentageNetworkFailureAlarmPolicy is not currently supported, see class comments");

			//unsigned int numFails, unsigned int setSize
			//the params specify the ratio numFails/setSize where for any consecutive 
			//number (setSize) of pings if numFails are bad then alarm
			
			int numFails = 1;
			int setSize = 2;
			//m_alarmPolicy = new PercentageNetworkFailureAlarmPolicy(numFails, setSize);
			break;
		}
		default :
		{
			TA_ASSERT(false, "The failure policy specified is unknown");

		}
	}

	FUNCTION_EXIT;
}



PingableItem::~PingableItem()
{
	FUNCTION_ENTRY("~PingableItem");

	delete m_alarmPolicy;

	FUNCTION_EXIT;
}

TA_Base_Core::InetAddress& PingableItem::getInternetAddressRef()
{
	return m_ipAddress;
}

INetworkFailureAlarmPolicy* const PingableItem::getAlarmPolicy()
{
	return m_alarmPolicy;
}

const bool PingableItem::hasBeenProcessed() const
{
	return m_hasBeenProcessed;
}

void PingableItem::setProcessed(bool state)
{
	m_hasBeenProcessed = state;
}

void PingableItem::setMaxConsecutiveFails(unsigned int consecutiveFails)
{
	FUNCTION_ENTRY("setMaxConsecutiveFails");

	ConsecutiveNetworkFailureAlarmPolicy* pConPolicy = 
			dynamic_cast<ConsecutiveNetworkFailureAlarmPolicy*>(m_alarmPolicy);

	if(0 != pConPolicy)
	{
		pConPolicy->setMaxConsecutiveFails(consecutiveFails);
	}

	FUNCTION_EXIT;
}


std::string PingableItem::getAddressIdentifier()
{
	return m_ipAddress.getHostName();
}



