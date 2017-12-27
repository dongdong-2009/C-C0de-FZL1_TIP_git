/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveTableHelper.cpp $
  * @author Katherine Thomson
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/05/25 17:45:22 $
  * Last modified by:  $Author: raghu.babu $
  * 
  * ArchiveTableHelper is an object that is held by ArchiveTable and ConfigArchiveTable objects. 
  * It contains all data used by ArchiveTables, and manipulation
  * methods for the data. It helps avoid re-writing code for both ArchiveTable and ConfigArchiveTable.
  */

// KT 26 Feb 04, PW #3133:  ONLINE_PERIOD has been moved from AR_TABLES and put in AR_ONLINE.

#ifdef _MSC_VER
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated
#endif // _MSC_VER

#include <vector>
#include <memory>
#include <ctime>

#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineTable.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveManagerDaiUtilities.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveTableHelper.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveTableAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveTableTable.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveOnline.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace
{
    const unsigned short MAX_ROWS_TO_DELETE_AT_ONCE( 5000 );
    const int NUM_SECS_PER_DAY( 60 * 60 * 24 );
}

namespace TA_Base_Core
{
    //
    // ArchiveTableHelper (For configuration)
    //
    ArchiveTableHelper::ArchiveTableHelper()
        : m_isValidData( false ),
          m_key( 0 ),
          m_name( "" ),
          m_queryColumn( "" ),
          m_filePostfix( "" ),
          m_isNew( true )
    {
        // Do nothing.
    }


    //
    // ArchiveTableHelper
    //
    ArchiveTableHelper::ArchiveTableHelper( unsigned long archiveTableKey )
        : m_isValidData( false ),
          m_key( archiveTableKey ),
          m_name( "" ),
          m_queryColumn( "" ),
          m_filePostfix( "" ),
          m_isNew( false )
    {
        // Do nothing.
		reload();
    }


    //
    // ArchiveTableHelper (Copy Constructor)
    // 
	ArchiveTableHelper::ArchiveTableHelper( const ArchiveTableHelper& theArchiveTableHelper )
		: m_isValidData( theArchiveTableHelper.m_isValidData ),
          m_key( theArchiveTableHelper.m_key ),
          m_name( theArchiveTableHelper.m_name ),
          m_queryColumn( theArchiveTableHelper.m_queryColumn ),
          m_filePostfix( theArchiveTableHelper.m_filePostfix ),
          m_isNew( theArchiveTableHelper.m_isNew )
	{
        // Do nothing.
	}


    //
    // getKey
    //
    unsigned long ArchiveTableHelper::getKey()
    {
        // This method cannot be called until the key has been set.

        TA_ASSERT( 0 != m_key, "The ArchiveTable key has not been initialised." );
        return m_key;
    }


    //
    // getName
    //
    std::string ArchiveTableHelper::getName()
    {
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        return m_name;
    }


    //
    // setName
    //
    void ArchiveTableHelper::setName( const std::string& name )
    {
        FUNCTION_ENTRY( "setName" );

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        m_name = name;
        
        FUNCTION_EXIT;
    }


    //
    // getQueryColumn
    //
    std::string ArchiveTableHelper::getQueryColumn()
    {
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        return m_queryColumn;
    }


    //
    // setQueryColumn
    //
    void ArchiveTableHelper::setQueryColumn( const std::string& queryColumn )
    {
        FUNCTION_ENTRY( "setQueryColumn" );

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        m_queryColumn = queryColumn;
        
        FUNCTION_EXIT;
    }


    //
    // getFilePostfix
    //
    std::string ArchiveTableHelper::getFilePostfix()
    {
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        return m_filePostfix;
    }


    //
    // setFilePostfix
    //
    void ArchiveTableHelper::setFilePostfix( const std::string& filePostfix )
    {
        FUNCTION_ENTRY( "setFilePostfix" );

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        m_filePostfix = filePostfix;
        
        FUNCTION_EXIT;
    }


