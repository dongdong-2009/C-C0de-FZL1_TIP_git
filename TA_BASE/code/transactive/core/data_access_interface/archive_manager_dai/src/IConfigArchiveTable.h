#if !defined(IConfigArchiveTable_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigArchiveTable_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/IConfigArchiveTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IConfigArchiveTable - This is the interface supported by objects that allow configuration of
  * ArchiveTables. It provides methods for retrieving, settings, and writing data for a 
  * ArchiveTable object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

// KT 26 Feb 04, PW #3133:  ONLINE_PERIOD has been moved from AR_TABLES and put in AR_ONLINE.

#include <string>

#include "core/data_access_interface/archive_manager_dai/src/IArchiveTable.h"

namespace TA_Base_Core
{
    class IConfigArchiveTable : public IArchiveTable
    {
    public:
        /**
         * ~IConfigArchiveTable
         *
         * Standard destructor.
         */

        virtual ~IConfigArchiveTable() {};

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
         *              Thrown if there are multiple entries for the same position.
         */

        virtual void setName( const std::string& name ) = 0;

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

        virtual void setQueryColumn( const std::string& queryColumn ) = 0;
        
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

        virtual void setFilePostfix( const std::string& filePostfix ) = 0;

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

        virtual void deleteRestoredDataForDate( const time_t& date ) = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this ArchiveTable as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    deleteThisObject() has NOT been called
         */

	    virtual void invalidate() = 0;

        /**
         * deleteThisObject
         *
	     * Removes this ArchiveTable mapping from the database. 
         * The calling application MUST then delete this ArchiveTable object, as it makes no sense
         * to keep it any longer.
         *
         * Precondition:    Either - this ArchiveTable mapping was initially loaded from the database
         *                  OR     - writeArchiveTableData() has already been called
         *                  AND    - deleteThisObject() has NOT been called
         *
         * @exception   DatabaseException 
         *              Thrown if there is a problem accessing the data to the database.
         */

	    virtual void deleteThisObject() = 0;

        /**
         * applyChanges
         *
	     * This will apply all changes made to the database.
         *
         * Precondition:    deleteThisObject() has NOT been called
         *
         * @exception   DatabaseException 
         *              Thrown if there is a problem accessing the data to the database.
         * @exception   DataException 
         *              Thrown if there is more than one VideoOutput per position.
         * @exception   DataConfigurationException 
         *              Thrown if the data contained in the ArchiveTable object
         *              is not sufficent to create an entry in the database.
	     */

	    virtual void applyChanges() = 0;
    };

} //close namespace TA_Base_Core

#endif // !defined(IConfigArchiveTable_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
