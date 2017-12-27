/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File$
  * @author:	Darren Sampson
  * @version:	$Revision$
  *
  * Last modification:	$DateTime$
  * Last modified by:	$Author$
  *
  * Description:
  *
  *	DataNodeAgent implementes the GenericAgent interface.  DataNodeAgent monitors and controls the 
  * the entities associated with its process, such as DataNode entities.
  *
  */

#include "ace/OS.h"
#include "cots/WinTech/OPCServerToolKit/opc_ae.h"
#include "cots/WinTech/OPCServerToolKit/opcda.h"
#include "cots/WinTech/OPCServerToolKit/WTOPCsvrAPI.h"

#include "app/scada/OpcBridgeLib/src/OpcBridge.h"
#include "app/scada/OpcBridgeLib/src/AttributeMapManager.h"
#include "app/scada/OpcBridgeLib/src/TransActiveDataSource.h"
#include "app/scada/OpcBridgeLib/src/DisplayProxyManager.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/exceptions/src/UserSettingsException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{   
	const std::string OpcBridge::TRANSACTIVEDATASOUCE_UPDATEITEM_QUEUESIZE = "OPCBridge.TransActiveDataSource.UpdateItem.QueueSize";
    const std::string OpcBridge::OPCTAGMANAGER_MAPSIZE = "OPCBridge.OpcTagManager.MapSize";
    const std::string OpcBridge::ATTRIBUTES_COUNT = "OPCBridge.Attributes.Count";
    const std::string OpcBridge::OPCTAG_CREATION_TIME = "OPCBridge.OpcTag.Creation.Time";
	//
	// Constructor
	//
	OpcBridge::OpcBridge ( int argc, char* argv[] )
	:
    m_opcTagManager ( NULL ),
    m_transActiveDataSource ( NULL ),
    m_attributeMapManager ( NULL ),
	m_bStarted(false)
	{
        FUNCTION_ENTRY("OpcBridge::OpcBridge");

        m_transActiveDataSource = new TransActiveDataSource(this);
        
        FUNCTION_EXIT;
	}

	OpcBridge::~OpcBridge()
	{
        FUNCTION_ENTRY("OpcBridge::~OpcBridge");
        FUNCTION_EXIT;
	}

	void OpcBridge::start()
	{
		FUNCTION_ENTRY("start");

        m_opcTagManager = new OpcTagManager();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "starting OPC Bridge");

		m_attributeMapManager = new AttributeMapManager(*this, *m_opcTagManager);

		//make sure our server be unregistered first, if last time don't unregister
		//some times it will happen, for example, our service is killed by task manager
		if( UnregisterServer ( (BYTE *) & CLSID_SVR, SVR_NAME.c_str() ) == TRUE )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "unregister WtOPCSvr, last time don't unregister");
		}

		// initialise DCOM and OPC Server on the thread
        m_transActiveDataSource->enableCallback( true );
		m_transActiveDataSource->initialiseWtOPCsvr(CLSID_SVR, SVR_NAME, SVR_DESC, SVR_EXE_PATH, SVR_RATE, VENDOR_INFO);

		m_bStarted = true;

		FUNCTION_EXIT;
	}

	//
	// operations required by IGenericAgentUser
	//

	void OpcBridge::agentTerminate()
	{
		FUNCTION_ENTRY("agentTerminate");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Opc bridge agent terminate begins");

		bool isDeleteCache = TA_Base_Core::RunParams::getInstance().isSet("DeleteOPCCache");
		if (m_attributeMapManager != NULL && isDeleteCache)
		{
			m_attributeMapManager->deleteOPCCacheFile();
		}

		if(!m_bStarted)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "MarkB--opc bridge terminate while starting");

			//when this situation happend, we only uninitWtOPCsvr and exit process. otherwise, if we do 
			//other clean, our service will crash, because one thread is do destruction while one thread
			//is still use them
			m_transActiveDataSource->enableCallback(false);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Uninitialising opc server");
			m_transActiveDataSource->uninitialiseWtOPCsvr(CLSID_SVR, SVR_NAME);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--Opc bridge terminate while starting");
			ExitProcess(-1);
		}
		else
		{
			DisplayProxyManager::getInstance()->stopSynchronise();
			m_transActiveDataSource->enableCallback(false);
			m_transActiveDataSource->terminateAndWait();
			m_transActiveDataSource->terminateAndWaitThreadPool();
			DisplayProxyManager::removeInstance();

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Disconnecting client of opc server");
			// Request clients to disconnect
			for ( int request = 0; 0 < ::NumbrClientConnections() && request < 6; ++request )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--Calling ::RequestDisconnect - %d", request + 1);
				::RequestDisconnect();
				TA_Base_Core::Thread::sleep(500);  // Sleep for 500 milli-seconds
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Uninitialising opc server");
			//uninitialize DCOM and OPC Server on the thread
			//TD14190, should uninitialize first, then to do other things
			m_transActiveDataSource->uninitialiseWtOPCsvr(CLSID_SVR, SVR_NAME); 
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Destroying opc tag manager");
			// Remove all Opc Tag
			delete m_opcTagManager;
			m_opcTagManager = NULL;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Destroying TransactiveDatasource");
			// Need to shutdown all Scada stuff before returning from agentTerminate
			// The generic Service Agent will shutdown the Naming service.
			delete m_transActiveDataSource;
			m_transActiveDataSource = NULL;		
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Destroying attribute map manager");   
			delete m_attributeMapManager;
			m_attributeMapManager = NULL;

			TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Opc bridge agent terminate exits");
		}
    
        FUNCTION_EXIT;
	}

	TA_Base_Bus::IEntity* OpcBridge::createEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		return NULL;
	}

	void OpcBridge::agentSetMonitor()
	{
	}

	void OpcBridge::agentSetControl()
	{
	}

	void OpcBridge::registerForStateUpdates()
	{
	}

	void OpcBridge::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
	}

	void OpcBridge::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
	}

	void OpcBridge::notifyGroupOffline( const std::string& group )
	{
	}

	void OpcBridge::notifyGroupOnline( const std::string& group )
	{
	}

	OpcTagManager& OpcBridge::getOpcTagManager()
	{
		if (m_opcTagManager == NULL)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException("m_opcTagManager not available"));
		}

        return *m_opcTagManager;
	}

	//
	// getAttributeMapManager
	//
	AttributeMapManager& OpcBridge::getAttributeMapManager()
	{
		if (m_attributeMapManager == NULL)
		{
			TA_THROW(TA_Base_Core::ScadaCommonException("AttributeMapManager not available"));
		}

		return *m_attributeMapManager;
	}

	bool OpcBridge::parseLocationString(const std::string& locationStr, std::vector<unsigned long>& outLocs)
	{
		outLocs.clear();

		bool loadAllLocations = false;
		std::string locationString(locationStr);

		int maxLoop = 100;
		while (locationString.size() > 0 && maxLoop-- > 0)
		{
			int locationKey = INT_MIN;
			std::string::size_type loc = locationString.find(",");

			std::string tmpStr;
			if (loc != std::string::npos)
			{
				tmpStr = locationString.substr(0, loc);	
				locationString = locationString.substr(loc+1);
			}
			else
			{
				tmpStr = locationString;
				locationString.clear();
			}

			bool isDigitStr = true;
			bool hasDigit = false;
			for (std::string::iterator iter = tmpStr.begin(); iter != tmpStr.end() && isDigitStr; ++iter)
			{
				if (*iter <= '9' && *iter >= '0')
				{
					hasDigit = true;
				}
				else if (*iter != ' ' && *iter != '\t')
				{
					isDigitStr = false;
				}
			}

			if (!hasDigit )
			{
				isDigitStr = false;
			}

			if ( isDigitStr )
			{
				locationKey = atoi(tmpStr.c_str());
				if (0 == locationKey)
				{
					loadAllLocations = true;
					break;
				}
				else
				{
					outLocs.push_back(locationKey);
				}	
			}
		}
		return loadAllLocations;
	}
}
