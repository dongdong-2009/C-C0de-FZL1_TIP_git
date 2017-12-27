/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IEntityData.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IEntitData is an interface representing the methods to retrieve generic data from an entity.
  * In addition it provides the getType() and clone() methods that allow the concrete entity
  * class to be created dynamically based on the entity type returned from the database.
  */

#if !defined(IEntityData_40BA7E41_79B1_4eac_9ED9_7A269545342E__INCLUDED_)
#define IEntityData_40BA7E41_79B1_4eac_9ED9_7A269545342E__INCLUDED_

#include <string>
#include <vector>
#include <map>
#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/entity_access/src/EntityStatusData.h"

#include <boost/shared_ptr.hpp>

namespace TA_Base_Core
{

    class IEntityData : public virtual IItem
    {

    public:
        
        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
		virtual std::string getType() =0;

        /**
         * clone
         *
         * Returns a new instance of this entity. This method is used to create a concrete
         * entity based upon type in the EntityAccessFactory.
         *
         * @param key The pkey of this entity in the database.
         */ 
        //virtual IEntityData* clone(unsigned long key) =0;

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
        virtual unsigned long getTypeKey() =0;

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
        virtual std::string getAddress() =0;

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
        virtual std::string getDescription() =0;

        /**
         * getSubsystem
         *
         * Returns the TransActive subsystem (not the physical subsystem) for this entity.
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
        virtual unsigned long getSubsystem() =0;


        /**
         * getSubsystemName
         *
         * Returns the TransActive subsystem name (not the physical subsystem) for this entity.
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
        virtual std::string getSubsystemName() =0;


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
        virtual unsigned long getPhysicalSubsystem() =0;

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
        virtual std::string getPhysicalSubsystemName() =0;

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
        virtual unsigned long getLocation() = 0;


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
		virtual std::string getLocationName() = 0;


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
        virtual unsigned long getRegion() = 0;


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
		virtual std::string getRegionName() = 0;


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
        virtual unsigned long getParent() =0;


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
        virtual std::string getParentName() =0;

        
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
        virtual unsigned long getAgent() = 0;


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
 		virtual std::string getAgentName() = 0;


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
        virtual time_t getDateCreated() =0;

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
        virtual time_t getDateModified() =0;

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
        virtual std::string getAlarmDisplay() =0;

        virtual ~IEntityData() {};

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
        virtual EntityStatusData getEntityStatusData(EEntityStatusType statustype) = 0;
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
        virtual EntityStatusData getEntityStatusData(unsigned int statustype) = 0;
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
        virtual bool getBoolEntityStatusValue(EEntityStatusType statustype) = 0;
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
        virtual bool getBoolEntityStatusValue(unsigned int statustype) = 0;
        //TD15302 Mintao++

    };

    // old skool, manage your own
    typedef std::vector< IEntityData* > IEntityDataList;

    // new age, it does itself
    typedef boost::shared_ptr<IEntityData> IEntityDataPtr;
    typedef std::vector< boost::shared_ptr<IEntityData> > SharedIEntityDataList;
    typedef std::map< unsigned long, boost::shared_ptr<IEntityData> > SharedIEntityDataMap;

} //close namespace TA_Base_Core

#endif // !defined(IEntityData_40BA7E41_79B1_4eac_9ED9_7A269545342E__INCLUDED_)
