/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ConfigEntity.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigEntity is a concrete implementation of IConfigEntity, which is in turn an implementation
  * of IEntityData. It does not provide member variables for parameters, and as such is 
  * generic to all entities. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for entities.
  */


#if !defined(ConfigEntity_E6E0F340_659B_4296_A3FE_DC363DFB78B8__INCLUDED_)
#define ConfigEntity_E6E0F340_659B_4296_A3FE_DC363DFB78B8__INCLUDED_

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"

namespace TA_Base_Core
{

    class ConfigEntity : public IConfigEntity
    {

    public:
        /**
         * ConfigEntity (constructor)
         *
         * Consturcts an instance of ConfigEntity that represents a "new" entity - that is
         * one that does not already exist in the database.
         *
         * @param type The type of entity represented by this ConfigEntity object.
         */
        ConfigEntity(const std::string& type);

        /**
         * ConfigEntity (constructor)
         *
         * Constructs an instance of ConfigEntity for the entity of the specified key.
         *
         * @param key The key into the entity table for which to create the config entity.
         */
        ConfigEntity(const unsigned long key, const std::string& type);


        /**
         * Copy Constructor
         *
         * This copies the ConfigEntity object passed in. It sets up a new ConfigEntity object
         * with the key set to 0 (as it has not yet been applied to the database). This object
         * will be similar to that returned by EntityAccessFactory::createEntity except that
         * the majority of the fields will be populated.
         *
         * @param ConfigEntity& - The object to copy.
         *
         * @exception DatabaseException Thrown if there is a database error.
         */
        ConfigEntity( const ConfigEntity& theConfigEntityToCopy);

        /**
         * Destructor
         */
        virtual ~ConfigEntity();
    
    private:
        ConfigEntity& operator=(const ConfigEntity &);


    public:


       
        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the chagnes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual ItemChanges getAllItemChanges()
        {
            return m_entityChanges;
        }


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
        virtual bool hasChanged()
        {
            // If the entity changes is empty then nothing has changed
            return !m_entityChanges.empty();
        }

        
        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew()
        {
            return m_isNewEntity;
        }

        
        /**
         * getParameterNames
         *
         * Retrieves a list of all parameter names associated with this entity type.
         *
         * @return The list of all parameter names as a vector of strings
         *
         * @exception DatabaseException If there is an error loading from the database.
         * @exception DataException If there are any duplicate parameters.
         *
         * pre: This entity has not been deleted
         */
        virtual std::vector<std::string> getParameterNames();


        /**
         * getParameterValue
         *
         * This retrieves the value of a parameter. The parameter name is provided as an argument.
         *
         * @param name The name of the parameter for which to return the value.
         *
         * @return The value of the named parameter as a string. Any conversion must be 
         * performed by the calling application.
         *
         * @exception DatabaseException If there is an error loading from the database.
         * @exception DataException If there are any duplicate parameters, or if the named
         *            parameter does not exist for this entity.
         *
         * pre: This entity has not been deleted
         */
        virtual std::string getParameterValue(const std::string& name);

        

        /**
         * getParameterDataType
         *
         * This retrieves the data type of the entity parameter.
         *
         * @param name The name of the parameter for which to return the data type.
         *
         * @return The data type of the named parameter as one of the DataType enumerations.
         *
         * @exception DatabaseException If there is an error loading from the database.
         * @exception DataException If there are any duplicate parameters, or if the named
         *            parameter does not exist for this entity.
         *
         * pre: This entity has not been deleted
         */
        virtual DataType getParameterDataType(const std::string& name);


        /**
         * getParameterValidation
         *
         * This retrieves the validation string for this entity parameter.
         *
         * @param name The name of the parameter for which to return the validation string.
         *
         * @return The validation string of the named parameter as a string
         *
         * @exception DatabaseException If there is an error loading from the database.
         * @exception DataException If there are any duplicate parameters, or if the named
         *            parameter does not exist for this entity.
         *
         * pre: This entity has not been deleted
         */
        virtual std::string getParameterValidation(const std::string& name);


