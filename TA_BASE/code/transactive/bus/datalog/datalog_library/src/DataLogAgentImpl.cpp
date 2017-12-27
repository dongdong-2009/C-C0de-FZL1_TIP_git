///////////////////////////////////////////////////////////
//  DataLogAgentImpl.cpp
//  Implementation of the Class DataLogAgentImpl
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:05
//  Original author: luohuirong
///////////////////////////////////////////////////////////
#include "DataLogManager.h"
#include "DataLogAgentImpl.h"

#include "core/message/src/NameValuePair.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/datalog/IDL/src/DataLogMessageCorbaDef.h"
#include "LocalRunParamDefinitions.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

using TA_Base_Core::DataLogSyncCorbaDef;


namespace TA_Base_Bus
{

	DataLogAgentImpl::DataLogAgentImpl(std::string& agentName)
	{
		//std::string corbaName=agentName + TA_Base_Bus::TA_DataLogAgent::DATALOG_ACCESS_INTERFACE;
		//activateServantWithName(corbaName);
	}	 


	DataLogAgentImpl::~DataLogAgentImpl()
	{
	    deactivateServant();
	}

	TA_Base_Core::DataLogSyncSequence* DataLogAgentImpl::getAllDataLogSync()
	{
		
		return DataLogManager::getInstance()->getAllDataLogSync();

	}
}