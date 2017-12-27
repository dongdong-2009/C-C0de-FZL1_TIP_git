
#include "CommonDef.h"
#include "HTTPAppRequestHandler.h"
#include "GENAMethod.h"
#include "GENAEvent.h"

#include "core/utilities/src/DebugUtil.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/NameValueCollection.h"
#include "Poco/Net/HTTPResponse.h"


#include "Poco/StreamCopier.h"
#include <istream>
#include <ostream>
#include <sstream>

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::HTTPResponse;
using Poco::StreamCopier;
using Poco::Net::HTTPMessage;

namespace TA_Base_Bus
{
	HTTPAppRequestHandler::HTTPAppRequestHandler(IRequestProccessor* pRequestListener)
		: m_pRequestListener(pRequestListener)
	{
		FUNCTION_ENTRY("HTTPAppRequestHandler constructor");
		//Empty
		FUNCTION_EXIT;
	}

	void HTTPAppRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		FUNCTION_ENTRY("handleRequest");

		if (request.getChunkedTransferEncoding())
			response.setChunkedTransferEncoding(true);
		else if (request.getContentLength() !=HTTPMessage::UNKNOWN_CONTENT_LENGTH)
			response.setContentLength(request.getContentLength());

		response.setContentType(request.getContentType());



		std::string str;
		std::string body;
		//Convert the request
		GENARequestPtr pRequest = _converToGENARequest(request);
		int len=request.getContentLength();
		if (len > 0) // We will only read data if content length is specified
		{			
			std::istream& rs = request.stream();
			if (rs.good())
			{				 
				std::ostringstream st;
				std::ostream &osReq = st;
				Poco::StreamCopier::copyStream(rs,osReq,len);
				//st << rs.rdbuf();
				body = st.str(); 
				pRequest->setContent(body);
			}
		}

		std::ostringstream ostrReq;
		std::ostream &osReq = ostrReq;
		request.write(osReq);
		str = ostrReq.str();

		LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "received request from %s \n\r %s \n\r %s",request.clientAddress().toString().c_str(),str.c_str(),body.c_str());

	
		GenaResponse objResponse;

		//Process the request
		m_pRequestListener->proccessRequest(pRequest, objResponse);

		NameValueCollection::ConstIterator itr2 = objResponse.msgHeader.begin();
		for (itr2; itr2!= objResponse.msgHeader.end(); itr2++)
		{
			response.set(itr2->first, itr2->second);
		}
		response.setStatus(_convertoHTTPStatus(objResponse.status));
		response.setKeepAlive(true);
		// response to client

		std::ostringstream ostrResp;
		std::ostream & osResp = ostrResp;
		response.write(osResp);
		str = ostrResp.str();
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sent response \n\r %s",str.c_str());
		response.setContentLength(0);
		response.send();

		

		

		// Check if we need to send out Birth Messages
		if (pRequest->getMethod() == REQ_SUBSCRIBE && 
			pRequest->getHeaderValue(HEAD_SID).empty() &&
			objResponse.status == GENA_OK)
		{  // Request to send birth Messages
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Requesting for birth Messages: %s = %s", pRequest->getTopicURI().c_str(), response.get(HEAD_SID).c_str());
			m_pRequestListener->requestBirthMessage(pRequest->getTopicURI(), response.get(HEAD_SID).c_str());
		}

		FUNCTION_EXIT;
	}

	GENARequestPtr HTTPAppRequestHandler::_converToGENARequest(HTTPServerRequest& request)
	{
		FUNCTION_ENTRY("_converToGENARequest");
		GENARequestPtr pRetVal;

		if (request.getMethod() == REQ_SUBSCRIBE) 
		{
			if(request.has(HEAD_SID))
			{
				GENARenewSubscribeMethod* renewMethod= new GENARenewSubscribeMethod(request.getURI());				
				pRetVal.reset(renewMethod);
			}
			else
			{
				GENASubscribeMethod* subMethod= new GENASubscribeMethod(request.getURI());
				//subMethod->setCallBackAddress(request.getHeaderValue(HEAD_CALLBACK));
				pRetVal.reset(subMethod);
			}
			 
			 
		}
		else if (request.getMethod() == REQ_UNSUBSCRIBE)
		{
			pRetVal.reset(new GENAUnsubscribeMethod(request.getURI()));
		}
		else if (request.getMethod() == REQ_NOTIFY)
		{
			if (request.get(HEAD_NT) == GENA_BIRTH)
				pRetVal.reset(new GENABirthEvent(request.getURI()));
			else if (request.get(HEAD_NT) == GENA_DEATH)
				pRetVal.reset(new GENADeathEvent(request.getURI()));
			else if (request.get(HEAD_NT) == GENA_UPDATE)
				pRetVal.reset(new GENAUpdateEvent(request.getURI()));
		}

		pRetVal->setHost(request.getHost());
		pRetVal->setRequestHeader(request);
	
		FUNCTION_EXIT;
		return pRetVal;
	}

	Poco::Net::HTTPResponse::HTTPStatus HTTPAppRequestHandler::_convertoHTTPStatus(GENARequestStatus reqStatus)
	{
		FUNCTION_ENTRY("_convertoHTTPStatus");
		HTTPResponse::HTTPStatus retVal;

		switch (reqStatus)
		{
		case GENA_OK:
			retVal = HTTPResponse::HTTP_OK;	
			break;
		case GENA_SID_NOT_FOUND:
			retVal = HTTPResponse::HTTP_NOT_FOUND;	
			break;
		case GENA_BAD_REQUEST:
			retVal = HTTPResponse::HTTP_BAD_REQUEST;	
			break;
		case GENA_PASSIVE_MODE:
			retVal = HTTPResponse::HTTP_SERVICE_UNAVAILABLE;	
			break;
		case GENA_NETWORK_FAILURE:
			retVal = HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;	
			break;
		case GENA_UNKNOWN_ERROR:
			retVal = HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;	
			break;
		default:
			retVal = HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
			break;
		}

		FUNCTION_EXIT;
		return retVal;
	}
} //TA_Base_Bus
