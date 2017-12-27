/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/TransactiveAgentInfo.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  A static class that exposes transActive configuration values to anyone who is interested.
  */
#ifndef TRANSACTIVEAGENTINFO_H_INCLUDED
#define TRANSACTIVEAGENTINFO_H_INCLUDED

namespace TA_Base_Bus
{
	class TransactiveAgentInfo
	{
	public:
		static DASType::DbKey m_entity;
		static std::string    m_name;
		static DASType::DbKey m_entityType;
		static DASType::DbKey m_subsystem;
		static DASType::DbKey m_location;
		static DASType::DbKey m_uncontrolledTimeout;
		static IDutyAgentRespCorbaDef_var m_responseObjRef;
		static DASConst::PolicyType m_policyType;
		static std::string    m_assetName;
	};
}

#endif //  TRANSACTIVEAGENTINFO_H_INCLUDED
