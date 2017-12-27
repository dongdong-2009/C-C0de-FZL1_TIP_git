/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EntityData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * EntitData is an implementation of the IEntityData interface. It can be used by specific
 * to handle the actions of retrieving the standard (default) entity data from the 
 * EntityHelper. It should be sub-classed by the specific entity class.
 */

#if !defined(ENTITY_DATA_HEADER)
#define ENTITY_DATA_HEADER

#if defined(_MSC_VER)
#pragma warning(disable:4250 4786)
#endif // defined (_MSC_VER)

#include <string>

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"

namespace TA_Base_Core
{

    class EntityData : public virtual IEntityData
    {

    public:

        /**
         * EntityData (constructor)
         *
         * Constructs a new EntityData instance based upon the key
         *
         * @param key The pkey of this entity
         * @param typeName The type name for this entity
         */
        EntityData(const unsigned long key, const std::string& typeName);

        /**
         * EntityData (constructor)
         *
         * Constructs an empty EntityData instance 
         *
         */
        EntityData(): m_entityHelper(NULL) {};

        
        /**
         * getKey
         *
         * Returns the key for this entity.
         *
         * @return The key for this entity as an unsigned long.
         *
         */
        unsigned long getKey();

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
         */
        std::string getName();

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
         */
        std::string getAddress();

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
         */
        std::string getDescription();

        /**
         * getSubsystem
         *
         * Returns the subsystem for this entity.
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
        unsigned long getSubsystem();


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
         * @return The parent for this entity as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
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
         * getAlarmDisplay
         *
         * Returns the AlarmDisplay parameter for this entity
         *
         * @return The value of the AlarmDisplay parameter for this entity as a std::string
         * The empty string ("") will be returned if the parameter is not set.
         *
         * @exception DatabaseException Thrown if there is a database communication error
         * @excpeiton DataException Thrown if the data in the value column is not as expected
         */
        std::string getAlarmDisplay();

        /**
         * getHelper
         *
         * Returns a pointer to the EntityHelper instance held by this class.
         *
         * @return A pointer to the EntityHelper instance held by this class.
         */
        EntityHelper* getHelper();
		
		/** 
		  * assignDefaultData
		  *
		  * This function will initialise the EntityHelper with valid data
		  *
		  * @param  IData* data, the data as read from the database
		  * @param unsigned long row, the row number to access
		  */
		void assignDefaultData( IData* data, unsigned long row );

		void assignDefaultParameterValueData( IData* data, unsigned long row );

        void assignDefaultParameterValueData( const std::string& parameterName, const std::string& parameterValue);

        virtual ~EntityData();
        //TD15302 Mintao++
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
        //TD15302 Mintao++

    private:

        EntityHelper* m_entityHelper;

        static const std::string ALARM_DISPLAY_PARAMETER;
        
    };

} //close namespace TA_Base_Core

#endif // !defined(ENTITY_DATA_HEADER)
