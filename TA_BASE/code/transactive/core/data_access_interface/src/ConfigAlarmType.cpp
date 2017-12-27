/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigAlarmType.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by:  $Author: Ouyang $
 * 
 * ConfigAlarmType is a read/write interface to the database for Alarm Types.
 */


#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/ConfigAlarmType.h"


namespace TA_Base_Core
{
    const std::string ConfigAlarmType::NAME                 = "Name";
	const std::string ConfigAlarmType::DISPLAY_NAME           = "Display Name";
    const std::string ConfigAlarmType::DESCRIPTION          = "Description";
    const std::string ConfigAlarmType::SEVERITY             = "Severity";
    const std::string ConfigAlarmType::PRINT_ALARM_STATUS   = "Print Status";
    const std::string ConfigAlarmType::PERSIST_ALARM_STATUS = "Persist Status";
	const std::string ConfigAlarmType::SYSTEM_ALARM_STATUS	= "System Status";
	//TD17862 marc_bugfix support for MMS State 
	const std::string ConfigAlarmType::MMS_STATE			= "MMS State";

	const std::string ConfigAlarmType::PARAM_ALARM_VALUE = "AlarmValue";
	const std::string ConfigAlarmType::PARAM_NORMAL_VALUE = "NormalValue";


    long ConfigAlarmType::s_nextAvailableIdentifier = 0;


    ConfigAlarmType::ConfigAlarmType(unsigned long key)
        :
        m_helper( new AlarmTypeHelper(key) ),
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