        /**
         * getPossibleEnumerationValues
         *
         * This retrieves a map of all possible values for an enumeration entity parameter. 
         *
         * @param name The name of the parameter for which to return the values.
         *
         * @return map<string,string> - The first item in the map is the display name for the user.
         *                              The second is item is the value that will be stored in the database.
         *
         * @exception DatabaseException If there is an error loading from the database.
         * @exception DataException If there are any duplicate parameters, or if the named
         *            parameter does not exist for this entity.
         *
         * pre: This entity has not been deleted
         */
        virtual std::map<std::string,std::string> getPossibleEnumerationValues(const std::string& name);


        /**
         * isParameterMandatory
         *
         * This determines if a parameter is mandatory or not
         *
         * @param name The name of the parameter for which to see if it is mandatory
         *
         * @return bool - true if this parameter is mandatory. false if it is not
         *
         * @exception DatabaseException If there is an error loading from the database.
         * @exception DataException If there are any duplicate parameters, or if the named
         *            parameter does not exist for this entity.
         *
         * pre: This entity has not been deleted
         */
        virtual bool isParameterMandatory(const std::string& name);


        /**
         * isParameterOnlineUpdatable
         *
         * This determines if a parameter is online updateable or whether it requires the 
         * agent in question to be restarted.
         *
         * @param name The name of the parameter for which to see if it is online updatable
         *
         * @return bool - true if this parameter can be updated online. false if it is not
         *
         * @exception DatabaseException If there is an error loading from the database.
         * @exception DataException If there are any duplicate parameters, or if the named
         *            parameter does not exist for this entity.
         *
         * pre: This entity has not been deleted
         */
        virtual bool isParameterOnlineUpdatable(const std::string& name);

        
        /**
         * setParameterValue
         *
         * Sets the value of the named parameter locally
         *
         * @param value The value to give the named parameter.
         * @param name The parameter name to assign the value to.
         *
         * pre: This entity has not been deleted
         */
        virtual void setParameterValue(const std::string& value, const std::string& name);


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
         * deleteThisEntity
         *
         * Removes this entity from the database. 
         * The calling application MUST then delete this entity object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         *      This entity has not been deleted
         */
        virtual void deleteThisEntity();

        /**
         * deleteAndResetEntity
         *
         * Deletes this entity from the database, but flags it as new, so it can continue to be used.
         * This is used when "undoing" a write when a related write fails.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         *      This entity has not been deleted
         */
        virtual void deleteAndResetEntity();

        /**
         * resetEntity
         *
         * Resets the entity flags as new, so it can continue to be used.
         * This is used when "undoing" a write when it has failed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         *      This entity has not been deleted
         */
        virtual void resetEntity();


        /**
         * setName
         *
         * Sets the name of this entity locally.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param name The name to give this entity.
         */
        virtual void setName(const std::string& name);


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
         * setSubsystem
         *
         * Sets the subsystem of this entity locally. This sets the physical subsystem.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param subsystem The Subsystem to give this entity.
         */
        virtual void setSubsystem(const unsigned long subsystem);


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
         * setRegion
         *
         * Sets the region of this entity. This should only be used by the ConfigEntity class.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param region The region to give this entity.
         */
        virtual void setRegion(const unsigned long region);


        /**
         * setParent
         *
         * Sets the parent of this entity.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param parent The parent to give this entity.
         */
        virtual void setParent(const unsigned long parent);
		

        /**
         * setAgent
         *
         * Sets the agent of this entity. This should only be called on very special entity types
         * that allow the agent to be set separately and where the agent/parent heirarchy is not
         * standard.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param parent The agent to give this entity.
         */
        virtual void setAgent(const unsigned long agent);

        
        /**
         * clone
         *
         * Returns a pointer to a new ConfigEntity object that has been initialised with the 
         * specified key.
         * NOTE: This method is not necessary for the use of ConfigEntity, but is provided
         * to satisfy the IEntityData interface.
         *
         * @param key The key with which to construct the Camera object.
         */
        virtual IEntityData* clone(unsigned long key) ;

