/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: ITopicListener.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class Implementation for AbstractTopicListener
  */

#include "AbstractTopicListener.h"
#include "XmlUtility.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	/*
	* Virtual subscribe
	*/
	void AbstractTopicListener::onNotifyUpdate(const TopicUpdate& topicUpdate)
	{
		boost::shared_ptr<GENATopicData> pGenaMgs(new GENATopicData(topicUpdate.msgContent));
		if (topicUpdate.updateType == GENA_BIRTH)
		{
			onRecBirthMsg(pGenaMgs);
		}
		else if (topicUpdate.updateType == GENA_UPDATE)
		{
			onRecUpdateMsg(pGenaMgs);
		}
		else if (topicUpdate.updateType == GENA_DEATH)
		{
			onRecDeathMsg(pGenaMgs);
		}
	}

	
} //TA_Base_Bus