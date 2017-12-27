/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/STISEntityData.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * STISEntityData is a concrete implementation of ISTISEntityData.  
  * It provides read only access to STIS entity data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string STISEntityData::ENTITY_TYPE           = "STis";
    const std::string STISEntityData::SERVER_IP_ADDRESS   = "STisServerIPAddress";
	const std::string STISEntityData::SERVER_PORT = "STisServerPort";
	const std::string STISEntityData::MESSAGE_TIMEOUT = "MessageTimeout";
	const std::string STISEntityData::STATION_LIBRARY_SYNCHRONISATION_TIMEOUT = "StationLibrarySynchronisationTimeout";
	const std::string STISEntityData::MESSAGE_RETRIES = "MessageRetries";
	const std::string STISEntityData::STATION_DB_CONNECTION_STRING = "StationDBConnectionString";
	const std::string STISEntityData::VETTING_STRING = "Vetting";
    const std::string STISEntityData::PARAMETER_TRUE          = "1";
    const std::string STISEntityData::PARAMETER_FALSE         = "0";

    const std::string STISEntityData::CCT_IP_ADDRESS = "CCTIPAddress";
    const std::string STISEntityData::CCT_PORT = "CCTPort";

	const std::string STISEntityData::STIS_SFTP_IP_ADDRESS = "STisSftpIPAddress";
	const std::string STISEntityData::STIS_SFTP_PORT = "STisSftpPort";
	const std::string STISEntityData::STIS_SFTP_USER_NAME = "STisSftpUserName";
	const std::string STISEntityData::STIS_SFTP_PASSWORD = "STisSftpPassword";

	const std::string STISEntityData::CCT_SFTP_IP_ADDRESS = "CCTSftpIPAddress";
	const std::string STISEntityData::CCT_SFTP_PORT = "CCTSftpPort";
	const std::string STISEntityData::CCT_SFTP_USER_NAME = "CCTSftpUserName";
	const std::string STISEntityData::CCT_SFTP_PASSWORD = "CCTSftpPassword";

    STISEntityData::STISEntityData(unsigned long key)
		: EntityData(key, getType())
    {
    }


    STISEntityData::~STISEntityData()
    {
    }


    std::string STISEntityData::getCCTIPAddress()
    {
        return getHelper()->getParameter(CCT_IP_ADDRESS);
    }

    int STISEntityData::getCCTPort()
    {	
        std::string value = getHelper()->getParameter(CCT_PORT);
		int cctPort = 0;
		if (!value.empty())
		{
			cctPort = EntityHelper::getIntegerData(value);
		}

        return cctPort;
    }

    std::string STISEntityData::getServerIPAddress()
	{
		return  getHelper()->getParameter(SERVER_IP_ADDRESS);
	}


    int STISEntityData::getMessageTimeout()
	{
		std::string value = getHelper()->getParameter(MESSAGE_TIMEOUT);
		int messageTimeout = 0;
		if (!value.empty())
		{
			messageTimeout = EntityHelper::getIntegerData(value);
		}

        return messageTimeout;
	}


	int STISEntityData::getStationLibrarySynchronisationTimeout()
	{
		std::string value = getHelper()->getParameter(STATION_LIBRARY_SYNCHRONISATION_TIMEOUT);
		int stationLibrarySynchronisationTimeout = 0;
		if (!value.empty())
		{
			stationLibrarySynchronisationTimeout = EntityHelper::getIntegerData(value);
		}

        return stationLibrarySynchronisationTimeout;
	}


    int STISEntityData::getMessageRetries()
	{
		std::string value = getHelper()->getParameter(MESSAGE_RETRIES);
		int messageRetries = 0;
		if (!value.empty())
		{
			messageRetries = EntityHelper::getIntegerData(value);
        }

        return messageRetries;
	}


    int STISEntityData::getServerPort()
	{
		std::string value = getHelper()->getParameter(SERVER_PORT);
		int serverPort = 0;
		if (!value.empty())
		{
			serverPort = EntityHelper::getIntegerData(value);
		}

        return serverPort;
	}


	std::string STISEntityData::getLocalDBConnectionString()
	{
		return getHelper()->getParameter(STATION_DB_CONNECTION_STRING);
	}


	bool STISEntityData::getVetting()
	{
		std::string value = getHelper()->getParameter(VETTING_STRING);
		bool vetting = false; 
		if (!value.empty())
		{
			vetting = EntityHelper::getBooleanData(value);
		}

		return vetting;
	}


	void STISEntityData::setVetting(bool const vetting)
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


    std::string STISEntityData::getType()
    {
        return getStaticType();
    }


	std::string STISEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}


    IEntityData* STISEntityData::clone(unsigned long key)
    {
        return new STISEntityData(key);        
    }


    void STISEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

	std::string STISEntityData::getStisSftpIPAddress()
	{
		return getHelper()->getParameter(STIS_SFTP_IP_ADDRESS);
	}

	std::string STISEntityData::getStisSftpUserName()
	{
		return getHelper()->getParameter(STIS_SFTP_USER_NAME);
	}

	std::string STISEntityData::getStisSftpPassword()
	{
		return getHelper()->getParameter(STIS_SFTP_PASSWORD);
	}

	int STISEntityData::getStisSftpPort()
	{

		std::string value = getHelper()->getParameter(STIS_SFTP_PORT);
		int port = 22; 
		if (!value.empty())
		{
			port = EntityHelper::getIntegerData(value);
		}

        return port;
	}

	std::string STISEntityData::getCCTSftpIPAddress()
	{
		return getHelper()->getParameter(CCT_SFTP_IP_ADDRESS);
	}

	std::string STISEntityData::getCCTSftpUserName()
	{
		return getHelper()->getParameter(CCT_SFTP_USER_NAME);
	}

	std::string STISEntityData::getCCTSftpPassword()
	{
		return getHelper()->getParameter(CCT_SFTP_PASSWORD);
	}

	int STISEntityData::getCCTSftpPort()
	{
	    std::string value = getHelper()->getParameter(CCT_SFTP_PORT);
		int port = 22; 
		if (!value.empty())
		{
			port = EntityHelper::getIntegerData(value);
		}

        return port;
	}
} //close namespace TA_Base_Core