        /**
         * getKey
         *
         * Returns the key for this entity.
         *
         * @return The key for this entity as an unsigned long.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         *      This entity has not been deleted
         */
        virtual unsigned long getKey();

        /**
         * getTypeKey
         *
         * Returns the type key of this entity.
         *
         * @return The entity type of this entity as an unsigned long
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual unsigned long getTypeKey();

        /**
         * getName
         *
         * Returns the name of this Entity. If this is the first time data for this entity
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this entity as a std::string
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
         * getSubsystem
         *
         * Returns the subsystem for this entity.
         *
         * @return The subsystem for this entity as a std::string.
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
         *      OR     - setSubsystem() has already been called
         *      This entity has not been deleted
         */
        virtual unsigned long getSubsystem();

        /**
         * getSubsystemName
         *
         * Returns the subsystem name for this entity.
         *
         * @return The subsystem name for this entity.
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
         * getPhysicalSubsystem
         *
         * Returns the physical subsystem (not the TransActive subsystem) for this entity.
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
         */
        virtual unsigned long getPhysicalSubsystem();


        /**
         * getPhysicalSubsystemName
         *
         * Returns the physical subsystem name (not the TransActive subsystem) for this entity.
         *
         * @return The subsystem name for this entity.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual std::string getPhysicalSubsystemName();

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
         * getRegion
         *
         * Returns the region for this entity.
         *
         * @return The region for this entity as an unsigned long.
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
        virtual unsigned long getRegion();

        /**
         * getRegionName
         *
         * Returns the region name for this entity.
         *
         * @return The region name for this entity as a string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
		virtual std::string getRegionName();

        /**
         * getParent
         *
         * Returns the parent for this entity.
         *
         * @return The parent for this entity as a std::string.
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
         *      OR     - setParent() has already been called
         *      This entity has not been deleted
         */
        virtual unsigned long getParent();

        /**
         * getParentName
         *
         * Returns the name of the parent entity for this entity.
         * Precondition: If the parent entity key is zero then this method will assert. It should
         *               be checked first to see if the parent key is 0.
         *
         * @return The parent name for this entity as a string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual std::string getParentName();

        /**
         * getAgent
         *
         * Returns the agent that this entity 'belongs' to.
         *
         * @return The agent for this entity as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual unsigned long getAgent();	

        /**
         * getAgentName
         *
         * Returns the name of the agent entity for this entity.
         * Precondition: If the agent entity key is zero then this method will assert. It should
         *               be checked first to see if the agent key is 0.
         *
         * @return The agent name for this entity as a string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual std::string getAgentName();

        /**
         * getDateCreated
         *
         * Returns the date created for this entity.
         *
         * @return The date created for this entity as a std::string.
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
         * Returns the date modified for this entity.
         *
         * @return The date modified for this entity as a std::string.
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
         * getAlarmDisplay
         *
         * Returns the AlarmDisplay parameter for this entity
         *
         * @return The value of the AlarmDisplay parameter for this entity as a std::string
         *
         * @exception DatabaseException Thrown if there is a database communication error
         * @excpeiton DataException Thrown if the data in the value column is not as expected
         */
        virtual std::string getAlarmDisplay();


        /**
         * getType
         *
         * Returns the type name for this entity.
         *
         * @return The type name of this entity as an unsigned long
         */
        virtual std::string getType();


