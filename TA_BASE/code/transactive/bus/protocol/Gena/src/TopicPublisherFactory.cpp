
#include "TopicPublisherFactory.h"
#include "GENAPublicationManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "GENATopicPusblisherImp.h"

namespace TA_Base_Bus
{
	TopicPublisherFactory::TopicPublisherFactory(ConnectionFactoryPtr pConnectionMgr)
		: m_pConnectionMgr(pConnectionMgr)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}

	TopicPublisherFactory::~TopicPublisherFactory(void)
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
	}

	/*   
	* createPublisher
	*/ 
	ITopicPublisher* TopicPublisherFactory::createPublisher(GENAPublicationManager* pubMgr, const std::string& topic)
	{
		FUNCTION_ENTRY("createPublisher");
		GENATopicPusblisherImp* pTopic = new GENATopicPusblisherImp(pubMgr, topic, m_pConnectionMgr);

		FUNCTION_EXIT;
		return pTopic;
	}

} //TA_Base_Bus