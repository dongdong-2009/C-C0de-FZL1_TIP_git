/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada_agent/src/ScadaAgentUser.cpp $
  * @author:   Dhanshri
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/02/06 10:52:23 $
  * Last mofified by:  $Author: Dhanshri $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */

#include "ScadaAgentUser.h"
#include "ScadaAgent.h"

namespace TA_Base_Bus
{
	ScadaAgentUser::ScadaAgentUser()
					 :m_scadaAgent(NULL)
	{
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgentUser::ScadaAgentUser() - Empty constructor" );
	}

	ScadaAgentUser::~ScadaAgentUser()
	{
	}

	TA_Base_Core::IEntityDataPtr ScadaAgentUser::getAgentEntityConfiguration()
	{
		return m_scadaAgent->getAgentEntityConfiguration();
	}

	void  ScadaAgentUser::initialize(int argc, char* argv[])
	{
		m_scadaAgent = new ScadaAgent(argc, argv, this);
	}

	void ScadaAgentUser::startScadaAgent()
	{
		m_scadaAgent->startScadaAgent();
	}

	void ScadaAgentUser::checkOperationMode()
	{
		m_scadaAgent->checkOperationMode();
	}
	
	TA_Base_Bus::DataPoint * ScadaAgentUser::getDataPoint( unsigned long entityKey )
	{
		return m_scadaAgent->getDataPoint(entityKey);
	}
	
	void ScadaAgentUser::getAllDataPoints(std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		m_scadaAgent->getAllDataPoints(dataPointList);
	}
	
	TA_Base_Bus::DataNode * ScadaAgentUser::getDataNode( unsigned long entityKey )
	{
		return m_scadaAgent->getDataNode(entityKey);
	}
	
	void ScadaAgentUser::getAllDataNodes(std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
	{
		m_scadaAgent->getAllDataNodes(dataNodeList);
	}
	
	TA_Base_Bus::DataPointFactory* ScadaAgentUser::getDataPointFactory()
	{
		return m_scadaAgent->getDataPointFactory();
	}

}
