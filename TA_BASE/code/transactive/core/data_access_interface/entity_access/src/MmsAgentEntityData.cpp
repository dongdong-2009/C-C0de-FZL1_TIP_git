/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MmsAgentEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementataion of MmsAgentEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string MmsAgentEntityData::ENTITY_TYPE       = "MmsAgent";
	const std::string MmsAgentEntityData::SERVER_ADDRESS = "ServerAddress";
	const std::string MmsAgentEntityData::ASSET_NAME = "AssetName";
	
	//BEA Message Queue 
	const std::string MmsAgentEntityData::SERVER_GROUP_ID = "ServerQueueGroupId";   //using database script to delete it on CE
	const std::string MmsAgentEntityData::SERVER_QUEUE_ID = "ServerQueueId";		//using database script to delete it on CE
	const std::string MmsAgentEntityData::CLIENT_GROUP_ID = "ClientQueueGroupId";	//using database script to delete it on CE
	const std::string MmsAgentEntityData::CLIENT_QUEUE_ID = "ClientQueueId";		//using database script to delete it on CE
	
	
	
	//New for SFTP of C955
	const std::string MmsAgentEntityData::SFTP_PORT = "SFTPPort";
	const std::string MmsAgentEntityData::SFTP_USER_NAME = "SFTPUserName";
	const std::string MmsAgentEntityData::SFTP_PASSWORD = "SFTPPassword";
	const std::string MmsAgentEntityData::SFTP_LOCALFOLDER = "SFTPLocalFolder";
	const std::string MmsAgentEntityData::SFTP_DESTINATION_FOLDER = "SFTPDestinationFolder";
	const std::string MmsAgentEntityData::SFTP_PEROID_IN_SECONDS = "SFTPPeriodInSeconds";

    MmsAgentEntityData::MmsAgentEntityData(unsigned long key) 
        : EntityData(key,getStaticType())
    {
    }


    MmsAgentEntityData::~MmsAgentEntityData()
    { // Nothing to clean up
    }


    IEntityData* MmsAgentEntityData::clone(unsigned long key)
    {
        return new MmsAgentEntityData(key);        
    }


    void MmsAgentEntityData::invalidate()
    {
		getHelper()->invalidate();
    }


    std::string MmsAgentEntityData::getType()
    {
        return getStaticType();
    }


    std::string MmsAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    std::string MmsAgentEntityData::getServerAddress()
    {
        return getHelper()->getParameter(SERVER_ADDRESS);
    }


    std::string MmsAgentEntityData::getAssetName()
    {
		return getHelper()->getParameter(ASSET_NAME);
    }


	std::string MmsAgentEntityData::getSFTPport()
	{
		return getHelper()->getParameter(SFTP_PORT);
	}


	std::string MmsAgentEntityData::getSFTPUserName()
	{
		return getHelper()->getParameter(SFTP_USER_NAME);
	}


	std::string MmsAgentEntityData::getSFTPPassword()
	{
		return getHelper()->getParameter(SFTP_PASSWORD);
	}


	std::string MmsAgentEntityData::getSFTPLocalFolder()
	{
		return getHelper()->getParameter(SFTP_LOCALFOLDER);
	}


	std::string MmsAgentEntityData::getSFTPDestinationFolder()
	{
		return getHelper()->getParameter(SFTP_DESTINATION_FOLDER);
	}


	unsigned long MmsAgentEntityData::getSFTPPeroidInSeconds()
	{
		std::string value = getHelper()->getParameter(SFTP_PEROID_IN_SECONDS);

        unsigned long ulPeriod = 0u;
        if (!value.empty())
        {
            ulPeriod = EntityHelper::getUnsignedLongData(value);
        }

		return ulPeriod;
	}


    unsigned int MmsAgentEntityData::getServerQueueId()
    {
        unsigned int serverQueueId = 120;
		std::string value = getHelper()->getParameter(SERVER_QUEUE_ID);
		if (!value.empty())
		{
			serverQueueId = static_cast<unsigned int>(EntityHelper::getUnsignedLongData(value));
		}

        return serverQueueId;
    }


	unsigned int MmsAgentEntityData::getServerGroupId()
    {
        unsigned int serverGroupId = 1;

		std::string value = getHelper()->getParameter(SERVER_GROUP_ID);
		if (!value.empty())
		{
			serverGroupId = static_cast<unsigned int>(EntityHelper::getUnsignedLongData(value));
		}

        return serverGroupId;
    }


	unsigned int MmsAgentEntityData::getClientQueueId()
    {
        unsigned int clientQueueId = 121;
		std::string value = getHelper()->getParameter(CLIENT_QUEUE_ID);
		if (!value.empty())
		{
			clientQueueId = static_cast<unsigned int>(EntityHelper::getUnsignedLongData(value));
		}
        return clientQueueId;
    }


	unsigned int MmsAgentEntityData::getClientGroupId()
    {
        unsigned int clientGroupId = 1;

		std::string value = getHelper()->getParameter(CLIENT_GROUP_ID);
		if (!value.empty())
		{
			clientGroupId = static_cast<unsigned int>(EntityHelper::getUnsignedLongData(value));
		}

        return clientGroupId;
    }

}
