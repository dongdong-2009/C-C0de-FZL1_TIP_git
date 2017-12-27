/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IECSStationColourData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * IAlarmStationData is an interface to a Alarm Station object. It allows the Alarm Station object implementation
  * to be changed without changing the code that uses this interface.
  */


#ifndef IECS_STATION_COLOUR_DATA_H
#define IECS_STATION_COLOUR_DATA_H

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IECSStationColourData : public IItem
	{
	public:


        enum EColourType
        {
            CURRENT_PROPOSED_CF
        };


		/**
		 * getStationColourKey
		 *
		 * Returns the Station RGB level of this Alarm type. 
         *
         * @param EColourType - Which colour to retrieve. This defaults to OPEN_ACKED which was the
         *                      behaviour when only one alarm colour was available.
		 *
		 * @return The Station RGB level of the Alarm type as an unsigned int. 
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
		 */
		virtual unsigned long getStationColourKey(EColourType type = CURRENT_PROPOSED_CF) =0;


		/**
         * invalidate
         *
         * Mark the data contained by this Alarm Station as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;


        /**
         * getDateCreated
         *
         * Returns the date created for this alarm Station.
         *
         * @return The date created for this alarm Station as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this alarm Station.
         *
         * @return The date modified for this alarm Station as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual time_t getDateModified() =0;



        virtual ~IECSStationColourData() {};
	};

} //close namespace TA_Base_Core

#endif // IECS_STATION_COLOUR_DATA_H
