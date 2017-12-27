/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/shared_alarm/src/SubsytemDataCache.h$
  * @author:  Lin Weikun
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/10/02 10:23:40 $
  * Last modified by:  $Author: Noel Tajanlangit $
  *
  * SubsystemDataCache is a singleton  that get subsystem from subsystem table
  */

  
#include "bus\alarm\alarm_store_library\src\SubsytemDataCache.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core\exceptions\src\DatabaseException.h"  
#include <algorithm>

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	// Initialise statics
	SubsystemDataCache* SubsystemDataCache::m_instance = 0;
	TA_Base_Core::NonReEntrantThreadLockable SubsystemDataCache::m_singletonLock;

	// This is used in retrieving the data from map to vector
	// use in call to std::transform
	template<typename Pair>
	struct select2nd
	{
		typedef Pair argument_type ;
		typedef typename Pair::second_type result_type ;

		const result_type &
			operator()(const argument_type &p) const
		{
			return p.second ;
		}
	};

	SubsystemDataCache::SubsystemDataCache()
	{
	}
	void SubsystemDataCache::InitializeSubsystem()
	{
		std::vector<TA_Base_Core::ISubsystem*> subSystems;
		//load subsystem from database.
		try
		{
			subSystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllSubsystems(false);	
			 // convert systemSubSystem to map
			for (int i = 0; i < subSystems.size(); i++)
			{
				TA_Base_Core::Subsystem* pSubSystem = dynamic_cast<TA_Base_Core::Subsystem *>(subSystems[i]);
				m_subSystems.insert(SubSystemT::value_type(pSubSystem->getKey(), subSystems[i]));
			}
		
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", 
				"Errors occurred while trying to retrieve subsystems matching the one passed in");
			for (unsigned int i = 0;i < subSystems.size(); ++i)
			{
				delete subSystems[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", 
				"Errors occured while trying to retrieve the severity information from the database.");
			for (unsigned int i = 0;i < subSystems.size(); ++i)
			{
				delete subSystems[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "UnknownException", 
				"Unknown error whilst retrieving subsystem information!");
			for (unsigned int i = 0;i < subSystems.size(); ++i)
			{
				delete subSystems[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving subsystem information."));
		}
		return;
	}

	SubsystemDataCache& SubsystemDataCache::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);

		if ( m_instance == NULL )
		{
			m_instance = new SubsystemDataCache();
			m_instance->InitializeSystem();
			m_instance->InitializeSubsystem();
		}

		return *m_instance;
	}

	void SubsystemDataCache::releaseInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);

		if (NULL != m_instance)
		{
			delete m_instance;
			m_instance = NULL;
		}
	}

	unsigned long SubsystemDataCache::getSystemKeyFromSubsystem(unsigned long subsystemKey)
	{
		FUNCTION_ENTRY("getSystemFormSubsystem");
		unsigned long ulSystemKey = 0;
		
		SubSystemT::iterator itFind = m_subSystems.find(subsystemKey);
		if (m_subSystems.end() != itFind)
		{
			TA_Base_Core::Subsystem* pSubSystem = dynamic_cast<TA_Base_Core::Subsystem *>(itFind->second);
			ulSystemKey = pSubSystem->getSystemKey();
		}
		else
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Cann't find system_key where subsystem_key=%ul, please check configuration",  subsystemKey);
		
		FUNCTION_EXIT;
		return ulSystemKey;
	}

	std::string SubsystemDataCache::getSystemNameFromSubsystem(unsigned long subsystemKey)
	{
		FUNCTION_ENTRY("getSystemFormSubsystem");
		std::string strSystemName="";
		SystemT::iterator itrSystem;
		SubSystemT::iterator itFind = m_subSystems.find(subsystemKey);
		if (m_subSystems.end() == itFind)
		{			
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Cann't find system_key where subsystem_key=%ul, please check configuration",  subsystemKey);
			FUNCTION_EXIT;
			return strSystemName;
		}
		
		TA_Base_Core::Subsystem* pSubSystem = dynamic_cast<TA_Base_Core::Subsystem *>(itFind->second);

		itrSystem = m_systems.find(pSubSystem->getSystemKey());
		if (m_systems.end() == itrSystem)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Can't find system_key where subsystem_key=%ul, please check configuration",  subsystemKey);
			FUNCTION_EXIT;
			return strSystemName;
		}
		TA_Base_Core::SystemData* pSystem = itrSystem->second;
		strSystemName = pSystem->getName();

		FUNCTION_EXIT;
		return strSystemName;
	}

	std::string SubsystemDataCache::getSubSystemName(unsigned long subsystemKey)
	{
		FUNCTION_ENTRY("getSystemFormSubsystem");
		std::string strRetVal = "";

		SubSystemT::iterator itFind = m_subSystems.find(subsystemKey);
		if (m_subSystems.end() != itFind)
		{
			TA_Base_Core::Subsystem* pSubSystem = dynamic_cast<TA_Base_Core::Subsystem *>(itFind->second);
			strRetVal = pSubSystem->getName();
		}
		else
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Cann't find system_key where subsystem_key=%ul, please check configuration",  subsystemKey);
		
		FUNCTION_EXIT;
		return strRetVal;
	}

	std::vector<TA_Base_Core::ISubsystem*> SubsystemDataCache::getSubsystemsFromSystems(std::vector<unsigned long> systemKeys)
	{
		FUNCTION_ENTRY("getSubsystemsFormSystems");
		std::vector<TA_Base_Core::ISubsystem*> vecRetVal;
		if (systemKeys.size()==0)
		{
			std::transform(m_subSystems.begin(), m_subSystems.end(), std::back_inserter(vecRetVal),
				select2nd<SubSystemT::value_type>());
			return vecRetVal;
		}
		
		for(std::vector<unsigned long>::iterator systemItr=systemKeys.begin();systemItr!=systemKeys.end();systemItr++)
		{
			SubSystemT::iterator itSubSystem;

			itSubSystem = m_subSystems.find(*systemItr);
			if (itSubSystem != m_subSystems.end())
			{
				vecRetVal.push_back(itSubSystem->second);
			}
		}
		
		FUNCTION_EXIT;
		return vecRetVal;
	}

	SubsystemDataCache::~SubsystemDataCache()
	{
		//release the memory
		SubSystemT::iterator itr;
		for (itr=m_subSystems.begin(); itr != m_subSystems.end(); itr++)
		{
			delete itr->second;
		}
	}
  
	void SubsystemDataCache::InitializeSystem()
	{
		std::vector<TA_Base_Core::SystemData*> allSystemData;
		try
		{
			allSystemData = TA_Base_Core::SystemAccessFactory::getInstance().getAllSystems();
			for (int i=0; i < allSystemData.size(); i++)
			{
				m_systems.insert(SystemT::value_type(allSystemData[i]->getPkey(), allSystemData[i]));
			}
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", 
				"Errors occured while trying to retrieve subsystems matching the one passed in");
			for (unsigned int i = 0;i < allSystemData.size(); ++i)
			{
				delete allSystemData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", 
				"Errors occured while trying to retrieve the severity information from the database.");
			for (unsigned int i = 0;i < allSystemData.size(); ++i)
			{
				delete allSystemData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "UnknownException", 
				"Unknown error whilst retrieving subsystem information!");
			for (unsigned int i = 0;i < allSystemData.size(); ++i)
			{
				delete allSystemData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving subsystem information."));
		}
		return;
	}
}
