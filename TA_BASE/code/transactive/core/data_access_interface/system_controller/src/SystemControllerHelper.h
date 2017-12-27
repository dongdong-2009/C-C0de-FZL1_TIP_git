/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/SystemControllerHelper.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/10/18 11:29:58 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class contains the data associated with one 
  * entry in the sn_system_controller table.
  *
  */

#ifndef SYSTEMCONTROLLER_HELPER
#define SYSTEMCONTROLLER_HELPER

#include <string>

namespace TA_Base_Core
{
    class SystemControllerHelper
    {

    public:
        /**
         * Constructor
         *
         * Creates an object for an existing System Controller
         */
        SystemControllerHelper(unsigned long key);


        /**
         * Constructor
         *
         * Creates an object for a new System Controller
         */
        SystemControllerHelper();


        /**
         * Copy Constructor
         *
         * Creates an object of a new System Controller copied from an existing one
         * This should only be used by configuration System Controller objects.
         */
        SystemControllerHelper( const SystemControllerHelper& );


        virtual ~SystemControllerHelper();
  

        /**
         * isNew
         * 
         * @return Returns whether or not this object is new (meaning it has not yet been applied to the database
         */
        bool isNew() const
        {
            return m_isNew;
        }


        /** 
          * getKey
          *
          * Returns the snscon_id primary key for this System Controller.
          *
          * @return The key as an unsigned long.
          *
          */  
        unsigned long getKey();

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
        std::string getName();

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
        unsigned long getAgentPollPeriod();

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
        unsigned long getHeartbeatSendPeriod();
        unsigned long getHeartbeatSendNumber();

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
        unsigned long getServerEntityKey();

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
         unsigned long getGroupId();

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
         std::string getDebugLogDir();

        /** 
          * getBinDir
          *
          * Returns the directory that the System Controller can find its binaries
          * 
          *
          * @return The directory.
          *
          * @exception DatabaseException
          * @exception DataException
          */
         std::string getBinDir();

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
        time_t getDateCreated();


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
        time_t getDateModified();



        /**
         * getGroupName
         *
         * This retrieves the name of the group from the key
         *
         * @param A key from the SN_SYSTEM_CONTROLLER_GROUP table
         *
         * @return A matching name from the SN_SYSTEM_CONTROLLER_GROUP table
         */
        std::string getGroupName(unsigned long key);


        /**
         * getEntityName
         *
         * This retrieves the name of an entity from the key. This can be used to get server or
         * process names.
         *
         * @param A key from the ENTITY table
         *
         * @return A matching name from the ENTITY table
         */
        std::string getEntityName(unsigned long key);

        
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
        void setName(const std::string& name);

        
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
        void setAgentPollPeriod(unsigned long agentPollPeriod);


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
        void setHeartbeatSendPeriod(unsigned long heartbeatSendPeriod);
        void setHeartbeatSendNumber(unsigned long heartbeatSendNumber);


        /**
         *  setAgentResponseTimeout
         *
         *  Sets the time an agent has to respond to a status
         *  change request
         *  
         *  @param The time in seconds an agent has to respond when it
         *         is asked to change state
         */
        void setAgentResponseTimeout(unsigned long agentResponseTimeout);

        
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
        void setServerEntityKey(unsigned long serverEntityKey);


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
        void setGroupId(unsigned long groupId);


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
        void setDebugLogDir(const std::string& debugLogDir);


        /** 
          * setBinDir
          *
          * Sets the directory that the System Controller can find its binaries
          * 
          *
          *
          * @exception DatabaseException
          * @exception DataException
          */
        void setBinDir(const std::string& binDir);

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
         * pre: Either - this system controller was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This system controller has not been deleted
         */
        void deleteThisSystemController();


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
        void applyChanges();


        /**
          * invalidate
          *
          * Mark the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */
        void invalidate();

    private:
		SystemControllerHelper& operator=( const SystemControllerHelper& );

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
         * modifyExisting
         *
         * This will update an existing system controller in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the system controller object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExisting();

        
        /**
         * addNew
         *
         * This will add a new system controller in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the system controller object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNew();



        bool m_isValidData;
        bool m_isNew;

        unsigned long m_key;
        std::string m_name;
        unsigned int m_agentPollPeriod;
        unsigned int m_heartbeatSendPeriod;
        unsigned int m_heartbeatSendNumber;
        unsigned int m_agentResponseTimeout;
        unsigned long m_serverEntityKey;
        unsigned int m_groupId;
        std::string m_debugLogDir;
        std::string m_binDir;
        time_t m_dateCreated;
        time_t m_dateModified;
    };

} // namespace TA_Base_Core

#endif // SYSTEMCONTROLLER_HELPER
