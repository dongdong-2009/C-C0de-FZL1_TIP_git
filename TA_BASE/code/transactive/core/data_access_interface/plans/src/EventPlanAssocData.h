/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/EventPlanAssocData.h $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * EventPlanAssocData is an implementation of IEventPlanAssocData. It holds the data specific to 
  * event plan associations.
  */

#if !defined(_EVENT_PLAN_ASSOC_DATA_H_)
#define _EVENT_PLAN_ASSOC_DATA_H_

#include <string>

#include "core/data_access_interface/plans/src/EventPlanAssocHelper.h"
#include "core/data_access_interface/plans/src/IEventPlanAssocData.h"
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class EventPlanAssocData : public IEventPlanAssocData
    {
    public:
        /**
         * Constructor
         *
         * Constructs an association based off the provided key.
         *
         * @param key The association key.
         */
        EventPlanAssocData( unsigned long key );

        /**
         * Constructor
         *
         * Constructs an association with the provided details.
         *
         * @param key The association key.
         * @param enabled True if the association is enabled.
         * @param eventTypeKey The key of the associated event type.
         * @param entityKey The key of the associated entity.
         * @param entityTypeKey The key of the associated entity type.
         * @param planPath The full path of the associated plan.
         * @param dateCreated The date this association was created.
         * @param dateModified The date this association was last modified.
         */
        EventPlanAssocData( unsigned long key, bool enabled, unsigned long eventTypeKey,
                            unsigned long entityKey, unsigned long entityTypeKey,
                            const std::string& planPath, time_t dateCreated, time_t dateModified );

        /**
         * Destructor
         */
        virtual ~EventPlanAssocData();

        //////////////////////////////////
        // Methods inherited from IItem //
        //////////////////////////////////

        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey()
        {
            return m_helper.getKey();
        }


        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName()
        {
            return "";
        }


        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

        ////////////////////////////////////////////////
        // Methods inherited from IEventPlanAssocData //
        ////////////////////////////////////////////////

        /**
         * getEventTypeKey
         *
         * Returns the event type key for this association.
         *
         * @return unsigned long The event type key.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual unsigned long getEventTypeKey();


        /**
         * isEnabled
         *
         * Returns whether the association is enabled.
         *
         * @return bool True if enabled.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual bool isEnabled();


        /** 
         * getEntityKey
         *
         * Returns the entity key for this association.
         *
         * @return unsigned long The entity key.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual unsigned long getEntityKey();


        /** 
         * getEntityTypeKey
         *
         * Returns the entity type key for this association.
         *
         * @return unsigned long the entity type key.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual unsigned long getEntityTypeKey();


		/** 
		  * getPlanPath
		  *
		  * Returns the full path of the plan this association maps to.
          * The path is in the format "[/<category1>/<categoryN>]/<planName>"
		  *	
		  * @return std::string fully qualified plan path
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, if the wrong amount of data is retrieved,
          *            or if the event plan association key is invalid.
		  */
		virtual std::string getPlanPath();


        /**
         * getDateCreated
         *
         * Returns the date created for this alarm plan association.
         *
         * @return The date created for this alarm plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this alarm plan association.
         *
         * @return The date modified for this alarm plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual time_t getDateModified();


    private:

        // Hidden methods.
        EventPlanAssocData();
        EventPlanAssocData( const EventPlanAssocData& theEventPlanAssoc );
        EventPlanAssocData& operator=( const EventPlanAssocData& theEventPlanAssoc );

        // The helper used for database access.
        EventPlanAssocHelper m_helper;
    };

} // Closes TA_Base_Core

#endif // !defined(_EVENT_PLAN_ASSOC_DATA_H_)

