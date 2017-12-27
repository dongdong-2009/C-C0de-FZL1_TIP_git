/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENAHttpClient.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for processing the request.
  */

#ifndef __CONNECTION_H_INCLUDED__
#define __CONNECTION_H_INCLUDED__
#pragma once

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

#include "CommonDef.h"
#include "GENARequest.h"

#include <list>
#include <string>
#include <queue>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;

namespace TA_Base_Bus
{
	// Forward declaration
	class GENAConnectionFactory;

	class GENAHttpClient
	{
	public:
		/*
		* Constructor
		*/
		GENAHttpClient(const std::string& targetUri, 
			unsigned long timeOut, int maxRetry, 
			GENAConnectionFactory* pConManager);
		
		/*
		* Destructor
		*/
		~GENAHttpClient(void);

		/*
		* sendRequest
		*/
		GENARequestStatus sendRequest(GENARequestPtr request, GenaResponse& objResponse);

		/*
		* resetConnection
		*/
		void resetConnection();

		/*
		* getStatus
		*/
		ConnectionStatus getStatus();

		/*
		* getUri
		*/
		std::string getUri() { return m_targetUri; }

		/*
		* getLastResponse
		*/
		GenaResponse getLastResponse();

		/*
		* getLastResult
		*/
		GENARequestStatus getLastResult();

		/*
		* cancelRetry
		*/
		void cancelRetry();

	private:
		GENARequestStatus _convertHttpStatus(HTTPResponse::HTTPStatus status);

	private:
		int m_maxRetry;
		unsigned long m_timeOut;
		std::string m_targetUri;
		GENARequestStatus m_lastResult;
		GenaResponse m_lastResponse;
		
		GENAConnectionFactory* m_pConManager;
		HTTPClientSession* m_clientSession;
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		TA_Base_Core::Condition m_exitRetry;
		ConnectionStatus m_status;
		
	}; //Connection

	typedef boost::shared_ptr<GENAHttpClient> ConnectionPtr;
} //TA_Base_Bus


#endif //__CONNECTION_H_INCLUDED__
