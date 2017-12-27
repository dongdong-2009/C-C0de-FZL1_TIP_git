// File: GenericAgentAudit_MessageTypes.h
// Created: 12/04/2006 4:13:31 PM
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 34.06, Status: Draft, Dated: 06 Apr 06
//
// * DO NOT MODIFY! *

#ifndef GENERICAGENTAUDIT_MESSAGETYPES_H
#define GENERICAGENTAUDIT_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace GenericAgentAudit
	{
		// Root context
		static MessageContext Context("Audit", "GenericAgentAudit", AuditMessage);

		// Message Types for context GenericAgentAudit
		static const MessageType AgentChangeToControl(Context, "30128");
		static const MessageType AgentChangeToMonitor(Context, "30129");
		static const MessageType AgentShutdown(Context, "30130");
	};
};

#endif // GENERICAGENTAUDIT_MESSAGETYPES_H