// ScadaBusinessItem.h: interface for the ScadaBusinessItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCADABUSINESSITEM_H__B22EFC47_AFBF_417A_90FE_000123456789__INCLUDED_)
#define AFX_SCADABUSINESSITEM_H__B22EFC47_AFBF_417A_90FE_000123456789__INCLUDED_

/**
  * 
  * ScadaBusinessItem is an implementation of IScadaBusinessItem. It holds the data specific to a scada business item
  * in the database, and allows read-only access to that data.
  *
  * This class holds the data in the following DB tables: SC_SCADA_BUSINESS, SC_SCADA_HEARTBEAT,
  SC_SCADA_MONITOR_AND_WRITE
    */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/src/IScadaBusinessItem.h"

namespace TA_Base_Core
{
	class ScadaBusinessItem : public IScadaBusinessItem
	{
	public:
		ScadaBusinessItem(const std::string& agentName);
		virtual ~ScadaBusinessItem();

		virtual const std::string & getAgentName();
		
		virtual EScadaBusinessType getType();
		
		virtual EScadaBusinessSubsystem getSubsystem();

		virtual unsigned long getIntervalTime();

		virtual unsigned long getLocation();

		virtual const ScadaBusinessEntitiesVector * getInputEntites();
		
		virtual const ScadaBusinessEntitiesVector * getOutputEntities();
		
		virtual const ScadaBusinessEntitiesVector * getOutputEntitiesOrdering();
		
	private:
		ScadaBusinessItem& operator=(const ScadaBusinessItem &);
		ScadaBusinessItem(const ScadaBusinessItem &);

		std::string m_agent_name;
	};
}

namespace TA_Base_Core
{
	class ScadaBusinessHeartbeat : public ScadaBusinessItem
	{
	public:
		ScadaBusinessHeartbeat(const std::string& agentName, unsigned long time);
		~ScadaBusinessHeartbeat();

		EScadaBusinessType getType();
		
		unsigned long getIntervalTime();

		const ScadaBusinessEntitiesVector * getOutputEntities();

		ScadaBusinessEntitiesVector & getOutputEntitiesRef();

	private:
		ScadaBusinessHeartbeat& operator=(const ScadaBusinessHeartbeat &);
		ScadaBusinessHeartbeat(const ScadaBusinessHeartbeat &);

		unsigned long m_interval_time;
		ScadaBusinessEntitiesVector m_output_entities;
	};
} // closes TA_Base_Core

namespace TA_Base_Core
{
	class ScadaBusinessMonitorAndWrite : public ScadaBusinessItem
	{
	public:
		ScadaBusinessMonitorAndWrite(const std::string& agentName, 
			unsigned long location, EScadaBusinessSubsystem subsystem);
		~ScadaBusinessMonitorAndWrite();

		EScadaBusinessType getType();
		
		EScadaBusinessSubsystem getSubsystem();

		unsigned long getLocation();

		const ScadaBusinessEntitiesVector * getInputEntites();

		ScadaBusinessEntitiesVector & getInputEntitesRef();
		
		const ScadaBusinessEntitiesVector * getOutputEntitiesOrdering();

		ScadaBusinessEntitiesVector & getOutputEntitiesOrderingRef();

	private:
		ScadaBusinessMonitorAndWrite& operator=(const ScadaBusinessMonitorAndWrite &);
		ScadaBusinessMonitorAndWrite(const ScadaBusinessMonitorAndWrite &);

		unsigned long m_location;
		EScadaBusinessSubsystem m_subsystem;
		ScadaBusinessEntitiesVector m_input_entities;
		ScadaBusinessEntitiesVector m_output_entities_ordering;
	};
} // closes TA_Base_Core

#endif // !defined(AFX_SCADABUSINESSITEM_H__B22EFC47_AFBF_417A_90FE_000123456789__INCLUDED_)
