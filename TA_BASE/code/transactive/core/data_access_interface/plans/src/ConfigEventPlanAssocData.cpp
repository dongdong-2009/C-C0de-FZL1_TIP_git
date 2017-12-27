/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/ConfigEventPlanAssocData.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/28 10:18:09 $
  * Last modified by:  $Author: weikun.lin $
  *
  * ConfigEventPlanAssocData is an interface to the EventPlanMap table. It provides read and write
  * access for new and existing EventPlanAssociations
  */

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <sstream>
#include "core/data_access_interface/plans/src/ConfigEventPlanAssocData.h"
#include "core/utilities/src/DebugUtil.h"

namespace
{
    const std::string CHANGE_ENABLED("Enabled");
    const std::string CHANGE_EVENTTYPE("Event Type");
    const std::string CHANGE_ENTITY("Entity");
    const std::string CHANGE_ENTITYTYPE("Entity Type");
    const std::string CHANGE_PLAN("Plan");

    const std::string TRUE("True");
    const std::string FALSE("False");
}

namespace TA_Base_Core
{
    unsigned long ConfigEventPlanAssocData::s_uniqueIdentifier(0);

    ConfigEventPlanAssocData::ConfigEventPlanAssocData() : m_helper(),
        m_uniqueIdentifier(++s_uniqueIdentifier), m_using(E_UNDETERMINED)
    {
    }


    ConfigEventPlanAssocData::ConfigEventPlanAssocData( unsigned long key, bool enabled,
        unsigned long eventTypeKey, unsigned long entityKey, unsigned long entityTypeKey,
        const std::string& planPath, time_t dateCreated, time_t dateModified ) :
        m_helper( key, enabled, eventTypeKey, entityKey, entityTypeKey, planPath, dateCreated, dateModified ),
        m_uniqueIdentifier(++s_uniqueIdentifier), m_using(E_UNDETERMINED)
    {
    }


    ConfigEventPlanAssocData::ConfigEventPlanAssocData( const ConfigEventPlanAssocData& rhs ) :
        m_helper( rhs.m_helper ), m_uniqueIdentifier(++s_uniqueIdentifier), m_using(E_UNDETERMINED)
    {
    }


    ConfigEventPlanAssocData::~ConfigEventPlanAssocData()
    {
    }


    void ConfigEventPlanAssocData::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // Invalidate the helper.
        m_helper.invalidate();

        // Reset the change list.
        m_eventPlanChanges.clear();

