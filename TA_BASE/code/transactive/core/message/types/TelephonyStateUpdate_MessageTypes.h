// File: TelephonyStateUpdate_MessageTypes.h
// Created: 6/02/2007 3:35:41 PM
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 38.05, Status: Draft, Dated: 26 Jan 07
//
// * DO NOT MODIFY! *

#ifndef TELEPHONYSTATEUPDATE_MESSAGETYPES_H
#define TELEPHONYSTATEUPDATE_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace TelephonyStateUpdate
	{
		// Root context
		static MessageContext Context("StateUpdate", "TelephonyStateUpdate", StateUpdateMessage);

		// Message Types for context TelephonyStateUpdate
		static const MessageType TelephoneAgentLinkStateUpdate(Context, "10102");
		static const MessageType TelephoneAgentStatusUpdate(Context, "10103");
		static const MessageType TelephoneAgentStatusUpdateRequest(Context, "10104");
		static const MessageType TelephoneAgentFullStateUpdate(Context, "10105");
	};
};

#endif // TELEPHONYSTATEUPDATE_MESSAGETYPES_H