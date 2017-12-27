/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigEventColourData.cpp $
 * @author Derrick Liew
 * @version $Revision: #2 $
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by: $Author: Ouyang $
 * 
 * Implements IConfigEventColourData to provide a writable interface to
 * event colour data in the database.
 */

// ConfigEventColourData.cpp: implementation of the ConfigEventColourData class.
//
//////////////////////////////////////////////////////////////////////
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ConfigEventColourData.h"
#include "core/data_access_interface/src/EventColourHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    long ConfigEventColourData::s_nextAvailableIdentifier = 0;

    const std::string ConfigEventColourData::NAME   = "Name";
    const std::string ConfigEventColourData::OPEN_ACKED_COLOUR_FG1 = "Open Acknowledged Foreground Colour 1";
    const std::string ConfigEventColourData::OPEN_ACKED_COLOUR_BG1 = "Open Acknowledged Background Colour 1";
    const std::string ConfigEventColourData::OPEN_UNACKED_COLOUR_FG1 = "Open Unacknowledged Foreground Colour 1";
    const std::string ConfigEventColourData::OPEN_UNACKED_COLOUR_BG1 = "Open Unacknowledged Background Colour 1";
    const std::string ConfigEventColourData::CLOSED_UNACKED_COLOUR_FG1 = "Closed Unacknowledged Foreground Colour 1";
    const std::string ConfigEventColourData::CLOSED_UNACKED_COLOUR_BG1 = "Closed Unacknowledged Background Colour 1";
    const std::string ConfigEventColourData::CLOSED_ACKED_COLOUR_FG1 = "Closed Acknowledged Foreground Colour 1";
    const std::string ConfigEventColourData::CLOSED_ACKED_COLOUR_BG1 = "Closed Acknowledged Background Colour 1";


	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

    ConfigEventColourData::ConfigEventColourData( const unsigned long key )
		: m_helper( new EventColourHelper(key) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

	ConfigEventColourData::ConfigEventColourData(const unsigned long row, TA_Base_Core::IData& data)
		: m_helper( new EventColourHelper(row, data) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }
	
    ConfigEventColourData::~ConfigEventColourData()
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

    
    bool ConfigEventColourData::hasChanged()
    {
        // If the event colour type changes list is empty then nothing has changed
        return !m_eventColourChanges.empty();
    }

    ItemChanges ConfigEventColourData::getAllItemChanges()
    {
        return m_eventColourChanges;
    }

    unsigned long ConfigEventColourData::getUniqueIdentifier()
    {
        return m_uniqueIdentifier;
    }

    bool ConfigEventColourData::isNew()
    {
        // New event colours cannot be created.
        return false;
    }


    unsigned long ConfigEventColourData::getKey()
    {
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        return m_helper->getKey();
    }


    std::string ConfigEventColourData::getName()
    {
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        return m_helper->getName();
    }

	unsigned long ConfigEventColourData::getEventColourKey(EEventColourType type)
	{
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

		return m_helper->getEventColourKey(type);
	}


    time_t ConfigEventColourData::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        return m_helper->getDateCreated();
    }


    time_t ConfigEventColourData::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        return m_helper->getDateModified();

    }


    void ConfigEventColourData::invalidate()
    {
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        m_helper->invalidate();
        m_eventColourChanges.clear();
    }


    void ConfigEventColourData::applyChanges()
    {
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        m_helper->applyChanges();
        m_eventColourChanges.clear();
    }

    

    void ConfigEventColourData::setName(const std::string& name)
    {
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");
        
        std::string oldName = m_helper->getName();
        
        m_helper->setName(name);

        updateEventColourChanges(NAME, oldName, name);
    }


    void ConfigEventColourData::setEventColourKey(unsigned long colour, EEventColourType type)
    {
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        std::string oldColourStr = getHexColourString(m_helper->getEventColourKey(type));
        std::string newColourStr = getHexColourString(colour);

        m_helper->setEventColourKey(colour, type);

        std::string changeType = "";
        switch(type)
        {
            case(OPEN_ACKED_FG1):
                changeType = OPEN_ACKED_COLOUR_FG1;
                break;
            case(OPEN_ACKED_BG1):
                changeType = OPEN_ACKED_COLOUR_BG1;
                break;

            case(OPEN_UNACKED_FG1):
                changeType = OPEN_UNACKED_COLOUR_FG1;
                break;
            case(OPEN_UNACKED_BG1):
                changeType = OPEN_UNACKED_COLOUR_BG1;
                break;

            case(CLOSED_UNACKED_FG1):
                changeType = CLOSED_UNACKED_COLOUR_FG1;
                break;
            case(CLOSED_UNACKED_BG1):
                changeType = CLOSED_UNACKED_COLOUR_BG1;
                break;

            case(CLOSED_ACKED_FG1):
                changeType = CLOSED_ACKED_COLOUR_FG1;
                break;
            case(CLOSED_ACKED_BG1):
                changeType = CLOSED_ACKED_COLOUR_BG1;
                break;

        }
        updateEventColourChanges(changeType, oldColourStr, newColourStr);
    }

    
    std::string ConfigEventColourData::getHexColourString(unsigned long colour)
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


    void ConfigEventColourData::updateEventColourChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateEventColourChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_eventColourChanges.find(name);

        if (matching != m_eventColourChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_eventColourChanges.erase(matching);
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
            m_eventColourChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    std::string ConfigEventColourData::getUnsignedLongAsString(unsigned long number)
    {
        char buff[256] = {0};

        sprintf(buff,"%ld",number);

        return std::string(buff);
    }

    unsigned long ConfigEventColourData::getUnsignedLongFromString(const std::string& theString)
    {
        return atol(theString.c_str());
    }


}// close namespace