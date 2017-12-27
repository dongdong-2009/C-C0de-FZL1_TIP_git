/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmPublicationManager.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/alarm/src/AlarmPublicationManager.h"
#include "core/alarm/src/AlarmSender.h"

namespace TA_Base_Core
{
	// initialise statics
	TA_Base_Core::NonReEntrantThreadLockable AlarmPublicationManager::m_singletonLock;
	AlarmPublicationManager* AlarmPublicationManager::m_theClassInstance = NULL;
	
	AlarmPublicationManager::~AlarmPublicationManager()
	{
		m_theClassInstance = NULL;
	}


	AlarmPublicationManager& AlarmPublicationManager::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);

		// If guard acquired then make sure the singleton is still NULL
		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
			m_theClassInstance = new AlarmPublicationManager();
		}

		return *m_theClassInstance;
	}


	AlarmSender* AlarmPublicationManager::getAlarmSender(const TA_Base_Core::MessageContext& messageContext)
	{
        return gCreateMessageSender<AlarmSender>(messageContext.getChannelName());
	}


	AlarmPublicationManager::AlarmPublicationManager()
	{

	}


}; // namespace TA_Base_Core

