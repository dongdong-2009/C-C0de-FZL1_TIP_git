/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IConfigEntity.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IConfigEntity - This is the interface supported by all types of configuration entity.
  * it provides methods for retrieving, settings, and writing data for an entity object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */


#if !defined(IConfigEntity_9DF33F11_4314_4bbb_A67C_24D0E1850A05__INCLUDED_)
#define IConfigEntity_9DF33F11_4314_4bbb_A67C_24D0E1850A05__INCLUDED_

#include <vector>
#include <map>
#include <string>

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IConfigItem.h"

#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{

    class IConfigEntity : public IEntityData, public virtual IConfigItem
    {

    public:
        // These are all the different data types an entity parameter can be. The default is STRING.
        enum DataType
        {
            STRING = 0,
            INTEGER,
            BOOLEAN,
            REAL,
            ENUMERATION
        };


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
        virtual std::vector<std::string> getParameterNames() =0;


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
        virtual std::string getParameterValue(const std::string& name) =0;


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
        virtual DataType getParameterDataType(const std::string& name) =0;


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
        virtual std::string getParameterValidation(const std::string& name) =0;


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
        virtual std::map<std::string,std::string> getPossibleEnumerationValues(const std::string& name) =0;


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
        virtual bool isParameterMandatory(const std::string& name) = 0;

        
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
        virtual bool isParameterOnlineUpdatable(const std::string& name) = 0;

        
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
        virtual void setParameterValue(const std::string& value, const std::string& name) =0;


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
        virtual void deleteThisEntity() =0;

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
        virtual void deleteAndResetEntity() =0;

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
        virtual void resetEntity() =0;

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
        virtual void setAddress(const std::string& address) =0;


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
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         * @param description The description to give this entity
         */
        virtual void setDescription(const std::string& description) =0;


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
        virtual void setSubsystem(const unsigned long subsystem) =0;


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
        virtual void setLocation(const unsigned long location) = 0;


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
        virtual void setRegion(const unsigned long region) = 0;


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
        virtual void setParent(const unsigned long parent) = 0;
		

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
        virtual void setAgent(const unsigned long agent) = 0;

        
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
        virtual std::map<unsigned long, std::string> getPossibleParentEntities() =0;

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
		  /**
	      * setEntityStatusData
	      * 
	      * This function is to set the current status of the entity
	      * 
	      * @return virtual void 
	      * @param : EntityStatusData& entityStatus
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setEntityStatusData(const EntityStatusData& entityStatus) = 0;
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
        virtual void setEntityStatusData(unsigned int statustype, EStatusDataType datatype, const std::string& statusvalue, time_t dateModified ) = 0;
		// ++lizettejl

        virtual ~IConfigEntity() {};

    };

} //close namespace TA_Base_Core

#endif // !defined(IConfigEntity_9DF33F11_4314_4bbb_A67C_24D0E1850A05__INCLUDED_)
