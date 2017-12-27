
#include "GENAClientProxy.h"

#include "GENAConnectionFactory.h"
#include "GENAHttpClient.h"
#include "GENAEvent.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	GENAClientProxy::GENAClientProxy(SubscriberInfoPtr objSubInfo, ConnectionFactoryPtr pConnectionMgr)
		: m_sid(objSubInfo->subID)
		, m_hostName(objSubInfo->host)
		, m_callbackAddress(objSubInfo->callback)
		, m_pConnectionMgr(pConnectionMgr)
	{
		FUNCTION_ENTRY("Construction");
		m_pConnection = m_pConnectionMgr->createConnection(objSubInfo->callback, atol(objSubInfo->timeout.c_str()), 3);

		start();
		FUNCTION_EXIT;
	}

	GENAClientProxy::~GENAClientProxy(void)
	{
		FUNCTION_ENTRY("Destructor");
		terminateAndWait();
		//Todo: Should we delete the connection???
		FUNCTION_EXIT;
	}

// 	void GENAClientProxy::notifyUpdate(GENATopicDataPtr ptopicUpdate)
// 	{
// 		FUNCTION_ENTRY("notifyUpdate");
// 
// 		GENAEvent* pNewEv = new GENAEvent(REQ_NOTIFY);
// 		pNewEv->setNotifyType(ptopicUpdate->getUpdateType());
// 		pNewEv->setTopicURI(ptopicUpdate->getTopicName());
// 		pNewEv->setContent(ptopicUpdate->getDataStream());
// 
// 		GENARequestPtr objReq(pNewEv);
// 		insert(objReq);
		//char szTemp[20] = {0};
		//sprintf(szTemp, "%d", m_sid);
		//objReq.msgHeader.add(HEAD_SID, szTemp);

		//objReq.msgContent = update.msgContent;
		//objReq.uri = update.topicURI; // must very

 		//GENARequestPtr objReq(pNewEv);
// 		GenaResponse objRes;
// 		if (m_pConnection->sendRequest(objReq, objRes) != 200)
// 		{	//Todo: Ok
// 			
// 		}
		
// 		FUNCTION_EXIT;
// 	}

	std::string GENAClientProxy::getCallbackAddress()
	{
		FUNCTION_ENTRY("getCallbackAddress");
		FUNCTION_EXIT;
		return m_callbackAddress;
	}

	std::string GENAClientProxy::getSID()
	{
		FUNCTION_ENTRY("getSID");
		FUNCTION_EXIT;
		return m_sid;
	}

	void GENAClientProxy::processEvent( boost::shared_ptr<GENATopicData> ptopicUpdate )
	{
		FUNCTION_ENTRY("processEvent");
		
		GENAEventPtr pEvent;
		if (ptopicUpdate->getUpdateType() == GENA_BIRTH)
			pEvent.reset(new GENABirthEvent(ptopicUpdate->getTopicURI()));
		else if (ptopicUpdate->getUpdateType() == GENA_DEATH)
			pEvent.reset(new GENADeathEvent(ptopicUpdate->getTopicURI()));
		else if (ptopicUpdate->getUpdateType() == GENA_UPDATE)
			pEvent.reset(new GENAUpdateEvent(ptopicUpdate->getTopicURI()));

		pEvent->setSID(m_sid);
		pEvent->setContent(ptopicUpdate->getDataStream());
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending data to SID %s", m_sid.c_str());

		GenaResponse objRes;
		if (m_pConnection->sendRequest(pEvent, objRes) != GENA_OK)
		{	
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to process request for topic: %s and SID %s", ptopicUpdate->getTopicURI().c_str(), m_sid.c_str());
		}

		FUNCTION_EXIT;
	}

} //TA_Base_Bus

