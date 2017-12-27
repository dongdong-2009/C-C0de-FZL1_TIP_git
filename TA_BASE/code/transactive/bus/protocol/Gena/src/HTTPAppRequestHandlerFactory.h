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

#ifndef __HTTP_APP_REQUEST_HANDLER_FACTORY_H_INCLUDED__
#define __HTTP_APP_REQUEST_HANDLER_FACTORY_H_INCLUDED__

#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "IRequestProccessor.h"

#pragma once

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;

namespace TA_Base_Bus
{
	class HTTPAppRequestHandlerFactory
		: public HTTPRequestHandlerFactory
	{
	public:
		HTTPAppRequestHandlerFactory(IRequestProccessor* pRequestListener);

		HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request);
	private:
		IRequestProccessor* m_pRequestListener;
	};

} //TA_Base_Bus

#endif //__HTTP_APP_REQUEST_HANDLER_FACTORY_H_INCLUDED__