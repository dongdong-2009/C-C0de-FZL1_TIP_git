
#include "GENAHttpClient.h"
#include "GENAConnectionFactory.h"
#include "SIDNotFoundException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "Poco/URI.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/NameValueCollection.h"
#include "Poco/Exception.h"
#include "Poco/StreamCopier.h"
#include <istream>
#include <ostream>
#include <sstream>

using Poco::URI;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPResponse;
using Poco::Net::NameValueCollection;
using Poco::Exception;
using Poco::StreamCopier;

namespace TA_Base_Bus
{
	GENAHttpClient::GENAHttpClient(const std::string& targetUri, unsigned long timeOut, int maxRetry, GENAConnectionFactory* pConManager)
		: m_targetUri(targetUri)
		, m_timeOut(timeOut)
		, m_maxRetry(maxRetry)
		, m_status(StatusUnknown)
		, m_pConManager(pConManager)
	{
		FUNCTION_ENTRY("Connection Constructor");

		try
		{	
			URI pUri(m_targetUri);
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Creating HTTP Client for Host: %s, Port: %d", pUri.getHost().c_str(), pUri.getPort());
			
			m_clientSession = new HTTPClientSession(pUri.getHost(), pUri.getPort());
			m_clientSession->setKeepAlive(true);			
			//enable permanent connection, connection will not be dropped by keep alive timeout
			m_clientSession->setPermanentConnection(true);
			Poco::Timespan tt(timeOut*1000);
			int sec=tt.seconds();
			int msec=tt.microseconds();
			m_clientSession->setKeepAliveTimeout(timeOut*1000*3);
		}
		catch (const Exception& exc)
		{
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s: Exception occurred while trying to create client session. error: %s", m_targetUri.c_str(), exc.displayText().c_str());
			m_status = StatusBad;
			m_pConManager->notifyStateChange(m_targetUri, StatusBad);
		}

		FUNCTION_EXIT;
	}

	GENAHttpClient::~GENAHttpClient(void)
	{
		FUNCTION_ENTRY("Connection Destructor");
		m_exitRetry.signal();
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (0 != m_clientSession)
		{
			delete m_clientSession;
			m_clientSession = 0;
		}
		FUNCTION_EXIT;
	}


	GENARequestStatus GENAHttpClient::sendRequest(GENARequestPtr objGenReq, GenaResponse& objResponse)
	{
		FUNCTION_ENTRY("sendRequest");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		GENARequestStatus nRetVal = GENA_UNKNOWN_ERROR;

		LOG4(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: Sending Request, Method: %s(%s), Topic: %s", m_targetUri.c_str(), objGenReq->getMethod().c_str(),objGenReq->getRealMethod().c_str(), objGenReq->getTopicURI().c_str());
		int nRetry = 0;
		while (nRetry < m_maxRetry)
		{
			try
			{
				// Create the request
				HTTPRequest* request = objGenReq->generateHttpRequest();
				
				// Set host header
				request->setHost(m_clientSession->getHost());
				int sizeContent=objGenReq->getContent().size();
				// Set Content header
				request->setContentLength(objGenReq->getContent().size());
				std::ostream& osMsgBody = m_clientSession->sendRequest(*request);
				if (!objGenReq->getContent().empty())
				{	// Write Message Body
					osMsgBody << objGenReq->getContent();
				}

				std::ostringstream ostrReq;
				std::ostream &osReq = ostrReq;
				request->write(osReq);
				std::string str = ostrReq.str();
				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sent Request %s: %s",str.c_str(), objGenReq->getContent().c_str());

				// Wait for response
				HTTPResponse response;
				std::istream& rs = m_clientSession->receiveResponse(response);
				int len=response.getContentLength();
				if (len> 0)
				{	// Retrieve message body if content length is not equal to zero.										 
					
					std::ostringstream ostr;
					std::ostream &osReq = ostrReq;
					Poco::StreamCopier::copyStream(rs,osReq,len);
					//st << rs.rdbuf();
					std::string body = ostr.str(); 
					objResponse.msgContent = body;
				}
				
				nRetVal = _convertHttpStatus(response.getStatus());
				objResponse.status = nRetVal;
				NameValueCollection::ConstIterator itr1 = response.begin();
				for (itr1; itr1!=response.end(); itr1++)
				{
					objResponse.msgHeader.set(itr1->first, itr1->second);
				}

				std::ostringstream ostrResp;
				std::ostream &osResp = ostrResp;
				response.write(osResp);
				str = ostrResp.str();
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "received response \n\r %s: ",str.c_str());

				 
				
				// Store the last response
				m_lastResponse = objResponse; 
				if (nRetVal == GENA_OK ||
					nRetVal == GENA_SID_NOT_FOUND)
				{	// We have successfully sent the message
					if (nRetVal == GENA_OK) {
						LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: Message sent successfully", m_targetUri.c_str());
					} else {
						LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s: Message sent successfully but SID is not found", m_targetUri.c_str());
					}
					// Set connection to good
					if (m_status != StatusGood)
					{
						m_status = StatusGood; 
						m_pConManager->notifyStateChange(m_targetUri, StatusGood);
					}
					break;
				}
				else if (nRetVal == GENA_PASSIVE_MODE)
				{
					// Set connection to good
					m_status = StatusGood; 
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s: Server is in passive mode.", m_targetUri.c_str());
					break;
				}
				
				LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "%s: Unable to send request error code: %d. Retry attemp no. %d", m_targetUri.c_str(), response.getStatus(), nRetry+1);
			}
			catch (const TA_Base_Core::SIDNotFoundException& ex)
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s: SID not found!!! We may have to re-subscribe due to failover.", m_targetUri.c_str());
				throw ex;
			}
			catch (const Exception& exc)
			{
				nRetVal = GENA_NETWORK_FAILURE;
				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s: Exception occurred while sending request. Error: %s", m_targetUri.c_str(), exc.displayText().c_str());
			}

