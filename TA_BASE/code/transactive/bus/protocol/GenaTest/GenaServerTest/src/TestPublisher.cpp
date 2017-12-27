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


#include "TestPublisher.h"
#include "bus/protocol/Gena/src/GENALibrary.h"

#include "core/threads/src/Thread.h"
#include <stdio.h>
#pragma once

using namespace TA_Base_Bus;

TestPublisher::TestPublisher(GenaTestTopic objTestTopic)
		: m_bTerminate(false)
		, m_objTestTopic(objTestTopic)
		, m_updateInterval(objTestTopic.updateInterval)
	{
	}

	TestPublisher::~TestPublisher() 
	{
		terminateAndWait();
	}

	void TestPublisher::run()
	{
		TA_Base_Core::Thread::sleep(m_updateInterval);
		while (!m_bTerminate)
		{			
			for (int nCtr=0; nCtr< m_objTestTopic.itemCount; nCtr++)
			{
				// Send updates
				for (int nIdx=0; nIdx<m_objTestTopic.updates.size(); nIdx++)
				{	
					GENATopicDataPtr pData = m_objTestTopic.updates[nIdx];
					GENALibrary::getInstance().publish(pData);
				
					TA_Base_Core::Thread::sleep(m_updateInterval);
				}

				// Send Death Messages
				for (int nIdx=0; nIdx<m_objTestTopic.deaths.size(); nIdx++)
				{
					GENATopicDataPtr pDeathData = m_objTestTopic.deaths[nIdx];
					GENALibrary::getInstance().publish(pDeathData);
					TA_Base_Core::Thread::sleep(m_updateInterval);

					//Find the Birth Message for this object
					for (int bIdx=0; bIdx<m_objTestTopic.births.size(); bIdx++)
					{
						GENATopicDataPtr pBirthData = m_objTestTopic.births[bIdx];
						if (pBirthData->getObjectID() == pDeathData->getObjectID())
						{
							GENALibrary::getInstance().publish(pBirthData);
							TA_Base_Core::Thread::sleep(m_updateInterval);
							break;
						}
					}
				}
			}
		}
	}

	void TestPublisher::terminate()
	{
		m_bTerminate = true;
	}
