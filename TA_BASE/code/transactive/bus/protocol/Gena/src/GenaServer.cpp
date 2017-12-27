
#include "GenaServer.h"
#include "GENAHttpServer.h"
#include "GenaServerImp.h"
#include "IGENAClientStatusForwarder.h"
#include "Poco/Net/HTTPServerParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using Poco::Net::HTTPServerParams;

namespace TA_Base_Bus
{
	GenaServer::GenaServer(const GenaServerParams& pServerParams)
		: m_pServerImp(0)
		, m_pHttpServer(0)
	{
		FUNCTION_ENTRY("Constructor");

		// create connection manager
		m_pConnectionMgr.reset(new GENAConnectionFactory());
		
		// Create the Gena server
		m_pServerImp = new GenaServerImp(m_pConnectionMgr,pServerParams);
		
		// Set up server parameters
		HTTPServerParams::Ptr pParams = new HTTPServerParams();
		pParams->setServerName(pServerParams.serverName);
		pParams->setKeepAlive(pServerParams.keepAlive);

		Poco::Timespan timeout(0,pServerParams.timeout*1000);  //milliSeconds to microseconds

		pParams->setKeepAliveTimeout(timeout);
		pParams->setTimeout(timeout);
		pParams->setMaxKeepAliveRequests(0);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Creating Server with serverName: %s, port: %d, timeout(Ms)=%d", pServerParams.serverName.c_str(), pServerParams.serverPort,pServerParams.timeout );

		// Set up HTTP Server
		m_pHttpServer = new GENAHttpServer(pServerParams.ipAddress, 
			pServerParams.serverPort, pParams, m_pServerImp);

		start();
		FUNCTION_EXIT;
	}

	GenaServer::~GenaServer()
	{
		FUNCTION_ENTRY("Destructor");
		terminateAndWait();
		destroyService();
		FUNCTION_EXIT;
	}

	void GenaServer::startService()
	{
		FUNCTION_ENTRY("startService");
		m_pServerImp->startService();
		FUNCTION_EXIT;
	}

	void GenaServer::stopService()
	{
		FUNCTION_ENTRY("stopService");
		m_pServerImp->stopService();
		FUNCTION_EXIT;
	}

	void GenaServer::destroyService()
	{
		FUNCTION_ENTRY("destroyService");
		if (0 != m_pServerImp)
		{
			delete m_pServerImp;
			m_pServerImp = 0;
		}

		if (0 != m_pHttpServer)
		{
			delete m_pHttpServer;
			m_pHttpServer = 0;
		}
		FUNCTION_EXIT;
	}

	void GenaServer::run()
	{
		FUNCTION_ENTRY("GenaServer::run");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Starting Server");
		// Start HTTP Server
		m_pHttpServer->start();

		FUNCTION_EXIT;
	}

	void GenaServer::terminate()
	{
		FUNCTION_ENTRY("terminate");
		// Stop HTTP Server
		m_pHttpServer->stop();

		FUNCTION_EXIT;
	}

	void GenaServer::publishUpdate(GENATopicDataPtr ptopicUpdate)
	{
		FUNCTION_ENTRY("publishUpdate");
		m_pServerImp->publishUpdate(ptopicUpdate);

		FUNCTION_EXIT;
	}

	void GenaServer::registerForwarder(IGENAClientStatusForwarder* pRequestFwd)
	{
		TA_ASSERT(pRequestFwd != 0, "Request forwarder cannot be null");
		FUNCTION_ENTRY("registerForwarder");

		m_pServerImp->subscribeToRequest(pRequestFwd);

		FUNCTION_EXIT;
	}

	void GenaServer::registerDataStore(IGENATopicDataStore* pDataStore)
	{
		FUNCTION_ENTRY("registerDataStore");
		m_pServerImp->registerDataStore(pDataStore);
		FUNCTION_EXIT;
	}
}