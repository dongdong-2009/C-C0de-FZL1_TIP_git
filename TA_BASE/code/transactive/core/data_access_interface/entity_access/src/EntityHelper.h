/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EntityHelper.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * EntityHelper is an object that is held by all type-specific entity objects, as well as by
  * the ConfigEntity. It contains all data common across entity types, and manipulation
  * methods for the data. It helps avoid re-writing code for each entity type object.
  *
  * NOTE: The EntityHelper class works on the assumption that the key passed in the constructor
  * is correct.
  */


#if !defined(EntityHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
#define EntityHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_

#if defined(_MSC_VER)
  #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>

#include "core/data_access_interface/src/IData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/EntityStatusData.h"
namespace TA_Base_Core
{

	class IDatabase;

    class EntityHelper
    {
    public:
		
		typedef std::map< std::string, std::string > EntityParameterNameValueMap;
		typedef std::map< std::string, std::string >::iterator EntityParameterNameValueMapIter;

        /**
         * EntityHelper
         *
         * This constructor creates a new EntityHelper object for the specified key and type name.
         *
         * @param key The key into the ENTITY table for this entity. EntityHelper assumes this
         * key is correct. If it is not, the behaviour of the class will be inconsistent.
         *
         * @param typeName The type name of this entity.
         *
         * pre: The key is correct, and for a valid entity.
         */
        EntityHelper(const unsigned long key, const std::string& typeName);


        /**
         * EntityHelper
         *
         * This constructor creates a new EntityHelper object for use with a ConfigEntity object
         *
         * @param typeName The type name of the entity.
         */
        EntityHelper(const std::string& typeName);


        virtual ~EntityHelper() 
        {
            if (NULL != m_configParameterData)
            {
                delete m_configParameterData;
                m_configParameterData = NULL;
            }
        };

		/** 
		  * assignDefaultData
		  *
		  * This function will initialise the EntityHelper with valid data
		  *
		  * @param  IData* data, the data as read from the database
		  * @param unsigned long row, the row number to access
		  */
		void assignDefaultData( IData* data, unsigned long row );

        /**
         * Copy Constructor
         *
         * This copies the EntityHelper object passed in. It sets up a new EntityHelper object
         * with the key set to 0 (as it has not yet been applied to the database). This object
         * will be similar to that returned by EntityAccessFactory::createEntity except that
         * the majority of the fields will be populated.
         *
         * @param EntityHelper& - The object to copy.
         *
         * @exception DatabaseException Thrown if there is a database error.
         */
        EntityHelper( const EntityHelper& theEntityHelper);

		static bool getBooleanData  ( const std::string & parameterValue );
        static int getIntegerData  ( const std::string & parameterValue );
        static int getIntegerData  ( const std::string & parameterValue, int defaultValue );
		static unsigned long getUnsignedLongData  ( const std::string & parameterValue );
		static double getRealData  ( const std::string & parameterValue );

		/* getNextPkey()
         *
         * This method get a new pkey for insert new entity to entity table
         * 
         * @param unsigned long - the new entity belonged location's key
         * 
         * @return unsigned long - the new entity key
		 *
         * @exception DatabaseException
		 *
         * @exception DataException
         */
        static unsigned long getNextPkey(unsigned long locationKey);

    private:
		EntityHelper& operator=(const EntityHelper &);


    public:
        /**
         * getKey
         *
         * Returns the key for this entity.
         *
         * @return The key for this entity as an unsigned long.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        unsigned long getKey();

        /**
         * getEntityType
         *
         * Returns the type name for this entity.
         *
         * @return The type name of this entity as a string
         */
        std::string getEntityType();

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
        unsigned long getTypeKey();

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
         */
        std::string getName();

        /**
         * setName
         *
         * Sets the name of this entity. This should only be used by the ConfigEntity class
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
        void setName(const std::string& name);

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
         */
        std::string getDescription();

        /**
         * setDescription
         *
         * Sets the description of this entity. This should only be used by the ConfigEntity class
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * @param description The description to give this entity
         */
        void setDescription(const std::string& description);

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
         */
        std::string getAddress();

        /**
         * setAddress
         *
         * Sets the address of this entity. This should only be used by the ConfigEntity class.
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
        void setAddress(const std::string& address);

        /**
         * getSubsystem
         *
         * Returns the subsystem for this entity.
         *
         * @param bool - This determines whether the physical subsystem (non-fixed used for datapoints) or the
         *               fixed TransActive subsystem should be returned (true)
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
         */
        unsigned long getSubsystem(bool isFixed);

