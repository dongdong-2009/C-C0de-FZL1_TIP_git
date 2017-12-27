/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENAHttpServer.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the HTTPAppServer which inherits from poco library HTTPServer.
  */

#ifndef __HTTP_APP_SERVER_H_INCLUDED__
#define __HTTP_APP_SERVER_H_INCLUDED__

#include "IRequestProccessor.h"
#include "HTTPAppRequestHandlerFactory.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/ServerSocket.h"
#include "boost/enable_shared_from_this.hpp"


#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

#pragma once

namespace TA_Base_Bus
{
	class GENAHttpServer 
		: public boost::enable_shared_from_this<GENAHttpServer>
	{
	public:
		// Constructor
		GENAHttpServer( const std::string& ipaddress,
			unsigned int serverPort, 
			Poco::Net::HTTPServerParams::Ptr pServerParams, 
			IRequestProccessor* pRequestProcessor); 

		// Destructor
		~GENAHttpServer(void){};

		/* 
		 * start
		 *
		 * Initializes necessary configurations and server components.
		*/
		void start();

		/* 
		 * stop
		*/
		void stop();

	protected:
		//void exec();

	private: 
		Poco::Net::ServerSocket m_serverSocket;
		Poco::Net::HTTPServer m_httpServer;

	}; //HTTPAppServer

} //TA_Base_Bus

#endif //__HTTP_APP_SERVER_H_INCLUDED__