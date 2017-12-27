/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/ConfigSystemController.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/10/18 11:29:58 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class is an implementation of the IConfigSystemController
  * interface. It provides read and write access to the data stored in a single
  * entry in the sn_system_controller table.
  *
  */

#ifndef CONFIGSYSTEMCONTROLLER_H
#define CONFIGSYSTEMCONTROLLER_H

#include "core/data_access_interface/system_controller/src/IConfigSystemController.h"

namespace TA_Base_Core
{
    class SystemControllerHelper;

    class ConfigSystemController: public IConfigSystemController
    {

    public:

        /** 
          * Constructor
          *
          * Constructs a new ConfigSystemController object.
          *
          * @param key  The primary key of the System Controller.
          */
        ConfigSystemController(unsigned long key);


        /** 
          * Constructor
          *
          * Constructs a new ConfigSystemController object.
          */
        ConfigSystemController();


        /**
         * Constructor
         *
         * Construct a system controller class based around an existing system controller. This will
		 * copy all the paramters of the existing system controller
         *
         * @param theSystemController The system controller to copy
         */
		ConfigSystemController( const ConfigSystemController& theSystemController);            	

        
        /**
          * Destructor
          */
        virtual ~ConfigSystemController();
 

        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the monitor group changes is empty then nothing has changed
            return !m_systemControllerChanges.empty();
        }


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();

        
        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        };

        
        /** 
         * getKey
         *
         * Returns the snscon_id primary key for this System Controller.
         *
         * @return The key as an unsigned long.
         *
         */
        virtual unsigned long getKey();


        /** 
         * getName
         *
         * Returns the hostname of this System Controller.
         *
         * @return The hostname.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual std::string getName();


        /** 
         * getAgentPollPeriod
         *
         * Returns the period, in seconds, between polls to an agent
         * managed by this System Controller.
         *
         * @return The poll period, in seconds.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual unsigned long getAgentPollPeriod();


        /** 
         * getHeartbeatSendPeriod
         *
         * Returns the interval, in seconds, between heartbeats that this
         * System Controller sends to peers and other System Controllers
         * monitoring the group it is in (if any).
         *
         * @return The interval between heartbeats, in seconds.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual unsigned long getHeartbeatSendPeriod();
        virtual unsigned long getHeartbeatSendNumber();


        /**
         *  getAgentResponseTimeout
         *
         *  Returns the time an agent has to respond to a status
         *  change request
         *  
         *  @return The time in seconds an agent has to respond when it
         *          is asked to change state
         */
        virtual unsigned long getAgentResponseTimeout(); 


        /**
         * getServerEntityKey
         *
         * Returns the entity key of the Server entity for this System
         * Controller.
         *
         * @return The entity key.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual unsigned long getServerEntityKey();


        /**
         * getServerName
         *
         * This retrieves the name of the server
         *
         * @return A matching name from the ENTITY table
         */
        virtual std::string getServerByName();

        
        /** 
         * getGroupId
         *
         * Returns the primary key (snscgr_id) of the group that this 
         * System Controller is in, or 0 if it is not in any group.
         *
         * @return The primary key of the group.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual unsigned long getGroupId();


        /** 
         * getGroupByName
         *
         * Returns the name of the group that this System Controller is
         * in, or an empty string if it is not in any group.
         *
         * @return The name of the group, or "".
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual std::string getGroupByName();


        /** 
         * getDebugLogDir
         *
         * Returns the directory that the System Controller should put
         * log files in.
         *
         * @return The directory.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual std::string getDebugLogDir();


        /** 
          * getBinDir
          *
          * Returns the directory that the System Controller can find agent binarys
          *
          * @return The directory.
          *
          */
        virtual std::string getBinDir();

        /**
         * getMonitoredGroups
         *
         * Retrieves a list of all monitored groups keys
         *
         * @param set<unsigned long> - The group keys of all monitored groups.
         */
        virtual std::set<unsigned long> getMonitoredGroups();


        /**
         * getProcesses
         *
         * Retrieves a list of all processes run by this system controller
         *
         * @param ProcessMap - The proceses run by this System Controller
         */
        virtual ProcessMap getProcesses();

        
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
        virtual time_t getDateCreated();


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
        virtual time_t getDateModified();


        /**
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name);

        
        /** 
         * setAgentPollPeriod
         *
         * Sets the period, in seconds, between polls to an agent
         * managed by this System Controller.
         *
         * @param The poll period, in seconds.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual void setAgentPollPeriod(unsigned long agentPollPeriod);


        /** 
         * setHeartbeatSendPeriod
         *
         * Sets the interval, in seconds, between heartbeats that this
         * System Controller sends to peers and other System Controllers
         * monitoring the group it is in (if any).
         *
         * @param The interval between heartbeats, in seconds.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual void setHeartbeatSendPeriod(unsigned long heartbeatSendPeriod);
        virtual void setHeartbeatSendNumber(unsigned long heartbeatSendNumber);


        /**

         *  setAgentResponseTimeout

         *

         *  Sets the time an agent has to respond to a status

         *  change request

         *  

         *  @param The time in seconds an agent has to respond when it

         *         is asked to change state

         */

        virtual void setAgentResponseTimeout(unsigned long agentResponseTimeout); 



        

        /**
         * setServerEntityKey
         *
         * Sets the entity key of the Server entity for this System
         * Controller.
         *
         * @param The entity key of the server
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual void setServerEntityKey(unsigned long serverEntityKey);


        /** 
         * setGroupId
         *
         * Sets the primary key (snscgr_id) of the group that this 
         * System Controller is in, or 0 if it is not in any group.
         *
         * @param The primary key of the group.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual void setGroupId(unsigned long groupId);


        /** 
         * setDebugLogDir
         *
         * Sets the directory that the System Controller should put
         * log files in.
         *
         * @param The directory.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual void setDebugLogDir(const std::string& debugLogDir);

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
        virtual void setBinDir(const std::string& binDir);

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
        virtual void setMonitoredGroup( unsigned long groupKey, bool isMonitored);

        
        /**
         * setProcess
         *
         * This will add or remove a process that is to be run by the System Controller
         *
         * @param unsigned long - The entity key of the process to run
         * @param OperatingMode - Whether the process should run as monitor or control as default
         * @param bool - Whether we are adding or removing this process. True to add, false to remove
         */
        virtual void setProcess(unsigned long key, OperatingMode mode, bool isRun);

        
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
        virtual void deleteThisSystemController();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();


        /**
         * invalidate
         *
         * Make the data contained by this System Controller as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    
    private:
        /**
         * updateChanges
         *
         * This updates the map recording the changes to the system controller. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        void updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);


        /**
         * getGroupNames
         * 
         * This takes a set of group keys and converts it into a comma separated list of
         * strings. This can be used for audit messages.
         *
         * @param set<unsigned long> - A set of group keys
         * 
         * @return string - A comma separated list of group names. This is suitable to be displayed to the user.
         */
        std::string getGroupNames(std::set<unsigned long>& groupKeys);

        
        /**
          * reload()
          *
          * This method reloads the data from the database. It is called when a get
          * method is called and the data state is not valid.
          *
          * @exception DatabaseException
          * @exception DataException
          */
        void reload();


        /**
         * writeMonitoredGroupChanges
         *
         * This will update the monitored groups in the database based on what is stored in the
         * sets m_originalMonitoredGroups and m_newMonitoredGroups
         *
         * @exception DatabaseException
         */
        void writeMonitoredGroupChanges();


        /**
         * writeProcessChanges
         *
         * This will update the processes in the database based on what is stored in the
         * database and in the map m_processes
         *
         * @exception DatabaseException
         */
        void writeProcessChanges();



        // Assignment operator not used so it is made private
		ConfigSystemController& operator=(const ConfigSystemController&);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string AGENT_POLL_PERIOD;
        static const std::string HEARTBEAT_SEND_PERIOD;
        static const std::string HEARTBEAT_SEND_NUMBER;

        static const std::string AGENT_RESPONSE_TIMEOUT;
        static const std::string SERVER;
        static const std::string GROUP;
        static const std::string DEBUG_LOG_DIR;
        static const std::string BIN_DIR;
        static const std::string MONITORED_GROUPS;
        static const std::string PROCESS;

        static const std::string ADDED_CONTROL;
        static const std::string ADDED_MONITOR;
        static const std::string REMOVED;

        static long s_nextAvailableIdentifier; // The next unique identifier available

        bool m_isValidData;

        SystemControllerHelper* m_helper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of monitor groups in the
                                          // Configuration Editor because new monitor groups do not have keys.
                                          // Therefore we use this method for identification.

        std::set<unsigned long> m_originalMonitoredGroups; // This is a set of all the original groups monitored
                                                           // by this system controller. This is used to determine
                                                           // whether changes have been made and allows us to report
                                                           // those changes.
        std::set<unsigned long> m_newMonitoredGroups;      // This is a set of all the group changes. This allows us
                                                           // to compare the sets to see if any changes have been made.

        ProcessMap m_processes; // This is the list of processes this System controller must run.

        ItemChanges m_systemControllerChanges; // This is a map of all changes made to the monitor group so that they are
                                    // recorded and can be sent in audit or online update messages.
        

    };

} // namespace TA_Base_Core

#endif // CONFIGSYSTEMCONTROLLER_H
