/**
  * The source code in this file is the property of
  * STE and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmMessageQueue.h $
  * @author:  LuoHuiRong
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 09:49:21 $
  * Last modified by:  $Author: grace.koh $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmMessageQueue.h
//  Provide a asynchronous mechanism to submit alarm to shared memory pool
//  Created on:      30-Oct-2009 04:17:11 PM
///////////////////////////////////////////////////////////
 

#if !defined(AFX_ALARMMESSAGEQUEUE_H__2B60680F_3B6F_4C7F_A583_1866A0CA89B4__INCLUDED_)
#define AFX_ALARMMESSAGEQUEUE_H__2B60680F_3B6F_4C7F_A583_1866A0CA89B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/QueueProcessor.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "IAlarmUpdate.h"

namespace TA_Base_Bus
{
	class AlarmMessageQueue : public TA_Base_Core::QueueProcessor<TA_Base_Core::AlarmMessageCorbaDef>
	{
	public:
		AlarmMessageQueue(IAlarmUpdate& callback, std::string entityName);
		virtual ~AlarmMessageQueue();
		virtual void processEvent( boost::shared_ptr<TA_Base_Core::AlarmMessageCorbaDef> newEvent );
	private:
		IAlarmUpdate&						m_callback;
		std::string m_entityName;
	};
}

#endif // !defined(AFX_ALARMMESSAGEQUEUE_H__2B60680F_3B6F_4C7F_A583_1866A0CA89B4__INCLUDED_)
