/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveOnlineHelper.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveOnlineHelper is an object that is held by ArchiveOnline and ConfigArchiveOnline objects. 
  * It contains all data used by ArchiveOnline enrty, and manipulation methods for the data.
  * It helps avoid re-writing code for both ArchiveOnline and ConfigArchiveOnline.
  */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated
#endif // _MSC_VER

#include <memory>

#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineHelper.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineTable.h"
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
    const unsigned short MIN_ONLINE_PERIOD_IN_DAYS      ( 7   );      // 1 week
    const unsigned short MAX_ONLINE_PERIOD_IN_DAYS      ( 365 );      // 1 year
    const unsigned short MIN_DATA_EXPIRY_IN_DAYS        ( 7   );      // 1 week
    const unsigned short MAX_DATA_EXPIRY_IN_DAYS        ( 365 );      // 1 year
    const unsigned short DEFAULT_ONLINE_PERIOD_IN_DAYS  ( 30  );      // 1 month
    const unsigned short DEFAULT_DATA_EXPIRY_IN_DAYS    ( 14  );      // 2 weeks
}

namespace TA_Base_Core
{
    //
    // ArchiveOnlineHelper (For configuration)
    //
    ArchiveOnlineHelper::ArchiveOnlineHelper()
        : m_isValidData( false ),
          m_isNew( true ),
          m_key( 0 ),
          m_onlinePeriodInDays( DEFAULT_ONLINE_PERIOD_IN_DAYS ),
          m_dataExpiryInDays( DEFAULT_DATA_EXPIRY_IN_DAYS )
    {
        // Do nothing.
    }


    //
    // ArchiveOnlineHelper (For configuration)
    //
    ArchiveOnlineHelper::ArchiveOnlineHelper( unsigned long archiveOnlineKey )
        : m_isValidData( false ),
          m_isNew( false ),
          m_key( archiveOnlineKey ),
          m_onlinePeriodInDays( DEFAULT_ONLINE_PERIOD_IN_DAYS ),
          m_dataExpiryInDays( DEFAULT_DATA_EXPIRY_IN_DAYS )
    {
        // Do nothing.
    }


    //
    // ArchiveOnlineHelper (Copy Constructor)
    // 
	ArchiveOnlineHelper::ArchiveOnlineHelper( const ArchiveOnlineHelper& theArchiveOnlineHelper )
		: m_isValidData( theArchiveOnlineHelper.m_isValidData ),
          m_key( theArchiveOnlineHelper.m_key ),
          m_onlinePeriodInDays( theArchiveOnlineHelper.m_onlinePeriodInDays ),
          m_dataExpiryInDays( theArchiveOnlineHelper.m_dataExpiryInDays )
	{
        // Do nothing.
	}


    //
    // getKey
    //
    unsigned long ArchiveOnlineHelper::getKey()
    {
        // This method cannot be called until the key has been set.

        TA_ASSERT( 0 != m_key, "The ArchiveOnline key has not been initialised." );
        return m_key;
    }


    //
    // getOnlinePeriodInDays
    //
    unsigned short ArchiveOnlineHelper::getOnlinePeriodInDays()
    {
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        return m_onlinePeriodInDays;
    }


    //
    // setOnlinePeriodInDays
    //
    void ArchiveOnlineHelper::setOnlinePeriodInDays( unsigned short onlinePeriodInDays )
    {
        FUNCTION_ENTRY( "setOnlinePeriodInDays" );

        TA_ASSERT( onlinePeriodInDays >= MIN_ONLINE_PERIOD_IN_DAYS, "The online period is less than the minimum number of days." );
        TA_ASSERT( onlinePeriodInDays <= MAX_ONLINE_PERIOD_IN_DAYS, "The online period is greater than the maximum number of days." );

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        m_onlinePeriodInDays = onlinePeriodInDays;
        
        FUNCTION_EXIT;
    }


    //
    // getDataExpiryInDays
    //
    unsigned short ArchiveOnlineHelper::getDataExpiryInDays()
    {
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        return m_dataExpiryInDays;
    }


    //
    // setDataExpiryInDays
    //
    void ArchiveOnlineHelper::setDataExpiryInDays( unsigned short dataExpiryInDays )
    {
        FUNCTION_ENTRY( "setDataExpiryInDays" );

        TA_ASSERT( dataExpiryInDays >= MIN_DATA_EXPIRY_IN_DAYS, "The data expiry period is less than the minimum number of days." );
        TA_ASSERT( dataExpiryInDays <= MAX_DATA_EXPIRY_IN_DAYS, "The data expiry period is greater than the maximum number of days." );

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        m_dataExpiryInDays = dataExpiryInDays;
        
        FUNCTION_EXIT;
    }


