// NamingPortAccessFactory.cpp: implementation of the NamingPortAccessFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "core/data_access_interface/src/NamingPortAccessFactory.h"
#include "core/data_access_interface/src/NamingPort.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/FunctionEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
/**
  * This factory class is used for create NamingPort object.
  * This class is added for CL-21230
  */

	NamingPortAccessFactory* NamingPortAccessFactory::s_instance = NULL;
	ReEntrantThreadLockable NamingPortAccessFactory::s_lock;

	NamingPortAccessFactory::NamingPortAccessFactory()
	{	
	}
	
	NamingPortAccessFactory::~NamingPortAccessFactory()
	{	
		NamingPortMap::iterator iter = m_namingPorts.begin();
		while(iter != m_namingPorts.end())
		{
			delete iter->second;
			++iter;
		}

		m_namingPorts.clear();
	}

	NamingPortAccessFactory& NamingPortAccessFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");

		ThreadGuard guard(s_lock);
		if(s_instance == NULL)
		{
			s_instance = new NamingPortAccessFactory();
		}

		FUNCTION_EXIT;
		return *s_instance;
	}

	void NamingPortAccessFactory::removeInstance()
	{
		FUNCTION_ENTRY("removeInstance");
		
		ThreadGuard guard(s_lock);
		if(s_instance != NULL)
		{
			delete s_instance;
			s_instance = NULL;
		}
		FUNCTION_EXIT;
	}

	INamingPort& NamingPortAccessFactory::getNamingPort(const std::string& agentName)
	{
		FUNCTION_ENTRY("getNamingPort");
		
		ThreadGuard guard(s_lock);
		NamingPortMap::iterator iter = m_namingPorts.find(agentName);
		if( iter == m_namingPorts.end() )
		{
			iter = m_namingPorts.insert(NamingPortMap::value_type(agentName, new NamingPort(agentName))).first;
		}

		INamingPort* np = iter->second;
		FUNCTION_EXIT;
		return *np;
	}
}
