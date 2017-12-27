/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/IConfigSystemController.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/10/18 11:29:58 $
  * Last modified by:  $Author: Ouyang $
  *
  * This interface contains methods to access the data
  * in the sn_system_controller table.
  *
  */

#ifndef ICONFIGSYSTEMCONTROLLER
#define ICONFIGSYSTEMCONTROLLER

#include <string>
#include <vector>
#include <set>
#include <map>

#include "core/data_access_interface/system_controller/src/ISystemControllerData.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_Base_Core
{
    class IConfigSystemController: public ISystemControllerData, virtual public IConfigItem
    {

    public:
        enum OperatingMode
        {
            CONTROL =0,
            MONITOR =1
        };


        // This is a map of processes based on entity key
        typedef std::map<unsigned long, OperatingMode> ProcessMap;


        /** 
          * setAgentPollPeriod
          *
          * Sets the period, in seconds, between polls to an agent
          * managed by this System Controller.
          *
          * @param The poll period, in seconds.
          *
          */
        virtual void setAgentPollPeriod(unsigned long agentPollPeriod) = 0;


        /** 
          * setHeartbeatSendPeriod
          *
          * Sets the interval, in seconds, between heartbeats that this
          * System Controller sends to peers and other System Controllers
          * monitoring the group it is in (if any).
          *
          * @param The interval between heartbeats, in seconds.
          *
          */
        virtual void setHeartbeatSendPeriod(unsigned long heartbeatSendPeriod) = 0;
        virtual void setHeartbeatSendNumber(unsigned long heartbeatSendNumber) = 0;


        /**
         *  setAgentResponseTimeout
         *
         *  Sets the time an agent has to respond to a status
         *  change request
         *  
         *  @param The time in seconds an agent has to respond when it
         *         is asked to change state
         */
        virtual void setAgentResponseTimeout(unsigned long agentResponseTimeout) =0; 

        
        /**
          * setServerEntityKey
          *
          * Sets the entity key of the Server entity for this System
          * Controller.
          *
          * @param The entity key of the server
          */
        virtual void setServerEntityKey(unsigned long serverEntityKey) = 0;


        /** 
          * setGroupId
          *
          * Sets the primary key (snscgr_id) of the group that this 
          * System Controller is in, or 0 if it is not in any group.
          *
          * @param The primary key of the group.
          *
          */
        virtual void setGroupId(unsigned long groupId) = 0;


        /** 
          * setDebugLogDir
          *
          * Sets the directory that the System Controller should put
          * log files in.
          *
          * @param The directory.
          *
          */
        virtual void setDebugLogDir(const std::string& debugLogDir) = 0;


        /** 
          * setBinDir
          *
          * sets the directory that the System Controller can find its binaries
          * 
          *
          *
          * @exception DatabaseException
          * @exception DataException
          */
        virtual void setBinDir(const std::string& binDir) = 0;

        /**
         * setMonitoredGroup
         *
         * Changes group monitored by the system controller
         *
         * @param unsigned long - This is the key of the group to add or remove from the monitor list
         * @param bool - This indicates whether this group is to be added or removed. True if the
         *               group is to be added to the monitor list, false if the group is to be
         *               removed.
         */
        virtual void setMonitoredGroup( unsigned long groupKey, bool isMonitored) =0;


        /**
         * setProcess
         *
         * This will add or remove a process that is to be run by the System Controller
         *
         * @param unsigned long - The entity key of the process to run
         * @param OperatingMode - Whether the process should run as monitor or control as default
         * @param bool - Whether we are adding or removing this process. True to add, false to remove
         */
        virtual void setProcess(unsigned long key, OperatingMode mode, bool isRun) =0;
        

        /**
         * getServerName
         *
         * This retrieves the name of the server
         *
         * @return A matching name from the ENTITY table
         */
        virtual std::string getServerByName() =0;

        
        /**
         * getMonitoredGroups
         *
         * Retrieves a list of all monitored groups keys
         *
         * @param set<unsigned long> - The group keys of all monitored groups.
         */
        virtual std::set<unsigned long> getMonitoredGroups() =0;

        
        /**
         * getProcesses
         *
         * Retrieves a list of all processes run by this system controller
         *
         * @param ProcessMap - The proceses run by this System Controller
         */
        virtual ProcessMap getProcesses() =0;

        
        /**
         * getDateCreated
         *
         * Returns the date created for this system controller
         *
         * @return The date created for this system controller as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this system controller.
         *
         * @return The date modified for this system controller as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;


        /**
         * deleteThisSystemController
         *
         * Removes this system controller from the database. 
         * The calling application MUST then delete this system controller object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this group was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This group has not been deleted
         */
        virtual void deleteThisSystemController() =0;


        virtual ~IConfigSystemController() {};
    };

} // namespace TA_Base_Core

#endif // ICONFIGSYSTEMCONTROLLER
