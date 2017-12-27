// ScadaBusinessItem.cpp: implementation of the ScadaBusinessItem class.
//
//////////////////////////////////////////////////////////////////////

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ScadaBusinessItem.h"

namespace TA_Base_Core
{
	ScadaBusinessItem::ScadaBusinessItem(const std::string& agentName)
		: m_agent_name(agentName)
	{
	}

	ScadaBusinessItem::~ScadaBusinessItem()
	{
	}

	const std::string & ScadaBusinessItem::getAgentName()
	{
		return m_agent_name;
	}
	
	EScadaBusinessType ScadaBusinessItem::getType()
	{
		return SCADA_BUSINESS_TYPE_UNKNOWN;
	}
	
	EScadaBusinessSubsystem ScadaBusinessItem::getSubsystem()
	{
		return SCADA_BUSINESS_SUBSYSTEM_UNKNOWN;
	}

	unsigned long ScadaBusinessItem::getIntervalTime()
	{
		return 0;
	}

	unsigned long ScadaBusinessItem::getLocation()
	{
		return 0;
	}

	const ScadaBusinessEntitiesVector * ScadaBusinessItem::getInputEntites()
	{
		return NULL;
	}
	
	const ScadaBusinessEntitiesVector * ScadaBusinessItem::getOutputEntities()
	{
		return NULL;
	}
	
	const ScadaBusinessEntitiesVector * ScadaBusinessItem::getOutputEntitiesOrdering()
	{
		return NULL;
	}
} // closes TA_Base_Core

namespace TA_Base_Core
{
	ScadaBusinessHeartbeat::ScadaBusinessHeartbeat(const std::string& agentName, unsigned long time)
		: ScadaBusinessItem(agentName), m_interval_time(time), m_output_entities()
	{
	}

	ScadaBusinessHeartbeat::~ScadaBusinessHeartbeat()
	{
	}

	EScadaBusinessType ScadaBusinessHeartbeat::getType()
	{
		return SCADA_BUSINESS_TYPE_HEARTBEAT;
	}
	
	unsigned long ScadaBusinessHeartbeat::getIntervalTime()
	{
		return m_interval_time;
	}
	
	const ScadaBusinessEntitiesVector * ScadaBusinessHeartbeat::getOutputEntities()
	{
		return &m_output_entities;
	}
	
	ScadaBusinessEntitiesVector & ScadaBusinessHeartbeat::getOutputEntitiesRef()
	{
		return m_output_entities;
	}
} // 

namespace TA_Base_Core
{
	ScadaBusinessMonitorAndWrite::ScadaBusinessMonitorAndWrite(const std::string& agentName,
		unsigned long location, EScadaBusinessSubsystem subsystem
	)
		: ScadaBusinessItem(agentName), m_location(location), m_subsystem(subsystem),
		m_input_entities(), m_output_entities_ordering()
	{
	}

	ScadaBusinessMonitorAndWrite::~ScadaBusinessMonitorAndWrite()
	{
	}

	EScadaBusinessType ScadaBusinessMonitorAndWrite::getType()
	{
		return SCADA_BUSINESS_TYPE_MONITOR_AND_WRITE;
	}
	
	EScadaBusinessSubsystem ScadaBusinessMonitorAndWrite::getSubsystem()
	{
		return m_subsystem;
	}

	unsigned long ScadaBusinessMonitorAndWrite::getLocation()
	{
		return m_location;
	}

	const ScadaBusinessEntitiesVector * ScadaBusinessMonitorAndWrite::getInputEntites()
	{
		return &m_input_entities;
	}
	
	ScadaBusinessEntitiesVector & ScadaBusinessMonitorAndWrite::getInputEntitesRef()
	{
		return m_input_entities;
	}

	const ScadaBusinessEntitiesVector * ScadaBusinessMonitorAndWrite::getOutputEntitiesOrdering()
	{
		return &m_output_entities_ordering;
	}

	ScadaBusinessEntitiesVector & ScadaBusinessMonitorAndWrite::getOutputEntitiesOrderingRef()
	{
		return m_output_entities_ordering;
	}
} // 

