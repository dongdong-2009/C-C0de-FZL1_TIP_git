/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigAlarmSeverityData.cpp $
 * @author Dirk McCormick
 * @version $Revision: #2 $
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by: $Author: Ouyang $
 * 
 * Implements IConfigAlarmSeverityData to provide a writable interface to
 * Alarm Severity data in the database.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ConfigAlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    long ConfigAlarmSeverityData::s_nextAvailableIdentifier = 0;

    const std::string ConfigAlarmSeverityData::NAME   = "Name";
    const std::string ConfigAlarmSeverityData::OPEN_ACKED_COLOUR_FG1 = "Open Acknowledged Foreground Colour 1";
    const std::string ConfigAlarmSeverityData::OPEN_ACKED_COLOUR_FG2 = "Open Acknowledged Foreground Colour 2";
    const std::string ConfigAlarmSeverityData::OPEN_ACKED_COLOUR_BG1 = "Open Acknowledged Background Colour 1";
    const std::string ConfigAlarmSeverityData::OPEN_ACKED_COLOUR_BG2 = "Open Acknowledged Background Colour 2";
    const std::string ConfigAlarmSeverityData::OPEN_UNACKED_COLOUR_FG1 = "Open Unacknowledged Foreground Colour 1";
    const std::string ConfigAlarmSeverityData::OPEN_UNACKED_COLOUR_FG2 = "Open Unacknowledged Foreground Colour 2";
    const std::string ConfigAlarmSeverityData::OPEN_UNACKED_COLOUR_BG1 = "Open Unacknowledged Background Colour 1";
    const std::string ConfigAlarmSeverityData::OPEN_UNACKED_COLOUR_BG2 = "Open Unacknowledged Background Colour 2";
    const std::string ConfigAlarmSeverityData::CLOSED_UNACKED_COLOUR_FG1 = "Closed Unacknowledged Foreground Colour 1";
    const std::string ConfigAlarmSeverityData::CLOSED_UNACKED_COLOUR_FG2 = "Closed Unacknowledged Foreground Colour 2";
    const std::string ConfigAlarmSeverityData::CLOSED_UNACKED_COLOUR_BG1 = "Closed Unacknowledged Background Colour 1";
    const std::string ConfigAlarmSeverityData::CLOSED_UNACKED_COLOUR_BG2 = "Closed Unacknowledged Background Colour 2";
    const std::string ConfigAlarmSeverityData::SELECTED_OPEN_ACKED_COLOUR_FG1 = "Selected Open Acknowledged Foreground Colour 1";
    const std::string ConfigAlarmSeverityData::SELECTED_OPEN_ACKED_COLOUR_FG2 = "Selected Open Acknowledged Foreground Colour 2";
    const std::string ConfigAlarmSeverityData::SELECTED_OPEN_ACKED_COLOUR_BG1 = "Selected Open Acknowledged Background Colour 1";
    const std::string ConfigAlarmSeverityData::SELECTED_OPEN_ACKED_COLOUR_BG2 = "Selected Open Acknowledged Background Colour 2";
    const std::string ConfigAlarmSeverityData::SELECTED_OPEN_UNACKED_COLOUR_FG1 = "Selected Open Unacknowledged Foreground Colour 1";
    const std::string ConfigAlarmSeverityData::SELECTED_OPEN_UNACKED_COLOUR_FG2 = "Selected Open Unacknowledged Foreground Colour 2";
    const std::string ConfigAlarmSeverityData::SELECTED_OPEN_UNACKED_COLOUR_BG1 = "Selected Open Unacknowledged Background Colour 1";
    const std::string ConfigAlarmSeverityData::SELECTED_OPEN_UNACKED_COLOUR_BG2 = "Selected Open Unacknowledged Background Colour 2";
    const std::string ConfigAlarmSeverityData::SELECTED_CLOSED_UNACKED_COLOUR_FG1 = "Selected Closed Unacknowledged Foreground Colour 1";
    const std::string ConfigAlarmSeverityData::SELECTED_CLOSED_UNACKED_COLOUR_FG2 = "Selected Closed Unacknowledged Foreground Colour 2";
    const std::string ConfigAlarmSeverityData::SELECTED_CLOSED_UNACKED_COLOUR_BG1 = "Selected Closed Unacknowledged Background Colour 1";
    const std::string ConfigAlarmSeverityData::SELECTED_CLOSED_UNACKED_COLOUR_BG2 = "Selected Closed Unacknowledged Background Colour 2";


    ConfigAlarmSeverityData::ConfigAlarmSeverityData( const unsigned long key )
		: m_helper( new AlarmSeverityHelper(key) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

	ConfigAlarmSeverityData::ConfigAlarmSeverityData(const unsigned long row, TA_Base_Core::IData& data)
		: m_helper( new AlarmSeverityHelper(row, data) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigAlarmSeverityData::~ConfigAlarmSeverityData()
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

    
    bool ConfigAlarmSeverityData::hasChanged()
    {
        // If the alarm type changes list is empty then nothing has changed
        return !m_alarmSeverityChanges.empty();
    }

    ItemChanges ConfigAlarmSeverityData::getAllItemChanges()
    {
        return m_alarmSeverityChanges;
    }

    unsigned long ConfigAlarmSeverityData::getUniqueIdentifier()
    {
        return m_uniqueIdentifier;
    }

    bool ConfigAlarmSeverityData::isNew()
    {
        // New Alarm Severities cannot be created.
        return false;
    }


    unsigned long ConfigAlarmSeverityData::getKey()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        return m_helper->getKey();
    }


    std::string ConfigAlarmSeverityData::getName()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        return m_helper->getName();
    }

	unsigned long ConfigAlarmSeverityData::getSeverityColourKey(EColourType type)
	{
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

		return m_helper->getSeverityColourKey(type);
	}


    time_t ConfigAlarmSeverityData::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        return m_helper->getDateCreated();
    }


    time_t ConfigAlarmSeverityData::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        return m_helper->getDateModified();

    }


    void ConfigAlarmSeverityData::invalidate()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        m_helper->invalidate();
        m_alarmSeverityChanges.clear();
    }


    void ConfigAlarmSeverityData::applyChanges()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        m_helper->applyChanges();

        m_alarmSeverityChanges.clear();
    }

    

    void ConfigAlarmSeverityData::setName(const std::string& name)
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");
        
        std::string oldName = m_helper->getName();
        
        m_helper->setName(name);

        updateAlarmSeverityChanges(NAME, oldName, name);
    }


    void ConfigAlarmSeverityData::setSeverityColourKey(unsigned long colour, EColourType type)
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        std::string oldColourStr = getHexColourString(m_helper->getSeverityColourKey(type));
        std::string newColourStr = getHexColourString(colour);

        m_helper->setSeverityColourKey(colour, type);

        std::string changeType = "";
        switch(type)
        {
            case(OPEN_ACKED_FG1):
                changeType = OPEN_ACKED_COLOUR_FG1;
                break;
            case(OPEN_ACKED_FG2):
                changeType = OPEN_ACKED_COLOUR_FG2;
                break;
            case(OPEN_ACKED_BG1):
                changeType = OPEN_ACKED_COLOUR_BG1;
                break;
            case(OPEN_ACKED_BG2):
                changeType = OPEN_ACKED_COLOUR_BG2;
                break;
            case(OPEN_UNACKED_FG1):
                changeType = OPEN_UNACKED_COLOUR_FG1;
                break;
            case(OPEN_UNACKED_FG2):
                changeType = OPEN_UNACKED_COLOUR_FG2;
                break;
            case(OPEN_UNACKED_BG1):
                changeType = OPEN_UNACKED_COLOUR_BG1;
                break;
            case(OPEN_UNACKED_BG2):
                changeType = OPEN_UNACKED_COLOUR_BG2;
                break;
            case(CLOSED_UNACKED_FG1):
                changeType = CLOSED_UNACKED_COLOUR_FG1;
                break;
            case(CLOSED_UNACKED_FG2):
                changeType = CLOSED_UNACKED_COLOUR_FG2;
                break;
            case(CLOSED_UNACKED_BG1):
                changeType = CLOSED_UNACKED_COLOUR_BG1;
                break;
            case(CLOSED_UNACKED_BG2):
                changeType = CLOSED_UNACKED_COLOUR_BG2;
                break;
            case(SELECTED_OPEN_ACKED_FG1):
                changeType = SELECTED_OPEN_ACKED_COLOUR_FG1;
                break;
            case(SELECTED_OPEN_ACKED_FG2):
                changeType = SELECTED_OPEN_ACKED_COLOUR_FG2;
                break;
            case(SELECTED_OPEN_ACKED_BG1):
                changeType = SELECTED_OPEN_ACKED_COLOUR_BG1;
                break;
            case(SELECTED_OPEN_ACKED_BG2):
                changeType = SELECTED_OPEN_ACKED_COLOUR_BG2;
                break;
            case(SELECTED_OPEN_UNACKED_FG1):
                changeType = SELECTED_OPEN_UNACKED_COLOUR_FG1;
                break;
            case(SELECTED_OPEN_UNACKED_FG2):
                changeType = SELECTED_OPEN_UNACKED_COLOUR_FG2;
                break;
            case(SELECTED_OPEN_UNACKED_BG1):
                changeType = SELECTED_OPEN_UNACKED_COLOUR_BG1;
                break;
            case(SELECTED_OPEN_UNACKED_BG2):
                changeType = SELECTED_OPEN_UNACKED_COLOUR_BG2;
                break;
            case(SELECTED_CLOSED_UNACKED_FG1):
                changeType = SELECTED_CLOSED_UNACKED_COLOUR_FG1;
                break;
            case(SELECTED_CLOSED_UNACKED_FG2):
                changeType = SELECTED_CLOSED_UNACKED_COLOUR_FG2;
                break;
            case(SELECTED_CLOSED_UNACKED_BG1):
                changeType = SELECTED_CLOSED_UNACKED_COLOUR_BG1;
                break;
            case(SELECTED_CLOSED_UNACKED_BG2):
                changeType = SELECTED_CLOSED_UNACKED_COLOUR_BG2;
                break;
        }
        updateAlarmSeverityChanges(changeType, oldColourStr, newColourStr);
    }

    
    std::string ConfigAlarmSeverityData::getHexColourString(
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


    void ConfigAlarmSeverityData::updateAlarmSeverityChanges(
        const std::string& name,
        const std::string& oldValue,
        const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAlarmSeverityChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_alarmSeverityChanges.find(name);

        if (matching != m_alarmSeverityChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_alarmSeverityChanges.erase(matching);
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
            m_alarmSeverityChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    std::string ConfigAlarmSeverityData::getUnsignedLongAsString(
        unsigned long number)
    {
        char buff[256] = {0};

        sprintf(buff,"%ld",number);

        return std::string(buff);
    }

    unsigned long ConfigAlarmSeverityData::getUnsignedLongFromString(
        const std::string& theString)
    {
        return atol(theString.c_str());
    }
    


} // closes TA_Base_Core
