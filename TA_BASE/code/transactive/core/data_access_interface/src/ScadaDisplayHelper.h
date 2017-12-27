/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ScadaDisplayHelper.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ScadaDisplayHelper is an object that is held by ScadaDisplay and ConfigScadaDisplay objects. 
  * It contains all data used by display, and manipulation
  * methods for the data. It helps avoid re-writing code for both ScadaDisplay and ConfigScadaDisplay.
  */

#if !defined(ScadaDisplayHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define ScadaDisplayHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{
    class ScadaDisplayHelper
    {

    public:
        /**
         * ScadaDisplayHelper
         * 
         * This constructor creates a new ScadaDisplayHelper for the specified key.
         *
         * @param key The key into the SE_SCADA_SCHEMATIC table for this display
         */
        ScadaDisplayHelper(const unsigned long key);

	    /**
	      * ScadaDisplayHelper
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
		ScadaDisplayHelper(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * ScadaDisplayHelper
         * 
         * Copy Constructor
         */
        ScadaDisplayHelper( const ScadaDisplayHelper& theScadaDisplayHelper);


        /**
         * ScadaDisplayHelper
         * 
         * This constructor creates a new ScadaDisplayHelper for configuration, initiailising
         * the key to 0 - it will be specified once this display is written to the database
         * for the first time.
         */
        ScadaDisplayHelper();


        /**
         * Destructor
         */
        virtual ~ScadaDisplayHelper();


        /**
         * isNew
         *
         * This returns whether this is a new display or not
         *
         * @return bool - True if this is a new display
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

        
        /**
         * getKey
         *
         * Returns the key for this display.
         *
         * @return The key for this display as an unsigned long.
         *
         * pre: Either - this display was initially loaded from the database
         *      OR     - writeData() has already been called
         */
        unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this display. If this is the first time data for this display
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this display as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this operator was initially loaded from the database
         *      OR     - setName() has already been called
         */
        std::string getName();


        /**
         * setName
         *
         * Sets the name of this display. This should only be used by the ConfigScadaDisplay class
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * @param name The name to give this operator.
         */
        void setName(const std::string& name);


        /**
         * getPath
         *
         * Returns the path and file name of the display
         *
         * @return The path for this display as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        std::string getPath();


        /**
         * setPath
         *
         * Sets the path of this Scada display.
         *
         * @param string - The path for the display
         *
         * pre: This display has not been deleted
         */
        void setPath(const std::string& path);


        /**
         * getLocation
         *
         * Returns the location for this disply.
         *
         * @return The location for this display as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        unsigned long getLocation();
		

        /**
         * getLocationName
         *
         * Returns the location name for this display.
         *
         * @return The location name for this display as a string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        std::string getLocationName();

        
        /**
         * setLocation
         *
         * Sets the location of this Scada display.
         *
         * @param unsigned long - The key of the location for this display
         *
         * pre: This display has not been deleted
         */
        void setLocation(unsigned long location);
		

    
        /**
         * getDateCreated
         *
         * Returns the date created for this display
         *
         * @return The date created for this display as a time_t.
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
         * Returns the date modified for this display.
         *
         * @return The date modified for this display as a time_t.
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
         * invalidate
         *
         * Make the data contained by this display as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         * It is a precondition that writeData() has been called prior to calling
         * this method on a new display, as it does not make any sense to invalidate an
         * display that has not yet been written to the database.
         *
         * pre: Either - this display was initially loaded from the database
         *      OR     - writeData() has already been called
         */
        void invalidate();


        /**
         * writeData
         * 
         * Write this display to the database. If this is a new display, this will populate
         * the m_key field (which can be retrieved using getKey()).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         */
        void writeData();


        /**
         * deleteDisplay
         *
         * Remove this display from the database. Once this method has been executed, the
         * ScadaDisplayHelper object should be destroyed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this display was initially loaded from the database
         *      OR     - writeData() has already been called
         */
        void deleteDisplay();
		

    private:
        // Made private as it is not used
		ScadaDisplayHelper& operator=(const ScadaDisplayHelper &);

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this display was initially loaded from the database
         *      OR     - writeData() has already been called
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

        
        /**
         * modifyExisting
         *
         * This will update an existing display in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExisting();

        
        /**
         * addNew
         *
         * This will add a new display in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Location object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNew();

        /**
         * checkForReferencesFromProfileParameterTable
         *
         * Checks that there are no references to the display in the profile parameter 
         * table.
         *
         * @exception DataException thrown if there is a foreign key reference to this row.
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem communicating with the database.
         */
        void checkForReferencesFromProfileParameterTable();

        /**
         * getCommaSeparatedList
         *
         * Gets the values in columnName from the given data object as a comma
         * separated list.
         * @exception DataException thrown if the values are not string data.
         */
        std::string getCommaSeparatedList(IData* data, const std::string&  columnName);

        unsigned long m_key;
        std::string m_name;
        std::string m_path;
		unsigned long m_location;
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

    };
} // closes TA_Base_Core

#endif // !defined(ScadaDisplayHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