        FUNCTION_EXIT;
    }


    ItemChanges ConfigEventPlanAssocData::getAllItemChanges()
    {
        FUNCTION_ENTRY("getAllItemChanges");

        ItemChanges changes;

        // Convert each of the keys into names
        ItemChanges::const_iterator changeIt;
        for ( changeIt=m_eventPlanChanges.begin(); changeIt!=m_eventPlanChanges.end(); changeIt++ )
        {
            Values value;

            // Get the old and new values as strings.
            if ( CHANGE_ENABLED == changeIt->first ||
                 CHANGE_PLAN == changeIt->first )
            {
                value = changeIt->second;
            }
            else if ( CHANGE_ENTITY == changeIt->first )
            {
                value.newValue = m_helper.retrieveEntityName( ::atol(changeIt->second.newValue.c_str()) );
                value.oldValue = m_helper.retrieveEntityName( ::atol(changeIt->second.oldValue.c_str()) );
            }
            else if ( CHANGE_ENTITYTYPE == changeIt->first )
            {
                value.newValue = m_helper.retrieveEntityTypeName( ::atol(changeIt->second.newValue.c_str()) );
                value.oldValue = m_helper.retrieveEntityTypeName( ::atol(changeIt->second.oldValue.c_str()) );
            }
            else if ( CHANGE_EVENTTYPE == changeIt->first )
            {
                value.newValue = m_helper.retrieveEventTypeName( ::atol(changeIt->second.newValue.c_str()) );
                value.oldValue = m_helper.retrieveEventTypeName( ::atol(changeIt->second.oldValue.c_str()) );
            }

            // Add the change to the change list.
            changes.insert( ItemChanges::value_type( changeIt->first, value ) );
        }

        FUNCTION_EXIT;
        return changes;
    }


    void ConfigEventPlanAssocData::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // Apply the changes.
        m_helper.applyChanges();

        // Clear the change list.
        m_eventPlanChanges.clear();

        FUNCTION_EXIT;
    }

    unsigned long ConfigEventPlanAssocData::getEventTypeKey()
    {
        return m_helper.getEventTypeKey();
    }


    bool ConfigEventPlanAssocData::isEnabled()
    {
        return m_helper.isEnabled();
    }


    unsigned long ConfigEventPlanAssocData::getEntityKey()
    {
        return m_helper.getEntityKey();
    }


    unsigned long ConfigEventPlanAssocData::getEntityTypeKey()
    {
        return m_helper.getEntityTypeKey();
    }


	std::string ConfigEventPlanAssocData::getPlanPath()
	{
        return m_helper.getPlanPath();
	}


    time_t ConfigEventPlanAssocData::getDateCreated()
    {
        return m_helper.getDateCreated();
    }


    time_t ConfigEventPlanAssocData::getDateModified()
    {
        return m_helper.getDateModified();
    }


    void ConfigEventPlanAssocData::deleteThisAssociation()
    {
        // Delete this association from the database.
        m_helper.deleteThisAssociation();

        // Clear the change list.
        m_eventPlanChanges.clear();
    }


    bool ConfigEventPlanAssocData::isUsingEntity()
    {
        if ( E_UNDETERMINED == m_using )
        {
            if ( 0 == m_helper.getEntityTypeKey() )
            {
                m_using = E_ENTITY;
            }
            else
            {
                m_using = E_ENTITYTYPE;
            }
        }
        return ( E_ENTITY == m_using );
    }


    void ConfigEventPlanAssocData::setEventTypeKey( unsigned long eventTypeKey )
    {
        FUNCTION_ENTRY("setEventTypeKey");

        // Track the change.
        updateChanges( CHANGE_EVENTTYPE, m_helper.getEventTypeKey(), eventTypeKey );

        // Apply it.
        m_helper.setEventTypeKey( eventTypeKey );

        FUNCTION_EXIT;
    }


    void ConfigEventPlanAssocData::setEnabled( const bool enabled )
    {
        FUNCTION_ENTRY("setEnabled");

        // Convert the booleans into unsigned long values.
        std::string oldState = m_helper.isEnabled()? TRUE : FALSE;
        std::string newState = enabled? TRUE : FALSE;

        // Trach the change.
        updateChanges( CHANGE_ENABLED, oldState, newState );

        // Apply it.
        m_helper.setEnabled( enabled );

        FUNCTION_EXIT;
    }


    void ConfigEventPlanAssocData::setEntityKey( const unsigned long entityKey )
    {
        FUNCTION_ENTRY("setEntityKey");

        // Track the change.
        updateChanges( CHANGE_ENTITY, m_helper.getEntityKey(), entityKey );

        // Apply it.
        m_helper.setEntityKey( entityKey );

        // If the key is now zero the entity type is being used.
        if ( 0 == entityKey )
        {
            m_using = E_ENTITYTYPE;
        }

        FUNCTION_EXIT;
    }


    void ConfigEventPlanAssocData::setEntityTypeKey( const unsigned long entityTypeKey )
    {
        FUNCTION_ENTRY("setEntityTypeKey");

        // Track the change.
        updateChanges( CHANGE_ENTITYTYPE, m_helper.getEntityTypeKey(), entityTypeKey );

        // Apply it.
        m_helper.setEntityTypeKey( entityTypeKey );

        // If the key is now zero the entity is being used.
        if ( 0 == entityTypeKey )
        {
            m_using = E_ENTITY;
        }

        FUNCTION_EXIT;
    }


	void ConfigEventPlanAssocData::setPlanPath(const std::string& planPath)
    {
        FUNCTION_ENTRY("setPlanPath");

        // Remember the old path value.
        std::string oldPlanPath(m_helper.getPlanPath());

        // Apply the change. This will validate the path.
        m_helper.setPlanPath(planPath);

        // Track the change.
        updateChanges(CHANGE_PLAN, oldPlanPath, planPath);        

        FUNCTION_EXIT;
    }


    std::string ConfigEventPlanAssocData::getEventTypeName()
    {
        FUNCTION_ENTRY("getEventTypeName");

        std::string eventTypeName( m_helper.retrieveEventTypeName( m_helper.getEventTypeKey() ) );

        FUNCTION_EXIT;
        return eventTypeName;
    }

	std::string ConfigEventPlanAssocData::getEventTypeDisplayName()
	{
		FUNCTION_ENTRY("getEventTypeDisplayName");

		std::string eventTypeName( m_helper.retrieveEventTypeDisplayName( m_helper.getEventTypeKey() ) );

		FUNCTION_EXIT;
		return eventTypeName;
	}

    void ConfigEventPlanAssocData::setEventTypeKeyFromName( const std::string& eventTypeName )
    {
        FUNCTION_ENTRY("setEventTypeKeyFromName");

        unsigned long key = m_helper.retrieveEventTypeKey( eventTypeName );
        setEventTypeKey( key );

        FUNCTION_EXIT;
    }


    std::string ConfigEventPlanAssocData::getEntityName()
    {
        FUNCTION_ENTRY("getEntityName");

        std::string entityName( m_helper.retrieveEntityName( m_helper.getEntityKey() ) );

        FUNCTION_EXIT;
        return entityName;
    }


    void ConfigEventPlanAssocData::setEntityKeyFromName( const std::string& entityName )
    {
        FUNCTION_ENTRY("setEntityKeyFromName");

        unsigned long key = m_helper.retrieveEntityKey( entityName );
        setEntityKey( key );

        FUNCTION_EXIT;
    }


    std::string ConfigEventPlanAssocData::getEntityTypeName()
    {
        FUNCTION_ENTRY("getEntityTypeName");

        std::string entityTypeName( m_helper.retrieveEntityTypeName( m_helper.getEntityTypeKey() ) );

        FUNCTION_EXIT;
        return entityTypeName;
    }


    void ConfigEventPlanAssocData::setEntityTypeKeyFromName( const std::string& entityTypeName )
    {
        FUNCTION_ENTRY("setEntityTypeKeyFromName");

        unsigned long key = m_helper.retrieveEntityTypeKey( entityTypeName );
        setEntityTypeKey( key );

        FUNCTION_EXIT;
    }


    template <typename ItemType>
    void ConfigEventPlanAssocData::updateChanges(const std::string& name, const ItemType& oldValue, const ItemType& newValue)
    {
        FUNCTION_ENTRY("updateChanges");

        // Convert old and new values to string format.
        std::ostringstream oldValueStrm, newValueStrm;
        oldValueStrm << oldValue;
        newValueStrm << newValue;

        Values values;
        values.oldValue = oldValueStrm.str();
        values.newValue = newValueStrm.str();

        // Search for this name in the map
        ItemChanges::iterator matchingChange = m_eventPlanChanges.find(name);

        if (matchingChange != m_eventPlanChanges.end())
        {
            // We already have this attribute name in the list.
            if (values.newValue == matchingChange->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_eventPlanChanges.erase(matchingChange);
            }
            else
            {
                // Update the stored new value.
                matchingChange->second.newValue = values.newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (values.oldValue != values.newValue)
        {
            // If the old and new values are different then we can add this item to the map
            m_eventPlanChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

} // Closes TA_Base_Core


