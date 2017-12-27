#if !defined(ConfigArchiveTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigArchiveTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ConfigArchiveTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigArchiveTable is a concrete implementation of IConfigArchiveTable, which is in turn 
  * an implementation of IArchiveTable. It is to be used by the Configuration Editor, 
  * and other applications that wish to change the database content for ArchiveTables.
  */

// KT 26 Feb 04, PW #3133:  ONLINE_PERIOD has been moved from AR_TABLES and put in AR_ONLINE.

#include <memory>
#include <string>

#include "core/data_access_interface/archive_manager_dai/src/IConfigArchiveTable.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveTableHelper.h"

namespace TA_Base_Core
{
    class ConfigArchiveTable : public IConfigArchiveTable
    {
    public:
        /**
         * ConfigArchiveTable
         *
         * Standard constructer that creates a configurable ArchiveTable object with the given key.
         * This is used when creating a *new* ArchiveTable - that is, one that does not yet exist 
         * in the database.
         *
         * @param       unsigned long key 
         *              The unique key of this ArchiveTable.
         * 
         * @exception   ArchiveException
         *              Thrown if the ArchiveTableHelper cannot be created.
         */

        ConfigArchiveTable( unsigned long key );
        
        /**
         * ConfigArchiveTable
         *
         * Standard constructer that creates a configurable ArchiveTable object with the given key.
         * This is used when creating a *new* ArchiveTable - that is, one that does not yet exist 
         * in the database.
         *
         * @exception   ArchiveException
         *              Thrown if the ArchiveTableHelper cannot be created.
         */

        ConfigArchiveTable();

        /**
         * ~ConfigArchiveTable
         *
         * Standard destructor.
         */

        virtual ~ConfigArchiveTable() {};

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

        virtual void setName( const std::string& name );

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

        virtual void setQueryColumn( const std::string& queryColumn );
        
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

        virtual void setFilePostfix( const std::string& filePostfix );
      
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
         * Precondition:    deleteThisObject() has NOT been called
         */

	    virtual void invalidate();

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

	    virtual void deleteThisObject();

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

	    virtual void applyChanges();
        
    private:
        //
        // Disable copy constructor and assignment operator.
        //

        ConfigArchiveTable( const ConfigArchiveTable& theConfigArchiveTable );  
  		ConfigArchiveTable& operator=( const ConfigArchiveTable & );

        //
        // This is the object that does all the work.  It is a std::auto_ptr
        // so it will be automatically cleaned up on destruction of this object.
        //
        
        std::auto_ptr< ArchiveTableHelper > m_archiveTableHelper;
    };

} // closes TA_Base_Core

#endif // !defined(ConfigArchiveTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
