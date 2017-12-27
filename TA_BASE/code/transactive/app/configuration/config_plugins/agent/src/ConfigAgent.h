/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/ConfigAgent.h $
 * @author Dirk McCormick and Karen Graham
 * @version $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by: $Author: builder $
 * 
 * This provides the configuration for a monitored process entity and it's child agent entity
 */


#ifndef CONFIG_AGENT_H
#define CONFIG_AGENT_H

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"

namespace TA_Base_App
{

    class ConfigAgent : public virtual TA_Base_Core::IConfigItem, public virtual TA_Base_Core::IItem
	{
	public:

        virtual ~ConfigAgent();


        /**
         * Constructor
         * 
         * @param IConfigEntity& - An existing monitored process entity that we should store. From this
         *                         we can attempt to find a related agent entity
         */
        ConfigAgent(TA_Base_Core::IConfigEntity& monitoredProcessEntity);

        /** 
         * Constructor
         * 
         * Create new agent
         *
         * @param string - The entity type of the new agent that will be linked to the monitored process
         *                 If this is an empty string then this monitored process will not have an agent
         */
        ConfigAgent(const std::string& type);


        /**
         * Copy Constructor
         */
        ConfigAgent( const ConfigAgent& agent);


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         * This map is a collection of changes from the monitored process entity and the agent entity
         * with no duplicates.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges();


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
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in teh database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged();

        
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
         * getAgentEntity
         *
         * Most monitored processes have an associated entity
         * This will return that related entity
         *
         * @return IConfigEntity* - The entity related to this monitored process. If the monitored process
         * does not have a related entity this will return NULL.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the key is invalid 
         */
        virtual TA_Base_Core::IConfigEntity* getAgentEntity()
        {
            return m_agentEntity;
        }


        /**
         * getMonitoredProcessEntity
         *
         * This will return the monitored process entity.
         * This will retrieve that entity and return it.
         *
         * @return IConfigEntity& - The entity for this monitored process.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the key is invalid 
         */
        virtual TA_Base_Core::IConfigEntity& getMonitoredProcessEntity()
        {
            return *m_monitoredProcessEntity;
        }

        
        /**
         * getKey
         *
         * Retrieve the pkey of the monitored process entity
         *
         * @return unsigned long - the key of the monitored process entity
         */
        virtual unsigned long getKey();
 
 
 
        /**
         * getAddress
         *
         * Returns the address for this entity.
         *
         * @return The address for this entity as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - setAddress() has already been called
         *      This entity has not been deleted
         */
        virtual std::string getAddress();


        /**
         * getName
         *
         * Returns the monitored process description for this entity. This is the 'nice' string
         * that we need to display
         * For ConfigAgent's the description functions almost like a name as it is shown in the tree.
         *
         * @return The name for this entity as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - setName() has already been called
         *      This entity has not been deleted
         */
        virtual std::string getName();

        
        /**
         * getAgentName
         *
         * Returns the monitored process / agent name for this entity.
         *
         * @return The name for this entity as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - setName() has already been called
         *      This entity has not been deleted
         */
        virtual std::string getAgentName();

        
        /**
         * getDescription
         *
         * Returns the description for this entity.
         *
         * @return The description for this entity as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - setDescription() has already been called
         *      This entity has not been deleted
         */
        virtual std::string getDescription();


       /**
         * getLocation
         *
         * Returns the location for this entity.
         *
         * @return The location for this entity as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
		 *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - setRegion() has already been called
		 *			This entity has not been deleted
         */
        virtual unsigned long getLocation();


        /**
         * getLocationName
         *
         * Returns the location name for this entity.
         *
         * @return The location name for this entity as a string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
		virtual std::string getLocationName();


       /**
         * getSubsystem
         *
         * Returns the subsystem for the agent entity. If that does not exist it returns the subsystem
         * for the monitored process entity.
         *
         * @return The subsystem for this entity as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
		 *
         * pre: Either - this entity was initially loaded from the database
		 *			This entity has not been deleted
         */
        virtual unsigned long getSubsystem();


        /**
         * getSubsystemName
         *
         * Returns the subsystem name for the agent entity. If that does not exist it returns the subsystem
         * for the monitored process entity.
         *
         * @return The location name for this entity as a string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
		virtual std::string getSubsystemName();


        /**
         * getExecutable
         *
         * Returns the executable for this monitored Process.
         *
         * @return The executable for this monitored Process as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getExecutable();


        /**
         * setLocation
         *
         * Sets the location of this entity. This should only be used by the ConfigEntity class.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param location The location to give this entity.
         */
        virtual void setLocation(const unsigned long location);


        /**
         * setAddress
         *
         * Sets the address of this entity locally.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param address The address to give this entity.
         */
        virtual void setAddress(const std::string& address);


        /**
         * setName
         *
         * Sets the description (no not a typo) of the agent and monitored process entity locally. 
         * For ConfigAgent's the description functions almost like a name as it is shown in the tree.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved., or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param name The name to give this entity
         */
        virtual void setName(const std::string& name);


        /**
         * setAgentName
         *
         * Sets the name of the agent and monitored process entity locally. 
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved., or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param name The name to give this entity
         */
        virtual void setAgentName(const std::string& name);

        
        /**
         * setDescription
         *
         * Sets the description of this entity locally. 
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved., or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param description The description to give this entity
         */
        virtual void setDescription(const std::string& description);



        /**
         * setExecutable
         *
         * Sets the executable for this monitored Process.
         *
         * @param The executable for this monitored Process as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual void setExecutable(const std::string& executable);


        /**
         * deleteThisAgent
         *
         * Removes this monitored process from the database and it's related entity if necessary
         * The calling application MUST then delete this monitored process object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this monitored process was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This monitored process has not been deleted
         */
        virtual void deleteThisAgent();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Entity object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This entity has not been deleted
         */
        virtual void applyChanges();

        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         * It is a precondition that writeEntityData() has been called prior to calling
         * this method on a new entity, as it does not make any sense to invalidate an
         * entity that has not yet been written to the database.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        virtual void invalidate();


        /**
         * doesAgentEntityExist 
         *
         * Indicates whether there is an entity related to this monitored process/
         *
         * @return bool - true if an agent entity exists that is linked to this monitored process.
         *                false otherwise
         */
        virtual bool doesAgentEntityExist();


    protected:

        /**
         * getAgentEntityFromDB
         *
         * Gets the entity that is related to this monitored prcess from the database.
         *
         * @return IConfigEntity* - The agent entity that is a child of the monitored process. This
         *                          will be NULL if no child entity exists.
         */
        TA_Base_Core::IConfigEntity* getAgentEntityFromDB();


    private:
        const ConfigAgent& operator=(const ConfigAgent&);

        static unsigned long s_nextIdentifier;

        static const std::string MONITORED_PROCESS_NAME_APPEND;
        static const std::string AGENT_NAME_APPEND;

        static const std::string NAME_FIELD_NAME;
        static const std::string ADDRESS_FIELD_NAME;
        static const std::string EXECUTABLE_FIELD_NAME;

        static const std::string IS_MANAGED;

        static const std::string TA_TRUE;
        static const std::string TA_FALSE;

        TA_Base_Core::IConfigEntity* m_monitoredProcessEntity;

        // The child entity of the monitored process entity.
        TA_Base_Core::IConfigEntity* m_agentEntity;

        unsigned long m_uniqueIdentifier;

    };

} //close namespace TA_Base_Core

#endif // CONFIG_AGENT_H
