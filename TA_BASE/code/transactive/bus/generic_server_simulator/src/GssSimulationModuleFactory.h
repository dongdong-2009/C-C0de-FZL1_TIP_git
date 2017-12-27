/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssSimulationModuleFactory.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  GssSimulationModuleFactory.h
//  Implementation of the Class GssSimulationModuleFactory
//  Created on:      17-Jun-2004 17:56:45 PM
///////////////////////////////////////////////////////////

#if !defined(GssSimulationModuleFactory_E48D631A_D416_4751_B305_DCAA1DCC4066__INCLUDED_)
#define GssSimulationModuleFactory_E48D631A_D416_4751_B305_DCAA1DCC4066__INCLUDED_

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // #ifdef WIN32


#include <map>
#include <list>
#include <string>


namespace TA_Base_Bus
{

    class GssAbstractModuleCreator;

    class ISimulationModule;

    
    /**
     *
     */
    class GssSimulationModuleFactory
    {

    public:
        /**
          *
          */
        static GssSimulationModuleFactory & getInstance();

        /**
          *
          */
        ISimulationModule * createSimulationModule(const std::string & type, const std::string & name, const std::string & parameters);

        /**
          *
          */
        void registerModuleCreator(const std::string & type, GssAbstractModuleCreator * creator);

        /**
          *
          */
        void deregisterModuleCreator(const std::string & type, GssAbstractModuleCreator * creator);

        /** Populates the list with the names of all modules that can be created
          * with ModuleCreators that have registered.
          *
          * @param name_list the list to populate with names
          * @return the number of names in the list
          */
        int getRegisteredModuleNames(std::list<std::string> & name_list);
    private:

        /**
          *
          */
        typedef std::map< std::string, GssAbstractModuleCreator * > ModuleCreatorMap;

        /**
          * A map of pointers to AbstractModuleCreator instances, keyed by their std::
          * string type.
          */
        ModuleCreatorMap m_registeredModuleCreators;


    private:
        /**
          * Hidden constructor, as class is instantiated by self
          */
        GssSimulationModuleFactory();

        /**
          * Hidden destructor, as class is destroyed by self
          */
        virtual ~GssSimulationModuleFactory();

        /**
          * 
          */
        static GssSimulationModuleFactory * m_instance;

    }; // class GssSimulationModuleFactory

} // namespace TA_Base_Bus

#endif // !defined(GssSimulationModuleFactory_E48D631A_D416_4751_B305_DCAA1DCC4066__INCLUDED_)
