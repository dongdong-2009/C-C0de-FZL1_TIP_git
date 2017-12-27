/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/ConfigPaFasTriggeredBroadcast.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * ConfigPaFasTriggeredBroadcast is a concrete implementation of IConfigPaFasTriggeredBroadcast, 
  * which is in turn an implementation
  * of IPaFasTriggeredBroadcast. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaFasTriggeredBroadcasts.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaFasTriggeredBroadcast.h"
#include "core/data_access_interface/pa/src/PaFasTriggeredBroadcastHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaFasTriggeredBroadcast::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string ALARMENTITYKEY       = "Alarm Entity key";
    static const std::string LOCATIONKEY        = "Location Key";
    static const std::string CYCLICTIMES         = "Pa Cyclic Times";
    
    static const std::string PAZGROUPID         = "Pa Zone Group FK";
    static const std::string PADVAMESSAGEID     = "Pa DVA Message FK";
    static const std::string ISENABLED          = "Is Enabled";

    ConfigPaFasTriggeredBroadcast::ConfigPaFasTriggeredBroadcast()
     : m_PaFasTriggeredBroadcastHelper( new PaFasTriggeredBroadcastHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaFasTriggeredBroadcast::ConfigPaFasTriggeredBroadcast(unsigned long row, TA_Base_Core::IData& data)
     : m_PaFasTriggeredBroadcastHelper(new PaFasTriggeredBroadcastHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaFasTriggeredBroadcast::ConfigPaFasTriggeredBroadcast(const unsigned long key)
     : m_PaFasTriggeredBroadcastHelper( new PaFasTriggeredBroadcastHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaFasTriggeredBroadcast::ConfigPaFasTriggeredBroadcast( const ConfigPaFasTriggeredBroadcast& theConfigPaFasTriggeredBroadcast)
     : m_PaFasTriggeredBroadcastHelper( new PaFasTriggeredBroadcastHelper( *(theConfigPaFasTriggeredBroadcast.m_PaFasTriggeredBroadcastHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaFasTriggeredBroadcast::~ConfigPaFasTriggeredBroadcast()
    {
        try
        {
            delete m_PaFasTriggeredBroadcastHelper;
            m_PaFasTriggeredBroadcastHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaFasTriggeredBroadcast::isNew()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");

        return m_PaFasTriggeredBroadcastHelper->isNew();
    }

    void ConfigPaFasTriggeredBroadcast::setFasAlarmEntityKey(unsigned long eventType)
    {    
        FUNCTION_ENTRY("setFasAlarmEntityKey");
        
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");

        updatePaFasTriggeredBroadcastChanges(ALARMENTITYKEY, 
                        isNew() ? 0 : m_PaFasTriggeredBroadcastHelper->getFasAlarmEntityKey(), 
                        eventType);

        m_PaFasTriggeredBroadcastHelper->setFasAlarmEntityKey(eventType);

        FUNCTION_EXIT;    
    }


    void ConfigPaFasTriggeredBroadcast::setLocationKey(unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        
        updatePaFasTriggeredBroadcastChanges(LOCATIONKEY, 
                                             isNew() ? 0 : m_PaFasTriggeredBroadcastHelper->getLocationKey(),
                                             locationKey);

        m_PaFasTriggeredBroadcastHelper->setLocationKey(locationKey);

        FUNCTION_EXIT;
    }


    void ConfigPaFasTriggeredBroadcast::setPaCyclicTimes(unsigned long platformId)
    {
        FUNCTION_ENTRY("setPaCyclicTimes");
        
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,
                        "The PaFasTriggeredBroadcastHelper pointer is null.");
        
        updatePaFasTriggeredBroadcastChanges(CYCLICTIMES, 
                        isNew() ? 0 : m_PaFasTriggeredBroadcastHelper->getPaCyclicTimes(), 
                        platformId);

        m_PaFasTriggeredBroadcastHelper->setPaCyclicTimes(platformId);

        FUNCTION_EXIT;    
    }


    void ConfigPaFasTriggeredBroadcast::setPaZoneGroupKey(unsigned long key)
    {
        FUNCTION_ENTRY("setPaZoneGroupKey");
        
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,
                        "The PaFasTriggeredBroadcastHelper pointer is null.");
        
        updatePaFasTriggeredBroadcastChanges(PAZGROUPID, 
                        isNew() ? 0 : m_PaFasTriggeredBroadcastHelper->getPaZoneGroupKey(), 
                        key);

        m_PaFasTriggeredBroadcastHelper->setPaZoneGroupKey(key);

        FUNCTION_EXIT;    
    }


    void ConfigPaFasTriggeredBroadcast::setPaDvaMessageKey(unsigned long key)
    {
        FUNCTION_ENTRY("setPaDvaMessageKey");
        
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,
                        "The PaFasTriggeredBroadcastHelper pointer is null.");
        
        updatePaFasTriggeredBroadcastChanges(PADVAMESSAGEID, 
                        isNew() ? 0 : m_PaFasTriggeredBroadcastHelper->getPaDvaMessageKey(), 
                        key);

        m_PaFasTriggeredBroadcastHelper->setPaDvaMessageKey(key);

        FUNCTION_EXIT;    
    }


    void ConfigPaFasTriggeredBroadcast::setIsEnabledParameter(bool isEnabled)
    {
        FUNCTION_ENTRY("setIsEnabledParameter");
        
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,
                        "The PaFasTriggeredBroadcastHelper pointer is null.");
        
        updatePaFasTriggeredBroadcastChanges(ISENABLED, 
                        isNew() ? 0 : m_PaFasTriggeredBroadcastHelper->getIsEnabledParameter(), 
                        isEnabled);

        m_PaFasTriggeredBroadcastHelper->setIsEnabledParameter(isEnabled);

        FUNCTION_EXIT;    
    }


    unsigned long ConfigPaFasTriggeredBroadcast::getKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getKey();
    }


    unsigned long ConfigPaFasTriggeredBroadcast::getFasAlarmEntityKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getFasAlarmEntityKey();
    }


    unsigned long ConfigPaFasTriggeredBroadcast::getLocationKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getLocationKey();
    }
    
    unsigned long ConfigPaFasTriggeredBroadcast::getPaCyclicTimes()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getPaCyclicTimes();
    }

    unsigned long ConfigPaFasTriggeredBroadcast::getPaZoneGroupKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getPaZoneGroupKey();
    }


    unsigned long ConfigPaFasTriggeredBroadcast::getPaDvaMessageKey()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getPaDvaMessageKey();
    }


    bool          ConfigPaFasTriggeredBroadcast::getIsEnabledParameter()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
        return m_PaFasTriggeredBroadcastHelper->getIsEnabledParameter();
    }


    time_t ConfigPaFasTriggeredBroadcast::getDateCreated()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
		return m_PaFasTriggeredBroadcastHelper->getDateCreated();
    }


    time_t ConfigPaFasTriggeredBroadcast::getDateModified()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");
		return m_PaFasTriggeredBroadcastHelper->getDateModified();
    }
 
	
    void ConfigPaFasTriggeredBroadcast::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaFasTriggeredBroadcast::invalidate");
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");

        m_PaFasTriggeredBroadcastHelper->invalidate();
        m_PaFasTriggeredBroadcastChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaFasTriggeredBroadcast::invalidate");
    }


    void ConfigPaFasTriggeredBroadcast::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaFasTriggeredBroadcast::deleteThisObject");
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The PaFasTriggeredBroadcastHelper pointer is null.");

        // Delete the ConfigPaFasTriggeredBroadcast from the database
        m_PaFasTriggeredBroadcastHelper->deletePaFasTriggeredBroadcast();
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaFasTriggeredBroadcastHelper pointer m_PaFasTriggeredBroadcastHelper");

        delete m_PaFasTriggeredBroadcastHelper;
        m_PaFasTriggeredBroadcastHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaFasTriggeredBroadcast::deleteThisObject");
    }


    void ConfigPaFasTriggeredBroadcast::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaFasTriggeredBroadcast::applyChanges");
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL, "applyChanges() has been called after ConfigPaFasTriggeredBroadcast was deleted");

        m_PaFasTriggeredBroadcastHelper->writePaFasTriggeredBroadcastData();

        m_PaFasTriggeredBroadcastChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaFasTriggeredBroadcast::applyChanges");
    }


    void ConfigPaFasTriggeredBroadcast::updatePaFasTriggeredBroadcastChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaFasTriggeredBroadcastChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_PaFasTriggeredBroadcastChanges.find(name);

        if (matching != m_PaFasTriggeredBroadcastChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_PaFasTriggeredBroadcastChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            TA_Base_Core::Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_PaFasTriggeredBroadcastChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaFasTriggeredBroadcast::updatePaFasTriggeredBroadcastChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaFasTriggeredBroadcastChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaFasTriggeredBroadcastChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaFasTriggeredBroadcast::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    std::string ConfigPaFasTriggeredBroadcast::getName()
    {
        TA_ASSERT(m_PaFasTriggeredBroadcastHelper != NULL,"The m_PaFasTriggeredBroadcastHelper pointer is null.");

        std::ostringstream name;
		name << "PA ATS Triggered Broadcast " << m_PaFasTriggeredBroadcastHelper->getKey();

        return name.str();
    }      
} // closes TA_Base_Core

