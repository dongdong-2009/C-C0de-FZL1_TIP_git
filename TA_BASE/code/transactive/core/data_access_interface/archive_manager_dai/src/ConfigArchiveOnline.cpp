/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ConfigArchiveOnline.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigArchiveOnline is a concrete implementation of IConfigArchiveOnline, which is in turn 
  * an implementation of IArchiveOnline. It is to be used by the Configuration Editor, 
  * and other applications that wish to change the database content for ArchiveOnlines.
  */

#include "core/data_access_interface/archive_manager_dai/src/ConfigArchiveOnline.h"

#include "core/exceptions/src/ArchiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    //
    // ConfigArchiveOnline
    //
    ConfigArchiveOnline::ConfigArchiveOnline( unsigned long key )
        : m_archiveOnlineHelper( key )
    {
        // Do nothing.
    }


    //
    // ConfigArchiveOnline
    //
    ConfigArchiveOnline::ConfigArchiveOnline()
        : m_archiveOnlineHelper()
    {
        // Do nothing.
    }


    //
    // getKey
    //
    unsigned long ConfigArchiveOnline::getKey()
    {
        return m_archiveOnlineHelper.getKey();
    }

   
    //
    // getOnlinePeriodInDays
    //
    unsigned short ConfigArchiveOnline::getOnlinePeriodInDays()
    {
        return m_archiveOnlineHelper.getOnlinePeriodInDays();
    }


    //
    // setOnlinePeriodInDays
    //
    void ConfigArchiveOnline::setOnlinePeriodInDays( unsigned short onlinePeriodInDays )
    {
        m_archiveOnlineHelper.setOnlinePeriodInDays( onlinePeriodInDays );
    }


    //
    // getDataExpiryInDays
    //
    unsigned short ConfigArchiveOnline::getDataExpiryInDays()
    {
        return m_archiveOnlineHelper.getDataExpiryInDays();
    }


    //
    // setDataExpiryInDays
    //
    void ConfigArchiveOnline::setDataExpiryInDays( unsigned short dataExpiryInDays )
    {
        m_archiveOnlineHelper.setDataExpiryInDays( dataExpiryInDays );
    }


    //
    // invalidate
    //
    void ConfigArchiveOnline::invalidate()
    {
        m_archiveOnlineHelper.invalidate();
    }


    //
    // applyChanges
    //
    void ConfigArchiveOnline::applyChanges()
    {
        m_archiveOnlineHelper.applyChanges();
    }

} // closes TA_Base_Core

