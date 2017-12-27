/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssSimulationModuleFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  GssSimulationModuleFactory.cpp
//  Implementation of the Class GssSimulationModuleFactory
//  Created on:      17-Jun-2004 17:56:45 PM
///////////////////////////////////////////////////////////

#include "bus/generic_server_simulator/src/GssSimulationModuleFactory.h"
#include "bus/generic_server_simulator/src/GssAbstractModuleCreator.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"

#include <stdexcept>

using namespace TA_Base_Bus;

/**
 * Searches through the map of registered ModuleCreator classes to try and create
 * the appropriate module
 */
TA_Base_Bus::ISimulationModule * GssSimulationModuleFactory::createSimulationModule(const std::string & type, const std::string & name, const std::string & parameters)
{
	TA_Base_Bus::ISimulationModule * module = 0;
    ModuleCreatorMap::iterator found = m_registeredModuleCreators.find(type);

    if (found != m_registeredModuleCreators.end())
    {
		TA_Base_Bus::GssAbstractModuleCreator * creator = (*found).second;
        module = creator->createSimulationModule(name, parameters);
    }
    else
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Couldn't find ModuleCreator for type %s", type.c_str());
    }

	return module;
}


void GssSimulationModuleFactory::registerModuleCreator(const std::string & type, GssAbstractModuleCreator * creator)
{
    ModuleCreatorMap::iterator found = m_registeredModuleCreators.find(type);

    if (found == m_registeredModuleCreators.end())
    {
        ModuleCreatorMap::value_type value_pair(type, creator);
        m_registeredModuleCreators.insert(value_pair);
    }
    else
    {
        std::string err_str="ModuleCreator already registered for type \"";
        err_str.append(type);
        err_str.append("\"");
        TA_THROW(std::logic_error(err_str.c_str()));
    }
}

void GssSimulationModuleFactory::deregisterModuleCreator(const std::string & type, GssAbstractModuleCreator * creator)
{
    ModuleCreatorMap::iterator found = m_registeredModuleCreators.find(type);

    if ( (found != m_registeredModuleCreators.end())
        && ( (*found).second == creator )
       )
    {
        m_registeredModuleCreators.erase(type);
    }
    else
    {
        std::string err_str="ModuleCreator not found of type \"";
        err_str.append(type);
        err_str.append("\" and instance=");
        err_str.append(GssUtility::num2Str((int)creator, "%0.8X"));
        TA_THROW(std::logic_error(err_str.c_str()));
    }
}

int GssSimulationModuleFactory::getRegisteredModuleNames(std::list<std::string> & name_list)
{
    for ( ModuleCreatorMap::iterator iter = m_registeredModuleCreators.begin() ;
            iter != m_registeredModuleCreators.end() ;
            iter++
        )
    {
        name_list.push_back((*iter).first);
    }

    return name_list.size();
}

GssSimulationModuleFactory & GssSimulationModuleFactory::getInstance()
{
	if ( m_instance == 0 )
	{
		m_instance = new GssSimulationModuleFactory;
	}

    return *m_instance;
}


GssSimulationModuleFactory::GssSimulationModuleFactory()
{
}



GssSimulationModuleFactory::~GssSimulationModuleFactory()
{
}

GssSimulationModuleFactory * GssSimulationModuleFactory::m_instance = 0;
