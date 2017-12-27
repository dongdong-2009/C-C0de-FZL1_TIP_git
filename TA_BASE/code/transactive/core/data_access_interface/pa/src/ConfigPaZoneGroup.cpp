/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaZoneGroup.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaZoneGroup is a concrete implementation of IConfigPaZoneGroup, which is in turn an implementation
  * of IPaZoneGroup. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaZoneGroups.
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290 4284)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaZoneGroup.h"
#include "core/data_access_interface/pa/src/PaZoneGroupHelper.h"
#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/message/src/MessagePublicationManager.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaZoneGroup::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string LOCATIONKEY        = "Location Key";
    static const std::string LABEL              = "Label";
    static const std::string ISEVENTGROUP       = "Is Event Group";


    ConfigPaZoneGroup::ConfigPaZoneGroup()
     : m_paZoneGroupHelper( new PaZoneGroupHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaZoneGroup::ConfigPaZoneGroup(unsigned long row, TA_Base_Core::IData& data)
     : m_paZoneGroupHelper(new PaZoneGroupHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaZoneGroup::ConfigPaZoneGroup(const unsigned long key)
     : m_paZoneGroupHelper( new PaZoneGroupHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPaZoneGroup::ConfigPaZoneGroup( const ConfigPaZoneGroup& theConfigPaZoneGroup)
     : m_paZoneGroupHelper( new PaZoneGroupHelper( *(theConfigPaZoneGroup.m_paZoneGroupHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaZoneGroup::~ConfigPaZoneGroup()
    {
        try
        {
            delete m_paZoneGroupHelper;
            m_paZoneGroupHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaZoneGroup::isNew()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");

        return m_paZoneGroupHelper->isNew();
    }

    void ConfigPaZoneGroup::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        
        updatePaZoneGroupChanges(LOCATIONKEY, 
                                        isNew() ? 0 : m_paZoneGroupHelper->getLocationKey(), 
                                        locationKey);        

        m_paZoneGroupHelper->setLocationKey(locationKey);

        FUNCTION_EXIT;
    }
    
    
    void ConfigPaZoneGroup::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel");
        
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
    
        updatePaZoneGroupChanges(LABEL, 
                                    isNew() ? "" : m_paZoneGroupHelper->getLabel(), 
                                    label);        

        m_paZoneGroupHelper->setLabel(label);

        FUNCTION_EXIT;
    }


    void ConfigPaZoneGroup::setIsEventGroup(const unsigned long isEventGroup)
    {
        FUNCTION_ENTRY("setIsEventGroup");
        
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");

        
        updatePaZoneGroupChanges(ISEVENTGROUP, 
                                     isNew() ? 0 : m_paZoneGroupHelper->getIsEventGroup(), 
                                     isEventGroup);
        

        m_paZoneGroupHelper->setIsEventGroup(isEventGroup);

        FUNCTION_EXIT;
    }


    unsigned long ConfigPaZoneGroup::getKey()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        return m_paZoneGroupHelper->getKey();
    }


    unsigned long ConfigPaZoneGroup::getLocationKey()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        return m_paZoneGroupHelper->getLocationKey();
    }


    std::string ConfigPaZoneGroup::getLabel()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        return m_paZoneGroupHelper->getLabel();
    }    
    
    
    unsigned long ConfigPaZoneGroup::getIsEventGroup()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        return m_paZoneGroupHelper->getIsEventGroup();
    }


    time_t ConfigPaZoneGroup::getDateCreated()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
		return m_paZoneGroupHelper->getDateCreated();
    }


    time_t ConfigPaZoneGroup::getDateModified()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
		return m_paZoneGroupHelper->getDateModified();
    }
 
	
    void ConfigPaZoneGroup::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaZoneGroup::invalidate");
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");

        m_paZoneGroupHelper->invalidate();
        m_paZoneGroupChanges.clear();
        
        m_modifiedZoneAssociations.clear();
        m_originalZoneAssociations.resetValue();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaZoneGroup::invalidate");
    }

    void ConfigPaZoneGroup::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaZoneGroup::deleteThisObject");
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        
        unsigned long oldKey = getKey();
        std::string oldLabel = getLabel();

        // Delete the ConfigPaZoneGroup from the database
        // The associated helper records defining this zones will
        // be deleted automatically (via cascade operation of Oracle)
        m_paZoneGroupHelper->deletePaZoneGroup();
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaZoneGroupHelper pointer m_paZoneGroupHelper");

        delete m_paZoneGroupHelper;
        m_paZoneGroupHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaZoneGroup::deleteThisObject");
    }

    void ConfigPaZoneGroup::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaZoneGroup::applyChanges");
        TA_ASSERT(m_paZoneGroupHelper != NULL, "applyChanges() has been called after ConfigPaZoneGroup was deleted");
        
        bool newGroup = isNew();

        if (m_recreateModified.hasBeenSet() && m_recreateModified.getValue())
        {
            TA_Base_Core::PrimitiveWrapper<time_t> createTime;
            
            if (!m_paZoneGroupHelper->isNew())
            {
                createTime.setValue(m_paZoneGroupHelper->getDateCreated());
            }
        
            // Only follow this path if user sets flag
            m_paZoneGroupHelper->writePaZoneGroupData(true);
        
            if (createTime.hasBeenSet() && 
                m_paZoneGroupHelper->getDateCreated() != createTime.getValue())
            {
                // If the creation date has changed (for an existing item), this entry must have been
                // recreated within the database.  Invalidate the zone association
                // set so all the zones are correctly associated with this
                // newly (re)created group from call to writeZoneAssociationChanges

                // Save set of zones we want saved
                std::set<unsigned long> zoneSet = m_modifiedZoneAssociations;

                // Reset the original set (take data from database - typically there
                // will be no zones)
                m_originalZoneAssociations.resetValue();
                m_modifiedZoneAssociations.clear();

                refreshZoneAssociationSets();

                // In case the local modified cache was invalidated, refresh with desired data
                m_modifiedZoneAssociations = zoneSet;
            }
        }
        else
        {
            m_paZoneGroupHelper->writePaZoneGroupData(false);
        }
                
        writeZoneAssociationChanges();                

        m_paZoneGroupChanges.clear();

        // Now update the associated records as required

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaZoneGroup::applyChanges");
    }


    void ConfigPaZoneGroup::updatePaZoneGroupChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaZoneGroupChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paZoneGroupChanges.find(name);

        if (matching != m_paZoneGroupChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paZoneGroupChanges.erase(matching);
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
            m_paZoneGroupChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaZoneGroup::updatePaZoneGroupChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaZoneGroupChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaZoneGroupChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaZoneGroup::setName(const std::string& name)
    {
        //TA_ASSERT(false ,"setName() should not be called.");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setName() should not be called.");

    }

    std::string ConfigPaZoneGroup::getName()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The m_paZoneGroupHelper pointer is null.");

        std::ostringstream name;
		name << "PA Zone Group " << m_paZoneGroupHelper->getKey();

        return name.str();
    }
    
    std::vector<IPaZone*> ConfigPaZoneGroup::getAssociatedPaZones()
	{        
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The m_paZoneGroupHelper pointer is null.");

        if (!m_originalZoneAssociations.hasBeenSet())
        {
            std::vector<IPaZone*> result = m_paZoneGroupHelper->getAssociatedPaZones();

            for (std::vector<IPaZone*>::iterator itr = result.begin(); itr != result.end(); itr ++)
            {
                m_modifiedZoneAssociations.insert((*itr)->getKey());
            }
            m_originalZoneAssociations.setValue(m_modifiedZoneAssociations);
            return result;            
        }
        
        return m_paZoneGroupHelper->getAssociatedPaZones();
	}
    

    const std::set<unsigned long> ConfigPaZoneGroup::getQueuedAssociatedPaZoneKeys()
    {
        if (!m_originalZoneAssociations.hasBeenSet())
        {
            refreshZoneAssociationSets();
        }

        // we return the cached set of modified locations, until
        // an invalidation occurs (no need to continue fetching from the database)
        return m_modifiedZoneAssociations;
    }


    void ConfigPaZoneGroup::destroyExistingPaZoneGroupHelperRecords()
    {
        m_paZoneGroupHelper->destroyGroupZoneAssociations(getKey());
    }


    void ConfigPaZoneGroup::createPaZoneGroupHelperRecords(const std::list<unsigned long>& zoneKeys)
    {
        unsigned long groupKey = getKey();

        if (0 == groupKey)
        {
            TA_THROW(TA_Base_Core::DataException("Group doesn't exist", 
                            TA_Base_Core::DataException::MISSING_MANDATORY,
                            "Primary key"));
        }
        
        m_paZoneGroupHelper->associateGroupWithZones(groupKey, zoneKeys);
    }
    

    // Added as part of resolution for TD #3488
    void ConfigPaZoneGroup::deletePaZoneGroupHelperRecords(const std::list<unsigned long>& zoneKeys)
    {
        unsigned long groupKey = getKey();

        if (0 == groupKey)
        {
            TA_THROW(TA_Base_Core::DataException("Group doesn't exist", 
                            TA_Base_Core::DataException::MISSING_MANDATORY,
                            "Primary key"));
        }
        
        m_paZoneGroupHelper->removeGroupZoneAssociations(groupKey, zoneKeys);
    }

    bool ConfigPaZoneGroup::hasChanged()
    {
        // Perform test to see if zone associations have been modified
        if (m_originalZoneAssociations.hasBeenSet() && 
            m_modifiedZoneAssociations != m_originalZoneAssociations.getValue())
        {
            return true;
        }

        // If the PaZoneGroup changes is empty then nothing has changed
        return !m_paZoneGroupChanges.empty();
    }


    void ConfigPaZoneGroup::queueAdditionOfPaZoneGroupHelperRecords(const std::list<unsigned long>& zoneKeys)
    {
        queueZoneGroupHelperRecordUpdate(zoneKeys, true);
    }

        
    void ConfigPaZoneGroup::queueDeletionOfPaZoneGroupHelperRecords(const std::list<unsigned long>& zoneKeys)
    {
        queueZoneGroupHelperRecordUpdate(zoneKeys, false);
    }


    void ConfigPaZoneGroup::refreshZoneAssociationSets()
    {
        if (m_originalZoneAssociations.hasBeenSet())
        {
            // No need to do anything
            return;
        }

        // getAssociationPaZones will ensure a refresh
        std::vector<IPaZone*> zones = getAssociatedPaZones();

        // Ensure cleanup
        for (std::vector<IPaZone*>::iterator itr = zones.begin(); itr != zones.end(); itr ++)
        {
            delete *itr;
        }

        TA_ASSERT(m_originalZoneAssociations.hasBeenSet(), "queueZoneGroupHelperRecordUpdate: Internal Error");
    }


    void ConfigPaZoneGroup::queueZoneGroupHelperRecordUpdate(const std::list<unsigned long>& zoneKeys, bool add)
    {
        if (!m_originalZoneAssociations.hasBeenSet())
        {
            // Before modifying the m_modifiedZoneAssociations list, ensure
            // the original associations have been fetched
            refreshZoneAssociationSets();
        }
        
        for (std::list<unsigned long>::const_iterator itr = zoneKeys.begin(); itr != zoneKeys.end(); itr ++)
        {
            if (add)
            {
                m_modifiedZoneAssociations.insert(*itr);
            }
            else
            {
                m_modifiedZoneAssociations.erase(*itr);
            }
        }
    }


    void ConfigPaZoneGroup::writeZoneAssociationChanges()
    {
        FUNCTION_ENTRY("writeZoneAssociationChanges");

        if (!m_originalZoneAssociations.hasBeenSet())
        {
            // Nothing needs to be written
            return;
        }

        if (m_modifiedZoneAssociations != m_originalZoneAssociations.getValue())
        {
            // Only need to write the changes if changes made
            // We determine all the records added or removed from
            // the original set, and make the calls to update database appropriately
            // NOTE: removed the partial update code, which is more efficient - 
            // because the zones may have been modified via PA Manager, we need to
            // perform a full refresh to guarantee the modified zones to be updated properly
            /*
            std::list<unsigned long> recordsToDelete;
            std::list<unsigned long> recordsToAdd;           
                        
            for (std::set<unsigned long>::const_iterator itr = m_modifiedZoneAssociations.begin();
                    itr != m_modifiedZoneAssociations.end();
                    itr ++)
            {
                if (m_originalZoneAssociations.getValue().find(*itr) == 
                    m_originalZoneAssociations.getValue().end())
                {
                    // Additional items present in m_modifiedZoneAssociations 
                    // (that aren't present in the original set) need to be added
                    recordsToAdd.push_back(*itr);
                }
            }

            for (itr = m_originalZoneAssociations.getValue().begin();
                    itr != m_originalZoneAssociations.getValue().end();
                    itr ++)
            {
                if (m_modifiedZoneAssociations.find(*itr) == m_modifiedZoneAssociations.end())
                {
                    // Items in original set but no longer in 
                    // m_modifiedZoneAssociations need to be removed
                    recordsToDelete.push_back(*itr);
                }
            }            
            
            if (recordsToAdd.size() > 0)
            {    
                LOG( SourceInfo, 
                        TA_Core::DebugUtil::GenericLog, 
                        TA_Core::DebugUtil::DebugDebug, 
                        "About to create entries in PA_ZONE_HELPER");

                createPaZoneGroupHelperRecords(recordsToAdd);
            }
            
            if (recordsToDelete.size() > 0)
            {            
               LOG( SourceInfo, 
                       TA_Core::DebugUtil::GenericLog, 
                       TA_Core::DebugUtil::DebugDebug, 
                       "About to remove entries in PA_ZONE_HELPER");
        
                deletePaZoneGroupHelperRecords(recordsToDelete);
            }*/
            
            // Simply destroy all existing pa zone group helper records, 
            // and generate full set of new ones
            destroyExistingPaZoneGroupHelperRecords();

            std::list<unsigned long> recordsToAdd; 
            for (std::set<unsigned long>::const_iterator itr = m_modifiedZoneAssociations.begin();
                    itr != m_modifiedZoneAssociations.end();
                    itr ++)
            {
                recordsToAdd.push_back(*itr);
            }
            createPaZoneGroupHelperRecords(recordsToAdd);
        }
    
        m_modifiedZoneAssociations.clear();
        m_originalZoneAssociations.resetValue();

        FUNCTION_EXIT;
    }

	//TD17650
	std::string ConfigPaZoneGroup::formatZones(IPaZones& zones)
	{
		typedef std::list<unsigned long> ZoneList;
		typedef std::map<unsigned long, ZoneList> LocationKeyToZoneListMap;
		
		LocationKeyToZoneListMap locationKeyToZoneListMap;
		
		std::ostringstream destinationList;
		
		try
		{
			// build the map
			for (IPaZones::iterator it = zones.begin(); it != zones.end(); it++)
			{
				unsigned long zoneId = (*it)->getId();
				unsigned long locationKey = (*it)->getLocationKey();
				
				locationKeyToZoneListMap[locationKey].push_back(zoneId);
			}
			
			// then print the list out
			// DBG(1,2,3);MSM(4,7,1) ...
			
			for (LocationKeyToZoneListMap::iterator locIter = locationKeyToZoneListMap.begin();
			locIter != locationKeyToZoneListMap.end(); /*locIter incremented below */)
			{
				// location name
				ILocation* locationEntity = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locIter->first);
				destinationList << (*locationEntity).getName()
					<< "(";
				
				for (ZoneList::iterator zoneIter = locIter->second.begin();
				zoneIter != locIter->second.end(); /* zoneIter incremented below */)
				{
					destinationList << (*zoneIter);
					
					// avoid adding comma for the last item
					if ( ++zoneIter != locIter->second.end() )
					{
						destinationList << ",";
					}
				}
				
				// avoid adding semicolon for the last item
				if ( ++locIter != locationKeyToZoneListMap.end() )
				{
					destinationList << ");";
				}
				else
				{
					destinationList << ")";
				}
			}
			
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unkown error when formating zones");
		}
		
		return destinationList.str();

	}	
	//TD17650

} // closes TA_Base_Core