	ConfigAlarmType::ConfigAlarmType( const ConfigAlarmType& theAlarmType)
        :
        m_helper( new AlarmTypeHelper(*(theAlarmType.m_helper)) ),
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}

    ConfigAlarmType::ConfigAlarmType(
        const std::string &name,
        const std::string &description,
        unsigned long severityKey,
	    bool toBePrinted,
		bool toBePersisted,
		bool isSystemAlarm,
		bool isVisible /*=True*/)
        :
        m_helper(
            new AlarmTypeHelper(
                name,
                description,
                severityKey,
                toBePrinted,
                toBePersisted,
				isSystemAlarm,
				isVisible)
        ),
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

	ConfigAlarmType::ConfigAlarmType(const unsigned long row, TA_Base_Core::IData& data)
		:
		m_helper( new AlarmTypeHelper(row, data) ),
		m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}


	ConfigAlarmType::~ConfigAlarmType()
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "Caught in destructor");
        }
    }



    unsigned long ConfigAlarmType::getUniqueIdentifier()
    {
        return m_uniqueIdentifier;
    }


    ItemChanges ConfigAlarmType::getAllItemChanges()
    {
        return m_alarmTypeChanges;
    }

    bool ConfigAlarmType::hasChanged()
    {
        // If the alarm type changes list is empty then nothing has changed
        return !m_alarmTypeChanges.empty();
    }

    bool ConfigAlarmType::isNew()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->isNew();
    }

    void ConfigAlarmType::setName(const std::string& name)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        std::string oldName = m_helper->getName();

        m_helper->setName(name);
        
        updateAlarmTypeChanges(NAME, oldName, name);
    }

	std::string ConfigAlarmType::getDisplayName(){
		TA_ASSERT(m_helper != NULL,
			"Attempt to change state when object has been deleted.");
		return m_helper->getDisplayName();
		//return m_helper->getName();
	}
    
    void ConfigAlarmType::setDescription(const std::string& description)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        std::string oldDescription = m_helper->getDescription();

        m_helper->setDescription(description);

        updateAlarmTypeChanges(
            DESCRIPTION,
            oldDescription,
            description);

    }

	
    void ConfigAlarmType::setSeverityKey(const unsigned long severityKey)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        unsigned long oldSeverityKey = m_helper->getSeverityKey();

        std::string oldSeverityName = getSeverityName(oldSeverityKey);
        std::string newSeverityName = getSeverityName(severityKey);

        m_helper->setSeverityKey(severityKey);

        updateAlarmTypeChanges(SEVERITY, oldSeverityName, newSeverityName);
    }


    std::string ConfigAlarmType::getSeverityName(const unsigned long key)
    {
        IAlarmSeverityData* data =
            AlarmSeverityAccessFactory::getInstance().getSeverityByKey(key);

        std::string name = "";
        try
        {
            name = data->getName();
        }
        catch(...)
        {
            delete data;
            data = NULL;
            throw;
        }

        return name;
    }
    
    unsigned long ConfigAlarmType::getContextKey()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmHelper pointer is null.");
        return m_helper->getContextKey();
    }

    std::string ConfigAlarmType::getContextName()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getContextName();
    }

    void ConfigAlarmType::setPrintAlarmStatus(const bool printAlarmStatus)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        bool oldPrintStatus = m_helper->toBePrinted();

        m_helper->setPrintAlarmStatus(printAlarmStatus);

        updateAlarmTypeChanges(
            PRINT_ALARM_STATUS,
            getBoolAsString(oldPrintStatus),
            getBoolAsString(printAlarmStatus) );
    }


    void ConfigAlarmType::setSystemAlarmStatus(const bool systemAlarmStatus)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        bool oldSystemStatus = m_helper->isSystemAlarm();

        m_helper->setSystemAlarmStatus(systemAlarmStatus);

        updateAlarmTypeChanges(
            SYSTEM_ALARM_STATUS,
            getBoolAsString(oldSystemStatus),
            getBoolAsString(systemAlarmStatus) );
    }
	
    void ConfigAlarmType::setPersistAlarmStatus(const bool persistAlarmStatus)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        bool oldPersistStatus = toBePersisted();
        
        m_helper->setPersistAlarmStatus(persistAlarmStatus);

        updateAlarmTypeChanges(
            PERSIST_ALARM_STATUS,
            getBoolAsString(oldPersistStatus),
            getBoolAsString(persistAlarmStatus) );
    }

	// To set the alarm values for system alarm
	void ConfigAlarmType::setAlarmValue(const std::string& alarmValue)
	{
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

		std::string oldAlarmValue = m_helper->getAlarmValue();
        
        m_helper->setAlarmValue(alarmValue);

        updateAlarmTypeChanges(
            PARAM_ALARM_VALUE,
            oldAlarmValue,
            alarmValue );
	}

	void ConfigAlarmType::setNormalValue(const std::string& normalValue)
	{
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

		std::string oldNormalValue = m_helper->getNormalValue();
        
        m_helper->setNormalValue(normalValue);

        updateAlarmTypeChanges(
            PARAM_NORMAL_VALUE,
            oldNormalValue,
            normalValue );
	}
	
    void ConfigAlarmType::deleteThisObject()
    {
        m_helper->deleteAlarmType();
        delete m_helper;
        m_helper = NULL;
    }

    bool ConfigAlarmType::canDelete()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to query state when object has been deleted.");

        return m_helper->canDelete();
    }

    void ConfigAlarmType::applyChanges()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        m_helper->writeAlarmTypeData();
        m_alarmTypeChanges.clear();
    }

    
	unsigned long ConfigAlarmType::getKey()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getKey();
    }

  
    std::string ConfigAlarmType::getName()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getName();
    }

    std::string ConfigAlarmType::getDescription()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getDescription();
    }


    unsigned long ConfigAlarmType::getSeverityKey()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getSeverityKey();
    }

    std::string ConfigAlarmType::getSeverityName()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getSeverityName();
    }


	bool ConfigAlarmType::toBePrinted()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        return m_helper->toBePrinted();
    }

	bool ConfigAlarmType::isSystemAlarm()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        return m_helper->isSystemAlarm();
    }

	//20449++
	bool ConfigAlarmType::isPhysicalAlarm()
    {
        TA_ASSERT(m_helper != NULL,
			"Attempt to change state when object has been deleted.");
        
        return m_helper->isPhysicalAlarm();
    }
	//++20449
	bool ConfigAlarmType::toBePersisted()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->toBePersisted();
    }

	// To get all the alarm-param names
	std::vector<std::string> ConfigAlarmType::getParameterNames()
	{
		std::vector<std::string> parameterNames;
		
		parameterNames.push_back(PARAM_ALARM_VALUE);
		parameterNames.push_back(PARAM_NORMAL_VALUE);

		return parameterNames;
	}

	std::string ConfigAlarmType::getParameterValue(const std::string& paramName)
	{
		std::string defaultValue("");

		if (paramName == PARAM_ALARM_VALUE)
		{
			return getAlarmValue();
		}
		else if (paramName == PARAM_NORMAL_VALUE)
		{
			return getNormalValue();
		}
		
		// default empty string
		return defaultValue;		
	}

	void ConfigAlarmType::setParameterValue( const std::string& value, const std::string& paramName )
	{
		if (paramName == PARAM_ALARM_VALUE)
		{
			setAlarmValue(value);
		}
		else if (paramName == PARAM_NORMAL_VALUE)
		{
			setNormalValue(value);
		}
		// else do nothing as there are no other parameters
	}

	// return values will be empty for non-system alarm
	std::string ConfigAlarmType::getAlarmValue()
	{
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        return m_helper->getAlarmValue();
	}

	std::string ConfigAlarmType::getNormalValue()
	{
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        return m_helper->getNormalValue();
	}

	bool ConfigAlarmType::isVisible()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->isVisible();
    }


    time_t ConfigAlarmType::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getDateCreated();
    }


    time_t ConfigAlarmType::getDateModified()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getDateModified();
    }

    //Mintao++ TD15302
    void ConfigAlarmType::createMMSRule(IAlarmData::EMmsState mmsStateType) //TD17628 
    {
        TA_ASSERT(m_helper != NULL,"The AlarmHelper pointer is null.");
        m_helper->createMMSRule(mmsStateType); //TD17628 
    }

    void ConfigAlarmType::deleteMMSRule()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmHelper pointer is null.");
        m_helper->deleteMMSRule();
    }

    bool  ConfigAlarmType::isMMSRuleDeleted()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmHelper pointer is null.");
        return m_helper->isMMSRuleDeleted();       
    }
    
    bool    ConfigAlarmType::isMMSEnabled()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmHelper pointer is null.");
        return m_helper->isMMSEnabled();
    }

    MMSRuleData* ConfigAlarmType::getRelatedMMSRule()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmHelper pointer is null.");
        return m_helper->getRelatedMMSRule();
    }
    //Mintao++ TD15302
	//TD17862 marc_bugfix support for mms type 
	IAlarmData::EMmsState ConfigAlarmType::getRelatedMMSType()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmHelper pointer is null.");
        return m_helper->getRelatedMMSType();
    }

	void ConfigAlarmType::setRelatedMMSType(IAlarmData::EMmsState mmsState)
	{
        TA_ASSERT(m_helper != NULL,"The AlarmHelper pointer is null.");

		IAlarmData::EMmsState oldmmsState = m_helper->getRelatedMMSType();
		
        m_helper->setRelatedMMSType(mmsState);

        updateAlarmTypeChanges(
            MMS_STATE,
            getUnsignedLongAsString((long int)oldmmsState),
            getUnsignedLongAsString((long int)mmsState) );
	}

    void ConfigAlarmType::invalidate()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        m_helper->invalidate();
        m_alarmTypeChanges.clear();
    }


    void ConfigAlarmType::updateAlarmTypeChanges(
        const std::string& name,
        const std::string& oldValue,
        const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAlarmTypeChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_alarmTypeChanges.find(name);

        if (matching != m_alarmTypeChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well
                // remove this from the list as it is not a valid change
                m_alarmTypeChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the
        // old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this
            // item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_alarmTypeChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }




    std::string ConfigAlarmType::getBoolAsString(bool theBool)
    {
        //                 true  false
        return ( theBool ? "Yes" : "No" );
    }


    bool ConfigAlarmType::getBoolFromString(const std::string& theString)
    {
        // "Yes" is true, anything else is false
        return ( (theString == "Yes") ? true : false );
    }


    std::string ConfigAlarmType::getUnsignedLongAsString(
        unsigned long number)
    {
        char buff[256] = {0};

        sprintf(buff,"%ld",number);
        //ltoa(number, buff, 10);

        return std::string(buff);
    }

    unsigned long ConfigAlarmType::getUnsignedLongFromString(
        const std::string& theString)
    {
        return atol(theString.c_str());
    }
    
	void ConfigAlarmType::setDisplayName(std::string displayname){
		TA_ASSERT(m_helper != NULL,"The AlarmHelper pointer is null.");
		m_helper->setDisplayName(displayname);
	}

} //close namespace TA_Base_Core

