/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/ConfigTrend.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * ConfigTrend is an implementation of IConfigTrend. It holds the data specific to a Trend entry
  * in the database, and allows read-write access to that data.
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4786) // identifier truncated
#endif // _MSC_VER

#include <time.h>

#include "core/data_access_interface/trends/src/ConfigTrend.h"
#include "core/data_access_interface/trends/src/TrendHelper.h"

#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{

    // The next unique identifier available
    long ConfigTrend::s_nextAvailableIdentifier = 0;

    ConfigTrend::ConfigTrend(const std::string& trendName)
        : m_trendHelper(new TrendHelper(trendName)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    
    ConfigTrend::ConfigTrend()
        : m_trendHelper(new TrendHelper()),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigTrend::ConfigTrend( const ConfigTrend& theConfigTrend)
     : m_trendHelper( new TrendHelper( *(theConfigTrend.m_trendHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    std::string ConfigTrend::getDescription()
    {
        TA_ASSERT(m_trendHelper != NULL, "getDescription() has been called after trend was deleted");

        return m_trendHelper->getDescription();
    }
        
    std::string ConfigTrend::getGraphTypeAsString()
    {
        TA_ASSERT(m_trendHelper != NULL, "getGraphTypeAsString() has been called after trend was deleted");

        return m_trendHelper->getGraphTypeAsString();
    }
            
    /*int ConfigTrend::getRangeDaysComponent()
    {
        TA_ASSERT(m_trendHelper != NULL, "getRangeDaysComponent() has been called after trend was deleted");

        return m_trendHelper->getRangeDaysComponent();
    }
    
    long ConfigTrend::getRangeSecondsComponent()
    {
        TA_ASSERT(m_trendHelper != NULL, "getRangeSecondsComponent() has been called after trend was deleted");

        return m_trendHelper->getRangeSecondsComponent();
    }*/

    long ConfigTrend::getRangeSeconds()
    {
        TA_ASSERT(m_trendHelper != NULL, "getRangeSeconds() has been called after trend was deleted");

        return m_trendHelper->getRangeSeconds();
    }

    
    long ConfigTrend::getSampleTypeInSeconds()
    {
        TA_ASSERT(m_trendHelper != NULL, "getSampleTypeInSeconds() has been called after trend was deleted");

        return m_trendHelper->getSampleTypeInSeconds();
    }
    
    time_t ConfigTrend::getStartDateTime()
    {
        TA_ASSERT(m_trendHelper != NULL, "getStartDateTime() has been called after trend was deleted");

        return m_trendHelper->getStartDateTime();
    }
    
    bool ConfigTrend::isLive()
    {
        TA_ASSERT(m_trendHelper != NULL, "isLive() has been called after trend was deleted");

        return m_trendHelper->isLive();
    }
    
    long ConfigTrend::getOffsetInSeconds()
    {
        TA_ASSERT(m_trendHelper != NULL, "getOffsetInSeconds() has been called after trend was deleted");

        return m_trendHelper->getOffsetInSeconds();
    }
    
    std::map<std::string, ItemInfo> ConfigTrend::getItems()
    {
        TA_ASSERT(m_trendHelper != NULL, "getItems() has been called after trend was deleted");

        return m_trendHelper->getItems();
    }
   
    std::string ConfigTrend::getExtraInfo()
    {
        TA_ASSERT(m_trendHelper != NULL, "getExtraInfo() has been called after trend was deleted");

        return m_trendHelper->getExtraInfo();
    }


    void ConfigTrend::decomposeTimeComponent(long rangeTimeInSeconds, long& hours, long& minutes)
    {
        TA_ASSERT(m_trendHelper != NULL, "getExtraInfo() has been called after trend was deleted");

        m_trendHelper->decomposeTimeComponent(rangeTimeInSeconds, hours, minutes);
    }


    std::string ConfigTrend::getSamplePeriodAsString()
    {
        TA_ASSERT(m_trendHelper != NULL, "getSamplePeriodAsStrings() has been called after trend was deleted");

        return m_trendHelper->getSamplePeriodAsString();
    }

    void ConfigTrend::setGraphType(const std::string& graphType)
    {
        TA_ASSERT(m_trendHelper != NULL, "setGraphType() has been called after trend was deleted");

        std::string oldVal = m_trendHelper->getGraphTypeAsString();
        std::string newVal = graphType;
        
        if(oldVal != newVal)
        {
            updateChanges("GraphType", oldVal, newVal);
        }

        m_trendHelper->setGraphType(graphType);
    }


    /*void ConfigTrend::setRangeDayComponent(int days)
    {
        TA_ASSERT(m_trendHelper != NULL, "setRangeDayComponent() has been called after trend was deleted");

        std::ostringstream oldVal;
        oldVal << m_trendHelper->getRangeDaysComponent();
        std::ostringstream newVal;
        newVal << days;

        if(oldVal.str() != newVal.str())
        {
            updateChanges("RangeDayComponent", oldVal.str(), newVal.str());
        }

        m_trendHelper->setRangeDayComponent(days);
    }


    void ConfigTrend::setRangeSecondsComponent(long seconds)
    {
        TA_ASSERT(m_trendHelper != NULL, "setRangeSecondsComponent() has been called after trend was deleted");

        std::ostringstream oldVal;
        oldVal << m_trendHelper->getRangeSecondsComponent();
        std::ostringstream newVal;
        newVal << seconds;

        if(oldVal.str() != newVal.str())
        {
            updateChanges("RangeSecondsComponent", oldVal.str(), newVal.str());
        }

        m_trendHelper->setRangeSecondsComponent(seconds);
    }*/


    void ConfigTrend::setRangeSeconds(long rangeSeconds)
    {
        TA_ASSERT(m_trendHelper != NULL, "setRangeSeconds() has been called after trend was deleted");

        std::ostringstream oldVal;
        oldVal << m_trendHelper->getRangeSeconds();
        std::ostringstream newVal;
        newVal << rangeSeconds;

        if(oldVal.str() != newVal.str())
        {
            updateChanges("RangeSecondsComponent", oldVal.str(), newVal.str());
        }

        m_trendHelper->setRangeSeconds(rangeSeconds);
    }


    void ConfigTrend::setSampleType(long sampleInSeconds)
    {
        TA_ASSERT(m_trendHelper != NULL, "setSampleType() has been called after trend was deleted");

        std::ostringstream oldVal;
        oldVal << m_trendHelper->getSampleTypeInSeconds();
        std::ostringstream newVal;
        newVal << sampleInSeconds;
        
        if(oldVal.str() != newVal.str())
        {
            updateChanges("SamplePeriod", oldVal.str(), newVal.str());
        }

        m_trendHelper->setSamplePeriod(sampleInSeconds);
    }   


    void ConfigTrend::setStartDateTime(time_t startDateTime)
    {
        TA_ASSERT(m_trendHelper != NULL, "setStartDateTime() has been called after trend was deleted");

        
        std::ostringstream oldVal;
        oldVal << convertTimeToString(m_trendHelper->getStartDateTime());
        std::ostringstream newVal;
        newVal << convertTimeToString(startDateTime);
        
        if(oldVal.str() != newVal.str())
        {
            updateChanges("StartDateTime", oldVal.str(), newVal.str());
        }

        m_trendHelper->setStartDateTime(startDateTime);
    }


    void ConfigTrend::setLive(bool live)
    {
        TA_ASSERT(m_trendHelper != NULL, "setLive() has been called after trend was deleted");

        std::ostringstream oldVal;
        oldVal << m_trendHelper->isLive() ? 1 : 0;
        std::ostringstream newVal;
        newVal << live ? 1 : 0;

        if(oldVal.str() != newVal.str())
        {
            updateChanges("Live", oldVal.str(), newVal.str());
        }

        m_trendHelper->setLive(live);
    }


    void ConfigTrend::setOffset(long offsetInSeconds)
    {
        TA_ASSERT(m_trendHelper != NULL, "setOffset() has been called after trend was deleted");

        std::ostringstream oldVal;
        oldVal << m_trendHelper->getOffsetInSeconds();
        std::ostringstream newVal;
        newVal << offsetInSeconds;
        
        if(oldVal.str() != newVal.str())
        {
            updateChanges("OffsetTime", oldVal.str(), newVal.str());
        }
        
        m_trendHelper->setOffset(offsetInSeconds);
    }


    void ConfigTrend::setItems(const std::map<std::string, ItemInfo>& items)
    {
        TA_ASSERT(m_trendHelper != NULL, "setItems() has been called after trend was deleted");

        std::string oldVal = TrendHelper::createItemString(m_trendHelper->getItems());
        std::string newVal = TrendHelper::createItemString(items);
        
        if(oldVal != newVal)
        {
            updateChanges("Items", oldVal, newVal);
        }

        m_trendHelper->setItems(items);
    }


    void ConfigTrend::setExtraInfo(const std::string& info)
    {
        TA_ASSERT(m_trendHelper != NULL, "setExtraInfo() has been called after trend was deleted");

        std::string oldVal = m_trendHelper->getExtraInfo();
        std::string newVal = info;

        if(oldVal != newVal)
        {
            updateChanges("ExtraInfo", oldVal, newVal);
        }

        m_trendHelper->setExtraInfo(info);
    }


     void ConfigTrend::setDescription(const std::string& description)
    {
        TA_ASSERT(m_trendHelper != NULL, "setExtraInfo() has been called after trend was deleted");

        std::string oldVal = m_trendHelper->getDescription();
        std::string newVal = description;

        if(oldVal != newVal)
        {
            updateChanges("Description", oldVal, newVal);
        }

        m_trendHelper->setDescription(description);
    }


    ItemChanges ConfigTrend::getAllItemChanges()
    {
        FUNCTION_ENTRY("getAllItemChanges");

        // We must go through and find all location and display changes and convert them into displayable
        // strings for the user.
        ItemChanges changesToReturn;

        for (ItemChanges::iterator entry = m_trendChanges.begin(); entry != m_trendChanges.end(); ++entry)
        {
            // Don't need to modify this entry so just add it and continue onto the next one
            changesToReturn.insert( ItemChanges::value_type( entry->first, entry->second ) );
        }

        FUNCTION_EXIT;
        return changesToReturn;
    }


    bool ConfigTrend::hasChanged()
    {
        // If the profile changes is empty then nothing has changed
        return !m_trendChanges.empty();
    }


    bool ConfigTrend::isNew()
    {
        TA_ASSERT(m_trendHelper != NULL, "isNew() has been called after trend was deleted");

        return m_trendHelper->isNew();
    }


    void ConfigTrend::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");

        TA_ASSERT(m_trendHelper != NULL, "Helper was NULL");
    
        std::string oldVal = m_trendHelper->getName();
        std::string newVal = name;

        if(oldVal != newVal)
        {
            updateChanges("Name", oldVal, newVal);
        }

        m_trendHelper->setName(name);

        FUNCTION_EXIT;
    }


    unsigned long ConfigTrend::getKey()
    {
        return 0;
    }


    std::string ConfigTrend::getName()
    {
        TA_ASSERT(m_trendHelper != NULL, "Helper was NULL");

        return m_trendHelper->getName();
    }

        
    void ConfigTrend::invalidate()
    {
        m_trendHelper->invalidate();
    }


    void ConfigTrend::deleteThisObject()
    {
        FUNCTION_ENTRY("deleteThisObject");

        TA_ASSERT(m_trendHelper != NULL, "deleteThisObject() has been called after trend was deleted");

        // Delete the profile from the database
        m_trendHelper->deleteTrend();
        
        delete m_trendHelper;
        m_trendHelper = NULL;

        FUNCTION_EXIT;
    }


    time_t ConfigTrend::getDateCreated()
    {
        TA_ASSERT(m_trendHelper != NULL, "Helper was NULL");

        return m_trendHelper->getDateCreated();
    }

    
    time_t ConfigTrend::getDateModified()
    {
        TA_ASSERT(m_trendHelper != NULL, "Helper was NULL");

        return m_trendHelper->getDateModified();
    }


    void ConfigTrend::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        TA_ASSERT(m_trendHelper != NULL, "applyChanges() has been called after trend was deleted");

        m_trendHelper->writeData();

        m_trendChanges.clear();

        FUNCTION_EXIT;
    }


    void ConfigTrend::updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_trendChanges.find(name);

        if (matching != m_trendChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_trendChanges.erase(matching);
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
            m_trendChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


    std::string ConfigTrend::convertTimeToString(time_t timeToConvert)
    {
        FUNCTION_ENTRY("convertTimeToString");

        struct tm* t = localtime(&timeToConvert);
	    
        char str[22] = {0};
	    if(t != NULL)
	    {
            //
		    // Ensure that seconds will be inserted into database as 0.
            //
		    int sec = 0;
		    sprintf(str, "%02d/%02d/%04d %02d:%02d", 
                t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min);
            
            FUNCTION_EXIT;
            return std::string(str);
	    }

        FUNCTION_EXIT;
        return "";
    }
}

