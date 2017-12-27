/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EventColourHelper.h $
 * @author Derrick Liew
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by: $Author: CM $
 * 
 * Provides access to the Event Colour data in the database.
 */


// EventColourHelper.h: interface for the EventColourHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTCOLOURHELPER_H__44D70961_5EAC_4D4B_8C79_4B16DCA4E9DD__INCLUDED_)
#define AFX_EVENTCOLOURHELPER_H__44D70961_5EAC_4D4B_8C79_4B16DCA4E9DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include "core/data_access_interface/src/IConfigEventColourData.h"


namespace TA_Base_Core
{
	class IData;

	class EventColourHelper  
	{
	public:
		
		EventColourHelper(unsigned long pkey);

	    /**
	      * EventColourHelper
	      * 
	      * This constructor creates a new object using the input data
          * which is representative of a row returned from SQL statement
	      * 
	      * @param : const unsigned long row
          *          - the row of data in the data object that we should collect data from
          *          in order to construct ourselves
	      * @param : TA_Base_Core::IData& data
          *          - the IData interface that should have been obtained using the 
          *          getBasicQueryData function
	      * 
	      * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
	      */		
		EventColourHelper(const unsigned long row, TA_Base_Core::IData& data);

		virtual ~EventColourHelper();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         *
         * pre: This item has not been deleted
         */
        void applyChanges();
    

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();
    
    
        /**
         * setName
         *
         * Sets the name of the EventColour.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. colour should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        void setName(const std::string name);


        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        unsigned long getKey();


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
        std::string getName();
		

		/**
		 * setEventColourKey
		 *
		 * Sets the colour RGB level of this event colour type.
         *
         * @param unsigned long - The RGB colour
         * @param EEventColourType - Which colour type is being updated.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. colour should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
		 */
		void setEventColourKey(unsigned long colour, IEventColourData::EEventColourType type);


		/**
		 * getEventColourKey
		 *
		 * Returns the colour RGB level of this event colour type. 
         *
         * @param EEventColourType - Which colour to retrieve. This defaults to OPEN_ACKED which was the
         *                      behaviour when only one event colour was available.
		 *
		 * @return The RGB level of the event colour type as an unsigned int. 
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
		 */
        virtual unsigned long getEventColourKey(IEventColourData::EEventColourType type);


        /**
         * getDateCreated
         *
         * Returns the date created for this event colour.
         *
         * @return The date created for this event colour as a time_t.
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
         * Returns the date modified for this event colour.
         *
         * @return The date modified for this event colour as a time_t.
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

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this action was initially loaded from the database
         *      OR     - writeProfile() has already been called
         */		
		void reload();

        /**
          * reloadUsing
          *
          * Fills out the local members with data contained in the input data object
          *
          * @param : const unsigned long row - the row to query in the data object
          * @param : TA_Base_Core::IData& data - the IData interface that should 
          *          have been obtained using the getBasicQueryData function
          *         
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
          */
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data);

        std::string m_name;
        unsigned long m_key;
        std::map<IEventColourData::EEventColourType, unsigned long> m_eventColourKey;
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;

	};
} // close namespace

#endif // !defined(AFX_EVENTCOLOURHELPER_H__44D70961_5EAC_4D4B_8C79_4B16DCA4E9DD__INCLUDED_)
