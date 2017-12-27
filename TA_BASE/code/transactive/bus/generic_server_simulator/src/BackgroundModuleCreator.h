/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/BackgroundModuleCreator.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for the BackgroundModuleCreator class
  */
#ifndef BackgroundModuleCreator_H
#define BackgroundModuleCreator_H

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/GssAbstractModuleCreator.h"

namespace TA_Base_Bus
{

    class ISimulationModule;

    class BackgroundModuleCreator : public GssAbstractModuleCreator
    {

    public:
        virtual ~BackgroundModuleCreator();

        virtual ISimulationModule * createSimulationModule(const std::string & name, const std::string & parameters);

        BackgroundModuleCreator(const std::string & type);

    }; // class BackgroundModuleCreator

} // namespace TA_Base_Bus

#endif // #ifndef BackgroundModuleCreator_H
