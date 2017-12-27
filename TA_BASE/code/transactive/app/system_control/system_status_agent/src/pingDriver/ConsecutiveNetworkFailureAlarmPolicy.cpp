/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/ConsecutiveNetworkFailureAlarmPolicy.cpp $
  * @author:   Teik Oh
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * ConsecutiveNetworkFailureAlarmPolicy class implimentation. See header file for class descrption
  */

#include "ConsecutiveNetworkFailureAlarmPolicy.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_App
{

	ConsecutiveNetworkFailureAlarmPolicy::ConsecutiveNetworkFailureAlarmPolicy(
			unsigned int maxConsecutiveFails) 
			:
			m_maxConsecutiveFails(maxConsecutiveFails),
			m_failCount(0)
	{
		FUNCTION_ENTRY("ConsecutiveNetworkFailureAlarmPolicy");

		if (m_maxConsecutiveFails < 1)
		{
			m_maxConsecutiveFails = 1;
		}

		FUNCTION_EXIT;
	}


	void ConsecutiveNetworkFailureAlarmPolicy::initPolicyState(bool state)
	{
		FUNCTION_ENTRY("initPolicyState");

		if (state)
		{
			m_failCount = 0;

			FUNCTION_EXIT;
		}
		else
		{
			m_failCount = m_maxConsecutiveFails;

			FUNCTION_EXIT;
		}		
	}

 
	void ConsecutiveNetworkFailureAlarmPolicy::addResult(bool pingSuccess)
	{
		FUNCTION_ENTRY("addResult");

		if (pingSuccess)
		{
			m_failCount = 0;
		}
		else 
		{
			if (m_failCount < m_maxConsecutiveFails)
			{

				m_failCount++;
			}
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"addResult(): m_failCount(%u),  m_maxConsecutiveFails(%u)", m_failCount, m_maxConsecutiveFails);


		FUNCTION_EXIT;
	}


	bool ConsecutiveNetworkFailureAlarmPolicy::pointIsPingable()
	{
		FUNCTION_ENTRY("pointIsPingable");

		if(m_failCount >= m_maxConsecutiveFails)
		{

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"ConsecutiveNetworkFailureAlarmPolicy::pointIsPingable() is returning false because m_failCount(%u) > m_maxConsecutiveFails(%u)", m_failCount, m_maxConsecutiveFails);
		
			FUNCTION_EXIT;

			return false;		
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"ConsecutiveNetworkFailureAlarmPolicy::pointIsPingable() is returning true.");

			FUNCTION_EXIT;

			return true;
		}

	}

	void ConsecutiveNetworkFailureAlarmPolicy::setMaxConsecutiveFails(unsigned int maxConsecutiveFails)
	{
		FUNCTION_ENTRY("setMaxConsecutiveFails");

		m_maxConsecutiveFails = maxConsecutiveFails;

		if (m_maxConsecutiveFails < 1)
		{
			m_maxConsecutiveFails = 1;
		}

		FUNCTION_EXIT;
	}

} //end namespace TA_Base_App
