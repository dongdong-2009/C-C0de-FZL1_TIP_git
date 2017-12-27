/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssAbstractModuleCreator.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  GssAbstractModuleCreator.cpp
//  Implementation of the Class GssAbstractModuleCreator
//  Created on:      17-Jun-2004 17:57:14 PM
///////////////////////////////////////////////////////////

#include "bus/generic_server_simulator/src/GssAbstractModuleCreator.h"
#include "bus/generic_server_simulator/src/GssSimulationModuleFactory.h"

#include <stdexcept>

using namespace TA_Base_Bus;

ISimulationModule * GssAbstractModuleCreator::createSimulationModule(const std::string & name, const std::string & parameters)
{
    throw std::logic_error("GssAbstractModuleCreator::createSimulationModule should not be called");

    return 0;
}



GssAbstractModuleCreator::~GssAbstractModuleCreator()
{

}


GssAbstractModuleCreator::GssAbstractModuleCreator(const std::string & type)
{
    GssSimulationModuleFactory::getInstance().registerModuleCreator(type, this);
}


