#if !defined(ArchiveTableHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define ArchiveTableHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveTableHelper.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveTableHelper is an object that is held by ArchiveTable and ConfigArchiveTable objects. 
  * It contains all data used by ArchiveTables, and manipulation
  * methods for the data. It helps avoid re-writing code for both ArchiveTable and ConfigArchiveTable.
  */

// KT 26 Feb 04, PW #3133:  ONLINE_PERIOD has been moved from AR_TABLES and put in AR_ONLINE.

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <string>
#include <climits>
#include <memory>

#include "core/utilities/src/DateFormat.h" // TD12474 

namespace TA_Base_Core
{
    class ArchiveTableHelper
    {
    public:
        /**
         * ArchiveTableHelper
         * 
         * This constructor creates a new ArchiveTableHelper for configuration.
         */

       ArchiveTableHelper();
   
       /**
         * ArchiveTableHelper
         * 
         * This constructor creates a new ArchiveTableHelper for the specified key.
         *
         * @param       unsigned long archiveTableKey
         *              The database (entity) key for this ArchiveTable.
         */

        ArchiveTableHelper( unsigned long archiveTableKey );

        /**
          * ArchiveTableHelper
          *
          * Standard copy constructor.
          */

        ArchiveTableHelper( const ArchiveTableHelper& theArchiveTableHelper );

        /**
         * ~ArchiveTableHelper
         *
         * Standard destructor.
         */

        virtual ~ArchiveTableHelper() {};

        /**
         * getKey
         *
         * Returns the key for this ArchiveTable entry.
         *
         * Preconditions:   Either - this ArchiveTable entry was initially 
         *                           loaded from the database
         *                  OR     - writeArchiveTableData() has already been called
         *
         * @return      unsigned long
         *              The key for this ArchiveTable.
         */

	    unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this ArchiveTable.
         *
         * Preconditions:   Either - this ArchiveTable entry was initially 
         *                           loaded from the database
         *                  OR     - writeArchiveTableData() has already been called
         *
         * @return      std::string
         *              The name of the ArchiveTable.
         */

        std::string getName();

        /**
         * setName
         *
         * Sets the name of this ArchiveTable.
         * This should only be used by the ConfigArchiveTable class.
         *
         * @param       const std::string& name
         *              The name of the ArchiveTable.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   DataException 
         *              Thrown if:
         *              1) The data cannot be converted to the required format
         *              2) The wrong amount of data is retrieved
         *              3) The entity key is invalid (and this is not a new entity)
         *              4) The data
         */

        void setName( const std::string& name );

        /**
         * getQueryColumn
         *
         * Returns the queryColumn of this ArchiveTable.
         *
         * Preconditions:   Either - this ArchiveTable entry was initially 
         *                           loaded from the database
         *                  OR     - writeArchiveTableData() has already been called
         *
         * @return      std::string
         *              The queryColumn of the ArchiveTable.
         */

        std::string getQueryColumn();

        /**
         * setQueryColumn
         *
         * Sets the queryColumn of this ArchiveTable.
         * This should only be used by the ConfigArchiveTable class.
         *
         * @param       const std::string& queryColumn
         *              The queryColumn of the ArchiveTable.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   DataException 
         *              Thrown if there are multiple entries for the same position.
         */

        void setQueryColumn( const std::string& queryColumn );
        
        /**
         * getFilePostfix
         *
         * Returns the filePostfix of this ArchiveTable.
         *
         * Preconditions:   Either - this ArchiveTable entry was initially 
         *                           loaded from the database
         *                  OR     - writeArchiveTableData() has already been called
         *
         * @return      std::string
         *              The filePostfix of the ArchiveTable.
         */

        std::string getFilePostfix();

        /**
         * setFilePostfix
         *
         * Sets the filePostfix of this ArchiveTable.
         * This should only be used by the ConfigArchiveTable class.
         *
         * @param       const std::string& filePostfix
         *              The filePostfix of the ArchiveTable.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   DataException 
         *              Thrown if there are multiple entries for the same position.
         */

        void setFilePostfix( const std::string& filePostfix );
    
        /**
         * isDateInQueryColumn
         *
         * Determines if there are any entries in this table that have the given date
         * in their query column.
         *
         * Preconditions:   Either - this ArchiveTable entry was initially 
         *                           loaded from the database
         *                  OR     - writeArchiveTableData() has already been called
         *
         * @return      bool
         *              True if the date exists in the column,
         *              False otherwise.
         *
         * @param       const time_t& date          
         *              The date to search for.        
         *              KT 9 Mar 04: PW #3161 - Changed from a std::string to a time_t,
         *              so that the date format is not assumed across the interface
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   DataException 
         *              Thrown if the incorrect amount of data is retrieved.
         */

        bool isDateInQueryColumn( const time_t& date );
    
        /**
         * deleteRestoredDataForDate
         *
         * Deletes the restored data for the given date from this table.
         * If the date is within the last m_onlinePeriod days, this method
         * does nothing.  The data will be automatically deleted once the
         * online period expires.
         *
         * @param       const time_t& date          
         *              The date of the restored data to delete.        
         *              KT 9 Mar 04: PW #3161 - Changed from a std::string to a time_t,
         *              so that the date format is not assumed across the interface
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         */

        void deleteRestoredDataForDate( const time_t& date );

        /**
         * invalidate
         *
         * Marks the data contained by this ArchiveTable as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    Either - this ArchiveTable was initially loaded from the database
         *                  OR     - writeArchiveTableData() has already been called, as it does 
         *                           not make any sense to invalidate a ArchiveTable that has not 
         *                           yet been written to the database.
         */

	    void invalidate();

        /**
         * writeArchiveTableData
         * 
         * Write this ArchiveTable to the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   DataException 
         *              Thrown if:
         *              1) There are multiple entries for the same position.
         * @exception   DataConfigurationException 
         *              Thrown if the data contained in the ArchiveTable object
         *              is not sufficent to create an entry in the database.
         */

        void writeArchiveTableData();

        /**
         * deleteArchiveTable
         *
         * Remove this ArchiveTable from the database. Once this method has been executed, the
         * ArchiveTableHelper object will have been destroyed.
         *
         * Precondition:    Either - this ArchiveTable was initially loaded from the database
         *                  OR     - writeArchiveTableData() has already been called.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         */

        void deleteArchiveTable();

    private:
        //
        // Disable the assignment operator.
        //

		ArchiveTableHelper& operator=(const ArchiveTableHelper &);
        
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * Precondition:    Either - this ArchiveTable was initially loaded from the database
         *                  OR     - writeArchiveTableData() has already been called.
         */

	    void reload();

        /**
         * insertNewArchiveTable
         *
         * Inserts a new ArchiveTable into the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   DataException
         *              Thrown if the ArchiveTable already exists in the database.
         */

        void insertNewArchiveTable();

        /**
         * updateExistingArchiveTable
         *
         * Updates an existing ArchiveTable in the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   DataException
         *              Thrown if the ArchiveTable does not already exist in the database.
         */

        void updateExistingArchiveTable();

        //
        // Flag to indicate if the data needs reloading.
        //

	    bool m_isValidData;

        //
        // Flag to indicate if this is a new ArchiveTable or an existing one
        // loaded from the database.
        //

        bool m_isNew;
        
        //
        // The ArchiveTable database (entity) key and the table information.
        //
        unsigned long m_key;
        std::string m_name;
        std::string m_queryColumn;
        std::string m_filePostfix;

		TA_Base_Core::DateFormat m_DateFormat; // TD12474
    };

} // closes TA_Base_Core

#endif // !defined(ArchiveTableHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
