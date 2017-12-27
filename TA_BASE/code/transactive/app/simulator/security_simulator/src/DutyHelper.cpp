// DutyHelper.cpp: implementation of the DutyHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "security_simulator.h"
#include "DutyHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define CATCH_DUTY_EXCEPTION()  \
catch (const TA_Base_Core::AccessDeniedException& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::AccessDeniedException", ex.reason.in()); \
} \
catch (const TA_Base_Core::BadParameterException& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", ex.reason.in()); \
} \
catch (const TA_Base_Core::DatabaseErrorException& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseErrorException", ex.reason.in()); \
} \
catch (const TA_Base_Core::OperationModeException& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", ex.reason.in()); \
} \
catch (const TA_Base_Core::ObjectResolutionException& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ex.what()); \
} \
catch (const TA_Base_Core::TransactiveException& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", ex.what()); \
} \
catch (const TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "DutyAgentTypeCorbaDef::UnhandledException", ex.reason.in()); \
} \
catch (const CORBA::Exception& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str() ); \
} \
catch (...) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "");  \
}

namespace
{
	const std::string DUTY_AGENT_NAME = "DutyAgent";
}

DutyHelper::DutyHelper( std::vector< std::string >& agentLocation )
	: m_subsystemStateResultString(""),
	  m_subsystemDutyResultString(""),
	  m_regionDutyResultString(""),
	  m_subsystemStateCheckResult(false),
	  m_subsystemDutyCheckResult(false),
	  m_regionDutyCheckResult(false)
{
	SubsystemStateList	tempSubsystemStateList;
	SubsystemDutyList	tempSubsystemDutyList;
	RegionDutyList		tempRegionDutyList;

	for( std::vector< std::string >::iterator it = agentLocation.begin();
	it != agentLocation.end(); ++it )
	{
		DutyAgentNamedObj* dutyAgent = new DutyAgentNamedObj;
		dutyAgent->setEntityName( *it + DUTY_AGENT_NAME );
		m_agentMap.insert( DutyAgentMap::value_type(*it, dutyAgent) );

		m_subsystemStateResult.insert( AgentSubsystemStateMap::value_type(*it, tempSubsystemStateList) );
		m_subsystemDutyResult.insert( AgentSubsystemDutyMap::value_type(*it, tempSubsystemDutyList) );
		m_regionDutyResult.insert( AgentRegionDutyMap::value_type(*it, tempRegionDutyList) );
	}
}

DutyHelper::~DutyHelper()
{
	for (DutyAgentMap::iterator it = m_agentMap.begin(); it != m_agentMap.end(); ++it)
	{
		delete it->second;
	}
}

void DutyHelper::getDutyState(const std::string& location, const std::string& sessionId, 
							  SubsystemStateList& subsystemStates, SubsystemDutyList& subsystemDuties, 
							  RegionDutyList& regionDuties)
{
	DutyAgentNamedObj* dutyAgent = getDutyAgent(location);
	if (dutyAgent != NULL)
	{
		try
		{
			TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_var subsystemStateSeq;
			TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence_var subsystemDutySeq;
			TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence_var regionDutySeq;
			
			(*dutyAgent)->mgrGetDutyState(sessionId.c_str(), subsystemStateSeq.out(), 
									  subsystemDutySeq.out(), regionDutySeq.out());
			
			subsystemStates.clear();
			subsystemStates.reserve(subsystemStateSeq->length());
			SubsystemState tempSubsystemState;
			
			for (unsigned long stateIdx=0; stateIdx < subsystemStateSeq->length(); stateIdx++)
			{
				tempSubsystemState.regionKey	  = subsystemStateSeq[stateIdx].regionKey;
				tempSubsystemState.subsystemKey	  = subsystemStateSeq[stateIdx].subsystemKey;
				tempSubsystemState.prevState	  = subsystemStateSeq[stateIdx].prevState;
				tempSubsystemState.currState	  = subsystemStateSeq[stateIdx].currState;

				subsystemStates.push_back(tempSubsystemState);
			}
			
			subsystemDuties.clear();
			subsystemDuties.reserve(subsystemDutySeq->length());
			SubsystemDuty tempSubsystemDuty;
			for (unsigned long subsystemIdx = 0; subsystemIdx < subsystemDutySeq->length(); subsystemIdx++)
			{
				tempSubsystemDuty.actionGroupKey = subsystemDutySeq[subsystemIdx].actionGroupKey;
				tempSubsystemDuty.profileKey	 = subsystemDutySeq[subsystemIdx].profileKey;
				tempSubsystemDuty.regionKey		 = subsystemDutySeq[subsystemIdx].regionKey;
				tempSubsystemDuty.subsystemKey	 = subsystemDutySeq[subsystemIdx].subsystemKey;
				tempSubsystemDuty.session		 = subsystemDutySeq[subsystemIdx].session.in();

				subsystemDuties.push_back(tempSubsystemDuty);
			}
			
			regionDuties.clear();
			regionDuties.reserve(regionDutySeq->length());
			RegionDuty tempRegionDuty;
			
			for (unsigned long regionIdx = 0; regionIdx < regionDutySeq->length(); regionIdx++)
			{
				tempRegionDuty.profileKey	  = regionDutySeq[regionIdx].profileKey;
				tempRegionDuty.profileTypeKey = regionDutySeq[regionIdx].profileTypeKey;
				tempRegionDuty.regionKey	  = regionDutySeq[regionIdx].regionKey;
				tempRegionDuty.session		  = regionDutySeq[regionIdx].session.in();

				regionDuties.push_back(tempRegionDuty);
			}
		}
		CATCH_DUTY_EXCEPTION();
	}
}

void DutyHelper::transferRegionDuty( const std::string& agentLocation, 
									 const std::string& sessionId, const RegionKeyList& regionKeys)
{
	DutyAgentNamedObj* dutyAgent = getDutyAgent(agentLocation);
	if (dutyAgent != NULL)
	{
		try
		{
			TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence regionKeySeq;
			regionKeySeq.length(regionKeys.size());
			
			for (unsigned long r = 0; r < regionKeys.size(); r++)
			{
				regionKeySeq[r] = regionKeys[r];
			}
			
			(*dutyAgent)->mgrTransferRegionDuty(sessionId.c_str(), regionKeySeq);
		}
		CATCH_DUTY_EXCEPTION();
	}
}

void DutyHelper::transferSubsystemDuty(const std::string& agentLocation, 
									   const std::string& sessionId, const SubsystemTreeMap& subsystems)
{
	DutyAgentNamedObj* dutyAgent = getDutyAgent(agentLocation);
	if (dutyAgent != NULL)
	{
		try
		{
			TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence subsystemTreeSeq;
			subsystemTreeSeq.length(subsystems.size());
			
			unsigned long t = 0;
			for (SubsystemTreeMap::const_iterator treeIt = subsystems.begin(); treeIt != subsystems.end(); treeIt++, t++)
			{
				const std::vector<unsigned long>& subsystemKeys = treeIt->second;
				TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTree subsystemTree;
				
				subsystemTree.region = treeIt->first;
				subsystemTree.subsystems.length(subsystemKeys.size());
            
				for (unsigned long s = 0; s < subsystemKeys.size(); s++)
				{
					subsystemTree.subsystems[s] = subsystemKeys[s];
				}
				
				subsystemTreeSeq[t] = subsystemTree;
			}
			
			(*dutyAgent)->mgrTransferSubsystemDuty(sessionId.c_str(), subsystemTreeSeq);    
		}
		CATCH_DUTY_EXCEPTION();
	}
}

void DutyHelper::checkDuty(const std::string& sessionId)
{
	for( DutyAgentMap::iterator agentIter = m_agentMap.begin(); agentIter != m_agentMap.end(); ++agentIter )
	{
		AgentSubsystemStateMap::iterator subsystemStateResultIter = m_subsystemStateResult.find(agentIter->first);
		AgentSubsystemDutyMap::iterator  subsystemDutyResultIter  = m_subsystemDutyResult.find(agentIter->first);
		AgentRegionDutyMap::iterator     regionDutyResultIter	  = m_regionDutyResult.find(agentIter->first);

		getDutyState(agentIter->first, sessionId, subsystemStateResultIter->second, 
					 subsystemDutyResultIter->second, regionDutyResultIter->second);
	}

	m_subsystemStateCheckResult = isAllTheDutyInfoTheSame(m_subsystemStateResult);
	m_subsystemDutyCheckResult  = isAllTheDutyInfoTheSame(m_subsystemDutyResult);
	m_regionDutyCheckResult	    = isAllTheDutyInfoTheSame(m_regionDutyResult);

	logDutyDetail();
}

DutyHelper::DutyAgentNamedObj* DutyHelper::getDutyAgent(const std::string& location)
{
	DutyAgentNamedObj* ret ;;
	DutyAgentMap::iterator it = m_agentMap.find(location);

	if (it == m_agentMap.end())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"can't find %s in duty agent map", location.c_str());
	}
	else
	{
		ret = it->second;
	}

	return ret;
}

