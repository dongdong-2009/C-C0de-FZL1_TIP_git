
#include "GenaSubscriber.h"
#include "GenaCallbackProccessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "SIDNotFoundException.h"
#include "NoActiveConException.h"

namespace TA_Base_Bus
{
	GenaSubscriber::GenaSubscriber(const std::string& strTopic, const std::string& uri,
		const std::string& notification, const T_TopicFilterSet& filterSet, 
		GenaClientParams clientParams, GenaCallbackProccessor* pCallBackProcessor, ConnectionFactoryPtr pConnectionMgr)
		: m_strTopic(strTopic)
		, m_subscribedUri(uri)
		, m_filterSet(filterSet)
		, m_strSID("")
		, m_clientParammeter(clientParams)
		, m_pCallbackProc(pCallBackProcessor)
		, m_notification(notification)
		, m_lastSubscribeTime(0)
		, m_requestedStatus(InitialSubscribe)
		, m_currentStatus(ResetSubscribe)
	{
		FUNCTION_ENTRY("Constructor");

		m_pServer = new ServerProxy(pConnectionMgr);
		
		FUNCTION_EXIT;
	}

	GenaSubscriber::GenaSubscriber(const std::string& uri,
		const std::string& notification, GenaClientParams clientParams, GenaCallbackProccessor* pCallBackProcessor,
		ConnectionFactoryPtr pConnectionMgr)
		: m_strTopic(uri)
		, m_subscribedUri(uri)
		, m_strSID("")
		, m_clientParammeter(clientParams)
		, m_pCallbackProc(pCallBackProcessor)
		, m_notification(notification)
		, m_requestedStatus(InitialSubscribe)
		, m_currentStatus(ResetSubscribe)
	{
		FUNCTION_ENTRY("Constructor");

		m_pServer = new ServerProxy(pConnectionMgr);

		FUNCTION_EXIT;
	}

	GenaSubscriber::~GenaSubscriber()
	{
		FUNCTION_ENTRY("Destructor");
		// Unsubscribe here
		unsubscribe();

		FUNCTION_EXIT;
	}

	void GenaSubscriber::subscribe()
	{
		FUNCTION_ENTRY("subscribe");
		try
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Subscribe for topic URL: %s", m_subscribedUri.c_str());

			GENASubscribeMethodPtr objRequest( new GENASubscribeMethod(m_subscribedUri));
			objRequest->setNotifyType(m_notification);
			objRequest->setCallBackAddress(m_clientParammeter.callbackInfo.serverName);
			objRequest->setTimeOut(m_clientParammeter.callbackInfo.timeout);

			m_strSID = m_pServer->subscribe(objRequest);
			m_pCallbackProc->resetTopicSync(m_subscribedUri);
			m_lastSubscribeTime = time(0);
		}
		catch (const TA_Base_Core::NoActiveConException& e)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "subscribe, NoActiveConnection Exception: %s", e.what());	
			throw e;
		}
		catch(...) //Todo: Add more detailed exception catch
		{	
			throw;
		}

		if (!m_strSID.empty())
		{
			m_currentStatus = m_requestedStatus;
			m_requestedStatus = RenewSubscribe;
		}		 
		FUNCTION_EXIT;
	}

	void GenaSubscriber::unsubscribe()
	{
		FUNCTION_ENTRY("unsubscribe");
		try
		{	
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unsubscribe for topic URL: %s with SID: %s", m_subscribedUri.c_str(), m_strSID.c_str());
			GENAUnsubscribeMethodPtr objReqest(new GENAUnsubscribeMethod(m_subscribedUri));
			objReqest->setSID(m_strSID);

			m_pServer->unsubscribe(objReqest);
		}
		catch(const TA_Base_Core::SIDNotFoundException& sidEx)
		{	// Ignore error as SID is already not existing in the server
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unsubscirbe Error: SID %s not found on server.", m_strSID.c_str());
		}
		catch(...)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unsubscirbe Error: SID %s unknown exception on server.", m_strSID.c_str());
		}
		m_currentStatus = m_requestedStatus;
		// no need actually as we will be remove from the subscriber management
		FUNCTION_EXIT;
	}

	void GenaSubscriber::resubscribe()
	{
		FUNCTION_ENTRY("resubscribe");
		try
		{
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Resubscribe for topic URL: %s SID: %s", m_subscribedUri.c_str(), m_strSID.c_str());
			GENARenewSubscribeMethodPtr objRequest(new GENARenewSubscribeMethod(m_subscribedUri));
			objRequest->setSID(m_strSID);

			m_pServer->resubscribe(objRequest);
			m_lastSubscribeTime = time(0); // update the timestamp for the last subscription
		}
		catch(const TA_Base_Core::SIDNotFoundException& sidEx)
		{	// Ignore error as SID is already not existing in the server
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unsubscirbe Error: SID %s not found on server.", m_strSID.c_str());
			m_requestedStatus = InitialSubscribe;
			return;
		}
		catch(...)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unsubscirbe Error: SID %s unknown exception on server.", m_strSID.c_str());
			m_requestedStatus = InitialSubscribe;
			return;
		}
		
		m_currentStatus = m_requestedStatus;
		FUNCTION_EXIT;
	}

	void GenaSubscriber::resetSubscribe()
	{
		FUNCTION_ENTRY("resetSubscribe");	
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ResetSubscribe for topic URL: %s", m_subscribedUri.c_str());
		unsubscribe();
		subscribe();
	
		FUNCTION_EXIT;
	}

	std::string GenaSubscriber::getSID()
	{
		FUNCTION_ENTRY("getSID");
		FUNCTION_EXIT;
		return m_strSID;
	}

	std::string GenaSubscriber::getTopic()
	{
		FUNCTION_ENTRY("getTopic");
		FUNCTION_EXIT;
		return m_strTopic;
	}

	std::string GenaSubscriber::getUri()
	{
		FUNCTION_ENTRY("getUri");
		FUNCTION_EXIT;
		return m_subscribedUri;
	}

	T_TopicFilterSet GenaSubscriber::getFilterSet()
	{
		FUNCTION_ENTRY("getFilterSet");
		FUNCTION_EXIT;
		return m_filterSet;
	}

	void GenaSubscriber::processStateCheck()
	{
		try
		{
			if (m_requestedStatus == m_currentStatus 
				&& m_requestedStatus != InitialSubscribe 
				&& m_requestedStatus != RenewSubscribe)
				return;

			switch (m_requestedStatus)
			{
			case InitialSubscribe:
				subscribe();
				break;
			case RenewSubscribe:
				{
					time_t now;
					time(&now);
					double milliseconds = difftime(now, m_lastSubscribeTime)*1000;
					if (milliseconds >= m_clientParammeter.nResubscribeInterval) // 20 seconds
					{
						resubscribe();
					}
				}
				break;
			case UnSubscribe:
				unsubscribe();
				break;
			case StopSubscribe:
				unsubscribe();
				break;
			case ResetSubscribe:
				resetSubscribe();
				break;
			}
		}
		catch(...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "unknown error occured while trying to peform state update.");
			m_requestedStatus = InitialSubscribe;
		}
		
	}
}
