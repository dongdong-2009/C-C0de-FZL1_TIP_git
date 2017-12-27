/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/ManagedProcessHelper.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class contains the data associated with one
  * entry in the sn_managed_process table.
  *
  */

#ifndef MANAGEDPROCESSHELPER_H
#define MANAGEDPROCESSHELPER_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include <string>

namespace TA_Base_Core
{

    class ManagedProcessHelper
    {

    public:
        ManagedProcessHelper(unsigned long systemControllerKey,
                             unsigned long managedProcessKey);

        virtual ~ManagedProcessHelper();

        /** 
          * getSystemControllerKey
          *
          * Returns the key of the System Controller for this entry
          * in the sn_managed_process table (ie the sncon_id column).
          *
          * @return The System Controller's key.
          *
          */
        unsigned long getSystemControllerKey();

        /** 
          * getManagedProcessKey
          *
          * Returns the entity key of the managed process for this entry
          * in the sn_managed_process table.
          *
          * @return The managed process's entity key.
          *
          */
        unsigned long getManagedProcessKey();

        /** 
          * getDefaultOperatingMode
          *
          * Returns the default operating mode for the managed process
          * running on the System Controller in this entry in the
          * sn_managed_process table.
          *
          * @return The operation mode.
          *
          */
        EOperationMode getDefaultOperatingMode();

        /**
          * invalidate
          *
          * Mark the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */
        void invalidate();
        
		/** 
          * isStandbyProcess
          *
          * Return true if the managed process is standby process
          *
          */
		bool IsStandbyProcess();

		/** 
          * hasStandbyProcess
          *
          * Return true if the managed process has standby process
          *
          */
		bool hasStandbyProcess();

		/** 
          * getMonitorGroupName
          *
          * Return the group the managed process pertains to
          *
          */
		std::string getMonitorGroupName();

		/** 
          * getStartLag
          *
          * Return the start lag of the managed process
          *
          */
		virtual int getStartLag();
    private:
        ManagedProcessHelper( const ManagedProcessHelper& );
		ManagedProcessHelper& operator=( const ManagedProcessHelper& );

        /**
          * reload()
          *
          * This method reloads the data from the database. It is called when a get
          * method is called and the data state is not valid.
          */
        void reload();

        bool m_isValidData;

        unsigned long m_snsconId;
        unsigned long m_managedProcessPkey;
        EOperationMode m_defaultOperatingMode;
		bool m_isStandbyProcess;
		bool m_hasStandbyProcess;
		std::string m_monitorGroupName;
		int m_startLag;
    };

} // namespace TA_Base_Core

#endif // MANAGEDPROCESSHELPER_H
