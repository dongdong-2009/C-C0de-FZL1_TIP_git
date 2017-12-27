/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: AbstractTopicListener.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for AbstractTopicListener
  */


#ifndef __ABSTRACT_TOPIC_LISTENER_H_INCLUDED__
#define __ABSTRACT_TOPIC_LISTENER_H_INCLUDED__

#include "ITopicListener.h"
#include "GENATopicData.h"

#include <string>

namespace TA_Base_Bus
{
	typedef boost::shared_ptr<GENATopicData> T_GenaMsgPtr;
	class AbstractTopicListener
		: public ITopicListener
	{
	public:

		/*
		* Constructor
		*/
		AbstractTopicListener(){};

		/*
		* Virtual Destructor
		*/
		virtual ~AbstractTopicListener(){};

		virtual void onRecBirthMsg(T_GenaMsgPtr pGenaMgs) = 0;

		virtual void onRecUpdateMsg(T_GenaMsgPtr pGenaMgs) = 0;

		virtual void onRecDeathMsg(T_GenaMsgPtr pGenaMgs) = 0;

	protected:

		/*
		* Virtual subscribe
		*/
		virtual void onNotifyUpdate(const TopicUpdate& topicUpdate);
	};

} //TA_Base_Bus

#endif //__ABSTRACT_TOPIC_LISTENER_H_INCLUDED__