		/**
         * getPossibleParentEntities
         *
         * This figures out all the entities that could be a parent of this entity.
         *
         * @return map<unsigned long, string> This returns a map of all entity keys and names.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long)
         */
        virtual std::map<unsigned long, std::string> getPossibleParentEntities();


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
		  * assignDefaultData
		  *
		  * This function will initialise the EntityHelper with valid data
		  *
		  * @param  IData* data, the data as read from the database
		  * @param unsigned long row, the row number to access
		  */
		void assignDefaultData( IData* data, unsigned long row );
        //Mintao++
	    /**
	      * getEntityStatusData
	      * 
	      * This function is to get the related entity status which can dynamically changed
          * when entity is running
	      * @return virtual EntityStatusData 
	      * @param : EEntityStatusType& statustype
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual EntityStatusData getEntityStatusData(EEntityStatusType statustype);
	    /**
	      * getEntityStatusData
	      * 
	      * This function is to get the entity status data for a given status type. 
          * It can be used by specialised or derived classes of EntityData for retrieval 
          * of status info.
	      * @return virtual EntityStatusData 
	      * @param : unsigned int statustype
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual EntityStatusData getEntityStatusData(unsigned int statustype);
	    /**
	      * getBoolEntityStatusValue
	      * 
	      * This function directly returns the bool value of the entity status given the entity
          * status type. 
	      * 
	      * @return virtual bool 
	      * @param : EEntityStatusType& statustype
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual bool getBoolEntityStatusValue(EEntityStatusType statustype);
	    /**
	      * getBoolEntityStatusValue
	      * 
	      * This function directly returns the bool value of the entity status given the entity
          * status type
	      * 
	      * @return virtual bool 
	      * @param : unsigned int statustype
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual bool getBoolEntityStatusValue(unsigned int statustype);
	    /**
	      * setEntityStatusData
	      * 
	      * This function is to set the current status of the entity
	      * 
	      * @return virtual void 
	      * @param : EEntityStatusType& statustype
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setEntityStatusData(const EntityStatusData& entityStatus);
	    /**
	      * setEntityStatusData
	      * 
	      * This function is generic for setting specialised or specific entity status data
	      * 
	      * @return virtual void 
	      * @param : unsigned int statustype
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setEntityStatusData(unsigned int statustype, EStatusDataType datatype, const std::string& statusvalue, time_t dateModified );
        //TD15302 Mintao++

		//TD18520 
		//xxs++ add for MMSController
		/**
         * justApplyEntityStatusChange
         *
         * This applies any changes to the Entity Status Data and do nothing about other entity data
         *
         */
		void justApplyEntityStatusChange();

    protected:
        /**
         * reload()
         *
         * This method is required by the IEntityData interface, however, the private 
         * method loadParameters() is used hear instead of reload()
         */
        void reload(){};

        /**
         * updateEntityChanges
         *
         * This updates the map recording the changes to the entity. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateEntityChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);


		 /**
         * applyEntityStatusChange
         *
         * This applies any changes to the Entity Status Data.
         *
         */
		void applyEntityStatusChange();
    private:

        // These are the strings used when an item has no parent or agent set yet
        static const std::string NO_PARENT;
        static const std::string NO_AGENT;

        // These are the strings that the applications use to represent booleans.
        static const std::string BOOLEAN_TRUE;
        static const std::string BOOLEAN_FALSE;

        // These are the possible entity parameter data types that can be retrieved from the database
        static const std::string INTEGER_DATATYPE;
        static const std::string BOOLEAN_DATATYPE;
        static const std::string STRING_DATATYPE;
        static const std::string ENUMERATION_DATATYPE;
        static const std::string REAL_DATATYPE;

        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string ADDRESS;
        static const std::string DESCRIPTION;
        static const std::string PARENT;
        static const std::string AGENT;
        static const std::string LOCATION;
		static const std::string SE_REGION;
        static const std::string SUBSYSTEM;
        static const std::string PHYSICAL_SUBSYSTEM;

		// These are the neames of the Entity Status Types
	    static const std::string MMS_INHIBIT_STATUS;

