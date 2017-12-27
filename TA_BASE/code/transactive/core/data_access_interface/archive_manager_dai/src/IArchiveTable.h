#if !defined(IArchiveTable_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IArchiveTable_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/IArchiveTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IArchiveTable is the interface to an object that holds the data specific to a 
  * ArchiveTable entry in the database, and allows read-only access to that data.
  */

// KT 26 Feb 04, PW #3133:  ONLINE_PERIOD has been moved from AR_TABLES and put in AR_ONLINE.

#include <string>

namespace TA_Base_Core
{
    class IArchiveTable
    {
    public:
        /**
         * ~IArchiveTable
         *
         * Standard destructor.
         */

        virtual ~IArchiveTable() {};

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

	    virtual unsigned long getKey() = 0;

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

        virtual std::string getName() = 0;

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

        virtual std::string getQueryColumn() = 0;

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

        virtual std::string getFilePostfix() = 0;

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

        virtual bool isDateInQueryColumn( const time_t& date ) = 0;
        
        /**
         * deleteRestoredDataForDate
         *
         * Deletes the restored data for the given date from this table.
         * If the date is within the last m_OnlinePeriod days, this method
         * does nothing.  The data will be automatically deleted once the
         * Online period expires.
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

        virtual void deleteRestoredDataForDate( const time_t& date ) = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this ArchiveTable as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    0 != m_archiveTableHelper.
         */

	    virtual void invalidate() = 0;
    };

} //close namespace TA_Base_Core

#endif // !defined(IArchiveTable_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
