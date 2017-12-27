/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/DeleteArchiveHelper.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Performs back end work for the Archive Manager deletion process.
  */

#if defined( WIN32 )
#pragma warning ( disable : 4786 4250 ) 
#endif // defined( WIN32 )

#include <memory>
#include <sstream>
#include <afx.h>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/DeleteArchiveHelper.h"
#include "app/data_management/archive_manager/src/resource.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveTable.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryTable.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveTableAccessFactory.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/types/ArchiveAudit_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_App
{
    // KT 11 Mar 04: PW #3133. The methods initiateDelete(), completeDelete(),
    // initiateChangeDeletionDate() and completeChangeDeletionDate() have replaced
    // the deleteRestoredArchives() method because the 
    // DeleteArchiveDlg now does both the deletion of the data and the
    // modification of the deletion due date, so the message sending
    // requirements are different.

    //
    // initiateDelete
    //
    void DeleteArchiveHelper::initiateDelete()
    {
        // Send an audit message.

        TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::DeleteArchiveRequest ); 

        // Check that the user has permission.

        if( !TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_DELETE ) )
        {
            CString message;
            try
            {
                message.FormatMessage( UE050052_INSUF_PRIV_REMOVE , 
                    TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
                    TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                message.LoadString( UE050119_INSUF_PRIV_REMOVE );
            }
            TA_THROW( TA_Base_Core::ArchiveException( std::string( message ) ) );
        }
    }


    //
    // completeDelete
    //
    void DeleteArchiveHelper::completeDelete( int status, const std::string& abortMsg )
    {
        TA_ASSERT( ( IDOK == status ) || ( IDCANCEL == status ) || ( IDABORT == status ), "The status is not recognised." );

        if ( IDOK == status )
        {
            // The message is independent of the situation.

            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ArchiveDeleted ); 
        }
        else if ( IDCANCEL == status )
        {
            // The message is independent of the situation.

            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::DeleteArchiveCancelled );
        }
        else // if ( IDABORT == status )
        {
            // The message is dependent on the situation.

            TA_ASSERT( !abortMsg.empty(), "The abort message is empty." );
            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::DeleteArchiveFailed, abortMsg );
        }
    }


    //
    // initiateChangeDeletionDate
    //
    void DeleteArchiveHelper::initiateChangeDeletionDate()
    {
		/*
        // Send an audit message.

        TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ChangeDeletionDateRequest ); 
		*/

        // Check that the user has permission.

        if( !TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_DELETE ) )
        {
            CString error;
            try
            {
                error.FormatMessage( UE050109_INSUF_PRIV_CHANGE_DATE, 
                    TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
                    TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                error.LoadString( UE050120_INSUF_PRIV_CHANGE_DATE );
            }
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
    }


    //
    // completeChangeDeletionDate
    //
    void DeleteArchiveHelper::completeChangeDeletionDate( int status, const std::string& abortMsg )
    {
        TA_ASSERT( ( IDOK == status ) || ( IDCANCEL == status ) || ( IDABORT == status ), "The status is not recognised." );
        
        if ( IDOK == status )
        {
            // The message is independent of the situation.

            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::DeletionDateChanged ); 
        }
        else if ( IDCANCEL == status )
        {
            // The message is independent of the situation.

            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ChangeDeletionDateCancelled );
        }
        else // if ( IDABORT == status )
        {
            // The message is dependent on the situation.

            TA_ASSERT( !abortMsg.empty(), "The abort message is empty." );
            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ChangeDeletionDateFailed, abortMsg );
        }
    }


    //
    // getArchiveInventoryItems
    //
    TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap DeleteArchiveHelper::getArchiveInventoryItems()
    {
        std::auto_ptr< TA_Base_Core::IArchiveInventory > inventory(
            TA_Base_Core::ArchiveInventoryAccessFactory::getInstance().getArchiveInventory() );
        if( 0 == inventory.get() )
        {
            CString reason;
            reason.LoadString( UE050055_REASON_INV_ACCESS_FAILED );
            CString error;
            error.FormatMessage( UE050055_RETRIEVE_LIST_FAILED, reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap archives;
        try
        {
            archives = inventory->getAllArchiveInventoryItems();
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            
            CString error;
            error.FormatMessage( UE050055_RETRIEVE_LIST_FAILED, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            
            CString error;
            error.FormatMessage( UE050055_RETRIEVE_LIST_FAILED, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        return archives;
    }


    //
    // deleteRestoredData
    //
    void DeleteArchiveHelper::deleteRestoredData( unsigned long inventoryItemKey, const time_t& date )
    {
        // KT 27 Feb 04: PW #3133: This has been changed so that users can remove
        // an individual days worth of restored data from the database.  The 
        // date range (from date to to date) have been replaced with a single 
        // data date.

        // Update the archive inventory, so it says we're deleting the given archive.

        std::auto_ptr< TA_Base_Core::IArchiveInventory > inventory(
            TA_Base_Core::ArchiveInventoryAccessFactory::getInstance().getArchiveInventory() );
        if( 0 == inventory.get() )
        {
            CString reason;
            reason.LoadString( UE050118_REASON_INV_ACCESS_FAILED );
            CString error;
            error.FormatMessage( UE050054_REMOVE_FAILED, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        TA_Base_Core::IArchiveInventory::ArchiveInventoryItem item;
        try
        {
            item = inventory->getArchiveInventoryItem( inventoryItemKey );
            item.isDeleting = true;
            inventory->setArchiveInventoryItem( item );
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
            CString error;
            error.FormatMessage( UE050054_REMOVE_FAILED, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
            CString error;
            error.FormatMessage( UE050054_REMOVE_FAILED, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }  
        
        // Remove all the restored data for the given date from the archive tables.

        std::vector< TA_Base_Core::IArchiveTable* > tables( 
            TA_Base_Core::ArchiveTableAccessFactory::getInstance().getAllArchiveTables( false ) );
        for( std::vector< TA_Base_Core::IArchiveTable* >::iterator tableIt = tables.begin(); tableIt != tables.end(); ++tableIt )
        {
            try
            {
                (*tableIt)->deleteRestoredDataForDate( date );
            }
            catch( TA_Base_Core::DatabaseException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
                LOGMORE( SourceInfo, "Converting to ArchiveException" );

                for( tableIt = tables.begin(); tableIt != tables.end(); ++tableIt )
                {
                    delete *tableIt;
                    *tableIt = 0;
                }

                CString error;
                error.FormatMessage( UE050054_REMOVE_FAILED, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
            catch( TA_Base_Core::DataException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
                LOGMORE( SourceInfo, "Converting to ArchiveException" );
                
                for( tableIt = tables.begin(); tableIt != tables.end(); ++tableIt )
                {
                    delete *tableIt;
                    *tableIt = 0;
                }

                CString error;
                error.FormatMessage( UE050054_REMOVE_FAILED, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
        }

		for(std::vector< TA_Base_Core::IArchiveTable* >::iterator tableIt = tables.begin(); tableIt != tables.end(); ++tableIt )
        {
            delete *tableIt;
            *tableIt = 0;
        }
    
        // Finally remove the entry from the AR_INVENTORY table.
        // Do this last, so that the record remains in the database until absolutely 
        // all the inventory data has been deleted. That way if something breaks halfway
        // through the process we still have the database record.
         
        try
        {
            inventory->removeArchiveInventoryItem( inventoryItemKey );
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
            CString error;
            error.FormatMessage( UE050121_DELETE_INV_ITEM_FAILED, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
            CString error;
            error.FormatMessage( UE050121_DELETE_INV_ITEM_FAILED, TA_ArchiveManager::convertTimeTToStringDate( date ).c_str(), e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
    }

} // TA_Base_App
