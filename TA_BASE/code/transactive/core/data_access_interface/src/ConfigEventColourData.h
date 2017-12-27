/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigEventColourData.h $
 * @author Derrick Liew
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by: $Author: CM $
 * 
 * Implements IConfigEventColourData to provide a writable interface to
 * event colour data in the database.
 */

// ConfigEventColourData.h: interface for the ConfigEventColourData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGEVENTCOLOURDATA_H__D744175C_43C3_4F77_A415_868E652BBBA3__INCLUDED_)
#define AFX_CONFIGEVENTCOLOURDATA_H__D744175C_43C3_4F77_A415_868E652BBBA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "core/data_access_interface/src/IConfigEventColourData.h"
#include "core/data_access_interface/src/EventColourHelper.h"

namespace TA_Base_Core
{

	class ConfigEventColourData : public IConfigEventColourData  
	{
	public:

        ConfigEventColourData(unsigned long pkey);

		/**
         * Constructor
         *
         * Construct a ConfigEventColourData class based around dataset
         */
		ConfigEventColourData(const unsigned long row, TA_Base_Core::IData& data);


        virtual ~ConfigEventColourData();

        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        unsigned long getUniqueIdentifier();


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        bool hasChanged();


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        bool isNew();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
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
        void setName(const std::string& name);


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
        virtual std::string getName();
		

		/**
		 * setEventColourKey
		 *
		 * Sets the RGB level of this event type.
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
		virtual void setEventColourKey(unsigned long colour, EEventColourType type = OPEN_ACKED_FG1);


		/**
		 * getEventColourKey
		 *
		 * Returns the RGB level of this event type. 
         *
         * @param EColourType - Which colour to retrieve. This defaults to OPEN_ACKED which was the
         *                      behaviour when only one event colour was available.
		 *
		 * @return The colour RGB level of the event type as an unsigned int. 
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
		 */
		virtual unsigned long getEventColourKey(EEventColourType type = OPEN_ACKED_FG1);


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



        /**
         *  Gets an unsigned long as a string. Use getUnsignedLongFromString
         *  to convert this value back to an unsigned long.
         */
        std::string getUnsignedLongAsString(unsigned long number);

        /**
         *  Gets an unsigned long from a string created using
         *  getUnsignedLongAsString.
         */
        unsigned long getUnsignedLongFromString(const std::string& theString);

            
    private:

        ConfigEventColourData();
        const ConfigEventColourData& operator=(const ConfigEventColourData&);


        /**
         *  getHexColourString
         *
         *  Gets the given number as a hexadecinal string.
         */
        std::string getHexColourString(unsigned long colour);


        /**
         * updateEventColourChanges
         *
         * This updates the map recording the changes to the event colour.
         * Whenever a call to set is made in this class then this method
         * must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        void updateEventColourChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);


        /** The object that does the real database work */
        EventColourHelper* m_helper;

        /**
         *  We cannot use keys for identification of event colours in the
         *  Configuration Editor because new event colours do not have keys.
         *  Therefore we use this method for identification.
         */
        unsigned long m_uniqueIdentifier;

        /** The next unique identifier available */
        static long s_nextAvailableIdentifier;

        /**
         *  This is a map of all changes made to the event colour so that they
         *  are recorded and can be sent in audit or online update messages.
         */
        ItemChanges m_eventColourChanges;


        // Constants used as keys for the ItemChanges made to this object.
        static const std::string NAME;
        static const std::string OPEN_ACKED_COLOUR_FG1;
        static const std::string OPEN_ACKED_COLOUR_BG1;

        static const std::string OPEN_UNACKED_COLOUR_FG1;
        static const std::string OPEN_UNACKED_COLOUR_BG1;

        static const std::string CLOSED_UNACKED_COLOUR_FG1;
        static const std::string CLOSED_UNACKED_COLOUR_BG1;

        static const std::string CLOSED_ACKED_COLOUR_FG1;
        static const std::string CLOSED_ACKED_COLOUR_BG1;
	};

}

#endif // !defined(AFX_CONFIGEVENTCOLOURDATA_H__D744175C_43C3_4F77_A415_868E652BBBA3__INCLUDED_)
