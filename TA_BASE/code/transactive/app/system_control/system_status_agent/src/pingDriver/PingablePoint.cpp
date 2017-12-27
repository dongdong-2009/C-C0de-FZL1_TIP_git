/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/PingablePoint.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * PingablePoint class implimentation. See header file for class descrption
  */
//we need to pick up the sockets stuff

#ifdef WIN32
#pragma warning( disable : 4503 ) 
#endif

#include "app/system_control/system_status_agent/src/pingDriver/PingablePoint.h"

#include "bus/scada/datapoint_library/src/BooleanDataPoint.h"
#include "app/system_control/system_status_agent/src/pingDriver/PercentageNetworkFailureAlarmPolicy.h"
#include "app/system_control/system_status_agent/src/pingDriver/ConsecutiveNetworkFailureAlarmPolicy.h"

namespace TA_Base_App
{
	PingablePoint::PingablePoint
	( 
		TA_Base_Bus::BooleanDataPoint* const datapoint,
		const std::string ipAddress, 
		FailurePolicyEnum policyType /*= CONSECUTIVE*/
	)
		: m_datapoint(datapoint),
		  m_ipAddress(ipAddress),
		  m_hasBeenProcessed(false),
		  m_alarmPolicy(0)
	{
		FUNCTION_ENTRY("PingablePoint");

		switch(policyType)
		{
			case CONSECUTIVE :
			{
				//unsigned int consecutiveFails will be reset to the configured
				//value during init - see PingablePoint::setMaxConsecutiveFails()
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
				m_alarmPolicy = new PercentageNetworkFailureAlarmPolicy(numFails, setSize);
				break;
			}
			default :
			{
				TA_ASSERT(false, "The failure policy specified is unknown");

			}
		}

		FUNCTION_EXIT;
	}



	PingablePoint::~PingablePoint()
	{
		FUNCTION_ENTRY("~PingablePoint");

		delete m_alarmPolicy;

		FUNCTION_EXIT;
	}

	TA_Base_Bus::DataPoint* const PingablePoint::getDataPoint()
	{
		FUNCTION_ENTRY("getDataPoint");
		FUNCTION_EXIT;
		return m_datapoint;
	}

	TA_Base_Core::InetAddress& PingablePoint::getInternetAddressRef()
	{
		FUNCTION_ENTRY("getInternetAddressRef");
		FUNCTION_EXIT;
		return m_ipAddress;
	}

	INetworkFailureAlarmPolicy* const PingablePoint::getAlarmPolicy()
	{
		FUNCTION_ENTRY("getAlarmPolicy");
		FUNCTION_EXIT;
		return m_alarmPolicy;
	}

	const bool PingablePoint::hasBeenProcessed() const
	{
		FUNCTION_ENTRY("hasBeenProcessed");
		FUNCTION_EXIT;
		return m_hasBeenProcessed;
	}

	void PingablePoint::setProcessed(bool state)
	{
		//FUNCTION_ENTRY("setProcessed");
		//FUNCTION_EXIT;
		m_hasBeenProcessed = state;
	}

	void PingablePoint::setMaxConsecutiveFails(unsigned int consecutiveFails)
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


	std::string PingablePoint::getAddressIdentifier()
	{
		FUNCTION_ENTRY("getAddressIdentifier");
		FUNCTION_EXIT;
		return m_ipAddress.getHostName();
	}


} //end namespace TA_Base_App
