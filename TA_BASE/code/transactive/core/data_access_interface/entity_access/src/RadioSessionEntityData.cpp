/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioSessionEntityData.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * RadioSessionEntityData is a concrete implementation of IRadioSessionEntityData, which is in turn an implementation of
  * IEntityData. It proves read-only access to Radio session data, and a specific framework
  * for Radio session parameters.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/RadioSessionEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string RadioSessionEntityData::ENTITY_TYPE           = "RadioSession";
    const std::string RadioSessionEntityData::AUDIO_LINK					= "AudioLink";
    const std::string RadioSessionEntityData::AUDIO_LINK_SERVER				= "AudioLinkLoginServer";
    const std::string RadioSessionEntityData::USER_NAME						= "UserName";
    const std::string RadioSessionEntityData::TRAIN_COMMUMICATIONS			= "TrainCommunication";
    const std::string RadioSessionEntityData::CONSOLE_ID					= "ConsoleID";
    const std::string RadioSessionEntityData::CONSOLE_COM_PORT				= "ConsoleComPort";
	const std::string RadioSessionEntityData::SWITCH_USER_WAIT_PERIOD		= "SwitchUserWaitPeriod";

    RadioSessionEntityData::RadioSessionEntityData(unsigned long key)
		                   : EntityData(key, getType())

    {
    }


    RadioSessionEntityData::~RadioSessionEntityData()
    {
    }


	unsigned long RadioSessionEntityData::getAssociatedConsoleKey()
	{
		std::string value = getHelper()->getParameter(CONSOLE_ID);
		unsigned long consoleKey = 0;
		if (!value.empty())
		{
			consoleKey = EntityHelper::getUnsignedLongData(value);
		}

        return consoleKey;
	}


	unsigned long RadioSessionEntityData::getAudioLinkId()
	{
		std::string value = getHelper()->getParameter(AUDIO_LINK);
		unsigned long audioLink = 0;
		if (!value.empty())
		{
			audioLink = EntityHelper::getUnsignedLongData(value);
		}

        return audioLink;
	}


	unsigned long RadioSessionEntityData::getSwitchUserWaitPeriod()
	{
		std::string value = getHelper()->getParameter(SWITCH_USER_WAIT_PERIOD);
		unsigned long WaitPeriod = 0;
		if (!value.empty())
		{
			WaitPeriod = EntityHelper::getUnsignedLongData(value);
		}
		
        return WaitPeriod;
	}


	unsigned long RadioSessionEntityData::getAudioLinkServerId()
	{
		std::string value = getHelper()->getParameter(AUDIO_LINK_SERVER);
		unsigned long audioLinkServer = 0;
		if (!value.empty())
		{
			audioLinkServer = EntityHelper::getUnsignedLongData(value);
		}

        return audioLinkServer;
	}


	bool RadioSessionEntityData::useForTrainCommunication()
	{
		std::string value = getHelper()->getParameter(TRAIN_COMMUMICATIONS);
		bool useForTrainCommunications = false;
		if (!value.empty())
		{
			useForTrainCommunications = EntityHelper::getBooleanData(value);
		}

        return useForTrainCommunications;
	}


	std::string RadioSessionEntityData::getRadioUserName()
	{
		return getHelper()->getParameter(USER_NAME);
	}


    std::string RadioSessionEntityData::getConsoleComPort()
    {
		return getHelper()->getParameter(CONSOLE_COM_PORT);
    }

    
    std::string RadioSessionEntityData::getType()
    {
        return getStaticType();
    }


	std::string RadioSessionEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}
    
    IEntityData* RadioSessionEntityData::clone(unsigned long key)
    {
        return new RadioSessionEntityData(key);        
    }


    void RadioSessionEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core

