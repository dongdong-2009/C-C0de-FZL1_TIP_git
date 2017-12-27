/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveInventoryData.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class contains the database retrievals and writes for the inventory
  * to be archived using the Archive Manager.
  * It implements the IArchiveInventory interface.
  */

// KT, 26 Feb 04, PW# 3133: Deletion Due Date has been added.
// KT, 26 Feb 04, PW# 3135: References to Start Date and End Date have been replaced with Data Date.
 
#pragma warning( disable : 4786 )

#include <string>
#include <sstream>
#include <ctime>

#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryData.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryTable.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveManagerDaiUtilities.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{
    //
    // ArchiveInventoryData
    //
    ArchiveInventoryData::ArchiveInventoryData()
        : m_isValidData( false )
    {
        // Do nothing.
    }   

    
    //
    // getAllArchiveInventoryItems
    //
    IArchiveInventory::ArchiveInventoryItemMap ArchiveInventoryData::getAllArchiveInventoryItems()
    {
        if ( !m_isValidData )
        {
            reload();
        }
        return m_inventory;
    }


    //
    // getArchiveInventoryItem
    //
    IArchiveInventory::ArchiveInventoryItem ArchiveInventoryData::getArchiveInventoryItem( unsigned long inventoryItemKey )
    {
        FUNCTION_ENTRY("getArchiveInventory");

        if ( !m_isValidData )
        {
            reload();
        }

        ArchiveInventoryItemMap::iterator it( m_inventory.find( inventoryItemKey ) );
        if ( it == m_inventory.end() )
        {
            std::stringstream error;
            error << "Failed to find the data for the archive inventory item with the key " << inventoryItemKey;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE,TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN ) );
        }            

        FUNCTION_EXIT;
        return it->second;
    }


    //
    // setArchiveInventoryItem
    //
    void ArchiveInventoryData::setArchiveInventoryItem( const ArchiveInventoryItem& inventoryItem )
    {
        FUNCTION_ENTRY("setArchiveInventory");

        if ( !m_isValidData )
        {
            reload();
        }

        ArchiveInventoryItemMap::iterator it( m_inventory.find( inventoryItem.key ) );
        if ( it == m_inventory.end() )
        {
            std::stringstream error;
            error << "Failed to find the data for the archive inventory item with the key " << inventoryItem.key;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(),TA_Base_Core::DataException::NO_VALUE,TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN ) );
        }

        updateArchiveInventoryItemInDatabase( inventoryItem );
        it->second = inventoryItem;

        FUNCTION_EXIT;
    }


    //
    // addArchiveInventoryItem
    //
    void ArchiveInventoryData::addArchiveInventoryItem( ArchiveInventoryItem& inventoryItem )
    {
        FUNCTION_ENTRY("addArchiveInventory");

        if ( !m_isValidData )
        {
            reload();
        }
       
        ArchiveInventoryItemMap::iterator it( m_inventory.find( inventoryItem.key ) );
        if ( it == m_inventory.end() )
        {
            insertArchiveInventoryItemIntoDatabase( inventoryItem );
            m_inventory[ inventoryItem.key ] = inventoryItem;
        }
        else
        {
            updateArchiveInventoryItemInDatabase( inventoryItem );
            it->second = inventoryItem;
        }

        FUNCTION_EXIT;
    }


    //
    // removeArchiveInventoryItem
    //
    void ArchiveInventoryData::removeArchiveInventoryItem( unsigned long itemKey )
    {
        FUNCTION_ENTRY("removeArchiveInventoryItem");

        if ( !m_isValidData )
        {
            reload();
        }

        ArchiveInventoryItemMap::iterator it( m_inventory.find( itemKey ) );

		// liborm 04/05/2004
		// only if the specified key exists shall we attempt
		// to remove the item
        if ( it != m_inventory.end() )
        {
            m_inventory.erase( it );
			removeArchiveInventoryItemFromDatabase( itemKey );
        }

		// liborm 04/05/2004
		// the archive inventory item with the specified key does not exist
		// in the database, so throw an exception
		else
		{
			std::stringstream error;
            error << "Failed to remove the data for the archive inventory item with the key " << itemKey;
			error << " as this key does not exist";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(),TA_Base_Core::DataException::NO_VALUE,TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN ) );

		}    

        FUNCTION_EXIT;
    }
        
    
    //
    // isDateInInventory
    //
    bool ArchiveInventoryData::isDateInInventory( const time_t& date )
    {
        // KT 9 Mar 04: PW #3161 This method has been changed to take a time_t instead of a std::string.

        FUNCTION_ENTRY("isDateInInventory");

        //std::stringstream sql;
		std::string dateFormatStr;
		
		IDatabase *pDb = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
		/*int nDbType = pDb->getDbServerType();
		if ( enumOracleDb == nDbType )
			dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat2); 
		else
			dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat2a);*/
		SQLTypeAdapter sqlParam;
		sqlParam.add(enumDateFormat_6);

		// TD12474 
        /*sql << "select count(*) from " << TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME << " ";
        sql << "where " << TA_ArchiveManagerDAI::DATA_DATE_COLUMN << " like ";
        sql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( date );
        sql << "', '" << TA_ArchiveManagerDai::DATE_FORMAT << "' )";*/
		/*sql << "select count(*) from " << TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME << " ";
        sql << "where " << TA_ArchiveManagerDAI::DATA_DATE_COLUMN << " like ";
        sql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( date );
        sql << "', '" << dateFormatStr << "' )";*/
		// ++ TD12474
		/*std::string strSql = defPrepareSQLStatement(nDbType, ar_inventory_Oracle_SELECT_29551,
			TA_ArchiveManagerDai::convertTimeTToStringDate( date ), dateFormatStr);*/
//		std::string strSql = pDb->prepareSQLStatement(AR_INVENTORY_SELECT_29551,
		SQLStatement strSql;
		pDb->prepareSQLStatement(strSql, AR_INVENTORY_SELECT_29551,
			TA_ArchiveManagerDai::convertTimeTToStringDate( date ), sqlParam);
            
        // Set up the columnNames vector to be passed to executeQuery().
        
        const std::string COUNT( "COUNT" );
        std::vector< std::string > columnNames;
        columnNames.push_back( COUNT );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're 
        // done with it.

        std::auto_ptr< IData > data( pDb->executeQuery( strSql, columnNames ) );
        if ( 0 == data.get() )
        {
            TA_THROW( DataException( "Failed to load the data for the ArchiveInventory.",DataException::NO_VALUE, "" ));
        }

        if ( 0 == data->getIntegerData( 0, COUNT ) )
        {
            return false;
        }
     
        FUNCTION_EXIT;
        return true;
    }


    //
    // invalidate
    //
    void ArchiveInventoryData::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        m_isValidData = false;
        FUNCTION_EXIT;
    }


    //
    // reload
    //
    void ArchiveInventoryData::reload()
    {
        FUNCTION_ENTRY("reload");

        m_inventory.clear();

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);

       /* std::stringstream sql;

		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat2); */

		// TD12474 ++
        /*sql << "select " << TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN;
	    sql << ", TO_CHAR(" << TA_ArchiveManagerDAI::DATA_DATE_COLUMN << ",'" << TA_ArchiveManagerDai::DATE_TIME_FORMAT << "')";
        sql << "," << TA_ArchiveManagerDAI::VOLUME_LABEL_COLUMN;
        sql << "," << TA_ArchiveManagerDAI::DELETING_COLUMN;
        sql << ", TO_CHAR(" << TA_ArchiveManagerDAI::RESTORED_DATE_COLUMN << ",'" << TA_ArchiveManagerDai::DATE_TIME_FORMAT << "')";
        sql << "," << TA_ArchiveManagerDAI::RESTORED_BY_COLUMN;
        sql << ", TO_CHAR(" << TA_ArchiveManagerDAI::DELETION_DUE_DATE_COLUMN << ",'" << TA_ArchiveManagerDai::DATE_TIME_FORMAT << "') ";
        sql << "from " << TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME;*/
		/*sql << "select " << TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN;
	    sql << ", TO_CHAR(" << TA_ArchiveManagerDAI::DATA_DATE_COLUMN << ",'" << dateFormatStr << "')";
        sql << "," << TA_ArchiveManagerDAI::VOLUME_LABEL_COLUMN;
        sql << "," << TA_ArchiveManagerDAI::DELETING_COLUMN;
        sql << ", TO_CHAR(" << TA_ArchiveManagerDAI::RESTORED_DATE_COLUMN << ",'" << dateFormatStr << "')";
        sql << "," << TA_ArchiveManagerDAI::RESTORED_BY_COLUMN;
        sql << ", TO_CHAR(" << TA_ArchiveManagerDAI::DELETION_DUE_DATE_COLUMN << ",'" << dateFormatStr << "') ";
        sql << "from " << TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME;*/
		// ++ TD12474
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_inventory_Oracle_SELECT_29552);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_INVENTORY_SELECT_29552);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_INVENTORY_SELECT_29552);

        // Set up the columnNames vector to be passed to executeQuery().
        
        std::vector< std::string > columnNames;
        columnNames.push_back( TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN );
        columnNames.push_back( TA_ArchiveManagerDAI::DATA_DATE_COLUMN );
        columnNames.push_back( TA_ArchiveManagerDAI::VOLUME_LABEL_COLUMN );
        columnNames.push_back( TA_ArchiveManagerDAI::DELETING_COLUMN );
        columnNames.push_back( TA_ArchiveManagerDAI::RESTORED_DATE_COLUMN );
        columnNames.push_back( TA_ArchiveManagerDAI::RESTORED_BY_COLUMN );
        columnNames.push_back( TA_ArchiveManagerDAI::DELETION_DUE_DATE_COLUMN );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're 
        // done with it.

        IData* data = databaseConnection->executeQuery( strSql, columnNames );
        if ( 0 == data )
        {
            TA_THROW( DataException( "Failed to load the data for the ArchiveInventory.",DataException::NO_VALUE, "" ));
        }

        try
        {
            ArchiveInventoryItem inventoryItem;
            do // loop for each lot of IData that has been retrieved
            {
                for ( unsigned long row = 0; row < data->getNumRows(); ++row )
                {
                    // Retrieve the inventoryItem key.

                    inventoryItem.key = data->getIntegerData( row, TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN );

                    // Check that inventoryItem key does not already exist in the map. If it does we
                    // have an error so throw an exception.

                    ArchiveInventoryItemMap::iterator it( m_inventory.find( inventoryItem.key ) );
                    if ( it != m_inventory.end() )
                    {
                        delete data;
                        data = 0;

                        // This entry is a duplicate. This is an error so throw an exception.

                        std::stringstream error;
                        error << "A duplicate archive inventory item with the key " << inventoryItem.key;
                        error << " was found in the database.";
                        TA_THROW( TA_Base_Core::DataException( error.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN ) );
                    }

                    // Retrieve the rest of the inventoryItem data.

                    inventoryItem.dataDate = data->getDateData( row, TA_ArchiveManagerDAI::DATA_DATE_COLUMN );
                    inventoryItem.volumeLabel = data->getStringData( row, TA_ArchiveManagerDAI::VOLUME_LABEL_COLUMN );
                    inventoryItem.isDeleting = data->getBooleanData( row, TA_ArchiveManagerDAI::DELETING_COLUMN );
                    inventoryItem.restoredDate = data->getDateData( row, TA_ArchiveManagerDAI::RESTORED_DATE_COLUMN );
                    inventoryItem.restoredBy = data->getStringData( row, TA_ArchiveManagerDAI::RESTORED_BY_COLUMN );
                    inventoryItem.deletionDueDate = data->getDateData( row, TA_ArchiveManagerDAI::DELETION_DUE_DATE_COLUMN );
    
                    // Add the data to the map.

                    m_inventory.insert( ArchiveInventoryItemMap::value_type( inventoryItem.key, inventoryItem ) );
                    LOGMORE( SourceInfo, "Archive Inventory Item... Key %lu", inventoryItem.key );
                }

                // Delete ready to retrieve the next lot of data.

                delete data;
                data = 0;

            } while ( databaseConnection->moreData( data ) );
        }
        catch( ... )
        {
            // Just make absolutely sure that the data is cleaned up.

            delete data;
            data = 0;
        }

        m_isValidData = true;

        FUNCTION_EXIT;
    }


    //
    // updateArchiveInventoryItemInDatabase
    //
    void ArchiveInventoryData::updateArchiveInventoryItemInDatabase( const ArchiveInventoryItem& inventoryItem )
    {
        FUNCTION_ENTRY("updateArchiveInventoryInDatabase");

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);

        std::stringstream sql;
  //      sql << "update " << TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME;
  //      sql << " set " << TA_ArchiveManagerDAI::DATA_DATE_COLUMN;
  //      sql << "= TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.dataDate );

		//std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat2); // TD12474

		//// TD12474 ++
		////sql << "', '" << TA_ArchiveManagerDai::DATE_FORMAT << "' )";
		//sql << "', '" << dateFormatStr << "' )";
		//// ++ TD12474

  //      sql << ", " << TA_ArchiveManagerDAI::VOLUME_LABEL_COLUMN;
  //      sql << "= '" << databaseConnection->escapeInsertString( inventoryItem.volumeLabel );
  //      sql << "', " << TA_ArchiveManagerDAI::DELETING_COLUMN;
        if ( inventoryItem.isDeleting )
        {
            sql << "1"; 
        }
        else
        {
            sql << "0"; 
        }
  //      sql << TA_ArchiveManagerDAI::RESTORED_DATE_COLUMN;
  //      sql << "= TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.restoredDate, true );

		//dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat2); // TD12474

		//// TD12474 ++
  //      //sql << "', '" << TA_ArchiveManagerDai::DATE_TIME_FORMAT << "' )";
		//sql << "', '" << dateFormatStr << "' )";
		//// ++ TD12474

  //      sql << ", " << TA_ArchiveManagerDAI::RESTORED_BY_COLUMN;
  //      sql << "= '" << databaseConnection->escapeInsertString( inventoryItem.restoredBy );
  //      sql << "', " << TA_ArchiveManagerDAI::DELETION_DUE_DATE_COLUMN;
  //      sql << "= TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.deletionDueDate );

		//dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat2); // TD12474

		//// TD12474 ++
		////sql << "', '" << TA_ArchiveManagerDai::DATE_FORMAT << "' )";
		//sql << "', '" << dateFormatStr << "' )";
		//// ++ TD12474

  //      sql << "where " << TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN << "=" << inventoryItem.key;
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_inventory_Oracle_UPDATE_29701,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_INVENTORY_UPDATE_29701,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_INVENTORY_UPDATE_29701,
			TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.dataDate ), databaseConnection->escapeInsertString( inventoryItem.volumeLabel ),
			sql.str(), TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.restoredDate, true ), databaseConnection->escapeInsertString( inventoryItem.restoredBy ),
			TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.deletionDueDate ), inventoryItem.key);

        // Execute the modification. The method can throw a DatabaseException.
        // This is documented in the comment of this method.

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }


    //
    // insertArchiveInventoryItemIntoDatabase
    //
    void ArchiveInventoryData::insertArchiveInventoryItemIntoDatabase( ArchiveInventoryItem& inventoryItem )
    {
        FUNCTION_ENTRY("insertArchiveInventoryIntoDatabase");

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);

        /*std::stringstream selectSql;
        selectSql << "select ARINVE_SEQ.NEXTVAL from DUAL";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_inventory_Oracle_SELECT_29553);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AR_INVENTORY_SELECT_29553);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AR_INVENTORY_SELECT_29553);

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.

        std::auto_ptr< IData > data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data.get() )
        {
            TA_THROW( DataException( "Failed to insert the inventory item into the database. Get next key failed.", DataException::NO_VALUE, "" ));
        }
        if ( 1 != data->getNumRows() )
        {
            TA_THROW( DataException( "Failed to insert the inventory item into the database. An invalid number of next keys were retrieved.", DataException::NO_VALUE, "" ));
        }
        inventoryItem.key = data->getIntegerData( 0, TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN );
        
        std::stringstream sql;
  //      sql << "insert into " << TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME << " (" ;
  //      sql << TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN << ", ";
  //      sql << TA_ArchiveManagerDAI::DATA_DATE_COLUMN << ", ";
  //      sql << TA_ArchiveManagerDAI::VOLUME_LABEL_COLUMN << ", ";
  //      sql << TA_ArchiveManagerDAI::DELETING_COLUMN << ", ";
  //      sql << TA_ArchiveManagerDAI::RESTORED_DATE_COLUMN << ", ";
  //      sql << TA_ArchiveManagerDAI::RESTORED_BY_COLUMN << ", ";
  //      sql << TA_ArchiveManagerDAI::DELETION_DUE_DATE_COLUMN << ") values (";
  //      sql << inventoryItem.key << ", ";
  //      sql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.dataDate );

		//std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat2); // TD12474

		//// TD12474 ++
		////sql << "', '" << TA_ArchiveManagerDai::DATE_FORMAT << "' ), '";
		//sql << "', '" << dateFormatStr << "' ), '";
		//// ++ TD12474

  //      sql << databaseConnection->escapeInsertString( inventoryItem.volumeLabel ) << "', ";
        if ( inventoryItem.isDeleting )
        {
            sql << "1";
        }
        else
        {
            sql << "0";
        }
  //      sql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.restoredDate, true );

		//dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat2); // TD12474

		//// TD12474 ++
  //      //sql << "', '" << TA_ArchiveManagerDai::DATE_TIME_FORMAT << "' ), '";
		//sql << "', '" << dateFormatStr << "' ), '";
		//// ++ TD12474

  //      sql << databaseConnection->escapeInsertString( inventoryItem.restoredBy ) << "', ";
  //      sql << "TO_DATE( '" << TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.deletionDueDate );

		//dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat2); // TD12474

		//// TD12474 ++
  //      //sql << "', '" << TA_ArchiveManagerDai::DATE_FORMAT << "' ) )";
		//sql << "', '" << dateFormatStr << "' ) )";
		// ++ TD12474
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ar_inventory_Oracle_INSERT_29851, inventoryItem.key,
//		strSql  = databaseConnection->prepareSQLStatement(AR_INVENTORY_INSERT_29851, inventoryItem.key,

		databaseConnection->prepareSQLStatement(strSql, AR_INVENTORY_INSERT_29851, inventoryItem.key,
			TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.dataDate ), databaseConnection->escapeInsertString( inventoryItem.volumeLabel ),
			sql.str(), TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.restoredDate, true ), databaseConnection->escapeInsertString( inventoryItem.restoredBy ),
			TA_ArchiveManagerDai::convertTimeTToStringDate( inventoryItem.deletionDueDate ));

        // Execute the insertion. The method can throw a DatabaseException.
        // This is documented in the comment of this method.

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }


    //
    // removeArchiveInventoryItemFromDatabase
    //
    void ArchiveInventoryData::removeArchiveInventoryItemFromDatabase( unsigned long itemKey )
    {
        FUNCTION_ENTRY("removeArchiveInventoryItemFromDatabase");

		IDatabase *pDb = DatabaseFactory::getInstance().getDatabase(Archive_d, Write);
        /*std::stringstream sql;
        sql << "delete from " << TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME;
        sql << " where " << TA_ArchiveManagerDAI::INVENTORY_KEY_COLUMN << "=" << itemKey;*/
//		std::string strSql = defPrepareSQLStatement(pDb->GetDbServerType(), ar_inventory_STD_DELETE_29901, itemKey);
//		std::string strSql  = pDb->prepareSQLStatement(AR_INVENTORY_DELETE_29901, itemKey);
		SQLStatement strSql;
		pDb->prepareSQLStatement(strSql, AR_INVENTORY_DELETE_29901, itemKey);

        // Execute the insertion. The method can throw a DatabaseException.
        // This is documented in the comment of this method.

        pDb->executeModification( strSql );

        FUNCTION_EXIT;
    }

} //end namespace TA_Base_Core
