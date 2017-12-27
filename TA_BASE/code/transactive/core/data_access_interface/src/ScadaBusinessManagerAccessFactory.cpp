// ScadaBusinessManagerAccessFactory.cpp: implementation of the ScadaBusinessManagerAccessFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "core/data_access_interface/src/ScadaBusinessManagerAccessFactory.h"
#include "core/data_access_interface/src/ScadaBusinessManagerAccessFactoryImpl.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/FunctionEntry.h"
#include "core/utilities/src/TAAssert.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
	ScadaBusinessManagerAccessFactory* ScadaBusinessManagerAccessFactory::s_instance = NULL;
	ReEntrantThreadLockable ScadaBusinessManagerAccessFactory::s_lock;

	ScadaBusinessManagerAccessFactory::ScadaBusinessManagerAccessFactory()
	{
		m_impl = new ScadaBusinessManagerAccessFactoryImpl;
		TA_ASSERT( NULL != m_impl, "Failed to create ScadaBusinessManagerAccessFactoryImpl");
	}
	
	ScadaBusinessManagerAccessFactory::~ScadaBusinessManagerAccessFactory()
	{	
		delete m_impl;
	}

	ScadaBusinessManagerAccessFactory& ScadaBusinessManagerAccessFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");

		ThreadGuard guard(s_lock);
		if (s_instance == NULL)
		{
			s_instance = new ScadaBusinessManagerAccessFactory();
		}

		FUNCTION_EXIT;
		return *s_instance;
	}

	void ScadaBusinessManagerAccessFactory::removeInstance()
	{
		FUNCTION_ENTRY("removeInstance");
		
		ThreadGuard guard(s_lock);
		if (s_instance != NULL)
		{
			delete s_instance;
			s_instance = NULL;
		}
		FUNCTION_EXIT;
	}

	const ScadaBusinessItemVector & ScadaBusinessManagerAccessFactory::getScadaBusinessItems(const std::string& agentName)
	{
		FUNCTION_ENTRY("ScadaBusinessManagerAccessFactory::getScadaBusinessItems");
		
		ThreadGuard guard(s_lock);

		return m_impl->getScadaBusinessItems(agentName);
	}
}
