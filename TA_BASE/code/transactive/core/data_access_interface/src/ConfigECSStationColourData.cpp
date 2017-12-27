/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigECSStationColourData.cpp $
 * @author Andy Siow
 * @version $Revision: #2 $
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by: $Author: Ouyang $
 * 
 * Implements IConfigECSStationColourData to provide a writable interface to
 * ECS Station Colour data in the database.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ConfigECSStationColourData.h"
#include "core/data_access_interface/src/ECSStationColourHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    long ConfigECSStationColourData::s_nextAvailableIdentifier = 0;

    const std::string ConfigECSStationColourData::NAME   = "Name";
    const std::string ConfigECSStationColourData::CURRENT_ECS_STATION_COLOUR = "Current Station Proposed Colour";

    ConfigECSStationColourData::ConfigECSStationColourData( const unsigned long key )
		: m_helper( new ECSStationColourHelper(key) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigECSStationColourData::ConfigECSStationColourData(const unsigned long row, TA_Base_Core::IData& data)
		:
		m_helper( new ECSStationColourHelper(row, data) ),
		m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigECSStationColourData::~ConfigECSStationColourData()
    {
        try
        {
            if (m_helper != NULL)
            {
                delete m_helper;
                m_helper = NULL;
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }

    
    bool ConfigECSStationColourData::hasChanged()
    {
        // If the alarm type changes list is empty then nothing has changed
        return !m_ecsStationColourChanges.empty();
    }

    ItemChanges ConfigECSStationColourData::getAllItemChanges()
    {
        return m_ecsStationColourChanges;
    }

    unsigned long ConfigECSStationColourData::getUniqueIdentifier()
    {
        return m_uniqueIdentifier;
    }

    bool ConfigECSStationColourData::isNew()
    {
        // New Alarm Severities cannot be created.
        return false;
    }


    unsigned long ConfigECSStationColourData::getKey()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        return m_helper->getKey();
    }


    std::string ConfigECSStationColourData::getName()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        return m_helper->getName();
    }

	unsigned long ConfigECSStationColourData::getStationColourKey(EColourType type)
	{
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

		return m_helper->getStationColourKey(type);
	}


    time_t ConfigECSStationColourData::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        return m_helper->getDateCreated();
    }


    time_t ConfigECSStationColourData::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        return m_helper->getDateModified();

    }


    void ConfigECSStationColourData::invalidate()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        m_helper->invalidate();
        m_ecsStationColourChanges.clear();
    }


    void ConfigECSStationColourData::applyChanges()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        m_helper->applyChanges();

        m_ecsStationColourChanges.clear();
    }

    

    void ConfigECSStationColourData::setName(const std::string& name)
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");
        
        std::string oldName = m_helper->getName();
        
        m_helper->setName(name);

        updateECSStationColourChanges(NAME, oldName, name);
    }


    void ConfigECSStationColourData::setStationColourKey(unsigned long colour, EColourType type)
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        std::string oldColourStr = getHexColourString(m_helper->getStationColourKey(type));
        std::string newColourStr = getHexColourString(colour);

        m_helper->setStationColourKey(colour, type);

        std::string changeType = "";
        switch(type)
        {
            case(CURRENT_PROPOSED_CF):
                changeType = CURRENT_ECS_STATION_COLOUR;
                break;         
        }
        updateECSStationColourChanges(changeType, oldColourStr, newColourStr);
    }

    
    std::string ConfigECSStationColourData::getHexColourString(
        unsigned long colour)
    {
        char colourHexBGR[7] = {0};
        sprintf(colourHexBGR, "%06X", colour);
    
        // For some reason MFC stores colours backwards
        char colourHexRGB[7] = {0};
        colourHexRGB[0] = colourHexBGR[4];
        colourHexRGB[1] = colourHexBGR[5];
        colourHexRGB[2] = colourHexBGR[2];
        colourHexRGB[3] = colourHexBGR[3];
        colourHexRGB[4] = colourHexBGR[0];
        colourHexRGB[5] = colourHexBGR[1];
        colourHexRGB[6] = '\0';

        return std::string(colourHexRGB);
    }


    void ConfigECSStationColourData::updateECSStationColourChanges(
        const std::string& name,
        const std::string& oldValue,
        const std::string& newValue)
    {
        FUNCTION_ENTRY("updateECSStationColourChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_ecsStationColourChanges.find(name);

        if (matching != m_ecsStationColourChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_ecsStationColourChanges.erase(matching);
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
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_ecsStationColourChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    std::string ConfigECSStationColourData::getUnsignedLongAsString(
        unsigned long number)
    {
        char buff[256] = {0};

        sprintf(buff,"%ld",number);

        return std::string(buff);
    }

    unsigned long ConfigECSStationColourData::getUnsignedLongFromString(
        const std::string& theString)
    {
        return atol(theString.c_str());
    }

} // closes TA_Base_Core
