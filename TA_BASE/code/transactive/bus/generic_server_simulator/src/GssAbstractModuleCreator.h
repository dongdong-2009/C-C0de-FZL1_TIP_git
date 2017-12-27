/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssAbstractModuleCreator.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  GssAbstractModuleCreator.h
//  Implementation of the Class GssAbstractModuleCreator
//  Created on:      17-Jun-2004 17:57:14 PM
///////////////////////////////////////////////////////////

#if !defined(GssAbstractModuleCreator_4738D7DF_634A_4103_82E4_4D522801C21C__INCLUDED_)
#define GssAbstractModuleCreator_4738D7DF_634A_4103_82E4_4D522801C21C__INCLUDED_

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // #ifdef WIN32

#include <string>

namespace TA_Base_Bus
{

    class ISimulationModule;

    class GssAbstractModuleCreator
    {

    public:
        virtual ~GssAbstractModuleCreator();

        virtual ISimulationModule * createSimulationModule(const std::string & name, const std::string & parameters) =0;

    protected:
        GssAbstractModuleCreator(const std::string & type);

    }; // class GssAbstractModuleCreator

} // namespace TA_Base_Bus

#endif // !defined(GssAbstractModuleCreator_4738D7DF_634A_4103_82E4_4D522801C21C__INCLUDED_)
