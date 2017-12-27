/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/AuditMessageUtility.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Common utility class to prepare the DescriptionParameters collection before
  * calling bus/scada/common_library/AuditMessageUtility::logAuditMessage.
  *
  * The DescriptionParameters consists of NamedPairs for ReportName and LocationName, and
  * this is the same for all audit messages from the EquipmentStatusViewer. The entity key 
  * of the ESV application itself is stored in the static s_entityKey; this is then
  * available to all instances of this class, so we can create an instance just when we
  * need it and avoid the Singleton pattern.
  *
  */

#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "AuditMessageUtility.h"
#include "bus/scada/common_library/src/AuditMessageUtility.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{
    TA_Base_Core::PrimitiveWrapper<unsigned long> CAuditMessageUtility::s_entityKey;

	CAuditMessageUtility::CAuditMessageUtility()
	{		
	}

	CAuditMessageUtility::~CAuditMessageUtility()
	{
	}


    void CAuditMessageUtility::setEntityKey(unsigned long entityKey)
    {
        s_entityKey.setValue(entityKey);
    }


	void CAuditMessageUtility::LogAuditMessage(std::string &aReportName, 
												std::string &aLocationName, 
												const MessageType &aMessageType)
	{
	
		try
		{				
            unsigned long entityKey = s_entityKey.getValue();

			DescriptionParameters descriptionParameters;
			NameValuePair pair1("ReportName", aReportName);
			NameValuePair pair2("LocationName", aLocationName);

			descriptionParameters.push_back(&pair1);
			descriptionParameters.push_back(&pair2);

            TA_Base_Bus::AuditMessageUtility auditMsgUtil(ESVAudit::Context);

            auditMsgUtil.logAuditMessageWithOperatorId( aMessageType, descriptionParameters, entityKey);

        } 	
		catch(...)
		{
			// failed so just ignore
            LOG(SourceInfo, 
                TA_Base_Core::DebugUtil::ExceptionCatch, 
                "Unknown", 
                "Caught in LogAuditMessage");
		}
	}

} // TA_Base_App
