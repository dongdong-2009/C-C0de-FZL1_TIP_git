/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/IConfigEventPlanAssocData.h $
  * @author:  Brad Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/28 10:18:09 $
  * Last modified by:  $Author: weikun.lin $
  *
  * IConfigEventPlanAssocData is an interface to the EventPlanMap table. It provides read and write
  * access for new and existing EventPlanAssociations
  */

#if !defined(_I_CONFIG_EVENT_PLAN_ASSOC_DATA_H_)
#define _I_CONFIG_EVENT_PLAN_ASSOC_DATA_H_

#include <string>
#include "core/data_access_interface/plans/src/IEventPlanAssocData.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_Base_Core
{
    class IConfigEventPlanAssocData : public IEventPlanAssocData, public IConfigItem
    {
    public:

        virtual ~IConfigEventPlanAssocData() {}

        /**
         * deleteThisAssociation
         *
         * Removes this event plan association from the database. 
         * The calling application MUST then delete this event plan association object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this event plan association was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This event plan association has not been deleted
         */
        virtual void deleteThisAssociation() =0;


        /**
         * isUsingEntity
         *
         * This returns whether the entity or entity type is being used
         *
         * @return bool - True if entity is being used, false if entity type is being used
         */
        virtual bool isUsingEntity() =0;


        /** 
         * setEventTypeKey
         *
         * Sets the event type key for this association.
         *
         * @param eventTypeKey The event type key.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual void setEventTypeKey( const unsigned long eventTypeKey ) =0;


        /**
         * setEnabled
         *
         * Sets whether the association is enabled.
         *
         * @param enabled True if enabled.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual void setEnabled( const bool enabled ) =0;


        /** 
         * setEntityKey
         *
         * Sets the entity key for this association. Set to 0 if the entity key is not used.
         *
         * @param unsigned long the entitykey
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual void setEntityKey( const unsigned long entityKey ) =0;


        /** 
         * setEntityTypeKey
         *
         * Sets the entity type key for this association. Set to 0 if the entity type key is not used.
         *
         * @param entityTypeKey The entity type key.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual void setEntityTypeKey( const unsigned long entityTypeKey ) =0;


        /** 
		  * setPlanPath
		  *
		  * Sets the plan path for this association
		  *
		  * @param std::string the plan path
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, if the wrong amount of data is retrieved,
          *            or if the event plan association key is invalid.
		  */
		virtual void setPlanPath(const std::string& planPath) = 0;


        // The configuration editor requires that the event type, entity, entity type, and plan
        // be available in text form. The following methods allow access to these values as well
        // as providing the ability to set key values based off names.

        /**
         * getEventTypeName
         *
         * Returns the name of the event type for this association.
         *
         * @return std::string The event type name.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual std::string getEventTypeName() =0;

        /**
         * getEventTypeDisplayName
         *
         * Returns the name of the event type for this association.
         *
         * @return std::string The event type name.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual std::string getEventTypeDisplayName() =0;

        /**
         * setEventTypeKeyFromName
         *
         * Sets the event type key to that corresponding with the provided name.
         *
         * @param eventTypeName The name of the event type to associate.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual void setEventTypeKeyFromName( const std::string& eventTypeName ) =0;


        /**
         * getEntityName
         *
         * Returns the name of the entity for this association.
         *
         * @return std::string The entity name.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
         virtual std::string getEntityName() =0;


        /**
         * setEntityKeyFromName
         *
         * Sets the entity key to that corresponding with the provided name.
         *
         * @param entityName The name of the entity to associate.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
         virtual void setEntityKeyFromName( const std::string& entityName ) =0;


        /**
         * getEntityTypeName
         *
         * Returns the name of the entity type for this association.
         *
         * @return std::string The entity type name.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
         virtual std::string getEntityTypeName() =0;


        /**
         * setEntityTypeKeyFromName
         *
         * Sets the entity type key to that corresponding with the provided name.
         *
         * @param entityTypeName The name of the entity type to associate.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
         virtual void setEntityTypeKeyFromName( const std::string& entityTypeName ) =0;
    };

} // Closes TA_Base_Core

#endif // !defined(_I_CONFIG_EVENT_PLAN_ASSOC_DATA_H_) 

