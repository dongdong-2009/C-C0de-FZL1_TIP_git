// File: SchedulingAlarms_MessageTypes.h
// Created: 12/04/2006 4:13:33 PM
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 34.06, Status: Draft, Dated: 06 Apr 06
//
// * DO NOT MODIFY! *

#ifndef SCHEDULINGALARMS_MESSAGETYPES_H
#define SCHEDULINGALARMS_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace SchedulingAlarms
	{
		// Root context
		static MessageContext Context("Alarm", "SchedulingAlarms", AlarmMessage);

		// Message Types for context SchedulingAlarms
		static const MessageType JobExecutionFailureAlarm(Context, "50053");
		static const MessageType ScheduledAlarm(Context, "50054");
		static const MessageType SchedulingAgentInitialisationFailureAlarm(Context, "50055");
		static const MessageType SchedulingAgentUnexpectedTerminationAlarm(Context, "50056");
		static const MessageType SchedulingControllerAlarm(Context, "50057");
		static const MessageType SchedulingDatabaseFailureAlarm(Context, "50058");
		static const MessageType SchedulingInvalidDataConfigurationAlarm(Context, "50059");
		static const MessageType SchedulingMonitorAlarm(Context, "50060");
	};
};

#endif // SCHEDULINGALARMS_MESSAGETYPES_H