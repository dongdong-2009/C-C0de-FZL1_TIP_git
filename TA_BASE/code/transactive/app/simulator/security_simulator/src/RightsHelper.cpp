// RightsHelper.cpp: implementation of the RightsHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "security_simulator.h"
#include "RightsHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace
{
	const std::string DUTY_AGENT_NAME = "DutyAgent";
	const std::string RIGHTS_AGENT_NAME = "RightsAgent";
}

RightsHelper::RightsHelper( std::vector< std::string >& agentLocation )
{
	RightsCheckers temp;
	for( std::vector< std::string >::iterator it = agentLocation.begin();
		 it != agentLocation.end(); ++it )
	{
		DutyAgentNamedObj* dutyAgent = new DutyAgentNamedObj;
		dutyAgent->setEntityName( *it + DUTY_AGENT_NAME );
				
		RightsAgentNamedObj* rightsAgent = new RightsAgentNamedObj;
		rightsAgent->setEntityName( *it + RIGHTS_AGENT_NAME );

		SpecialRightLibrary* rightsLibrary = new SpecialRightLibrary( *it + RIGHTS_AGENT_NAME );

		temp.dutyAgentObj = dutyAgent;
		temp.rightsAgentObj = rightsAgent;
		temp.rightsLibrary = rightsLibrary;

		m_rightsMap.insert( AgentRightsAccessMap::value_type(*it, temp) );
	}
}

RightsHelper::~RightsHelper()
{
	for (AgentRightsAccessMap::iterator it = m_rightsMap.begin(); it != m_rightsMap.end(); ++it)
	{
		delete it->second.dutyAgentObj;
		delete it->second.rightsAgentObj;
		delete it->second.rightsLibrary;
	}
}

bool RightsHelper::checkRights(const std::string& location, const std::string& sessionId, 
							   unsigned long locationKey, unsigned long subsystem, unsigned long profile, 
							   unsigned long resourceKey, unsigned long actionKey, 
							   std::vector< unsigned long >& actionGroups)
{
	bool ret = false;
	AgentRightsAccessMap::iterator it = m_rightsMap.find(location);
	
	if(it == m_rightsMap.end())
	{
	}
	else
	{
		DutyAgentNamedObj* dutyAgent = it->second.dutyAgentObj;		
		RightsAgentNamedObj* rightsAgent = it->second.rightsAgentObj;				
		SpecialRightLibrary* rightsLibrary = it->second.rightsLibrary;
		
		try
		{
			m_dutyResult = false;
			DutyAgentTypeCorbaDef::Key actionGroup = DutyAgentTypeCorbaDef::INVALID_KEY;
			bool hasDuty = (*dutyAgent)->queryHasDuty( sessionId.c_str(), locationKey, subsystem, 
													   profile, actionGroup );
			if(hasDuty)
			{
				for( std::vector< unsigned long >::iterator it = actionGroups.begin(); 
				it != actionGroups.end(); ++it )
				{
					if(actionGroup == *it)
					{
						m_dutyResult = true;
						break;
					}
				}
			}
		}
		catch (...)
		{
			m_dutyResult = false;
			LOG_EXCEPTION_CATCH(SourceInfo, "...", "get an exception when check duty in duty agent");
		}

		std::string result;
		EDecisionModule decisionModule;
		m_rightsLibResult = rightsLibrary->isActionPermittedOnResource(sessionId, resourceKey, actionKey, result, decisionModule);

		try
		{
			CORBA::String_var corbaReason;
			IRightsAgentCorbaDef::ERightsResponse corbaResult = IRightsAgentCorbaDef::Indeterminate;
			IRightsAgentCorbaDef::EDecisionModule corbaDm = IRightsAgentCorbaDef::Rights;

			corbaResult = (*rightsAgent)->isActionPermittedOnResource(sessionId.c_str(), resourceKey, 
				actionKey, corbaReason.out(), corbaDm);
			
			switch (corbaResult)
			{
			case IRightsAgentCorbaDef::Permit:
				m_rightsAgentResult = true;
				break;
				
			case IRightsAgentCorbaDef::Deny:
				m_rightsAgentResult = false;
				break;
				
			case IRightsAgentCorbaDef::Indeterminate:
                m_rightsAgentResult = false;
				break;
				
			case IRightsAgentCorbaDef::NotApplicable:
                m_rightsAgentResult = false;
				break;
				
			default:
				m_rightsAgentResult = false;
				break;
			}

		}
		catch (...)
		{
			m_rightsAgentResult = false;
			LOG_EXCEPTION_CATCH(SourceInfo, "...", "get an exception when check right in rights agent");
		}

		if (m_dutyResult == m_rightsAgentResult && m_rightsAgentResult == m_rightsLibResult)
		{
			ret = true;
		}
	}
	return ret;
}

void RightsHelper::getReslt(bool& rightLibResult, bool& rightAgentResult, bool& dutyResult)
{
	rightLibResult = m_rightsLibResult;
	rightAgentResult = m_rightsAgentResult;
	dutyResult = m_dutyResult;
}


