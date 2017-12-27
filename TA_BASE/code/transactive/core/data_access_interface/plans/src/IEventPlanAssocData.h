/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/IEventPlanAssocData.h $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * IEventPlanAssocData is an interface to the EventPlanMap table. It provides read-only
  * access for existing EventPlanAssociations
  */

#if !defined(_I_EVENT_PLAN_ASSOC_DATA_H_)
#define _I_EVENT_PLAN_ASSOC_DATA_H_

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IEventPlanAssocData : public IItem
    {
    public:

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
        virtual unsigned long getEventTypeKey() =0;


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
        virtual bool isEnabled() =0;


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
        virtual unsigned long getEntityKey() =0;


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
        virtual unsigned long getEntityTypeKey() =0;


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
		virtual std::string getPlanPath() = 0;


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
        virtual time_t getDateCreated() =0;


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
        virtual time_t getDateModified() =0;

        /**
         * Destructor
         */
        virtual ~IEventPlanAssocData() {}
    };

} // Closes TA_Base_Core

#endif // !defined(_I_EVENT_PLAN_ASSOC_DATA_H_)

