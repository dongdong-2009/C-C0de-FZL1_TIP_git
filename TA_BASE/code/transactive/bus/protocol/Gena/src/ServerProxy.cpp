
#include "ServerProxy.h"
#include "GENAConnectionFactory.h"
#include "NoActiveConException.h"
#include "SIDNotFoundException.h"
#include "Poco/Net/HTTPResponse.h"
#include "core/utilities/src/DebugUtil.h"
#include <ostream>

using Poco::Net::HTTPResponse;
using namespace TA_Base_Core;
namespace TA_Base_Bus
{
	ServerProxy::ServerProxy(ConnectionFactoryPtr pConnectionMgr)
		: m_pConnectionMgr(pConnectionMgr)
	{
		FUNCTION_ENTRY("Constructor");
		// For now just create the connection
		try
		{
			m_pConnection = m_pConnectionMgr->getActiveServer();
		}
		catch(const TA_Base_Core::NoActiveConException& noCon)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to find active server.");
		}
		catch(...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred tying to retrieve active server.");
		}

		FUNCTION_EXIT;
	}

	ServerProxy::~ServerProxy(void)
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
	}	

	std::string ServerProxy::subscribe(GENASubscribeMethodPtr objMethod)
	{
		FUNCTION_ENTRY("subscribe");
		TA_ASSERT(m_pConnection.get() != 0, "Please configure active server.");

 		unsigned long ulRetVal = 0;
		std::string sid="";
		GenaResponse objRes;
		try
		{
			GENARequestStatus retVal =  _sendRequestInternal(objMethod, objRes); 
			if (retVal == GENA_OK) //HTTP_OK
			{
				sid = objRes.msgHeader.get(HEAD_SID);
				//ulRetVal = atol(strSID.c_str());
				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Subscribe successful with new SID = %s, type=%s", sid.c_str(),objMethod->getNotifyType().c_str() );
			}
			else
			{
				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to subscribe! error code = %d, type=%s", objRes.status,objMethod->getNotifyType().c_str() );
			}
		}
		catch (const NoActiveConException& e)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "subscribe, NoActiveConnection Exception: %s", e.what());	
			throw e;
		}
		catch (...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "subscribe, Unknown exception occured!");
			throw;
		}
		
		FUNCTION_EXIT;
		return sid;
	}

	void ServerProxy::resubscribe(GENARenewSubscribeMethodPtr objMethod) //const std::string& uri, const unsigned long sid)
	{
		FUNCTION_ENTRY("subscribe");
		TA_ASSERT(m_pConnection.get() != 0, "Please configure active server.");
		
		try
		{
			GenaResponse objRes;
			GENARequestStatus retVal = _sendRequestInternal(objMethod, objRes);

			if (retVal == GENA_SID_NOT_FOUND)
			{
				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError,  "ReSubscribe ID not found on server: SID = %s, type=%s", objMethod->getSID().c_str(),objMethod->getNotifyType().c_str());
				throw SIDNotFoundException("Subscriber ID not found");
			} 
			else if (retVal != GENA_OK)
			{
				LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to resubscribe! error code = %d SID = %s, type=%s", objRes.status,objMethod->getSID().c_str(),objMethod->getNotifyType().c_str());
			}
		}
		catch (const NoActiveConException& e)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Resubscribe, NoActiveConnection Exception: %s", e.what());	
			throw e;
		}
		catch (...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Resubscribe, Unknown exception occured!");
			throw;
		}
		FUNCTION_EXIT;
	}

	// unSubscribe should not be allowed to switch to other connection
	void ServerProxy::unsubscribe(GENAUnsubscribeMethodPtr objMethod) //const std::string& uri, const unsigned long& ulSID)
	{
		FUNCTION_ENTRY("unsubscribe");
		TA_ASSERT(m_pConnection.get() != 0, "Please configure active server.");

		GenaResponse objRes;
		m_pConnection->sendRequest(objMethod, objRes);

		if (objRes.status != GENA_OK) //HTTP_OK
		{
			LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to unsubscribe! error code = %d SID = %s, type=%s", objRes.status, objMethod->getSID().c_str(),objMethod->getNotifyType().c_str());
		}
		else
		{
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,  "unsubscribe successful, SID = %s, type=%s", objMethod->getSID().c_str(),objMethod->getNotifyType().c_str());
		}

		FUNCTION_EXIT;
	}

	GENARequestStatus ServerProxy::_sendRequestInternal(GENARequestPtr pRequest, GenaResponse& objResponse)
	{
		FUNCTION_ENTRY("_sendRequestInternal");

 		GENARequestStatus retVal = GENA_UNKNOWN_ERROR;
		try
		{
			retVal = m_pConnection->sendRequest(pRequest, objResponse);
		}
		catch (const NoActiveConException& e)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "sendRequest, NoActiveConnection Exception: %s", e.what());	
			throw e;
		}
		catch (...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "sendRequest, Unknown exception occured!");
			throw;
		}

		FUNCTION_EXIT;
		return retVal;
	}
	
} //TA_Base_Bus
