/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/ConfigPaScheduleBroadcast.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaScheduleBroadcast is a concrete implementation of IConfigPaScheduleBroadcast, which is in turn an implementation
  * of IPaDvaMessage. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessages.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>

#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/ConfigPaScheduleBroadcast.h"
#include "core/data_access_interface/pa/src/PaScheduleBroadcastHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    ConfigPaScheduleBroadcast::ConfigPaScheduleBroadcast()
     : m_pPaScheduleBroadcastHelper( new PaScheduleBroadcastHelper() )
    {
    }

    ConfigPaScheduleBroadcast::ConfigPaScheduleBroadcast(unsigned long row, TA_Base_Core::IData& data)
     : m_pPaScheduleBroadcastHelper(new PaScheduleBroadcastHelper(row, data))
    {
    }

    ConfigPaScheduleBroadcast::ConfigPaScheduleBroadcast(const unsigned long key)
     : m_pPaScheduleBroadcastHelper( new PaScheduleBroadcastHelper(key) )
    {
    }

    ConfigPaScheduleBroadcast::ConfigPaScheduleBroadcast( const ConfigPaScheduleBroadcast& theConfigPaScheduleBroadcast)
     : m_pPaScheduleBroadcastHelper( new PaScheduleBroadcastHelper( *(theConfigPaScheduleBroadcast.m_pPaScheduleBroadcastHelper)) )
    {
    }

    ConfigPaScheduleBroadcast::~ConfigPaScheduleBroadcast()
    {
        try
        {
            delete m_pPaScheduleBroadcastHelper;
            m_pPaScheduleBroadcastHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaScheduleBroadcast::isNew()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");

        return m_pPaScheduleBroadcastHelper->isNew();
    }

    void ConfigPaScheduleBroadcast::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setLocationKey(locationKey);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setDvaMsgKey( const unsigned long ulDVAMsgKey )
    {
        FUNCTION_ENTRY("setDvaMsgKey");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setDvaMsgKey(ulDVAMsgKey);

        FUNCTION_EXIT;
    }

    unsigned long ConfigPaScheduleBroadcast::getKey()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getKey();
    }

    unsigned long ConfigPaScheduleBroadcast::getLocationKey()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getLocationKey();
    }
    
    unsigned long ConfigPaScheduleBroadcast::getDvaMsgKey()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getDvaMsgKey();
    }

    void ConfigPaScheduleBroadcast::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");

        m_pPaScheduleBroadcastHelper->invalidate();
        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::deleteThisObject()
    {
        FUNCTION_ENTRY("deleteThisObject");
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");

        // Delete the ConfigPaScheduleBroadcast from the database
        m_pPaScheduleBroadcastHelper->deletePaScheduleBroadcast();
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete PaScheduleBroadcastHelper pointer m_pPaScheduleBroadcastHelper");

        delete m_pPaScheduleBroadcastHelper;
        m_pPaScheduleBroadcastHelper = NULL;

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL, "applyChanges() has been called after ConfigPaScheduleBroadcast was deleted");

        m_pPaScheduleBroadcastHelper->writePaScheduleBroadcast();
        FUNCTION_EXIT;
    }

    unsigned long ConfigPaScheduleBroadcast::getCyclicTimes()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getCyclicTimes();
    }

    unsigned long ConfigPaScheduleBroadcast::getPriorityGroup()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getPriorityGroup();
    }

    time_t ConfigPaScheduleBroadcast::getStartTime()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getStartTime();
    }

    time_t ConfigPaScheduleBroadcast::getEndTime()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getEndTime();
    }

    std::string ConfigPaScheduleBroadcast::getScheduleName()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getScheduleName();
    }

    std::string ConfigPaScheduleBroadcast::getIntervalType()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getIntervalType();
    }

    std::string ConfigPaScheduleBroadcast::getSourceType()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getSourceType();
    }

    std::string ConfigPaScheduleBroadcast::getStatus()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getStatus();
    }

    std::vector<unsigned long> ConfigPaScheduleBroadcast::getZoneKeys()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getZoneKeys();
    }

    bool ConfigPaScheduleBroadcast::getIsEnable()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getIsEnable();
    }

    bool ConfigPaScheduleBroadcast::getAutoDel()
    {
        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        return m_pPaScheduleBroadcastHelper->getAutoDel();
    }

    void ConfigPaScheduleBroadcast::setCyclicTimes( unsigned long ulCyclicTimes )
    {
        FUNCTION_ENTRY("setCyclicTimes");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setCyclicTimes(ulCyclicTimes);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setPriorityGroup( unsigned long ulPriorityGroup )
    {
        FUNCTION_ENTRY("setPriorityGroup");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setPriorityGroup(ulPriorityGroup);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setStartTime( time_t tmStartTime )
    {
        FUNCTION_ENTRY("setStartTime");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setStartTime(tmStartTime);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setEndTime( time_t tmEndTime )
    {
        FUNCTION_ENTRY("setEndTime");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setEndTime(tmEndTime);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setScheduleName( const std::string& strScheduleName )
    {
        FUNCTION_ENTRY("setScheduleName");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setScheduleName(strScheduleName);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setIntervalType( const std::string& strIntervalType )
    {
        FUNCTION_ENTRY("setIntervalType");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setIntervalType(strIntervalType);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setSourceType( const std::string& strSourceType )
    {
        FUNCTION_ENTRY("setSourceType");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setSourceType(strSourceType);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setStatus( const std::string& strStatus )
    {
        FUNCTION_ENTRY("setStatus");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setStatus(strStatus);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setZoneKeys( const std::vector<unsigned long>& vecZoneKeys )
    {
        FUNCTION_ENTRY("setZoneKeys");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setZoneKeys(vecZoneKeys);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setIsEnable( bool bEnable )
    {
        FUNCTION_ENTRY("setIsEnable");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setIsEnable(bEnable);

        FUNCTION_EXIT;
    }

    void ConfigPaScheduleBroadcast::setAutoDel( bool bAutoDel )
    {
        FUNCTION_ENTRY("setIsEnable");

        TA_ASSERT(m_pPaScheduleBroadcastHelper != NULL,"The PaScheduleBroadcastHelper pointer is null.");
        m_pPaScheduleBroadcastHelper->setAutoDel(bAutoDel);

        FUNCTION_EXIT;
    }

} // closes TA_IRS_Core

