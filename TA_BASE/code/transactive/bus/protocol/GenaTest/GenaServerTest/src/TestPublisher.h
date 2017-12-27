/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: TestPublisher.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for TestPublisher
  */

#ifndef __TEST_PUBLISHER_H_INCLUDED__
#define __TEST_PUBLISHER_H_INCLUDED__

#include "bus/protocol/Gena/src/CommonDef.h"
#include "bus\protocol\Gena\src\GENATopicData.h"
#include "core/threads/src/Thread.h"
#pragma once

using namespace TA_Base_Bus;

	struct GenaTestTopic
	{
		std::string topicURI;
		std::string topic;
		std::vector<GENATopicDataPtr > births;
		std::vector<GENATopicDataPtr > updates;
		std::vector<GENATopicDataPtr > deaths;
		unsigned long updateInterval;
		unsigned int itemCount;
	};
	
	class TestPublisher
		: public TA_Base_Core::Thread
	{
	public:
		TestPublisher(GenaTestTopic objTestTopic);

		virtual ~TestPublisher();
		
		virtual void run();

		virtual void terminate();

	protected:
	private:
		bool m_bTerminate;

		GenaTestTopic m_objTestTopic;
		int m_updateInterval;
	};

#endif //__TEST_PUBLISHER_H_INCLUDED__
