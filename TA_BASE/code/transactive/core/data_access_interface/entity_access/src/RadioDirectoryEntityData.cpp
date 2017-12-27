/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioDirectoryEntityData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * 
 * RadioDirectoryEntityData is an implementation of IEntityData. It proves read-only access to Radio directory data,
 * and a specific framework for Radio directory parameters.
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include <iostream>

#include "core/data_access_interface/entity_access/src/RadioDirectoryEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string RadioDirectoryEntityData::ENTITY_TYPE                 = "RadioDirectory";
    const std::string RadioDirectoryEntityData::SYNCHRONISATION_FREQUENCY = "SynchronisationFrequency";
    const std::string RadioDirectoryEntityData::LAST_UPDATE_TIME = "LastUpdateTime";

    RadioDirectoryEntityData::RadioDirectoryEntityData(unsigned long key)
		                   : EntityData(key, getType())

    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "RadioDirectoryEntityData (constructor)");
        LOG( SourceInfo, DebugUtil::FunctionExit, "RadioDirectoryEntityData (constructor)");
    }


    RadioDirectoryEntityData::~RadioDirectoryEntityData()
    {
    }


	unsigned long RadioDirectoryEntityData::getSynchronisationFrequency()
	{
		std::string value = getHelper()->getParameter(SYNCHRONISATION_FREQUENCY);
		unsigned long	synchronisationFrequency = 0;
		if (!value.empty())
		{
			synchronisationFrequency = EntityHelper::getUnsignedLongData(value);
		}

        return synchronisationFrequency;
	}

    
    void RadioDirectoryEntityData::setLastUpdateTime(time_t updateTime)
    {
        std::map<std::string,std::string> paramMap;
        std::stringstream updateTimeString;
        updateTimeString << updateTime;

        paramMap.insert(std::map<std::string,std::string>::value_type(LAST_UPDATE_TIME,updateTimeString.str()));

        getHelper()->invalidate();
        getHelper()->writeParameters(paramMap);
    }


    time_t RadioDirectoryEntityData::getLastUpdateTime()
    {
		std::string value = getHelper()->getParameter(LAST_UPDATE_TIME);
		time_t lastUpdateTime = 0;
		if (!value.empty())
		{
			lastUpdateTime = EntityHelper::getUnsignedLongData(value);
		}

        return lastUpdateTime;
    }

    
    std::string RadioDirectoryEntityData::getType()
    {
        return getStaticType();
    }


	std::string RadioDirectoryEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}

    
    IEntityData* RadioDirectoryEntityData::clone(unsigned long key)
    {
        return new RadioDirectoryEntityData(key);        
    }


    void RadioDirectoryEntityData::invalidate()
    {
        getHelper()->invalidate();
    }
} //close namespace TA_Base_Core

