
#include "HTTPAppRequestHandlerFactory.h"
#include "HTTPAppRequestHandler.h"


namespace TA_Base_Bus
{
	HTTPAppRequestHandlerFactory::HTTPAppRequestHandlerFactory(IRequestProccessor* pRequestListener)
		: m_pRequestListener(pRequestListener)
	{

	}

	HTTPRequestHandler* HTTPAppRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request)
	{
		HTTPRequestHandler* pRetVal = 0;

		pRetVal = new HTTPAppRequestHandler(m_pRequestListener);

		return pRetVal;
	}

} //TA_Base_Bus