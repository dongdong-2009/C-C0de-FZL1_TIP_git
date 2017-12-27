/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/AuditMessageUtility.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/04/15 19:15:46 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * This class provides utility functions for logging Audit Events.
  *
  */

#ifndef AUDITMESSAGEUTILITY_H
#define AUDITMESSAGEUTILITY_H

#ifdef __WIN32__
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

#include <string>
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"


namespace TA_Base_Bus
{
	class AuditMessageUtility  
	{
	public:
		AuditMessageUtility(const TA_Base_Core::MessageContext& messageContext);
		virtual ~AuditMessageUtility();

		/**
		 *	logAuditMessage()
		 *
		 *	Logs an audit message with the given type, parameters 
		 *  and entity key
		 *
		 *	@param aMessageType	Audit message type
		 *	@param descParams	Description Parameters
		 *	@param entityKey	Entity for the Audit Message
		 *
		 */
		void logAuditMessage(const TA_Base_Core::MessageType& aMessageType, 
							 TA_Base_Core::DescriptionParameters& descParams,
							 unsigned long entityKey);

		/**
		 *	logAuditMessage()
		 *
		 *	Logs an audit message with the given type, parameters 
		 *  and entity key. Adds an "OperatorID" description param
		 *  based on the current session id
		 *
		 *	@param aMessageType	Audit message type
		 *	@param descParams	Description Parameters
		 *	@param entityKey	Entity for the Audit Message
		 *
		 */
		void logAuditMessageWithOperatorId(	const TA_Base_Core::MessageType& aMessageType, 
											TA_Base_Core::DescriptionParameters& descParams,
											unsigned long entityKey);
	protected:
		/**
		 *	getOperatorForSession()
		 *
		 *	Gets the current operator for the given session id
		 *
		 *	@param sessionId	session id of the current session
		 *	@return				operator id as a string
		 *
		 */
		std::string getOperatorForSession(const std::string& sessionId) const;

	private:
		AuditMessageUtility();

	protected:
		TA_Base_Core::AuditMessageSender*	m_auditMsgSender;
	};

} // TA_Base_App

#endif // !ifndef AUDITMESSAGEUTILITY_H
