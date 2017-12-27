/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaAtsTriggeredBroadcast.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaAtsTriggeredBroadcast is a concrete implementation of IConfigPaAtsTriggeredBroadcast, 
  * which is in turn an implementation
  * of IPaAtsTriggeredBroadcast. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaAtsTriggeredBroadcasts.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaAtsTriggeredBroadcast.h"
#include "core/data_access_interface/pa/src/PaAtsTriggeredBroadcastHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaAtsTriggeredBroadcast::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string ATSEVENTTYPE       = "ATS Event Type";
    static const std::string LOCATIONKEY        = "Location Key";
    static const std::string DESTLOCATIONKEY    = "Destination Location Key";
    static const std::string PLATFORMID         = "Platform ID";
    
    static const std::string ISLASTTRAIN        = "Is Last Train";
    static const std::string PAZGROUPID         = "Pa Zone Group FK";
    static const std::string PADVAMESSAGEID     = "Pa DVA Message FK";
    static const std::string ISENABLED          = "Is Enabled";
    static const std::string DVAMESSAGEVERSION  = "DVA Message Version";

    ConfigPaAtsTriggeredBroadcast::ConfigPaAtsTriggeredBroadcast()
     : m_paAtsTriggeredBroadcastHelper( new PaAtsTriggeredBroadcastHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaAtsTriggeredBroadcast::ConfigPaAtsTriggeredBroadcast(unsigned long row, TA_Base_Core::IData& data)
     : m_paAtsTriggeredBroadcastHelper(new PaAtsTriggeredBroadcastHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaAtsTriggeredBroadcast::ConfigPaAtsTriggeredBroadcast(const unsigned long key)
     : m_paAtsTriggeredBroadcastHelper( new PaAtsTriggeredBroadcastHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaAtsTriggeredBroadcast::ConfigPaAtsTriggeredBroadcast( const ConfigPaAtsTriggeredBroadcast& theConfigPaAtsTriggeredBroadcast)
     : m_paAtsTriggeredBroadcastHelper( new PaAtsTriggeredBroadcastHelper( *(theConfigPaAtsTriggeredBroadcast.m_paAtsTriggeredBroadcastHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaAtsTriggeredBroadcast::~ConfigPaAtsTriggeredBroadcast()
    {
        try
        {
            delete m_paAtsTriggeredBroadcastHelper;
            m_paAtsTriggeredBroadcastHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaAtsTriggeredBroadcast::isNew()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");

        return m_paAtsTriggeredBroadcastHelper->isNew();
    }

    void ConfigPaAtsTriggeredBroadcast::setAtsEventType(unsigned long eventType)
    {    
        FUNCTION_ENTRY("setAtsEventType");
        
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");

        updatePaAtsTriggeredBroadcastChanges(ATSEVENTTYPE, 
                        isNew() ? 0 : m_paAtsTriggeredBroadcastHelper->getAtsEventType(), 
                        eventType);

        m_paAtsTriggeredBroadcastHelper->setAtsEventType(eventType);

        FUNCTION_EXIT;    
    }


    void ConfigPaAtsTriggeredBroadcast::setLocationKey(unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        
        updatePaAtsTriggeredBroadcastChanges(LOCATIONKEY, 
                                             isNew() ? 0 : m_paAtsTriggeredBroadcastHelper->getLocationKey(),
                                             locationKey);

        m_paAtsTriggeredBroadcastHelper->setLocationKey(locationKey);

        FUNCTION_EXIT;
    }
                 

    void ConfigPaAtsTriggeredBroadcast::setDestinationLocationKey(unsigned long key)
    {
        FUNCTION_ENTRY("setDestinationLocationKey");
        
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,
                        "The PaAtsTriggeredBroadcastHelper pointer is null.");
        
        updatePaAtsTriggeredBroadcastChanges(DESTLOCATIONKEY, 
                        isNew() ? 0 : m_paAtsTriggeredBroadcastHelper->getDestinationLocationKey(), 
                        key);

        m_paAtsTriggeredBroadcastHelper->setDestinationLocationKey(key);

        FUNCTION_EXIT;    
    }


    void ConfigPaAtsTriggeredBroadcast::setPlatformId(unsigned long platformId)
    {
        FUNCTION_ENTRY("setPlatformId");
        
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,
                        "The PaAtsTriggeredBroadcastHelper pointer is null.");
        
        updatePaAtsTriggeredBroadcastChanges(PLATFORMID, 
                        isNew() ? 0 : m_paAtsTriggeredBroadcastHelper->getPlatformId(), 
                        platformId);

        m_paAtsTriggeredBroadcastHelper->setPlatformId(platformId);

        FUNCTION_EXIT;    
    }

    
    void ConfigPaAtsTriggeredBroadcast::setIsLastTrain(bool isEnabled)
    {
        FUNCTION_ENTRY("setIsLastTrain");
        
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,
                        "The PaAtsTriggeredBroadcastHelper pointer is null.");
        
        updatePaAtsTriggeredBroadcastChanges(ISLASTTRAIN, 
                        isNew() ? 0 : m_paAtsTriggeredBroadcastHelper->getIsLastTrain(), 
                        isEnabled);

        m_paAtsTriggeredBroadcastHelper->setIsLastTrain(isEnabled);

        FUNCTION_EXIT;    
    }


    void ConfigPaAtsTriggeredBroadcast::setPaZoneGroupKey(unsigned long key)
    {
        FUNCTION_ENTRY("setPaZoneGroupKey");
        
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,
                        "The PaAtsTriggeredBroadcastHelper pointer is null.");
        
        updatePaAtsTriggeredBroadcastChanges(PAZGROUPID, 
                        isNew() ? 0 : m_paAtsTriggeredBroadcastHelper->getPaZoneGroupKey(), 
                        key);

        m_paAtsTriggeredBroadcastHelper->setPaZoneGroupKey(key);

        FUNCTION_EXIT;    
    }


    void ConfigPaAtsTriggeredBroadcast::setPaDvaMessageKey(unsigned long key)
    {
        FUNCTION_ENTRY("setPaDvaMessageKey");
        
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,
                        "The PaAtsTriggeredBroadcastHelper pointer is null.");
        
        updatePaAtsTriggeredBroadcastChanges(PADVAMESSAGEID, 
                        isNew() ? 0 : m_paAtsTriggeredBroadcastHelper->getPaDvaMessageKey(), 
                        key);

        m_paAtsTriggeredBroadcastHelper->setPaDvaMessageKey(key);

        FUNCTION_EXIT;    
    }


    void ConfigPaAtsTriggeredBroadcast::setIsEnabledParameter(bool isEnabled)
    {
        FUNCTION_ENTRY("setIsEnabledParameter");
        
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,
                        "The PaAtsTriggeredBroadcastHelper pointer is null.");
        
        updatePaAtsTriggeredBroadcastChanges(ISENABLED, 
                        isNew() ? 0 : m_paAtsTriggeredBroadcastHelper->getIsEnabledParameter(), 
                        isEnabled);

        m_paAtsTriggeredBroadcastHelper->setIsEnabledParameter(isEnabled);

        FUNCTION_EXIT;    
    }


    void ConfigPaAtsTriggeredBroadcast::setDvaMessageVersion(const std::string& version)
    {
        FUNCTION_ENTRY("setDvaMessageVersion");
        
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,
                        "The PaAtsTriggeredBroadcastHelper pointer is null.");
        
        updatePaAtsTriggeredBroadcastChanges(DVAMESSAGEVERSION, 
                        isNew() ? "" : m_paAtsTriggeredBroadcastHelper->getDvaMessageVersion(), 
                        version);

        m_paAtsTriggeredBroadcastHelper->setDvaMessageVersion(version);

        FUNCTION_EXIT;    
    }


    unsigned long ConfigPaAtsTriggeredBroadcast::getKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getKey();
    }


    unsigned long ConfigPaAtsTriggeredBroadcast::getAtsEventType()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getAtsEventType();
    }


    unsigned long ConfigPaAtsTriggeredBroadcast::getLocationKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getLocationKey();
    }
    

    unsigned long ConfigPaAtsTriggeredBroadcast::getDestinationLocationKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getDestinationLocationKey();
    }


    unsigned long ConfigPaAtsTriggeredBroadcast::getPlatformId()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getPlatformId();
    }


    bool          ConfigPaAtsTriggeredBroadcast::getIsLastTrain()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getIsLastTrain();
    }


    unsigned long ConfigPaAtsTriggeredBroadcast::getPaZoneGroupKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getPaZoneGroupKey();
    }


    unsigned long ConfigPaAtsTriggeredBroadcast::getPaDvaMessageKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getPaDvaMessageKey();
    }


    bool          ConfigPaAtsTriggeredBroadcast::getIsEnabledParameter()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getIsEnabledParameter();
    }

    
    std::string   ConfigPaAtsTriggeredBroadcast::getDvaMessageVersion()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getDvaMessageVersion();
    }


    time_t ConfigPaAtsTriggeredBroadcast::getDateCreated()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
		return m_paAtsTriggeredBroadcastHelper->getDateCreated();
    }


    time_t ConfigPaAtsTriggeredBroadcast::getDateModified()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
		return m_paAtsTriggeredBroadcastHelper->getDateModified();
    }
 
	
    void ConfigPaAtsTriggeredBroadcast::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaAtsTriggeredBroadcast::invalidate");
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");

        m_paAtsTriggeredBroadcastHelper->invalidate();
        m_paAtsTriggeredBroadcastChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaAtsTriggeredBroadcast::invalidate");
    }


    void ConfigPaAtsTriggeredBroadcast::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaAtsTriggeredBroadcast::deleteThisObject");
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");

        // Delete the ConfigPaAtsTriggeredBroadcast from the database
        m_paAtsTriggeredBroadcastHelper->deletePaAtsTriggeredBroadcast();
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaAtsTriggeredBroadcastHelper pointer m_paAtsTriggeredBroadcastHelper");

        delete m_paAtsTriggeredBroadcastHelper;
        m_paAtsTriggeredBroadcastHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaAtsTriggeredBroadcast::deleteThisObject");
    }


    void ConfigPaAtsTriggeredBroadcast::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaAtsTriggeredBroadcast::applyChanges");
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL, "applyChanges() has been called after ConfigPaAtsTriggeredBroadcast was deleted");

        m_paAtsTriggeredBroadcastHelper->writePaAtsTriggeredBroadcastData();

        m_paAtsTriggeredBroadcastChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaAtsTriggeredBroadcast::applyChanges");
    }


    void ConfigPaAtsTriggeredBroadcast::updatePaAtsTriggeredBroadcastChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaAtsTriggeredBroadcastChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paAtsTriggeredBroadcastChanges.find(name);

        if (matching != m_paAtsTriggeredBroadcastChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paAtsTriggeredBroadcastChanges.erase(matching);
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
            m_paAtsTriggeredBroadcastChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaAtsTriggeredBroadcast::updatePaAtsTriggeredBroadcastChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaAtsTriggeredBroadcastChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaAtsTriggeredBroadcastChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaAtsTriggeredBroadcast::setName(const std::string& name)
    {
        //TA_ASSERT(false ,"setName() should not be called.");
		LOG_GENERIC ( SourceInfo, DebugUtil::DebugError, "setName() should not be called.");
    }

    std::string ConfigPaAtsTriggeredBroadcast::getName()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The m_paAtsTriggeredBroadcastHelper pointer is null.");

        std::ostringstream name;
		name << "PA ATS Triggered Broadcast " << m_paAtsTriggeredBroadcastHelper->getKey();

        return name.str();
    }      
} // closes TA_Base_Core