void DutyHelper::logDutyDetail()
{
	std::string log;
	m_subsystemStateResultString = "";

	log += "SubsystemState info: ";	
	AgentSubsystemStateMap::iterator subsystemStateMapIter = m_subsystemStateResult.begin();
	for(; subsystemStateMapIter != m_subsystemStateResult.end(); ++subsystemStateMapIter)
	{
		std::stringstream logInfo;
		logInfo << subsystemStateMapIter->first << ": " << "\n";
		
		for ( SubsystemStateList::iterator it = subsystemStateMapIter->second.begin(); 
			  it != subsystemStateMapIter->second.end(); ++it)
		{
			logInfo << "regionKey: " << it->regionKey << " "
					<< "subsystemKey: " << it->subsystemKey << " "
					<< "prevState: " << it->prevState << " "
					<< "currState: " << it->currState << " ";
			logInfo << "\n";
		}
		
		m_subsystemStateResultString += logInfo.str().c_str();
	}

	log += m_subsystemStateResultString.c_str();

	m_subsystemDutyResultString = "";
	log += "SubsystemDuty info: ";	
	AgentSubsystemDutyMap::iterator subsystemDutyMapIter = m_subsystemDutyResult.begin();
	for(; subsystemDutyMapIter != m_subsystemDutyResult.end(); ++subsystemDutyMapIter)
	{
		std::stringstream logInfo;
		logInfo << subsystemDutyMapIter->first << ": " << "\n";
		
		for ( SubsystemDutyList::iterator it = subsystemDutyMapIter->second.begin(); 
		it != subsystemDutyMapIter->second.end(); ++it)
		{
			logInfo << "regionKey: " << it->regionKey << " "
					<< "subsystemKey: " << it->subsystemKey << " "
					<< "session: " << it->session << " "
					<< "profileKey: " << it->profileKey << " "
					<< "actionGroupKey: " << it->actionGroupKey << " ";
			logInfo << "\n";
		}
		
		m_subsystemDutyResultString += logInfo.str().c_str();
	}
	log += m_subsystemDutyResultString.c_str();

	m_regionDutyResultString = "";
	log += "RegionDuty info: ";	
	AgentRegionDutyMap::iterator regionDutyMapIter = m_regionDutyResult.begin();
	for(; regionDutyMapIter != m_regionDutyResult.end(); ++regionDutyMapIter)
	{
		std::stringstream logInfo;
		logInfo << regionDutyMapIter->first << ": " << "\n";
		
		for ( RegionDutyList::iterator it = regionDutyMapIter->second.begin(); 
		it != regionDutyMapIter->second.end(); ++it)
		{
			logInfo << "regionKey: " << it->regionKey << " "
					<< "session: " << it->session << " "
					<< "profileKey: " << it->profileKey << " "
					<< "profileTypeKey: " << it->profileTypeKey << " ";
			logInfo << "\n";
		}
		
		m_regionDutyResultString += logInfo.str().c_str();
	}
	log += m_regionDutyResultString.c_str();

	

	FILE *fp = fopen("DutyInfo.Log", "at");
	if(fp != NULL)
	{
		fprintf(fp, " %s \n", log.c_str());
		fclose(fp);
	}
	
}