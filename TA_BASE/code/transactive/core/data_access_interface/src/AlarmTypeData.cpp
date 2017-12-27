/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmTypeData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/21 00:07:03 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * AlarmTypeData is an implementation of IAlarmTypeData. It holds the data specific to a Alarm
 * type entry in the database.
 */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "core/data_access_interface/src/AlarmTypeData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{

    AlarmTypeData::AlarmTypeData(unsigned long key)
    {
        m_AlarmHelper = new AlarmTypeHelper(key);
    }

    AlarmTypeData::AlarmTypeData(unsigned long key,
            const std::string& name,
            const std::string& description,
            unsigned long severityKey,
	    	bool toBePrinted,
		    bool toBePersisted,
			bool isSystemAlarm,
			const std::string& alarmValue /*=""*/,
			const std::string& normalValue /*=""*/,
			bool isVisible /* =true*/)
    {
        m_AlarmHelper = new AlarmTypeHelper(key,name,description,severityKey,toBePrinted,toBePersisted,isSystemAlarm, isVisible);
		m_AlarmHelper->setAlarmValue(alarmValue);
		m_AlarmHelper->setNormalValue(normalValue);
    }


	AlarmTypeData::AlarmTypeData(const unsigned long row, TA_Base_Core::IData& data)
		:
		m_AlarmHelper( new AlarmTypeHelper(row, data) )
	{
	};

	AlarmTypeData::~AlarmTypeData()
	{
		if (NULL != m_AlarmHelper)
		{
			delete m_AlarmHelper;
			m_AlarmHelper = NULL;
		}
	};

    unsigned long AlarmTypeData::getKey()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getKey();
    }


    std::string AlarmTypeData::getName()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getName();
    }


    std::string AlarmTypeData::getDescription()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getDescription();
    }


    unsigned long AlarmTypeData::getSeverityKey()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getSeverityKey();
    }

    std::string AlarmTypeData::getSeverityName()
    {
        TA_ASSERT(m_AlarmHelper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_AlarmHelper->getSeverityName();
    }

    unsigned long AlarmTypeData::getContextKey()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getContextKey();
    }

    std::string AlarmTypeData::getContextName()
    {
        TA_ASSERT(m_AlarmHelper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_AlarmHelper->getContextName();
    }

    bool AlarmTypeData::toBePrinted()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->toBePrinted();
    }


    bool AlarmTypeData::toBePersisted()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->toBePersisted();
    }

	bool AlarmTypeData::isVisible()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->isVisible();
    }

	bool AlarmTypeData::isSystemAlarm()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->isSystemAlarm();
    }

	//20449++
	bool AlarmTypeData::isPhysicalAlarm()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->isPhysicalAlarm();
    }
	//++20449
	// return values will be empty for non-system alarm
	std::string AlarmTypeData::getAlarmValue()
	{
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getAlarmValue();
	}

	std::string AlarmTypeData::getNormalValue()
	{
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getNormalValue();
	}

    time_t AlarmTypeData::getDateCreated()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getDateCreated();
    }


    time_t AlarmTypeData::getDateModified()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getDateModified();
    }

    void AlarmTypeData::invalidate()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        m_AlarmHelper->invalidate();
    }
    //TD15302 Mintao++
    bool AlarmTypeData::isMMSEnabled()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->isMMSEnabled();
    }

    MMSRuleData* AlarmTypeData::getRelatedMMSRule()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getRelatedMMSRule();
    }
    //TD15302 Mintao++

	std::string AlarmTypeData::getDisplayName()
	{
		TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
		return m_AlarmHelper->getDisplayName();
	}

	IAlarmData::EMmsState AlarmTypeData::getRelatedMMSType()
    {
        TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
        return m_AlarmHelper->getRelatedMMSType();
    }

	void AlarmTypeData::setDisplayName(std::string displayname){
		TA_ASSERT(m_AlarmHelper != NULL,"The AlarmHelper pointer is null.");
		m_AlarmHelper->setDisplayName(displayname);
	}
} // closes TA_Base_Core

