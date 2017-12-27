
#include "GENAHttpServer.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"


#include "Poco/Exception.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

// #include <boost/asio/io_service.hpp>
// #include <boost/make_shared.hpp>
// #include <boost/bind.hpp>

using Poco::Net::ServerSocket;
using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;

// using Poco::Util::Application;
// using Poco::Util::Option;
// using Poco::Util::OptionSet;
// 
// boost::asio::io_service service(100);

namespace TA_Base_Bus
{
	GENAHttpServer::GENAHttpServer(const std::string& ipaddress, unsigned int serverPort, 
		Poco::Net::HTTPServerParams::Ptr pServerParams, IRequestProccessor* pRequestProcessor)
		: m_serverSocket(Poco::Net::SocketAddress(ipaddress, serverPort))
		, m_httpServer(new HTTPAppRequestHandlerFactory(pRequestProcessor), m_serverSocket, pServerParams)
	{
		FUNCTION_ENTRY("Constructor");

		// Allow reuse of address and port
		//m_serverSocket.setReuseAddress(true);
		//m_serverSocket.setReusePort(true);

		 

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Create server with port: %d, ServerName: %s", serverPort, pServerParams->getServerName().c_str());
		FUNCTION_EXIT;
	}

	void GENAHttpServer::start()
	{
		FUNCTION_ENTRY("start");

// 		service.post(
// 			boost::bind(&HTTPAppServer::exec, shared_from_this()));

		// start the HTTPServer
		m_httpServer.start();

		FUNCTION_EXIT;
	}

	void GENAHttpServer::stop()
	{
		FUNCTION_ENTRY("stop");
		
		m_httpServer.stop();
		FUNCTION_EXIT;
	}

// 	void HTTPAppServer::exec()
// 	{
// 		m_httpServer.start();
// 	}

} //TA_Base_Bus