			// Wait for cancel request before retry
			int nCancel = m_exitRetry.timedWait(1000); 
			if (nCancel != 0) 
			{ // Exit Retry has been requested get out of this loop
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: Exit Retry has been requested get out of this loop", m_targetUri.c_str());
				break; 
			}
			// We have to retry
			nRetry++;
		}

		if (nRetVal != GENA_OK 
			&& nRetVal != GENA_SID_NOT_FOUND
			&& nRetVal != GENA_PASSIVE_MODE )
		{	// Set status to bad
			if (m_status != StatusBad )
			{
				m_status = StatusBad;
				m_pConManager->notifyStateChange(m_targetUri, StatusBad);
			}
			//resetConnection();//LUHR ?
		}

		m_lastResult = nRetVal;
		FUNCTION_EXIT;
		return nRetVal;
	}

	void GENAHttpClient::resetConnection()
	{
		FUNCTION_ENTRY("resetConnection");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: Reset Connection!", m_targetUri.c_str())
		try
		{	
			if (m_clientSession != 0)
			{
				m_clientSession->reset();
				delete m_clientSession;
				m_clientSession = 0;
			}
			URI pUri(m_targetUri);			

			m_clientSession = new HTTPClientSession(pUri.getHost(), pUri.getPort());
			m_clientSession->setKeepAlive(true);
			m_clientSession->setKeepAliveTimeout(m_timeOut*1000*3);
			m_status = StatusUnknown;
		}
		catch (Poco::Exception& exc)
		{
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s: Exception occurred while trying to create client session. error: %s", m_targetUri.c_str(), exc.displayText().c_str());
		}

		FUNCTION_EXIT;
	}

	ConnectionStatus GENAHttpClient::getStatus() 
	{
		FUNCTION_ENTRY("getStatus");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		FUNCTION_EXIT;
		return m_status; 
	}

	GenaResponse GENAHttpClient::getLastResponse()
	{
		FUNCTION_ENTRY("getLastResponse");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		FUNCTION_EXIT;
		return m_lastResponse; 
	}

	GENARequestStatus GENAHttpClient::getLastResult()
	{
		FUNCTION_ENTRY("getLastResult");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		FUNCTION_EXIT;
		return m_lastResult;
	}

	void GENAHttpClient::cancelRetry()
	{
		FUNCTION_ENTRY("cancelRetry");
		m_exitRetry.signal();
		FUNCTION_EXIT;
	}

	GENARequestStatus GENAHttpClient::_convertHttpStatus(HTTPResponse::HTTPStatus status)
	{
		FUNCTION_ENTRY("_convertHttpStatus");
		GENARequestStatus retVal = GENA_UNKNOWN_ERROR;
		switch (status)
		{
		case HTTPResponse::HTTP_OK:
			retVal = GENA_OK;
			break;
		case HTTPResponse::HTTP_NOT_FOUND:
			retVal = GENA_SID_NOT_FOUND;
			break;
		case HTTPResponse::HTTP_SERVICE_UNAVAILABLE:
			retVal = GENA_PASSIVE_MODE;
			break;
		default:
			retVal = GENA_UNKNOWN_ERROR;
			break;
		}
		FUNCTION_EXIT;
		return retVal;
	}
} //TA_Base_Bus