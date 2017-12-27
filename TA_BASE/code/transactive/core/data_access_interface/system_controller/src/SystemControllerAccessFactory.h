/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/SystemControllerAccessFactory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * SystemControllerAccessFactory is a singleton that is
  * used to retrieve ISystemControllerData objects from
  * the database.
  *
  */

#ifndef SYSTEMCONTROLLERACCESSFACTORY_H
#define SYSTEMCONTROLLERACCESSFACTORY_H

#include <string>
#include <vector>

namespace TA_Base_Core
{
    class ISystemControllerData;
    class IConfigSystemController;

    class SystemControllerAccessFactory
    {

    public:
        virtual ~SystemControllerAccessFactory() {};

		/**
		  * getInstance
		  *
		  * Creates and returns an instance of this object.
		  *
		  * @return A reference to an instance of an SystemControllerAccessFactory object.
		  */
		static SystemControllerAccessFactory& getInstance();

        /** 
          * getSystemControllerDataByName
          *
          * Returns a pointer to an ISystemControllerData containing data
          * about the System Controller with the given name.
          *
          * @return An object conforming to the ISystemControllerData interface.
          *
          * @param name       The hostname of the System Controller.
          *
          * @exception DataException      Data was not found, or is not unique.
          * @exception DatabaseException  Problem communicating with the database.
          */
        ISystemControllerData* getSystemControllerDataByName(std::string name);

        /** 
          * getSystemControllersInGroup
          *
          * Returns the hostnames of the System Controllers that belong to a
          * particular group.
          *
          * @return A vector of hostnames.
          *
          * @param groupName  The name of the group.
          */
        std::vector<std::string> getSystemControllersInGroup(std::string groupName);


        /** 
          * getAllSystemControllers
          *
          * Returns all the System Controllers
          *
          * @param bool - this indicates if a configuration version (true) of the system controller
          *               objects should be returned or a read-only version (false). The default is
          *               a read-only version.
          *
          * @return A vector of system controllers
          *
          * @exception DatabaseException  Problem communicating with the database.
          * @exception DataException Problem converting the data retrieved
          */
        std::vector<ISystemControllerData*> getAllSystemControllers(bool isReadWrite = false);


        /**
          * createSystemController
          * 
          * Creates a new configuration System Controller object.
          *
          * @return The configuration System Controller object as a pointer to an IConfigSystemController object.
          *
          * @exception DatabaseException Thrown if there is a database problem.
          * @exception DataException Thrown if there is bad data.
          */
        IConfigSystemController* createSystemController();


        /**
         * copySystemController
         * 
         * Creates a new configuration System Controller object using the group passed in
         *
         * @param IConfigGroupMonitor* The System Controller to copy
         *
         * @return The configuration System Controller object as a pointer to an IConfigSystemController object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigSystemController* copySystemController(const IConfigSystemController* systemControllerToCopy);

    
    private:
        SystemControllerAccessFactory() {};
		SystemControllerAccessFactory(const SystemControllerAccessFactory&);
		SystemControllerAccessFactory& operator=(const SystemControllerAccessFactory &);

        static SystemControllerAccessFactory* m_instance;
    };

} // namespace TA_Base_Core

#endif // SYSTEMCONTROLLERACCESSFACTORY_H
