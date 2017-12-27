/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveTable.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveTable is an implementation of IArchiveTable. It holds the data specific to a 
  * ArchiveTable mapping entry in the database, and allows read-only access to that data.
  */

// KT 26 Feb 04, PW #3133:  ONLINE_PERIOD has been moved from AR_TABLES and put in AR_ONLINE.

#include "core/data_access_interface/archive_manager_dai/src/ArchiveTable.h"

#include "core/exceptions/src/ArchiveException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    //
    // ArchiveTable
    //
    ArchiveTable::ArchiveTable( unsigned long key )
        : m_archiveTableHelper( key )
    {
        // Do nothing.
    }


    //
    // getKey
    //
    unsigned long ArchiveTable::getKey()
    {
        return m_archiveTableHelper.getKey();
    }


    //
    // getName
    //
    std::string ArchiveTable::getName()
    {
        return m_archiveTableHelper.getName();
    }


    //
    // getQueryColumn
    //
    std::string ArchiveTable::getQueryColumn()
    {
        return m_archiveTableHelper.getQueryColumn();
    }


    //
    // getFilePostfix
    //
    std::string ArchiveTable::getFilePostfix()
    {
        return m_archiveTableHelper.getFilePostfix();
    }


    //
    // isDateInQueryColumn
    //
    bool ArchiveTable::isDateInQueryColumn( const time_t& date )
    {
        // KT 9 Mar 04: PW #3161 - Changed date from a std::string to a time_t,
        // so that the date format is not assumed across the interface

        return m_archiveTableHelper.isDateInQueryColumn( date );
    }


    //
    // deleteRestoredDataForDate
    //
    void ArchiveTable::deleteRestoredDataForDate( const time_t& date )
    {
        // KT 9 Mar 04: PW #3161 - Changed date from a std::string to a time_t,
        // so that the date format is not assumed across the interface

        m_archiveTableHelper.deleteRestoredDataForDate( date );
    }


    //
    // invalidate
    //
    void ArchiveTable::invalidate()
    {
        m_archiveTableHelper.invalidate();
    }

} // closes TA_Base_Core
