/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/TA_BASE/transactive/bus/scada/wf_global_inhibit/src/WFInhibit.h $
  * @author:   Liu Yong
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last mofified by:  $Author: CM $
  *
  * This class provides the following:
  * - impliments the IWFGlobalInhibitCorbaDef interface so that people can remotly get and set
  * - the WF Global inhibition state
  * 
  */
#ifndef WF_INHIBIT_H
#define WF_INHIBIT_H

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/wf_global_inhibit/IDL/src/IWFGlobalInhibitCorbaDef.h"
#include "core/corba/src/ServantBase.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <time.h>

namespace TA_Base_Bus
{
    class RightsLibrary;
    class WFInhibit
	:
	public virtual POA_TA_Base_Bus::IWFGlobalInhibitCorbaDef,
	public virtual TA_Base_Core::ServantBase
    {
    public:
        /**
          * The implimnented CTOR
          */
        WFInhibit(std::string strAgentName);
        /**
          * DTOR
		  * Deletes the instance of this class
		  * 
          */
        virtual ~WFInhibit();

        /** 
         * isWFGlobalInhibitPermitted
         *
         * Checks if the operator has rights to set WF Global inhibit.
         *
         * @param sessionID  The operator's session ID.
         *
         * @return True if WF Global inhibit is permitted, and false otherwise.
         *
         * @exception AccessDeniedException  Thrown if an error occurs while determining rights.
         */
        bool isWFGlobaInhibitPermitted(const std::string& sessionID);

	
	//IWFGlobalInhibitCorbaDef methods:

		/**
		*
		* get the WF global Inhibit state
		*
		* @throws TA_Base_Core::OperationModeException
		*/	
		virtual CORBA::Boolean getWFGlobalInhibit();

		/**
		*
		* get the pkey of the datapoint representing the WF global Inhibit state
		*
		* @throws TA_Base_Core::OperationModeException
		* @throws TA_Base_Core::DatabaseErrorException
		*/	
		virtual CORBA::ULong getWFGlobalInhibitKey();

		/**
		*
		* set the WF global Inhibit state
		*
		* @throws TA_Base_Core::OperationModeException
		* @throws TA_Base_Core::AccessDeniedException
		* @throws TA_Base_Core::DatabaseErrorException
		*/	
		virtual void setWFGlobalInhibit(CORBA::Boolean applied, const char* sessionID);


		/**
		*
		* verify password of the given session.
		*
		* @throws TA_Base_Core::AuthenticationAgentException
		* @throws TA_Base_Core::OperationModeException
		*/	
		virtual void verifyPassword(const char* sessionId, const char* password);
		
	private: //data members:

		std::string m_AgentName;
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;

		//provides a thread lock
		TA_Base_Core::ReEntrantThreadLockable m_lock;


	private: //methods:

         // Disable copy constructor and assignment operator
        WFInhibit( const WFInhibit& con);
        WFInhibit& operator=(const WFInhibit& con);
		

		 /**
          * checkOperationMode
          *
		  *
          * @throw OperationModeException if not in Control
          */
		void checkOperationMode();
		
    };
} // namespace TA_Base_Bus

#endif // !defined(WF_INHIBIT_H)