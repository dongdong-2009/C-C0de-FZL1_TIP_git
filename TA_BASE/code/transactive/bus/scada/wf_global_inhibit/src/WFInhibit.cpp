/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/TA_BASE/transactive/bus/scada/wf_global_inhibit/src/WFInhibit.cpp $
  * @author:   Liu Yong
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/07/03 09:46:47 $
  * Last mofified by:  $Author: Ouyang $
  *
  * WFInhibit class implimentation. See header file for class descrption
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786 4503)
#endif // defined (_MSC_VER)


#include "bus/scada/wf_global_inhibit/src/WFInhibit.h"
#include "bus/scada/wf_global_inhibit/src/WFGlobalInhibitState.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/utilities/src/TAAssert.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/NameValuePair.h"

#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/wf_global_inhibit/src/WFDefs.h"

#include <sstream>
#include <string>
#include <iomanip> 
#include <algorithm>


namespace TA_Base_Bus
{
	WFInhibit::WFInhibit(std::string strAgentName)
	:
	m_AgentName(strAgentName),
	m_rightsLibrary(NULL)

	{
		FUNCTION_ENTRY("WFInhibit");		

        //TA_Base_Bus::RightsLibraryFactory rlFactory;
    	m_rightsLibrary = ScadaUtilities::getInstance().getRightLibrary();
		// activate the ServantBase of this class
		std::string strCorbaName = m_AgentName + TA_Base_Bus::WF_GLOBAL_INHIBIT_INTERFACE;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "WF CORBA Interface Name is %s", strCorbaName.c_str());

		activateServantWithName ( strCorbaName);

		FUNCTION_EXIT;
	}


	WFInhibit::~WFInhibit()
	{
		FUNCTION_ENTRY("~WFInhibit");
		deactivateServant();
		FUNCTION_EXIT;
	}

	//Will add the right check in the future.
    bool WFInhibit::isWFGlobaInhibitPermitted(const std::string& sessionID)
    {
        FUNCTION_ENTRY("isWFGlobaInhibitPermitted");

		try
		{
			//need to change for the action and resource.***
			unsigned long entityKey = WFGlobalInhibitState::getInstance()->getGlobalInhibitKey();
			
			if (0 != entityKey)
			{
				/*
				TA_Base_Core::IResource* r = 
					TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( entityKey );
				unsigned long resourceID = r->getKey();
				delete r;

				unsigned long actionKey = TA_Base_Bus::aca_WF_GLOBAL_INHIBIT;
				TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
				std::string reason;
				
				bool response = m_rightsLibrary->isActionPermittedOnResource( sessionID, resourceID, 
					actionKey, reason, decisionModule );

				FUNCTION_EXIT;
				return response;
				*/
			}
		}
		catch(...)
		{
			throw TA_Base_Core::AccessDeniedException("Rights could not be determined for WF global inhibit.");
		}
		return true;
    }


  //IWFGlobalInhibitCorbaDef methods:

	CORBA::Boolean WFInhibit::getWFGlobalInhibit()
	{

		FUNCTION_ENTRY("getWFGlobalInhibit");

		checkOperationMode();
		bool state;
		bool bRet = WFGlobalInhibitState::getInstance()->getGlobalInhibitState(state);

		if (false == bRet)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"WFGlobalInhibit configuration error");			

			throw TA_Base_Core::DatabaseErrorException("WFGlobalInhibit configuration error");
		}
		
		//for logging:
		std::string tOrf = "true";
		if(!state) tOrf = "false";

        //do not think we need to log at the info level, change to log at the debug level only
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"getWFGlobalInhibit() returning %s", tOrf.c_str());

		FUNCTION_EXIT;

		return CORBA::Boolean(state);
		

	}

	void WFInhibit::setWFGlobalInhibit(CORBA::Boolean applied, const char* sessionID)
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("setWFGlobalInhibit");

		checkOperationMode();

		//for logging:
		std::string tOrf = "true";
		if(!applied) tOrf = "false";

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"setWFGlobalInhibit(%s, %s) called", tOrf.c_str(), sessionID);


		if(0 == sessionID)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "0 == sessionID");
			FUNCTION_EXIT;
			throw TA_Base_Core::AccessDeniedException("0 == sessionID");
			
		}

		//see if the requested state is already set
		bool currentState;
		bool bRet = WFGlobalInhibitState::getInstance()->getGlobalInhibitState(currentState);

		if(true == bRet )
		{
			if (CORBA::Boolean(currentState) == applied)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "requested state is already applied");
				FUNCTION_EXIT;
				return;
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "can not get current inhibit state");
			FUNCTION_EXIT;
			return;
		}

		//this is a critical write so the call asserts if it fails
		WFGlobalInhibitState::getInstance()->setGlobalInhibitState(applied);
		
		FUNCTION_EXIT;
	}

	
	CORBA::ULong WFInhibit::getWFGlobalInhibitKey()
	{
		FUNCTION_ENTRY("getWFGlobalInhibitKey");

		checkOperationMode();

		unsigned long key =  WFGlobalInhibitState::getInstance()->getGlobalInhibitKey();

		if(0 == key)
		{
			throw TA_Base_Core::DatabaseErrorException("Could not retrieve the key for this datapoint");
		}

		FUNCTION_EXIT;
		return key;
	}

	void WFInhibit::verifyPassword(const char* sessionId, const char* password)
	{
		FUNCTION_ENTRY("verifyPassword");
		checkOperationMode();
		std::string strPassword(password);

		unsigned long operatorKey = TA_Base_Core::SessionAccessFactory::getInstance().getOperatorKey(sessionId);
		if (0 == operatorKey )
		{
            throw TA_Base_Bus::VerifyPasswordException("Invalid Session");
		}
		
		TA_Base_Core::IOperator* newOp = TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(operatorKey, true);
		
		if (NULL == newOp)
		{
            throw TA_Base_Bus::VerifyPasswordException("Invalid User");
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"verify password for operator %s in the session %s",newOp->getName().c_str(),sessionId );
		
		std::string origin_Password = newOp->getName();
		std::string encrypted_Password;
		origin_Password +="/";
		origin_Password += strPassword;

		int rtnCode= AuthenticationLibrary::GetHash(origin_Password,encrypted_Password);		
		
		std::string  strOpertorPassword = newOp->getPassword();
		
        // Check the password - throw an exception if it does not match
        if (0 != strOpertorPassword.compare(encrypted_Password) )
        {
            throw TA_Base_Bus::VerifyPasswordException("The password does not match");
        }

		FUNCTION_EXIT;
		
	}
	
	void WFInhibit::checkOperationMode()
	{
		TA_Base_Bus::GenericAgent::ensureControlMode();
	}
} //namespace TA_Base_Bus