        struct ParameterValueType
        {
            std::string type;
            std::string value;
            bool isVisible;
            bool isMandatory;
            bool isOnlineUpdatable;
            DataType dataType;
            std::string validation;
            std::map<std::string,std::string> possibleEnumerationValues;
        };

        void loadParameters();


        /**
         * loadEnumerationValues
         *
         * This takes an SQL string that returns two columns. It then takes the results of the
         * SQL and puts it into the map and returns it.
         *
         * @param string - This is an SQL string that selects two columns. The first should be a 
         *                 value to write to the database and the second is a display name to show to
         *                 the user.
         * @param bool - This indicates if this parameter is mandatory. If it is not mandatory then a blank
         *               option must be added to the map.
         *
         * @return map<string, string> - This is a list of all values and display names. The display
         *                               names will come first in the map.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long)
         */
        std::map<std::string, std::string> loadEnumerationValues(const std::string& validation, const bool isMandatory);


        /**
         * parseEnumerationValues
         *
         * If the enumeration values are not an SQL statement but are a list of items then this method is
         * called to parse them out
         *
         * @param string - the validation string in the format
         *                 [value1;value2;value3][display1,display2,display3]
         *
         * @return - The parsed string into the map. The map will have the following in it:
         *            display1, value1
         *            display2, value2
         *            display3, value3
         */
        std::map<std::string, std::string> parseEnumerationValues(const std::string& validation);


        /**
         * checkDataIsValid
         *
         * This checks all parameters to see that:
         * 1) there are no mandatory parameters missing
         * 2) all values entered are valie
         *
         * @exception DataException This is thrown if either of the checks above fails
         */
        void checkDataIsValid();


        /**
         * convertToDisplayableValue
         *
         * Some parameters have different values for displaying and writing to the database. This takes
         * a value from the database and converts it into it's displayable format.
         *
         * @param ParameterValueType& - This is all the parameter details such as what data type it is.
         *                              This will hold the writeable value in parameterDetails.value and this
         *                              will be overwritten by this method to hold the displayable value.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long)
         */
        void convertToDisplayableValue(ParameterValueType& parameterDetails);


        /**
         * convertToWriteableValue
         *
         * Some parameters have different values for displaying and writing to the database. This takes
         * a displayable value converts it into one that can be written to the database.
         *
         * @param ParameterValueType& - This is all the parameter details such as what data type it is.
         *                              This will hold the displayable value in parameterDetails.value 
         *                              and this should be converted and returned via the return value.
         *                              parameterDetails.value will not be changed. This is not a const
         *                              parameter as if this is an enumerated type another parameter
         *                              could be set.
         *
         * @return std::string - The writeable value.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long)
         */
        std::string convertToWriteableValue(ParameterValueType& parameterDetails);
        /**
         * checkWithinValidRange
         *
         * This method checks if a certain parameter is within the range specified by the EP_VALIDATION
		 * table.  Please note that this method always returns true if the range string does not
		 * follow a specified format.
         *
         * @param std::string& szValueString- This is the parameter value in string form
		 * @param std::string& szRangeString- This is the range in string form
         *
         * @return bool - This is the return value.  Returns false if value is not within range
         *
         */

		bool checkWithinValidRange(const std::string &szValueString, const std::string &szRangeString);


        std::map<std::string,ParameterValueType> m_parameters;

        EntityHelper* m_entityHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of entities in the
                                          // Configuration Editor because new entities do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        bool m_isNewEntity;

        ItemChanges m_entityChanges; // This is a map of all changes made to the entity so that they are
                                       // recorded and can be sent in audit or online update messages.
        
        static const std::string m_alarmDisplayParameter;

    };

    
    typedef boost::shared_ptr<ConfigEntity> ConfigEntityPtr;
} // closes TA_Base_Core

#endif // !defined(ConfigEntity_E6E0F340_659B_4296_A3FE_DC363DFB78B8__INCLUDED_)
