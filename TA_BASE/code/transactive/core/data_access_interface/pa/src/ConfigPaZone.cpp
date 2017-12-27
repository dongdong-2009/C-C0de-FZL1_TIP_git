/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaZone.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaZone is a concrete implementation of IConfigPaZone, which is in turn an implementation
  * of IPaZone. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaZones.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaZone.h"
#include "core/data_access_interface/pa/src/PaZoneHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaZone::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string LOCATIONKEY        = "Location Key";
    static const std::string ID                 = "ID";
    static const std::string LABEL              = "Label";
    static const std::string AREA               = "Area";
    // static const std::string STATUSENTITYKEY    = "Status Entity Key";
    static const std::string STN_EQUIP_ENTITY_NAME     = "Station Equipment Name";
    static const std::string OCC_EQUIP_ENTITY_NAME     = "OCC Equipment Name";
    static const std::string EQUIPMENT          = "Equipment Entity Associations";

    
    ConfigPaZone::ConfigPaZone()
     : m_paZoneHelper( new PaZoneHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaZone::ConfigPaZone(unsigned long row, TA_Base_Core::IData& data)
     : m_paZoneHelper(new PaZoneHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaZone::ConfigPaZone(const unsigned long key)
     : m_paZoneHelper( new PaZoneHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaZone::ConfigPaZone( const ConfigPaZone& theConfigPaZone)
     : m_paZoneHelper( new PaZoneHelper( *(theConfigPaZone.m_paZoneHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaZone::~ConfigPaZone()
    {
        try
        {
            delete m_paZoneHelper;
            m_paZoneHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaZone::isNew()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");

        return m_paZoneHelper->isNew();
    }

    void ConfigPaZone::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        
        updatePaZoneChanges(LOCATIONKEY, 
                            isNew() ? 0 : m_paZoneHelper->getLocationKey(), 
                            locationKey);

        m_paZoneHelper->setLocationKey(locationKey);

        FUNCTION_EXIT;
    }

    

    void ConfigPaZone::setId(const unsigned long id)
    {
        FUNCTION_ENTRY("setId");
        
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
    
        updatePaZoneChanges(ID, 
                            isNew() ? 0 : m_paZoneHelper->getId(), 
                            id);

        m_paZoneHelper->setId(id);

        FUNCTION_EXIT;
    }

    
    void ConfigPaZone::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel");
        
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
    
        updatePaZoneChanges(LABEL, 
                            isNew() ? "" : m_paZoneHelper->getLabel(), 
                            label);

        m_paZoneHelper->setLabel(label);

        FUNCTION_EXIT;
    }


    void ConfigPaZone::setStnEntityName(const std::string& name)
    {
        FUNCTION_ENTRY("setStnEntityName");
        
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
    
        updatePaZoneChanges(STN_EQUIP_ENTITY_NAME, 
                                isNew() ? "" : m_paZoneHelper->getStnEntityName(), 
                                name);

        m_paZoneHelper->setStnEntityName(name);

        FUNCTION_EXIT;

    }


    void ConfigPaZone::setOccEntityName(const std::string& name)     
    {
        FUNCTION_ENTRY("setOccEntityName");
        
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
    
        updatePaZoneChanges(OCC_EQUIP_ENTITY_NAME, 
                                isNew() ? "" : m_paZoneHelper->getOccEntityName(), 
                                name);

        m_paZoneHelper->setOccEntityName(name);

        FUNCTION_EXIT;

    }
        
    /*
    void ConfigPaZone::setStatusEntityKey(const unsigned long statusEntityKey)
    {
        FUNCTION_ENTRY("setStatusEntityKey");
        
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
    
        updatePaZoneChanges(STATUSENTITYKEY, 
                                isNew() ? 0 : m_paZoneHelper->getStatusEntityKey(), 
                                statusEntityKey);

        m_paZoneHelper->setStatusEntityKey(statusEntityKey);

        FUNCTION_EXIT;
    }
    */

    //void ConfigPaZone::setAssociatedEquipmentEntity( const std::string& entityName, bool isAssociated)
    //{
    //    FUNCTION_ENTRY("setAssociatedEquipmentEntity");

    //    // We just simulate the adding and removing of changes from the list as we need to check the
    //    // profiles manually when the list of changes is retrieved. It is too difficult to try and
    //    // manage profile changes in strings. Instead we use the m_originalProfiles and m_newProfiles
    //    // sets. The haveProfilesChanged() call below will maintain these sets.

    //    bool haveChanged = haveEquipmentEntitiesChanged(entityName, isAssociated);

    //    if (haveChanged)
    //    {
    //        updatePaZoneChanges(EQUIPMENT, "old", "new");
    //    }
    //    else
    //    {
    //        updatePaZoneChanges(EQUIPMENT, "old", "old");
    //    }

    //    m_paZoneHelper->setAssociatedEquipmentEntity(entityName, isAssociated);

    //    FUNCTION_EXIT;    
    //}


    unsigned long ConfigPaZone::getKey()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getKey();
    }

    unsigned long ConfigPaZone::getLocationKey()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getLocationKey();
    }

    unsigned long ConfigPaZone::getId()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getId();
    }

    std::string ConfigPaZone::getLabel()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getLabel();
    }    
    
    std::string ConfigPaZone::getStnEntityName()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getStnEntityName();
    }    

    std::string ConfigPaZone::getOccEntityName()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getOccEntityName();
    }    
    
    /*
    unsigned long ConfigPaZone::getStatusEntityKey()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getStatusEntityKey();
    }
    */

    //std::list<std::string> ConfigPaZone::getAssociatedEquipmentEntities()
    //{
    //    TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");

    //    // Set the original equipment entity set upon the first call
    //    if (!m_originalEquipmentEntities.hasBeenSet())
    //    {
    //        std::list<std::string> result = m_paZoneHelper->getAssociatedEquipmentEntities();

    //        ///// Update m_originalEquipmentEntities once off
    //        ///// Must set initial state for the new entities (to enable a proper set difference check)
    //        m_modifiedEquipmentEntities.clear();
    //        for (std::list<std::string>::iterator itr = result.begin(); itr != result.end(); itr ++)
    //        {
    //            m_modifiedEquipmentEntities.insert( std::set<std::string>::value_type(*itr) );
    //        }

    //        // Equivalent to begin with
    //        m_originalEquipmentEntities.setValue(m_modifiedEquipmentEntities);

    //        return result;
    //    }
    //    
    //    return m_paZoneHelper->getAssociatedEquipmentEntities();
    //}

    time_t ConfigPaZone::getDateCreated()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
		return m_paZoneHelper->getDateCreated();
    }


    time_t ConfigPaZone::getDateModified()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
		return m_paZoneHelper->getDateModified();
    }
 
	
    void ConfigPaZone::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaZone::invalidate");
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");

        m_paZoneHelper->invalidate();

        // Reset internal change states
        m_paZoneChanges.clear();
        m_modifiedEquipmentEntities.clear();
        m_originalEquipmentEntities.resetValue();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaZone::invalidate");
    }


    void ConfigPaZone::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaZone::deleteThisObject");
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");

        // Delete the ConfigPaZone from the database
        m_paZoneHelper->deletePaZone(cascade);
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaZoneHelper pointer m_paZoneHelper");

        delete m_paZoneHelper;
        m_paZoneHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaZone::deleteThisObject");
    }


    void ConfigPaZone::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaZone::applyChanges");
        TA_ASSERT(m_paZoneHelper != NULL, "applyChanges() has been called after ConfigPaZone was deleted");

        m_paZoneHelper->writePaZoneData();

        m_paZoneChanges.clear();

        // Force this to be invalidated
        m_modifiedEquipmentEntities.clear();
        m_originalEquipmentEntities.resetValue();
        
        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaZone::applyChanges");
    }

    void ConfigPaZone::updatePaZoneChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaZoneChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paZoneChanges.find(name);

        if (matching != m_paZoneChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paZoneChanges.erase(matching);
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
            m_paZoneChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaZone::updatePaZoneChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaZoneChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaZoneChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaZone::setName(const std::string& name)
    {
        //TA_ASSERT(false ,"setName() should not be called.");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setName() should not be called.");
    }

    std::string ConfigPaZone::getName()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The m_paZoneHelper pointer is null.");

        std::ostringstream name;
		name << "PA Zone " << m_paZoneHelper->getKey();

        return name.str();
    }
    
    //void ConfigPaZone::createPaZoneHelperRecords(
    //                    const std::vector<std::string>& entityEquipmentKeys)
    //{
    //    unsigned long zoneKey = getKey();

    //    if (0 == zoneKey)
    //    {
    //        TA_THROW(TA_Base_Core::DataException("Zone doesn't exist", 
    //                        TA_Base_Core::DataException::MISSING_MANDATORY,
    //                        "Primary key"));
    //    }
    //    
    //    for (unsigned int i = 0; i < entityEquipmentKeys.size(); i ++)
    //    {
    //        m_paZoneHelper->associateZoneWithEquipmentEntity(
    //                                    zoneKey, 
    //                                    entityEquipmentKeys[i]);
    //    }
    //}

    //bool ConfigPaZone::haveEquipmentEntitiesChanged(const std::string& entityName, bool isAssociated)
    //{
    //    FUNCTION_ENTRY("haveEquipmentEntitiesChanged");
    //                           
    //    // Make this call to ensure the m_originalEquipmentEntities array is up to date
    //    // (otherwise if we update the equipment entities before requesting them, the original
    //    // array will still be in a default state)
    //    // NOTE: m_modifiedEquipmentEntities will be set initially to m_originalEquipmentEntities
    //    //if (!m_originalEquipmentEntities.hasBeenSet())
    //    //{
    //    //    //getAssociatedEquipmentEntities();
    //    //    // The original equipment entity array should now be set
    //    //    //TA_ASSERT(m_originalEquipmentEntities.hasBeenSet(), "haveEquipmentEntitiesChanged: Internal Error");
    //    //}

    //    if (isAssociated)
    //    {
    //        // This means we're adding a new profile so add it to the list of new profiles
    //        //m_modifiedEquipmentEntities.insert(entityName);
    //    }
    //    else
    //    {
    //        // This means we're removing a profile so remove it from the list of new profiles
    //        m_modifiedEquipmentEntities.erase( entityName );
    //    }

    //    // Now figure out if the two sets (original and new) are different.
    //    std::ostringstream setDifference;
    //    std::set_symmetric_difference(m_modifiedEquipmentEntities.begin(), m_modifiedEquipmentEntities.end(),
    //                                  m_originalEquipmentEntities.getValue().begin(), m_originalEquipmentEntities.getValue().end(),
    //                                  std::ostream_iterator<std::string>(setDifference,""));

    //    // If this is empty then there were no set differences
    //    if (setDifference.str().empty())
    //    {
    //        FUNCTION_EXIT;
    //        return false;
    //    }

    //    // Otherwise there were set differences
    //    FUNCTION_EXIT;
    //    return true;
    //}

} // closes TA_Base_Core

