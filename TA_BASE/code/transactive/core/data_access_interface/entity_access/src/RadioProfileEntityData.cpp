/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioProfileEntityData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * 
 * RadioProfileEntityData is an implementation of IEntityData. It proves read-only access to Radio directory data,
 * and a specific framework for Radio directory parameters.
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include <iostream>

#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string RadioProfileEntityData::ENTITY_TYPE               = "RadioProfile";
    const std::string RadioProfileEntityData::CONSOLE_EVENT_TIMEOUT = "ConsoleEventTimeout";
    const std::string RadioProfileEntityData::HISTORY_LIMIT = "HistoryLimit";
	const std::string RadioProfileEntityData::PROFILE_BUTTONS = "ProfileButtons";
	const std::string RadioProfileEntityData::USER_BUTTONS = "UserButtons";
	const std::string RadioProfileEntityData::DIALPAD_MAXNUMBER_LENGTH = "Dialpad_MaxNumberLength";
	const std::string RadioProfileEntityData::DIALPAD_HISTORY_LENGTH = "Dialpad_RecentlyDialledLength";
	const std::string RadioProfileEntityData::FLASH_BUTTON_INTERVAL = "FlashButtonInterval";
	const std::string RadioProfileEntityData::BIM_RESOURCE_ID = "BIMResourceID";
	const std::string RadioProfileEntityData::SPI_RESOURCE_ID = "SPIResourceID";  // TD16770

    RadioProfileEntityData::RadioProfileEntityData(unsigned long key)
		: EntityData(key, getType())
    {
    }

    RadioProfileEntityData::~RadioProfileEntityData()
    {
    }

	unsigned long RadioProfileEntityData::getEventTimeout()
	{
		std::string value = getHelper()->getParameter(CONSOLE_EVENT_TIMEOUT);
		unsigned long eventTimeout = 300;
		if (!value.empty())
		{
			eventTimeout = EntityHelper::getIntegerData(value);
		}

        return eventTimeout;
	}

    unsigned long RadioProfileEntityData::getHistoryLimit()
    {
		std::string value = getHelper()->getParameter(HISTORY_LIMIT);
		unsigned long historyLimit = 100;
		if (!value.empty())
		{
			historyLimit = EntityHelper::getIntegerData(value);
        }

        return historyLimit;
    }


    unsigned short RadioProfileEntityData::getProfileButtons()
    {
		std::string value = getHelper()->getParameter(PROFILE_BUTTONS);
		unsigned short profileButtons = 12;

		if (!value.empty())
		{
			profileButtons = static_cast < unsigned short> (EntityHelper::getIntegerData(value));
		}

        return profileButtons;
    }


    unsigned short RadioProfileEntityData::getUserDefinedButtons()
    {
		std::string value = getHelper()->getParameter(USER_BUTTONS);
		unsigned short userButtons = 64;

		if (!value.empty())
		{
			userButtons = static_cast < unsigned short> (EntityHelper::getIntegerData(value));
		}

        return userButtons;
    }


    unsigned short RadioProfileEntityData::getDialpadMaxNumber()
    {
		std::string value = getHelper()->getParameter(DIALPAD_MAXNUMBER_LENGTH);
		unsigned short m_dialpadMaxNum = 16;
		if (!value.empty())
		{
			m_dialpadMaxNum = static_cast < unsigned short> (EntityHelper::getIntegerData(value));
		}

        return m_dialpadMaxNum;
    }


    unsigned short RadioProfileEntityData::getDialpadMaxHistory()
    {
		std::string value = getHelper()->getParameter(DIALPAD_HISTORY_LENGTH);
		unsigned short dialpadHistory = 25;
		if (!value.empty())
		{
			dialpadHistory = static_cast < unsigned short> (EntityHelper::getIntegerData(value));
		}

        return dialpadHistory;
    }


	unsigned short RadioProfileEntityData::getFlashButtonInterval()
	{
		std::string value = getHelper()->getParameter(FLASH_BUTTON_INTERVAL);
		unsigned short flashBtnInterval = 500;
		if (!value.empty())
		{
			flashBtnInterval = static_cast < unsigned short> (EntityHelper::getIntegerData(value));
		}

        return flashBtnInterval;
	}


	unsigned short RadioProfileEntityData::getBIMResourceID()
	{
		std::string value = getHelper()->getParameter(BIM_RESOURCE_ID);
		unsigned short bimRid = 68;
		if (!value.empty())
		{
			bimRid = static_cast < unsigned short> (EntityHelper::getIntegerData(value));
		}

        return bimRid;		
	}
	// TD16770
	unsigned short RadioProfileEntityData::getSPIResourceID()
	{
		std::string value("");
		value = getHelper()->getParameter(SPI_RESOURCE_ID);
		unsigned short spiRid = 57;
		if (!value.empty())
		{
            spiRid = static_cast < unsigned short>(EntityHelper::getUnsignedLongData(value));
		}

        return spiRid;
	}

    std::string RadioProfileEntityData::getType()
    {
        return getStaticType();
    }


	std::string RadioProfileEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}


    IEntityData* RadioProfileEntityData::clone(unsigned long key)
    {
        return new RadioProfileEntityData(key);        
    }


    void RadioProfileEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core