        /**
         * getSubsystemName
         *
         * Returns the subsystem name for this entity.
         *
         * @param bool - This determines whether the physical subsystem (non-fixed used for datapoints) or the
         *               fixed TransActive subsystem should be returned (true)
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
        virtual std::string getSubsystemName(bool isFixed);

        
        /**
         * setSubsystem
         *
         * Sets the physical subsystem of this entity. This should only be used by the ConfigEntity class.
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
        void setSubsystem(const unsigned long subsystem);

      
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
        void setLocation(const unsigned long location);


		/**
         * getRegion
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
        void setRegion(const unsigned long region);


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
         */
        unsigned long getParent();

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
         * setParent
         *
         * Sets the parent of this entity. This should only be used by the ConfigEntity class.
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
        void setParent(const unsigned long parent);
		
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
        unsigned long getAgent();	
		
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
        std::string getAgentName();

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
        void setAgent(const unsigned long agent);

        
        /**
         * getDateCreated
         *
         * Returns the date created for this entity.
         *
         * @return The date created for this entity as a time_t.
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
         * Returns the date modified for this entity.
         *
         * @return The date modified for this entity as a time_t.
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
        void invalidate();

		std::string getParameter(const std::string& parameterName);
		
		EntityParameterNameValueMap& getAllParameters();
		
        //std::string getConfigParameter(const std::string& parameterName);

		void assignDefaultParameterValueData( IData* data, unsigned long row );
        void assignDefaultParameterValueData(const std::string &parameterName,const std::string& parameterValue);

        IData& getConfigParameters();

        /**
         * writeParameters
         *
         * Writes the map of entity parameters to the database. The map of parameters is
         * assumed to by already type checked, and will be written straight to the database.
         * The map is a map of parameter name to parameter value
         *
         * @param parameters The map of parameters. The map is in the format:
         * ParameterName->ParameterValue
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter, or if the
         *            the entity key is invalid (and this is not a new entity).
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        void writeParameters(std::map<std::string, std::string>& parameters);

        /**
         * writeEntityData
         * 
         * Write this entity to the database. If this is a new entity, this will populate
         * the m_key field (which can be retrieved using getKey()).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Entity object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         */
        void writeEntityData();

        /**
         * deleteEntity
         *
         * Remove this entity from the database. Once this method has been executed, the
         * EntityHelper object should be destroyed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        void deleteEntity();

        /**
         * resetEntityHelper
         *
         * Called in conjunction with deleteAndResetEntity in ConfigEntity, this resets the EntityHelper class
         * to the state where it represents a new entity that has yet to be written to the database.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Entity object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void resetEntityHelper();

		//TD15302 Lizettejl++
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
	      * 
	      * @return virtual void 
	      * @param : EntityStatusData& entityStatus
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
	      * writeEntityStatusData
	      * 
	      * This function  
	      * 
	      * @return virtual void 	       
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual void writeEntityStatusData(EEntityStatusType statustype, const std::string& newValue);
		// ++lizettejl

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... 
         * or set... method is called and the data state is not valid.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        void reload();

    private:

		void processData( IData* data, unsigned long row );

        /**
         * modifyExistingEntity
         *
         * This will update an existing entity in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Entity object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingEntity();

        
        /**
         * addNewEntity
         *
         * This will add a new entity in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Entity object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewEntity();


        /**
         * modifyAgentKeys
         *
         * This will modify the agent key for all children of this entity.
         * This is called when the parentkey has been changed and applied to the database.
         *
         * @param unsigned long - The agent key to set for the child entitis
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Entity object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyAgentKeys(unsigned long newAgentKey);


        /**
         * getNewAgentKey
         *
         * This will determine if the agent key needs to change for this entity. This will either return
         * the old agent key or the new key it should be set to.
         *
         * @return unsigned long- The Agent key to set in the database
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Entity object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        unsigned long getNewAgentKey();


        /**
         * checkForChildEntities
         *
         * This checks to see if this entity has any child entities. If it has child entities
         * it will throw an exception. It is checking these to determine if a delete can be 
         * performed. So discovering child entities is an error.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException - The type should be set to CANNOT_DELETE and it will contain
         *                            the list of child entities (maximum of 10)
         */
        void checkForChildEntities();


