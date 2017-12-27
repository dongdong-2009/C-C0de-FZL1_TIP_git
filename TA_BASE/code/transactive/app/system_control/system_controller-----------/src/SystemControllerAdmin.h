/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/SystemControllerAdmin.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the ISystemControllerAdminCorbaDef
  * interface, used by the System Manager.
  *
  */

#ifndef SYSTEMCONTROLLERADMIN_H
#define SYSTEMCONTROLLERADMIN_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/ISystemControllerAdminCorbaDef.h"
#include "core/corba/src/ServantBase.h"

namespace TA_Base_App
{
	class SystemController;	// Forward declaration

    class SystemControllerAdmin : public virtual POA_TA_Base_Core::ISystemControllerAdminCorbaDef,                           
                                  public virtual TA_Base_Core::ServantBase 
    {

    public:

        /**
          * Constructor
		  *
		  * @param systemController  A pointer to the parent SystemController object.
          */
        SystemControllerAdmin(SystemController* systemController);

        /**
          * Destructor
          */
        ~SystemControllerAdmin();

		/**
          * getProcessData
          *
          * Get a sequence with all the known processes for a System Controller
          */
        TA_Base_Core::ProcessDataSeq* getProcessData();

        /**
          * shutdown
          *
          * Shutdown a System Controller. The Process Monitors should sort out 
		  * between themselves how to run the processes that get dropped.
          */
        void shutdown(const char* hostname);

        /**
          * startProgram
          *
          * Start a process.
		  * 
          */
        void startProcess(const char* entityName);

        /**
          * changeAgentOperationMode
          *
          * Ask a System Controller to change the Operation mode of one of its
          * managed processes.
          * 
          */
        void changeAgentOperationMode(const char* entityName, 
                                      TA_Base_Core::EOperationMode operationMode);
        /**
          * stopProcess
          * 
          */
        void stopProcess(const char* entityName);

        /**
          * setRunParam
          *
          * Set a RunParam for a particular process
		  * 
          */
        void setRunParam(const char* entityName, const TA_Base_Core::RunParam& param);

		/**
		  * Returns the status of the agents running on this System Controller.
		  *
		  * @return AgentStatusEnum  An enum indiciating the overall status
		  *							 of the agents.
		  *
		  */
		TA_Base_Core::AgentStatusEnum getAgentsStatus();

		/**
         * notifyStandbyAgent
		 *
		 * notify standby agent to set operation mode to standby when systemcontroller on station start up
         * 
         */
		virtual void notifyStandbyAgent( const char* entityName, const char* groupName );
	private:

		/**
		  * A pointer to the parent SystemController object (needed to shutdown).
		  */
		SystemController* m_systemController;
    };

} // namespace TA_Base_App

#endif // SYSTEMCONTROLLERADMIN_H
