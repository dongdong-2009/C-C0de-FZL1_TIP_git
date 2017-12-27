/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/GroupMonitorAccessFactory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is a singleton that is used to retrieve
  * data from the sn_group_monitor table.
  *
  */

#ifndef GROUPMONITORACCESSFACTORY_H
#define GROUPMONITORACCESSFACTORY_H

#include <vector>
#include <string>

namespace TA_Base_Core
{
    class IConfigGroupMonitor;

    class GroupMonitorAccessFactory
    {

    public:

        virtual ~GroupMonitorAccessFactory() {}

        /**
		  * getInstance
		  *
		  * Creates and returns an instance of this object.
		  *
		  * @return A reference to an instance of a
          *         GroupMonitorAccessFactory object.
		  */
		static GroupMonitorAccessFactory& getInstance();

        /** 
          * getGroupsMonitoredBySystemController
          *
          * Returns the groups monitored by the System Controller with
          * the given name.
          *
          * @return A vector of group names.
          *
          * @param hostname  The hostname of the System Controller.
          *
          * @exception DatabaseException  Problem communicating with the database.
          */
        std::vector<std::string> getGroupsMonitoredBySystemController(std::string hostname);

        /** 
          * getSystemControllersMonitoringGroup
          *
          * Returns the hostnames of the System Controllers that are monitoring
          * the group with the given name.
          *
          * @return A vector of System Controller hostnames.
          *
          * @param groupName  The name of the group.
          *
          * @exception DatabaseException  Problem communicating with the database.
          */
        std::vector<std::string> getSystemControllersMonitoringGroup(std::string groupName);

        
        /**
         * getGroupName
         *
         * This returns a group name based on the key passed in.
         *
         * @param A key into the SN_SYSTEM_CONTROLLER_GROUP table.
         */
        std::string getGroupName(unsigned long key );


        /** 
          * getAllMonitorGroups
          *
          * Returns all the monitor groups for configuration
          *
          * @return A vector of group monitors for configuration
          *
          * @exception DatabaseException  Problem communicating with the database.
          * @exception DataException Problem converting the data retrieved
          */
        std::vector<IConfigGroupMonitor*> getAllMonitorGroups();


        /**
          * createGroup
          * 
          * Creates a new configuration group object.
          *
          * @return The configuration group object as a pointer to an IConfigGroupMonitor object.
          *
          * @exception DatabaseException Thrown if there is a database problem.
          * @exception DataException Thrown if there is bad data.
          */
        IConfigGroupMonitor* createGroup();


        /**
         * copyGroup
         * 
         * Creates a new configuration group object using the group passed in
         *
         * @param IConfigGroupMonitor* The group to copy
         *
         * @return The configuration group object as a pointer to an IConfigGroupMonitor object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigGroupMonitor* copyGroup(const IConfigGroupMonitor* groupToCopy);


    private:

        GroupMonitorAccessFactory() {};
		GroupMonitorAccessFactory(const GroupMonitorAccessFactory&);
		GroupMonitorAccessFactory& operator=(const GroupMonitorAccessFactory &);

        static GroupMonitorAccessFactory* m_instance;

    };

} // namespace TA_Base_Core

#endif // GROUPMONITORACCESSFACTORY_H