    //
    // invalidate
    //
    void ArchiveOnlineHelper::invalidate()
    {
        FUNCTION_ENTRY( "invalidate" );

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    //
    // applyChanges
    //
    void ArchiveOnlineHelper::applyChanges()
    {       
        FUNCTION_ENTRY( "applyChanges" );     
        
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
            "The ArchiveOnline with the key %lu should already exist. It's data will be updated.", m_key );

        // Get a connection to the database

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        if( 0 == databaseConnection )
        {
            std::stringstream error;
            error << "Failed to get a connection to the database.";
            TA_THROW( DbConnectionFailed(error.str().c_str()) );
        }

        // Write the ArchiveOnline data to the database.
  
        // First check if there is already a ArchiveOnline with this key in the database.
        // Use a std::auto_ptr for the IData so it is automatically cleaned up.
        
        /*std::stringstream selectSql;
        selectSql << "select " << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN;
        selectSql << " from " << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_online_STD_SELECT_30001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_ONLINE_SELECT_30001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_ONLINE_SELECT_30001);

		// liborm 03/05/04
		// removed "where key=m_key" part so that ALL entries that are in the archive
		// online table are returned

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN );
       
        std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data.get() )
        {
            std::stringstream error;
            error << "Failed to load the data for the ArchiveOnline with the key " << m_key;
            error << " from the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME << ".";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }

        // There should only ever be one entry in the ArchiveOnline table.

        if ( 0 == data->getNumRows() )      // This is a new ArchiveOnline.
        {
            if ( m_isNew )
            {
               /* std::stringstream selectSql;
                selectSql << "select ARINVE_SEQ.NEXTVAL from DUAL";*/
//				strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_online_Oracle_SELECT_30051);
//				strSql  = databaseConnection->prepareSQLStatement(AR_ONLINE_SELECT_30051);

		databaseConnection->prepareSQLStatement(strSql, AR_ONLINE_SELECT_30051);

                std::vector< std::string > columnNames;
                columnNames.push_back( TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN );

                // Execute the query. The method can throw a DatabaseException.
                // This is documented in the comment of this method.

                std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
                if ( 0 == data.get() )
                {
                    TA_THROW( DataException( "Failed to insert the online item into the database. Get next key failed.", DataException::NO_VALUE, "" ));
                }
                if ( 1 != data->getNumRows() )
                {
                    TA_THROW( DataException( "Failed to insert the online item into the database. An invalid number of next keys were retrieved.", DataException::NO_VALUE, "" ));
                }
                m_key = data->getIntegerData( 0, TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN );

                /*std::stringstream sql;
                sql << "insert into " << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME << "(";
                sql << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN << ", ";
                sql << TA_ArchiveManagerDAI::ONLINE_PERIOD_IN_DAYS_COLUMN << ", ";
                sql << TA_ArchiveManagerDAI::DATA_EXPIRY_IN_DAYS_COLUMN << ") VALUES(";
                sql << m_key << ", " << m_onlinePeriodInDays << ", " << m_dataExpiryInDays << ")";*/
//				strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_online_STD_INSERT_30301, m_key,
//				strSql  = databaseConnection->prepareSQLStatement(AR_ONLINE_INSERT_30301, m_key,

		databaseConnection->prepareSQLStatement(strSql, AR_ONLINE_INSERT_30301, m_key,
					m_onlinePeriodInDays, m_dataExpiryInDays);

                // Execute the modification. The method can throw a DatabaseException.
                // This is documented in the comment of this method.

                databaseConnection->executeModification( strSql );
            }
            else
            {
                std::stringstream error;
                error << "The ArchiveOnline with database key = " << m_key;
                error << " does not already exist in the database table ";
                error << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME;
                TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE,TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN ) );
            }
        }
        else if ( 1 == data->getNumRows() ) // This is an existing ArchiveOnline.
        {

			// liborm 03/05/04
			// added in check to see if we are attempting to
			// insert new data, when there is already one entry
			// in the archive online table
			if( m_isNew )
			{
				std::stringstream error;
				error << "Cannot insert a new ArchiveOnline table as one already exists";

				TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE,TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN ) );

			}

			else
			{
				// liborm 03/05/04
				// added in check to see if the key of the data we wish to update
				// corresponds to the key of the data already in the database
				if( m_key == data->getIntegerData( 0, TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN ) )
				{

					/*std::stringstream sql;
					sql << "update " << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME;
					sql << " set " << TA_ArchiveManagerDAI::ONLINE_PERIOD_IN_DAYS_COLUMN << "=" << m_onlinePeriodInDays;
					sql << ", " << TA_ArchiveManagerDAI::DATA_EXPIRY_IN_DAYS_COLUMN << "=" << m_dataExpiryInDays;
					sql << " where " << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN << "=" << m_key;*/
//					strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_online_STD_UPDATE_30151,
//					strSql  = databaseConnection->prepareSQLStatement(AR_ONLINE_UPDATE_30151,

		databaseConnection->prepareSQLStatement(strSql, AR_ONLINE_UPDATE_30151,
						m_onlinePeriodInDays, m_dataExpiryInDays, m_key);

					// Execute the modification. The method can throw a DatabaseException.
					// This is documented in the comment of this method.

					databaseConnection->executeModification( strSql );
				}

				else
				{
					std::stringstream error;
					error << "The ArchiveOnline with database key = " << m_key;
					error << " does not already exist in the database table ";
					error << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME;
					TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE,TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN ) );

				}

			}
        }
        else
        {
			std::stringstream error;
            error << "The ArchiveOnline with database key = " << m_key;
            error << " does not already exist in the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME;
            error << " and it cannot be inserted because there is already an entry in the table and only one entry is allowed.";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE,TA_ArchiveManagerDAI::ARCHIVE_ONLINE_KEY_COLUMN ) );
        }

        // Now that the data has been written, what we hold is valid data.

        m_isValidData = true;
        m_isNew = false;
        
        FUNCTION_EXIT;
    }


    //
    // reload
    //
    void ArchiveOnlineHelper::reload()
    {
        FUNCTION_ENTRY( "reload" );
        
        // Ensure the ArchiveOnline entry has already been written to the database. 
        // It doesn't make any sense to reload a ArchiveOnline entry that doesn't exist in the database.
        
        TA_ASSERT( 0 != m_key, "This ArchiveOnline does not yet exist in the database. Therefore it cannot be reloaded.");

        // Get a connection to the database.
        
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        if( 0 == databaseConnection )
        {
            std::stringstream error;
            error << "Failed to get a connection to the database.";
            TA_THROW( DbConnectionFailed(error.str().c_str()) );
        }

        // Create the SQL string to retrieve the data for the ArchiveOnline entry in the database.
        // There should only be one!

        /*std::stringstream selectSql;
        selectSql << "select " << TA_ArchiveManagerDAI::ONLINE_PERIOD_IN_DAYS_COLUMN;
        selectSql << "," << TA_ArchiveManagerDAI::DATA_EXPIRY_IN_DAYS_COLUMN;
        selectSql << " from " << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_online_STD_SELECT_30002);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_ONLINE_SELECT_30002);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_ONLINE_SELECT_30002);

        // Execute the query. 
        //
        // The executeQuery() method can throw a DatabaseException, as documented 
        // in the comment of this method.
        //
        // We are responsible for deleting the returned IData object when we're done with it,
        // so make it an std::auto_ptr, then it will automatically be cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_ArchiveManagerDAI::ONLINE_PERIOD_IN_DAYS_COLUMN );
        columnNames.push_back( TA_ArchiveManagerDAI::DATA_EXPIRY_IN_DAYS_COLUMN );

        std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data.get() )
        {
            std::stringstream error;
            error << "Failed to load the data for the ArchiveOnline with the key " << m_key;
            error << " from the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME << ".";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }

        // Check that one entry was found for this key.
        
        int numRows = data->getNumRows();
        if ( 0 == numRows )
        {
            std::stringstream error;
            error << "The ArchiveOnline entry with the key " << m_key;
            error << " was not found in the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME << ".";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }
        else if ( 1 < numRows )
        {
            std::stringstream error;
            error << "Multiple ArchiveOnline entries with the key " << m_key;
            error << " were found in the database table ";
            error << TA_ArchiveManagerDAI::ARCHIVE_ONLINE_TABLE_NAME << ".";
            TA_THROW( DataException( error.str().c_str(), DataException::NO_VALUE, "" ));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        
        m_onlinePeriodInDays = data->getIntegerData( 0, TA_ArchiveManagerDAI::ONLINE_PERIOD_IN_DAYS_COLUMN );
        m_dataExpiryInDays = data->getIntegerData( 0, TA_ArchiveManagerDAI::DATA_EXPIRY_IN_DAYS_COLUMN );

        // Note that m_onlinePeriodInDays and m_dataExpiryInDays must be in the valid ranges.  
        // If someone has edited the database and put in values outside this range, correct them in the
        // code (not in the database).

        if( MIN_ONLINE_PERIOD_IN_DAYS > m_onlinePeriodInDays )
        {
            m_onlinePeriodInDays = MIN_ONLINE_PERIOD_IN_DAYS;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "The online period in days has been set to the minimum of %d.", MIN_ONLINE_PERIOD_IN_DAYS );
        }
        else if( MAX_ONLINE_PERIOD_IN_DAYS < m_onlinePeriodInDays )
        {
            m_onlinePeriodInDays = MAX_ONLINE_PERIOD_IN_DAYS;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "The online period in days has been set to the maximum of %d.", MAX_ONLINE_PERIOD_IN_DAYS );
        }
        if( MIN_DATA_EXPIRY_IN_DAYS > m_dataExpiryInDays )
        {
            m_dataExpiryInDays = MIN_DATA_EXPIRY_IN_DAYS;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "The online period in days has been set to the minimum of %d.", MIN_DATA_EXPIRY_IN_DAYS );
        }
        else if( MAX_DATA_EXPIRY_IN_DAYS < m_dataExpiryInDays )
        {
            m_dataExpiryInDays = MAX_DATA_EXPIRY_IN_DAYS;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "The online period in days has been set to the maximum of %d.", MAX_DATA_EXPIRY_IN_DAYS );
        }

        // The data is now valid.

        m_isValidData = true;

        FUNCTION_EXIT;
    }


} // closes TA_Base_Core
