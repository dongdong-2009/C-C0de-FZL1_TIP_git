/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/LessAgentEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/LessAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string LessAgentEntityData::ENTITY_TYPE                      = "LessAgent";
	const std::string LessAgentEntityData::PRIMARY_OPC_MACHINE_NAME = "PrimaryOpcMachineName";
	const std::string LessAgentEntityData::PRIMARY_OPC_SERVER_NAME = "PrimaryOpcServerName";
	const std::string LessAgentEntityData::SECONDARY_OPC_MACHINE_NAME = "SecondaryOpcMachineName";
	const std::string LessAgentEntityData::SECONDARY_OPC_SERVER_NAME = "SecondaryOpcServerName";
	const std::string LessAgentEntityData::LESS_OPC_GUID = "LessOpcGuid";
	const std::string LessAgentEntityData::LESS_OPC_SERVER_NAME = "LessOpcServerName";
	const std::string LessAgentEntityData::LESS_OPC_SERVER_DESCRIPTION = "LessOpcServerDescription";
	const std::string LessAgentEntityData::LESS_OPC_SERVER_EXECUTABLE_PATH = "LessOpcServerExecutablePath";
	const std::string LessAgentEntityData::LESS_OPC_SERVER_RATE_IN_MS = "LessOpcServerRateInMs";
	const std::string LessAgentEntityData::LESS_OPC_SERVER_QUALIFIER = "LessOpcServerQualifier";
	const char LessAgentEntityData::DEFAULT_QUALIFIER = '/';
	const unsigned long LessAgentEntityData::DEFAULT_RATE = 500;

	LessAgentEntityData::LessAgentEntityData() : EntityData()
	{
	}


    LessAgentEntityData::LessAgentEntityData(unsigned long key) : EntityData(key,getStaticType())
    {
    }


    LessAgentEntityData::~LessAgentEntityData()
    { // Nothing to clean up
    }

    IEntityData* LessAgentEntityData::clone(unsigned long key)
    {
        return new LessAgentEntityData(key);        
    }


    void LessAgentEntityData::invalidate()
    {
		getHelper()->invalidate();
    }


    std::string LessAgentEntityData::getType()
    {
        return getStaticType();
    }
	
    std::string LessAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


	std::string LessAgentEntityData::getPrimaryOpcMachineName()
	{
		return getHelper()->getParameter(PRIMARY_OPC_MACHINE_NAME);
	}


	std::string LessAgentEntityData::getPrimaryOpcServerName()
	{
		return getHelper()->getParameter(PRIMARY_OPC_SERVER_NAME);
	}


	std::string LessAgentEntityData::getSecondaryOpcMachineName()
	{
		return getHelper()->getParameter(SECONDARY_OPC_MACHINE_NAME);
	}


	std::string LessAgentEntityData::getSecondaryOpcServerName()
	{
		return getHelper()->getParameter(SECONDARY_OPC_SERVER_NAME);
	}


	std::string LessAgentEntityData::getLessOpcGuid()
	{
		return getHelper()->getParameter(LESS_OPC_GUID);
	}


	std::string LessAgentEntityData::getLessOpcServerName()
	{
		return getHelper()->getParameter(LESS_OPC_SERVER_NAME);
	}


	std::string LessAgentEntityData::getLessOpcServerDescription()
	{
		return getHelper()->getParameter(LESS_OPC_SERVER_DESCRIPTION);
	}


	std::string LessAgentEntityData::getLessOpcServerExecutablePath()
	{
		return getHelper()->getParameter(LESS_OPC_SERVER_EXECUTABLE_PATH);
	}


	unsigned long LessAgentEntityData::getLessOpcServerRateInMs()
	{
		unsigned long lessOpcServerRateInMs = DEFAULT_RATE;

		std::string value = getHelper()->getParameter(LESS_OPC_SERVER_RATE_IN_MS);
		
		unsigned long tmpRate = 0;
		if (!value.empty())
		{
			tmpRate = EntityHelper::getUnsignedLongData(value);
		}

		if( tmpRate < 500 )
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Invalid rate %lu continuing to use %lu", tmpRate, lessOpcServerRateInMs );
		}
		else
		{
			lessOpcServerRateInMs = tmpRate;
		}

		return lessOpcServerRateInMs;
	}


	char LessAgentEntityData::getLessOpcServerQualifier()
	{
		char lessOpcServerQualifier = DEFAULT_QUALIFIER;

		std::string tmpString = getHelper()->getParameter(LESS_OPC_SERVER_QUALIFIER);
		
		if( tmpString.size() != 1)
		{
			if ( tmpString.size() == 0 )
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Empty qualifier \
					in the agent config. Use default qualifier %c instead",  lessOpcServerQualifier );//TD14337 yezh++
			}
			else
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Invalid qualifier \
					%s continuing to use %c", tmpString.c_str(), lessOpcServerQualifier );//TD14337 yezh++
			}
		}
		//TD15462
		//Mintao
		else
		{
			lessOpcServerQualifier = tmpString.at(0);
		}
		return lessOpcServerQualifier;
	}


} //close namespace TA_Base_Core