        /**
         * retrieveDeletedNameForEntity
         *
         * When an entity is deleted it is appended with #number. The number is incremented for
         * each entity deleted with the same name. This determines what the next number will be
         * for this entity when it is deleted.
         *
         * @param unsigned long - This is the maximum size of the field that the database will allow.
         *
         * @return string - The name of the entity when it is deleted
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        std::string retrieveDeletedNameForEntity(unsigned long maxFieldSize);


        /**
         * getSubsystemForNewEntity
         *
         * This determines the default subsystem for a new entity and sets it in m_fixedSubsystem
         *
         * @return unsigned long - The pkey of the subsystem for this entity.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long)
         */
         void getSubsystemForNewEntity();

         /**
         * initialiseMaxFieldLengths
         *
         * This retrieves the maximum field lengths for the name and description fields from the database.
         *
         * @exception DataException A DataException will be thrown if there is a 
         *            problem retrieving the data from the database.
         */
        void initialiseMaxFieldLengths();

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();

		//TD15302 Lizettejl++
        /**
         * modifyExistingEntityStatusData
         *
         * This will update an existing entity status data of a given entity
		 * in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Entity object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingEntityStatusData(TA_Base_Core::IDatabase* databaseConnection, const EntityStatusData& statusdata);

        
        /**
         * addNewEntityEntityStatusData
         *
         * This will add a new entity status data configured for an existing
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewEntityEntityStatusData(TA_Base_Core::IDatabase* databaseConnection, const EntityStatusData& statusdata);

		 /**
         * getDefaultValueForStatutsType
         *
         * This will return the default value for the given entity status data type		 
		 */
		std::string getDefaultValueForStatutsType(EStatusDataType datatype);
		 
		/**
         * getEntityStatusDataType
         *
         * This will return the data type for the given entity status type		 
		 */	    
		EStatusDataType getEntityStatusDataType(EEntityStatusType statustype);

		/* reloadEntitystatusdata()
         *
         * This method reloads the entity status data from the database. It is called when a get... 
         * or set... method is called and the data state is not valid.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityStatusData() has already been called
         */
	    void reloadEntitystatusdata(EEntityStatusType statustype);
		// ++lizettejl

    public:

        static const std::string PARAMETERNAME;
        static const std::string PARAMETERTYPE;
        static const std::string PARAMETERVALUE;
        static const std::string PARAMETERDEFAULTVALUE;
        static const std::string PARAMETERVISIBLE;
        static const std::string PARAMETERMANDATORY;
        static const std::string PARAMETERONLINEUPDATE;
        static const std::string PARAMETERDATATYPE;
        static const std::string PARAMETERVALIDATION;
            
        // This is the string to append to a name when it is being deleted
        static const std::string DELETED_FLAG;

        static const unsigned long DEFAULT_NAME_MAX_LENGTH;

        static const std::string NO_PARENT;
        static const std::string NO_AGENT;


    private:

		bool m_isValidData;

        unsigned long m_key;
        unsigned long m_typeKey;
        std::string m_name;            
        std::string m_type;
        std::string m_address;
        std::string m_description;

        // These contain keys to the referenced tables
        unsigned long m_subsystem;
        unsigned long m_fixedSubsystem;
        unsigned long m_location;
		unsigned long m_region;
        unsigned long m_parent;
        unsigned long m_agent;

        // These contain the corresponding string names of the keys listed above
        std::string m_subsystemName;
        std::string m_fixedSubsystemName;
        std::string m_locationName;
		std::string m_regionName;
        std::string m_parentName;
        std::string m_agentName;

        std::string m_localDatabase;
        unsigned long m_originalParent;  // This is used to monitor when the parent key changes. This is because
                                         // when the parent changes then the agent key must change for this entity
                                         // and possibly all child entities.

        bool m_isManuallySetAgent;      // This indicates if we should automatically set the agent or if the
                                        // user has set it manually.
        time_t m_dateCreated;
        time_t m_dateModified;

        IData* m_configParameterData;

	    static TA_Base_Core::NonReEntrantThreadLockable m_nameMaxLengthLock;
        static volatile unsigned long m_nameMaxLength;

			//TD15302 Mintao++
        std::map<EEntityStatusType, EntityStatusData>  m_entityStatusList;
		bool m_isValidStatusData;
		
		EntityParameterNameValueMap m_parameterData;
		TA_Base_Core::ReEntrantThreadLockable m_parameterDataLock;
    };
} // closes TA_Base_Core

#endif // !defined(EntityHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
