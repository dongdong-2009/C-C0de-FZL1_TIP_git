/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/PIDSEntityData.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by:  $Author: grace.koh $
  * 
  * 
  * PIDSEntityData is a concrete implementation of IPIDSEntityData.  
  * It provides read only access to PIDS entity data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/PIDSEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string PIDSEntityData::ENTITY_TYPE           = "PIDS";
    const std::string PIDSEntityData::SERVER_IP_ADDRESS   = "STisServerIPAddress";
	const std::string PIDSEntityData::SERVER_PORT = "STisServerPort";
	const std::string PIDSEntityData::MESSAGE_TIMEOUT = "MessageTimeout";
	const std::string PIDSEntityData::STATION_LIBRARY_SYNCHRONISATION_TIMEOUT = "StationLibrarySynchronisationTimeout";
	const std::string PIDSEntityData::MESSAGE_RETRIES = "MessageRetries";
	const std::string PIDSEntityData::STATION_DB_CONNECTION_STRING = "StationDBConnectionString";
	const std::string PIDSEntityData::VETTING_STRING = "Vetting";
    const std::string PIDSEntityData::PARAMETER_TRUE          = "1";
    const std::string PIDSEntityData::PARAMETER_FALSE         = "0";
    const std::string PIDSEntityData::MULTIBYTE_MSGCONTENT_ENCODING = "MultiByteMsgContentEncoding";
	
    PIDSEntityData::PIDSEntityData(unsigned long key)
		: EntityData(key, getType())
    {
    }


    PIDSEntityData::~PIDSEntityData()
    {
    }


    std::string PIDSEntityData::getServerIPAddress()
	{
		return  getHelper()->getParameter(SERVER_IP_ADDRESS);
	}


    int PIDSEntityData::getMessageTimeout()
	{
		std::string value = getHelper()->getParameter(MESSAGE_TIMEOUT);
		int messageTimeout = 0;
		if (!value.empty())
		{
			messageTimeout = EntityHelper::getIntegerData(value);
		}

        return messageTimeout;
	}


	int PIDSEntityData::getStationLibrarySynchronisationTimeout()
	{
		std::string value = getHelper()->getParameter(STATION_LIBRARY_SYNCHRONISATION_TIMEOUT);
		int stationLibrarySynchronisationTimeout = 0;
		if (!value.empty())
		{
			stationLibrarySynchronisationTimeout = EntityHelper::getIntegerData(value);
		}

        return stationLibrarySynchronisationTimeout;
	}


    int PIDSEntityData::getMessageRetries()
	{
		std::string value = getHelper()->getParameter(MESSAGE_RETRIES);
		int messageRetries = 0;
		if (!value.empty())
		{
			messageRetries = EntityHelper::getIntegerData(value);
        }

        return messageRetries;
	}


    int PIDSEntityData::getServerPort()
	{
		std::string value = getHelper()->getParameter(SERVER_PORT);
		int serverPort = 0;
		if (!value.empty())
		{
			serverPort = EntityHelper::getIntegerData(value);
		}

        return serverPort;
	}


	std::string PIDSEntityData::getLocalDBConnectionString()
	{
		return getHelper()->getParameter(STATION_DB_CONNECTION_STRING);
	}


	bool PIDSEntityData::getVetting()
	{
		std::string value = getHelper()->getParameter(VETTING_STRING);
		bool vetting = false; 
		if (!value.empty())
		{
			vetting = EntityHelper::getBooleanData(value);
		}

		return vetting;
	}


	void PIDSEntityData::setVetting(bool const vetting)
	{
		TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        if ( vetting )
        {
            paramMap[ VETTING_STRING ] = PARAMETER_TRUE;
        }
        else
        {
            paramMap[ VETTING_STRING ] = PARAMETER_FALSE;
        }

        getHelper()->writeParameters( paramMap );
	}


    std::string PIDSEntityData::getType()
    {
        return getStaticType();
    }


	std::string PIDSEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}


    IEntityData* PIDSEntityData::clone(unsigned long key)
    {
        return new PIDSEntityData(key);        
    }


    void PIDSEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

    std::string PIDSEntityData::getMultiByteMsgContentEncoding()
    {
		return getHelper()->getParameter(MULTIBYTE_MSGCONTENT_ENCODING);
    }

} //close namespace TA_Base_Core

