/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ConfigArchiveTable.cpp $
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

#include "core/data_access_interface/archive_manager_dai/src/ConfigArchiveTable.h"

#include "core/exceptions/src/ArchiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    //
    // ConfigArchiveTable
    //
    ConfigArchiveTable::ConfigArchiveTable( unsigned long key )
        : m_archiveTableHelper( std::auto_ptr< ArchiveTableHelper >( new ArchiveTableHelper( key ) ) )
    {
        if ( 0 == m_archiveTableHelper.get() )
        {
            TA_THROW( ArchiveException( "Failed to create ArchiveTableHelper." ) );
        }
    }


    //
    // ConfigArchiveTable
    //
    ConfigArchiveTable::ConfigArchiveTable()
        : m_archiveTableHelper( std::auto_ptr< ArchiveTableHelper >( new ArchiveTableHelper() ) )
    {
        if ( 0 == m_archiveTableHelper.get() )
        {
            TA_THROW( ArchiveException( "Failed to create ArchiveTableHelper." ) );
        }
    }


    //
    // getKey
    //
    unsigned long ConfigArchiveTable::getKey()
    {
        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        return m_archiveTableHelper->getKey();
    }

     
    //
    // getName
    //
    std::string ConfigArchiveTable::getName()
    {
        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        return m_archiveTableHelper->getName();
    } 


    //
    // setVideoInputs
    //
    void ConfigArchiveTable::setName( const std::string& name )
    {
        FUNCTION_ENTRY( "ConfigArchiveTable::setName");

        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        m_archiveTableHelper->setName( name );

        FUNCTION_EXIT;
    }

     
    //
    // getQueryColumn
    //
    std::string ConfigArchiveTable::getQueryColumn()
    {
        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        return m_archiveTableHelper->getQueryColumn();
    }
        
    
    //
    // setQueryColumn
    //
    void ConfigArchiveTable::setQueryColumn( const std::string& queryColumn )
    {
        FUNCTION_ENTRY( "ConfigArchiveTable::setQueryColumn");

        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        m_archiveTableHelper->setQueryColumn( queryColumn );

        FUNCTION_EXIT;
    }
     

    //
    // getFilePostfix
    //
    std::string ConfigArchiveTable::getFilePostfix()
    {
        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        return m_archiveTableHelper->getFilePostfix();
    }


    //
    // setFilePostfix
    //
    void ConfigArchiveTable::setFilePostfix( const std::string& filePostfix )
    {
        FUNCTION_ENTRY( "ConfigArchiveTable::setFilePostfix");

        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        m_archiveTableHelper->setFilePostfix( filePostfix );

        FUNCTION_EXIT;
    }
     

    //
    // isDateInQueryColumn
    //
    bool ConfigArchiveTable::isDateInQueryColumn( const time_t& date )
    {
        // KT 9 Mar 04: PW #3161 - Changed date from a std::string to a time_t,
        // so that the date format is not assumed across the interface

        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        return m_archiveTableHelper->isDateInQueryColumn( date );
    }


    //
    // deleteRestoredDataForDate
    //
    void ConfigArchiveTable::deleteRestoredDataForDate( const time_t& date )
    {
        // KT 9 Mar 04: PW #3161 - Changed date from a std::string to a time_t,
        // so that the date format is not assumed across the interface

        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        m_archiveTableHelper->deleteRestoredDataForDate( date );
    }
    

    //
    // invalidate
    //
    void ConfigArchiveTable::invalidate()
    {
        FUNCTION_ENTRY( "ConfigArchiveTable::invalidate");

        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        m_archiveTableHelper->invalidate();

        FUNCTION_EXIT;
    }


    //
    // deleteThisObject
    //
    void ConfigArchiveTable::deleteThisObject()
    {
        FUNCTION_ENTRY( "ConfigArchiveTable::deleteThisObject");

        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );

        // Delete the ArchiveTable from the database.

        m_archiveTableHelper->deleteArchiveTable();
        
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
            "About to delete ArchiveTableHelper pointer m_archiveTableHelper." );

        m_archiveTableHelper = std::auto_ptr< ArchiveTableHelper >( 0 );

        FUNCTION_EXIT;
    }


    //
    // applyChanges
    //
    void ConfigArchiveTable::applyChanges()
    {
        FUNCTION_ENTRY( "ConfigArchiveTable::applyChanges");

        TA_ASSERT( 0 != m_archiveTableHelper.get(), "The ArchiveTableHelper pointer is null." );
        m_archiveTableHelper->writeArchiveTableData();

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core

