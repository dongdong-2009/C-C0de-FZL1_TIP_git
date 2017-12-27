/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: HTTPAppRequestHandlerFactory.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the HTTPAppServer which inherits from poco library HTTPServer.
  */

#ifndef __HTTP_APP_REQUEST_HANDLER_H_INCLUDED__
#define __HTTP_APP_REQUEST_HANDLER_H_INCLUDED__

#include "GENARequest.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPResponse.h"

#include "IRequestProccessor.h"

#pragma once

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerRequest;

namespace TA_Base_Bus
{
	class HTTPAppRequestHandler
		: public HTTPRequestHandler
	{
	public:
		HTTPAppRequestHandler(IRequestProccessor* pRequestListener);

		void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);

	private:
		GENARequestPtr _converToGENARequest(HTTPServerRequest& request);
		Poco::Net::HTTPResponse::HTTPStatus _convertoHTTPStatus(GENARequestStatus reqStatus);
	private:
		IRequestProccessor* m_pRequestListener;
	};

} //TA_Base_Bus


#endif //__HTTP_APP_REQUEST_HANDLER_H_INCLUDED__