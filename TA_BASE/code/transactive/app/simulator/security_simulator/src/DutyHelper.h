// DutyHelper.h: interface for the DutyHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUTYHELPER_H__B63D0EAF_9C98_4D60_8F80_2A639197077E__INCLUDED_)
#define AFX_DUTYHELPER_H__B63D0EAF_9C98_4D60_8F80_2A639197077E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <vector>

#include "core/naming/src/NamedObject.h"
#include "core/configuration_updates/src/EObjectType.h"

#include "bus/security/duty_agent/idl/src/IDutyAgentCorbaDef.h"


class DutyHelper  
{
	typedef TA_Base_Core::NamedObject<TA_Base_Bus::IDutyAgentMgrCorbaDef, 
									  TA_Base_Bus::IDutyAgentMgrCorbaDef_ptr, 
									  TA_Base_Bus::IDutyAgentMgrCorbaDef_var>	DutyAgentNamedObj;
	typedef std::map< std::string, DutyAgentNamedObj* >							DutyAgentMap;

	typedef std::map< unsigned long, std::vector< unsigned long > >				SubsystemTreeMap;
	typedef std::vector< unsigned long >										RegionKeyList;

	struct RegionDuty
	{
		unsigned long	regionKey;
		std::string		session;
		unsigned long	profileKey;
		unsigned long	profileTypeKey;

		bool operator == ( const RegionDuty& rhs )
		{
			return (this->regionKey == rhs.regionKey) && (this->session == rhs.session) && 
				   (this->profileKey == rhs.profileKey) && (this->profileTypeKey == rhs.profileTypeKey);

		}
	};
	
	struct SubsystemDuty
	{
		unsigned long	regionKey;
		unsigned long	subsystemKey;
		std::string		session;
		unsigned long	profileKey;
		unsigned long	actionGroupKey;

		bool operator == ( const SubsystemDuty& rhs )
		{
			return (this->regionKey == rhs.regionKey) && (this->session == rhs.session) && 
				   (this->subsystemKey == rhs.subsystemKey) && (this->actionGroupKey == rhs.actionGroupKey) &&
				   (this->profileKey == rhs.profileKey);
			
		}
	};

	struct SubsystemState
	{
		unsigned long	regionKey;
		unsigned long	subsystemKey;
		unsigned long	prevState;
		unsigned long	currState;

		bool operator == ( const SubsystemState& rhs )
		{
			return (this->regionKey == rhs.regionKey) && (this->subsystemKey == rhs.subsystemKey) && 
				   (this->prevState == rhs.prevState) && (this->currState == rhs.currState);
			
		}
	};


	typedef std::vector< SubsystemState >	SubsystemStateList;
	
	typedef std::vector< SubsystemDuty >	SubsystemDutyList;
	
	typedef std::vector< RegionDuty >		RegionDutyList;

	typedef std::map< std::string, SubsystemStateList > AgentSubsystemStateMap;
	typedef std::map< std::string, SubsystemDutyList >	AgentSubsystemDutyMap;
	typedef std::map< std::string, RegionDutyList >		AgentRegionDutyMap;

public:
	DutyHelper( std::vector< std::string >& agentLocation );
	virtual ~DutyHelper();

	void transferRegionDuty( const std::string& agentLocation, 
							 const std::string& sessionId, const RegionKeyList& regionKeys);

	void transferSubsystemDuty(const std::string& agentLocation, 
							   const std::string& sessionId, const SubsystemTreeMap& subsystems);

	void getDutyState(const std::string& location, const std::string& sessionId, 
					  SubsystemStateList& subsystemStates, SubsystemDutyList& subsystemDuties, 
					  RegionDutyList& regionDuties);

	void checkDuty(const std::string& sessionId);

	bool getSubsystemStateResult() { return m_subsystemStateCheckResult; };
	bool getSubsystemDutyResult() { return m_subsystemDutyCheckResult; };
	bool getRegionDutyResult() { return m_regionDutyCheckResult; };

	
	std::string& getSubsystemStateResultString() { return m_subsystemStateResultString; };
	std::string& getSubsystemDutyResultString() { return m_subsystemDutyResultString; };
	std::string& getRegionDutyResultString() { return m_regionDutyResultString; };

private:

	DutyAgentNamedObj* getDutyAgent(const std::string& location);

	template<typename T>
	bool isTwoVecterTheSame( std::vector< T > & objOne, 
							 std::vector< T > & objTwo)
	{
		bool ret = true;
		if ( objOne.size() == objTwo.size() ) 
		{
			std::vector< T >::iterator itOne;
			std::vector< T >::iterator itTwo;
			for(itOne = objOne.begin(); itOne != objOne.end(); ++itOne)
			{
				bool isMatchOne = false;
				for(itTwo = objTwo.begin(); itTwo != objTwo.end(); ++itTwo)
				{
					if( *itOne == *itTwo  )
					{
						isMatchOne = true;
						break;
					}
				}
				
				if(!isMatchOne)
				{
					ret = false;
					break;
				}
			}
		}
		else
		{
			ret = false;
		}		

		return ret;
	}

	template<typename T>
	bool isAllTheDutyInfoTheSame( std::map< std::string, std::vector< T > >& checkObj)
	{
		bool result = true;
		
		std::map< std::string, std::vector< T > >::iterator iter = checkObj.begin();
		std::map< std::string, std::vector< T > >::iterator tempIter = checkObj.begin();
		for (++iter ; iter != checkObj.end(); ++iter)
		{
			if( isTwoVecterTheSame(tempIter->second, iter->second) == false )
			{
				result = false;
				break;
			}
		}

		return result;
	}

	void logDutyDetail();

	DutyAgentMap m_agentMap;

	AgentSubsystemStateMap m_subsystemStateResult;
	AgentSubsystemDutyMap  m_subsystemDutyResult;
	AgentRegionDutyMap     m_regionDutyResult;

	std::string			   m_subsystemStateResultString;
	std::string			   m_subsystemDutyResultString;
	std::string			   m_regionDutyResultString;

	bool				   m_subsystemStateCheckResult;
	bool				   m_subsystemDutyCheckResult;
	bool				   m_regionDutyCheckResult;
};

#endif // !defined(AFX_DUTYHELPER_H__B63D0EAF_9C98_4D60_8F80_2A639197077E__INCLUDED_)
