/**
*  The source code in this file is the property of
*  Ripple Systems and is not for redistribution
*  in any form.
*
*  @author:  Gregg Kirkpatrick
*  @version: $Revision: #1 $
*
*  Last modification: $DateTime: 2008/10/31 12:12:40 $
*  Last modified by:  $Author: grace.koh $
*
*  A static class that exposes transActive configuration values to anyone who is interested.
*/
#ifndef RIGHTSAGENTINFO_H_INCLUDED
#define RIGHTSAGENTINFO_H_INCLUDED

namespace TA_Base_Bus
{
	class RightsAgentInfo
	{
	public:
		static unsigned long m_entity;
		static std::string    m_name;
		static unsigned long m_entityType;
		static unsigned long m_subsystem;
		static unsigned long m_location;
		static unsigned long m_uncontrolledTimeout;
		static std::string    m_assetName;
	};
}

#endif //  RIGHTSAGENTINFO_H_INCLUDED
