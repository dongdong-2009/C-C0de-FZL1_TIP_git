// RightsHelper.h: interface for the RightsHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIGHTSHELPER_H__4BA192F8_5BD2_406F_B637_AB95A1C0DFF6__INCLUDED_)
#define AFX_RIGHTSHELPER_H__4BA192F8_5BD2_406F_B637_AB95A1C0DFF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SpecialRightLibrary.h"
#include "CommonInterface.h"

using namespace TA_Base_Bus;

class RightsHelper  : public IRightsChecker
{
	typedef TA_Base_Core::NamedObject< TA_Base_Bus::IRightsAgentCorbaDef,
									   TA_Base_Bus::IRightsAgentCorbaDef_ptr, 
									   TA_Base_Bus::IRightsAgentCorbaDef_var> RightsAgentNamedObj;
	typedef TA_Base_Core::NamedObject<IDutyAgentQueryCorbaDef,
									  IDutyAgentQueryCorbaDef_ptr,
									  IDutyAgentQueryCorbaDef_var> DutyAgentNamedObj;

	struct RightsCheckers
	{
		RightsAgentNamedObj* rightsAgentObj;
		DutyAgentNamedObj*   dutyAgentObj;
		SpecialRightLibrary* rightsLibrary;
	};
	

	typedef std::map< std::string, RightsCheckers > AgentRightsAccessMap;

public:
	RightsHelper( std::vector< std::string >& agentLocation );
	virtual ~RightsHelper();

	virtual bool checkRights(const std::string& location, const std::string& sessionId, 
							 unsigned long locationKey, unsigned long subsystem, unsigned long profile, 
							 unsigned long resourceKey, unsigned long actionKey, 
							 std::vector< unsigned long >& actionGroups);

	virtual void getReslt(bool& rightLibResult, bool& rightAgentResult, bool& dutyResult);

private:

	AgentRightsAccessMap m_rightsMap;

	bool m_rightsLibResult;
	bool m_rightsAgentResult;
	bool m_dutyResult;

};

#endif // !defined(AFX_RIGHTSHELPER_H__4BA192F8_5BD2_406F_B637_AB95A1C0DFF6__INCLUDED_)