    //
    // isDateInQueryColumn
    //
    bool ArchiveTableHelper::isDateInQueryColumn( const time_t& date )
    {
        // KT 9 Mar 04: PW #3161 This method has been changed to take a time_t instead of a std::string.

        FUNCTION_ENTRY( "isDateInQueryColumn" );

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        // Get a connection to the database.

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        if( 0 == databaseConnection )
        {
            std::stringstream error;
            error << "Failed to get a connection to the database.";
            TA_THROW( DbConnectionFailed(error.str().c_str()) );
        }

        // Count the number of rows that have a date like the given date 
        // in their query column.

      /*  std::stringstream selectSql;

		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat2);*/
		std::string strTableName(databaseConnection->escapeQueryString(m_name));
		const char *pszTableName = strlwr(const_cast<char*>(strTableName.c_str()));

		// TD12474 ++
		/*selectSql << "select count( * ) from " << databaseConnection->escapeQueryString(m_name) << " ";
        selectSql << "where " << databaseConnection->escapeQueryString(m_queryColumn) << " like ";
        selectSql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( date );
        selectSql << "', '" << TA_ArchiveManagerDai::DATE_FORMAT << "' )";*/
		/*selectSql << "select count( * ) from " << databaseConnection->escapeQueryString(m_name) << " ";
        selectSql << "where " << databaseConnection->escapeQueryString(m_queryColumn) << " like ";
        selectSql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( date );
        selectSql << "', '" << dateFormatStr << "' )";*/
		// ++ TD12474
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_tables_Oracle_SELECT_30551,pszTableName,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_TABLES_SELECT_30551,pszTableName,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_TABLES_SELECT_30551,pszTableName,
			databaseConnection->escapeQueryString(m_queryColumn), TA_ArchiveManagerDai::convertTimeTToStringDate( date ),
			TA_ArchiveManagerDai::convertTimeTToStringDate( date ));
        
