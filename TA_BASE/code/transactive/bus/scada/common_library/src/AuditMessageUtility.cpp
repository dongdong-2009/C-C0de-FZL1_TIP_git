/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/AuditMessageUtility.cpp $
  * @author:  Rod Rolirad
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/04/15 19:15:46 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * This class provides utility functions for logging Audit Events.
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif // defined _MSC_VER

#include "bus/scada/common_library/src/AuditMessageUtility.h"
#include "core/utilities/src/RunParams.h"

#if defined(_MSC_VER)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif // defined _MSC_VER

namespace TA_Base_Bus
{

	AuditMessageUtility::AuditMessageUtility(const TA_Base_Core::MessageContext& messageContext)
		: m_auditMsgSender (NULL)
	{		
		m_auditMsgSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(messageContext);	
	}

	AuditMessageUtility::~AuditMessageUtility()
	{	
		if (m_auditMsgSender != NULL)
		{
			delete m_auditMsgSender;
			m_auditMsgSender = NULL;
		}
	}


	void AuditMessageUtility::logAuditMessage(const TA_Base_Core::MessageType &aMessageType,
											  TA_Base_Core::DescriptionParameters& descParams,
											  unsigned long entityKey)
	{
		try
		{
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			
			m_auditMsgSender->sendAuditMessage(							
					aMessageType,				// message type
					entityKey,					// entity id
					descParams,					// description parameters
					"",							// additional details
					sessionId,					// sessionID
					"",                         // AlarmID if an alarm associated with event
					"",                         // IncidentKey if incident associated with event
					"");                        // EventID of a parent event, used to link events

		} 	
		catch(...)
		{
			// failed so just ignore
		}

	}

	void AuditMessageUtility::logAuditMessageWithOperatorId(const TA_Base_Core::MessageType &aMessageType,
															TA_Base_Core::DescriptionParameters& descParams,
															unsigned long entityKey)
	{
		try
		{				
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			std::string operatorId = getOperatorForSession(sessionId);

			TA_Base_Core::DescriptionParameters descriptionParameters = descParams;
			TA_Base_Core::NameValuePair pair("OperatorID", operatorId);

			descriptionParameters.push_back(&pair);

			logAuditMessage(aMessageType, descriptionParameters, entityKey);
		} 	
		catch(...)
		{
			// failed so just ignore
		}

	}

	std::string AuditMessageUtility::getOperatorForSession(const std::string& sessionId) const
    {
        TA_Base_Core::IOperator* opr = 0;
        std::string operatorName = "";
        try
        {
            opr = TA_Base_Core::OperatorAccessFactory::getInstance().getOperatorFromSession(sessionId, false);

            if ( opr != 0 )
            {
                operatorName = opr->getName();
                delete opr;
                opr = 0;
            }
        }
        catch (...)
		{

        }

        return operatorName;
    }

} // TA_Base_App
