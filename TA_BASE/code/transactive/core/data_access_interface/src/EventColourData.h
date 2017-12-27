/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EventColourData.h $
 * @author:  Derrick Liew
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * EventColourData is an implementation of IEventColourData. It holds the data specific to a Event
 * colour entry in the database.
 */

// EventColourData.h: interface for the EventColourData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTCOLOURDATA_H__542456D9_584B_43F2_BA46_4007EB934E80__INCLUDED_)
#define AFX_EVENTCOLOURDATA_H__542456D9_584B_43F2_BA46_4007EB934E80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/src/IEventColourData.h"
#include "core/data_access_interface/src/EventColourHelper.h"
#include <vector>

namespace TA_Base_Core
{
	class EventColourData : public IEventColourData  
	{
		public:

			/**
			 * Constructor
			 *
			 * Construct a event colour class based around a key, this will read
			 * the data from the database and throw the any DatabaseException
			 * if not succesful
			 *
			 * @param key The key of this event colour type in the database
			 */
			EventColourData( const unsigned long key );

			/**
			 * Constructor
			 *
			 * Construct a EventColourData class based around dataset
			 */
			EventColourData(const unsigned long row, TA_Base_Core::IData& data);

			virtual ~EventColourData();

			/**
			 * getKey
			 *
			 * Returns the key for this Event Colour type.
			 *
			 * @return The key for this Event Colour as an unsigned long.
			 */
			unsigned long getKey();


			/**
			 * getName
			 *
			 * Returns the name of this Event Colour Type. If this is the first time data for this EventColour
			 * has been requested, all the data is loaded from the database.
			 *
			 * @return The name for this Event Colour as a std::string
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there is a
			 *            problem establishing a connection with the database, or if an
			 *            error is encountered while retrieving data.
			 * @exception DataException A DataException will be thrown if the data cannot be 
			 *            converted to the required format (e.g. persist_message should be 
			 *            either 0 or 1), or if the wrong amount of data is retrieved.
			 */
			std::string getName();

			/**
			 * getEventColourKey
			 *
			 * Returns the severity RGB level of this event type. 
			 *
			 * @param EEventColourType - Which colour to retrieve. This defaults to OPEN_ACKED which was the
			 *                      behaviour when only one event colour was available.
			 *
			 * @return The severity RGB level of the event type as an unsigned int. 
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there is a
			 *            problem establishing a connection with the database, or if an
			 *            error is encountered while retrieving data.
			 * @exception DataException A DataException will be thrown if the data cannot be 
			 *            converted to the required format (e.g. persist_message should be 
			 *            either 0 or 1), or if the wrong amount of data is retrieved. 
			 */
			unsigned long getEventColourKey(EEventColourType type = OPEN_ACKED_FG1);


			/**
			 * invalidate
			 *
			 * Mark the data contained by this event as invalid. The next call to get...() 
			 * following a call to invalidate() will cause all the data to be reloaded from
			 * the database.
			 */
			void invalidate();


			/**
			 * getDateCreated
			 *
			 * Returns the date created for this event colour type.
			 *
			 * @return The date created for this event colour type as a time_t.
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there is a
			 *            problem establishing a connection with the database, or if an
			 *            error is encountered while retrieving data.
			 * @exception DataException A DataException will be thrown if the data cannot be 
			 *            converted to the required format (e.g. TYPEKEY should be an 
			 *            unsigned long), or if the wrong ammount of data is retrieved, or if the
			 *            the location key is invalid (and this is not a new location).
			 */
			time_t getDateCreated();


			/**
			 * getDateModified
			 *
			 * Returns the date modified for this event colour type.
			 *
			 * @return The date modified for this event colour type as a time_t.
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there is a
			 *            problem establishing a connection with the database, or if an
			 *            error is encountered while retrieving data.
			 * @exception DataException A DataException will be thrown if the data cannot be 
			 *            converted to the required format (e.g. TYPEKEY should be an 
			 *            unsigned long), or if the wrong ammount of data is retrieved, or if the
			 *            the location key is invalid (and this is not a new location).
			 */
			time_t getDateModified();

		private:

			void reload();

			// Assignment operator and copy constructor not used so
			// they are made private
			EventColourData& operator=(const EventColourData&);
			EventColourData( const EventColourData& theEventColourData);   

			EventColourHelper* m_helper;

	};

} // end of namespace

#endif // !defined(AFX_EVENTCOLOURDATA_H__542456D9_584B_43F2_BA46_4007EB934E80__INCLUDED_)
