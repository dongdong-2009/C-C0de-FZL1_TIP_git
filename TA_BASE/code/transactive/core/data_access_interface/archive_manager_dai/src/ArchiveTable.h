#if !defined(ArchiveTable_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ArchiveTable_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveTable is an implementation of IArchiveTable. It holds the data specific to a 
  * ArchiveTable mapping entry in the database, and allows read-only access to that data.
  */

// KT 26 Feb 04, PW #3133:  ONLINE_PERIOD has been moved from AR_TABLES and put in AR_ONLINE.

#include <string>

#include "core/data_access_interface/archive_manager_dai/src/ArchiveTableHelper.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveTable.h"

namespace TA_Base_Core
{
    class ArchiveTable : public IArchiveTable
    {
    public:
        /**
         * ArchiveTable
         *
         * Standard constructer that creates a ArchiveTable object with the given key.
         *
         * @param       unsigned long key 
         *              The unique key of this ArchiveTable.
         * 
         * @exception   ArchiveException
         *              Thrown if the ArchiveTableHelper cannot be created.
         */

        ArchiveTable( unsigned long key );

        /**
         * ~ArchiveTable
         *
         * Standard destructor.
         */

        virtual ~ArchiveTable() {};

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

	    virtual unsigned long getKey();

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

        virtual std::string getName();

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

        virtual std::string getQueryColumn();

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

        virtual std::string getFilePostfix();
        
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

        virtual bool isDateInQueryColumn( const time_t& date );
                
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

        virtual void deleteRestoredDataForDate( const time_t& date );

        /**
         * invalidate
         *
         * Mark the data contained by this ArchiveTable as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    0 != m_archiveTableHelper.
         */

	    virtual void invalidate();

    private:
        //
        // Disable copy constructor and assignment operator.
        //

        ArchiveTable( const ArchiveTable& theArchiveTable);  
  		ArchiveTable& operator=(const ArchiveTable &);

        //
        // This is the object that does all the work.
        //
    
        ArchiveTableHelper m_archiveTableHelper;
    };

} // closes TA_Base_Core

#endif // !defined(ArchiveTable_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