        const std::string COUNT( "COUNT" );
        std::vector< std::string > columnNames;
        columnNames.push_back( COUNT );
        std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data.get() )
        {
            std::stringstream error;
            error << "Failed to load the data for the ArchiveTable with the key " << m_key;
            error << " from the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME << ".";
            TA_THROW( DatabaseException( error.str().c_str()) );
        }

        // If there is not already an entry for this ArchiveTable.

        if ( 1 != data->getNumRows() )     // This is a new ArchiveTable.
        {
            std::stringstream error;
            error << "The incorrect amount of data was retrieved from the database. ";
            error << "Unable to determine if the date already exists in the query column for the table " << m_name;
            TA_THROW( DataException( error.str().c_str(), DataException::NOT_UNIQUE,TA_ArchiveManagerDAI::QUERY_COLUMN ) );
        }

        if ( 0 < data->getIntegerData( 0, COUNT ) )
        {
            return true;
        }

        return false;
    }


    //
    // deleteRestoredDataForDate
    //
    void ArchiveTableHelper::deleteRestoredDataForDate( const time_t& date )
    {
        // KT 9 Mar 04: PW #3161 This method has been changed to take a time_t instead of a std::string.

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        // KT 27 Feb 04: PW #3135: The Online Period has been moved from AR_TABLES
        // to AR_ONLINE.

        std::auto_ptr< TA_Base_Core::IArchiveOnline > archiveOnlineEntry( 
                TA_Base_Core::ArchiveOnlineAccessFactory::getInstance().getArchiveOnlineEntry() );
        if( 0 == archiveOnlineEntry.get() )
        {
            std::stringstream error;
            error << "Failed to retrieve the online period from the ";
            error << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME << " table.";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }
        else
        {
            // We can only delete data that has been kept in the database 
            // for a certain time after the query_column for that table
            // has been filled in.  The archive date will be the date in
            // the query column.

            time_t todaysDate;
            time( &todaysDate );

            if( (( todaysDate - date ) / NUM_SECS_PER_DAY ) <= archiveOnlineEntry->getOnlinePeriodInDays() )
            {
                // This is not an error!  The data will be deleted automatically when 
                // the online period expires.

                return;
            }
        }

        // Get a connection to the database.

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        if( 0 == databaseConnection )
        {
            std::stringstream error;
            error << "Failed to get a connection to the database.";
            TA_THROW( DbConnectionFailed(error.str().c_str()) );
        }

        // Create the SQL string to find out how many records need to be deleted.
        // We want to delete in blocks of MAX_ROWS_TO_DELETE_AT_ONCE (otherwise it locks everything up).

        /*std::stringstream selectSql;

		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat2);*/
		std::string strTableName(databaseConnection->escapeQueryString(m_name));
		const char *pszTableName = strlwr(const_cast<char*>(strTableName.c_str()));

		// TD12474 ++
        /*selectSql << "select count(*) from " << databaseConnection->escapeQueryString(m_name);
        selectSql << " where " << databaseConnection->escapeQueryString(m_queryColumn) << " like ";
        selectSql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( date );
        selectSql << "', '" << TA_ArchiveManagerDai::DATE_FORMAT << "' )";*/
        /*selectSql << "select count(*) from " << databaseConnection->escapeQueryString(m_name);
        selectSql << " where " << databaseConnection->escapeQueryString(m_queryColumn) << " like ";
        selectSql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( date );
        selectSql << "', '" << dateFormatStr << "' )";*/
		// ++ TD12474
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_tables_Oracle_SELECT_30551, pszTableName,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_TABLES_SELECT_30551, pszTableName,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_TABLES_SELECT_30551, pszTableName,
			databaseConnection->escapeQueryString(m_queryColumn), TA_ArchiveManagerDai::convertTimeTToStringDate( date ),
			TA_ArchiveManagerDai::convertTimeTToStringDate( date ));

        // Execute the query. 
        //
        // The executeQuery() method can throw a DatabaseException, as documented 
        // in the comment of this method.
        //
        // We are responsible for deleting the returned IData object when we're done with it,
        // so make it an std::auto_ptr, then it will automatically be cleaned up.

        const std::string COUNT( "Count" );
        std::vector< std::string > columnNames;
        columnNames.push_back( COUNT );

        std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( ( 0 == data.get() ) || ( 1 != data->getNumRows() ) )
        {
            std::stringstream error;
            error << "Failed to determine the amount of data to delete.";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }

        int numReps = data->getIntegerData( 0, COUNT ) / MAX_ROWS_TO_DELETE_AT_ONCE + 1;

        /*std::stringstream deleteSql;

		dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat2);*/

		// TD12474 ++
        /*deleteSql << "delete from " << databaseConnection->escapeQueryString(m_name) << " where ";
        deleteSql << databaseConnection->escapeQueryString(m_queryColumn) << " like ";
        deleteSql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( date );
        deleteSql << "', '" << TA_ArchiveManagerDai::DATE_FORMAT << "' ) ";
        deleteSql << "and rownum < " << MAX_ROWS_TO_DELETE_AT_ONCE;*/
		/*deleteSql << "delete from " << databaseConnection->escapeQueryString(m_name) << " where ";
        deleteSql << databaseConnection->escapeQueryString(m_queryColumn) << " like ";
        deleteSql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( date );
        deleteSql << "', '" << dateFormatStr << "' ) ";
        deleteSql << "and rownum < " << MAX_ROWS_TO_DELETE_AT_ONCE;*/
		// ++ TD12474
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_tables_Oracle_DELETE_30951, pszTableName, 
//		strSql  = databaseConnection->prepareSQLStatement(AR_TABLES_DELETE_30951, pszTableName, 

		databaseConnection->prepareSQLStatement(strSql, AR_TABLES_DELETE_30951, pszTableName,
			databaseConnection->escapeQueryString(m_queryColumn), TA_ArchiveManagerDai::convertTimeTToStringDate( date ),
			 TA_ArchiveManagerDai::convertTimeTToStringDate( date ), MAX_ROWS_TO_DELETE_AT_ONCE );
        
        do
        {
            databaseConnection->executeModification( strSql );
            numReps--;
        } while( numReps > 0 );
    }


    //
    // invalidate
    //
    void ArchiveTableHelper::invalidate()
    {
        FUNCTION_ENTRY( "invalidate" );

        // This method cannot be called until the ArchiveTableKey and 
        // the VideoInput position have been set.

        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new ArchiveTable entry.");
        
        m_isValidData = false;

        FUNCTION_EXIT;
    }


    //
    // reload
    //
    void ArchiveTableHelper::reload()
    {
        FUNCTION_ENTRY( "reload" );
        
        // Ensure the ArchiveTable entry has already been written to the database. 
        // It doesn't make any sense to reload a ArchiveTable entry that doesn't exist in the database.
        
        TA_ASSERT( 0 != m_key, 
            "This ArchiveTable does not yet exist in the database. Therefore it cannot be reloaded.");

        // Get a connection to the database.
        
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        if( 0 == databaseConnection )
        {
            std::stringstream error;
            error << "Failed to get a connection to the database.";
            TA_THROW( DbConnectionFailed(error.str().c_str()) );
        }

        // Create the SQL string to retrieve the data for the ArchiveTable
        // based upon the ArchiveTable key.

       /* std::stringstream selectSql;
        selectSql << "select " << TA_ArchiveManagerDAI::TABLE_NAME_COLUMN;
        selectSql << "," << TA_ArchiveManagerDAI::QUERY_COLUMN;
        selectSql << "," << TA_ArchiveManagerDAI::FILE_POSTFIX_COLUMN;
        selectSql << " from " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME;
        selectSql << " where " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN << " = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_tables_STD_SELECT_30503, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_TABLES_SELECT_30503, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_TABLES_SELECT_30503, m_key);

        // Execute the query. 
        //
        // The executeQuery() method can throw a DatabaseException, as documented 
        // in the comment of this method.
        //
        // We are responsible for deleting the returned IData object when we're done with it,
        // so make it an std::auto_ptr, then it will automatically be cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_ArchiveManagerDAI::TABLE_NAME_COLUMN );
        columnNames.push_back( TA_ArchiveManagerDAI::QUERY_COLUMN );
        columnNames.push_back( TA_ArchiveManagerDAI::FILE_POSTFIX_COLUMN );

        std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data.get() )
        {
            std::stringstream error;
            error << "Failed to load the data for the ArchiveTable with the key " << m_key;
            error << " from the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME << ".";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }

        // Check that at least one entry was found for this key.  Note that for this
        // class it is generally expected that more than one entry will be found -
        // in fact we are expecting one entry per position in the ArchiveTable, except
        // in the case where some positions are empty.
        
        int numRows = data->getNumRows();
        if ( 0 == numRows ) // No entry found with the specified key.
        {
            std::stringstream error;
            error << "The ArchiveTable with the key " << m_key;
            error << " was not found in the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME << ".";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        
        m_name = data->getStringData( 0, TA_ArchiveManagerDAI::TABLE_NAME_COLUMN );
        m_queryColumn = data->getStringData( 0, TA_ArchiveManagerDAI::QUERY_COLUMN );
        m_filePostfix = data->getStringData( 0, TA_ArchiveManagerDAI::FILE_POSTFIX_COLUMN );

        // The data is now valid.

        m_isValidData = true;

        FUNCTION_EXIT;
    }


    //
    // writeArchiveTableData
    //
    void ArchiveTableHelper::writeArchiveTableData()
    {       
        FUNCTION_ENTRY( "writeArchiveTableData" );     
        
        if ( m_isNew )
        {
            insertNewArchiveTable();
        }
        else
        {
            updateExistingArchiveTable();
        }

        // Now that the data has been written, what we hold is valid data.

        m_isValidData = true;
        
        FUNCTION_EXIT;
    }


    //
    // deleteArchiveTable
    //
    void ArchiveTableHelper::deleteArchiveTable()
    {
        FUNCTION_ENTRY( "deleteArchiveTable" );
        TA_ASSERT( 0 != m_key, "This ArchiveTable does not yet exist in the database, and therefore cannot be deleted." );
        
        // Get a connection to the database.

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        
        /*std::stringstream deleteSql;
        deleteSql << "delete from " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME;
        deleteSql << " where " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN;
        deleteSql << "=" << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_tables_STD_DELETE_30901, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_TABLES_DELETE_30901, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_TABLES_DELETE_30901, m_key);
        databaseConnection->executeModification( strSql );

        m_key = 0;
        m_name = "";
        m_queryColumn = "";
        m_filePostfix = "";

        FUNCTION_EXIT;
    }


    //
    // insertNewArchiveTable
    //
    void ArchiveTableHelper::insertNewArchiveTable()
    {
        FUNCTION_ENTRY( "insertNewArchiveTable" );

        // Get a connection to the database

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        if( 0 == databaseConnection )
        {
            std::stringstream error;
            error << "Failed to get a connection to the database.";
            TA_THROW( DbConnectionFailed(error.str().c_str()) );
        }

        // liborm 3/5/2004
		// Get the key for the new archive table, and update the local key copy
		/*std::stringstream getKeySql;
        getKeySql << "select ARTABL_SEQ.NEXTVAL from DUAL";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_tables_Oracle_SELECT_30552);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_TABLES_SELECT_30552);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_TABLES_SELECT_30552);

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN );

		std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data.get() )
        {
            TA_THROW( DataException( "Failed to insert the archive table into the database. Get next key failed.", DataException::NO_VALUE, "" ));
        }
        if ( 1 != data->getNumRows() )
        {
            TA_THROW( DataException( "Failed to insert the archive table into the database. An invalid number of next keys were retrieved.", DataException::NO_VALUE, "" ));
        }
        m_key = data->getIntegerData( 0, TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN );

        // Write the ArchiveTable to the database.

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
            "This is a new ArchiveTable. It will be added to the database." );
       
        /*std::stringstream insertSql;
        insertSql << "insert into " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME << " (" ;
        insertSql << TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN << ", ";
        insertSql << TA_ArchiveManagerDAI::TABLE_NAME_COLUMN << ", ";
        insertSql << TA_ArchiveManagerDAI::QUERY_COLUMN << ", ";
        insertSql << TA_ArchiveManagerDAI::FILE_POSTFIX_COLUMN << ") values (";
        insertSql << m_key << ", '";
        insertSql << databaseConnection->escapeInsertString( m_name ) << "', '";
        insertSql << databaseConnection->escapeInsertString( m_queryColumn ) << "', '";
        insertSql << databaseConnection->escapeInsertString( m_filePostfix ) << "')";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_tables_STD_INSERT_30801, m_key, 
//		strSql  = databaseConnection->prepareSQLStatement(AR_TABLES_INSERT_30801, m_key, 

		databaseConnection->prepareSQLStatement(strSql, AR_TABLES_INSERT_30801, m_key,
			databaseConnection->escapeInsertString( m_name ), databaseConnection->escapeInsertString( m_queryColumn ),
			databaseConnection->escapeInsertString( m_filePostfix ) );
      
        databaseConnection->executeModification( strSql );
      
        m_isNew = false;

        FUNCTION_EXIT;
    }


    //
    // updateExistingArchiveTable
    //
    void ArchiveTableHelper::updateExistingArchiveTable()
    {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
            "The ArchiveTable with the key %lu already exists. It's data will be updated.",
            m_key );

        // Get a connection to the database

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        if( 0 == databaseConnection )
        {
            std::stringstream error;
            error << "Failed to get a connection to the database.";
            TA_THROW( DbConnectionFailed(error.str().c_str()) );
        }

        // Write the ArchiveTable to the database.
  
        // First check if there is already a ArchiveTable with this key in the database.
        // Use a std::auto_ptr for the IData so it is automatically cleaned up.
        
        /*std::stringstream selectSql;
        selectSql << "select " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN;
        selectSql << " from " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME;
        selectSql << " where " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN;
        selectSql << "=" << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_tables_STD_SELECT_30501, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_TABLES_SELECT_30501, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_TABLES_SELECT_30501, m_key);

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN );
       
        std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data.get() )
        {
            std::stringstream error;
            error << "Failed to load the data for the ArchiveTable with the key " << m_key;
            error << " from the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME << ".";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }

        // If there is not already an entry for this ArchiveTable.

        if ( 1 != data->getNumRows() )     // This is a new ArchiveTable.
        {
            std::stringstream error;
            error << "The ArchiveTable with database key = " << m_key;
            error << " does not already exist in the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME;
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE,TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN ) );
        }
        else    // This is an existing ArchiveTable.
        {
           /* std::stringstream sql;
            sql << "update " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_TABLE_NAME;
            sql << " set " << TA_ArchiveManagerDAI::TABLE_NAME_COLUMN;
            sql << "='" << databaseConnection->escapeInsertString( m_name );
            sql << "', " << TA_ArchiveManagerDAI::QUERY_COLUMN;
            sql << "='" << databaseConnection->escapeInsertString( m_queryColumn );
            sql << "', " << TA_ArchiveManagerDAI::FILE_POSTFIX_COLUMN;
            sql << "='" << databaseConnection->escapeInsertString( m_filePostfix );
            sql << "' where " << TA_ArchiveManagerDAI::ARCHIVE_TABLE_KEY_COLUMN << "=" << m_key;*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_tables_STD_UPDATE_30651, 
//			strSql  = databaseConnection->prepareSQLStatement(AR_TABLES_UPDATE_30651, 

		databaseConnection->prepareSQLStatement(strSql, AR_TABLES_UPDATE_30651,
				databaseConnection->escapeInsertString( m_name ), databaseConnection->escapeInsertString( m_queryColumn ),
				databaseConnection->escapeInsertString( m_filePostfix ), m_key);

            // Execute the modification. The method can throw a DatabaseException.
            // This is documented in the comment of this method.

            databaseConnection->executeModification( strSql );
        }
        m_isNew = false;
    }

} // closes TA_Base_Core
