/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/ManagedProcessData.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is an implementation of the IManagedProcessData
  * interface. It provides access to the data stored in a single
  * entry in the sn_managed_process table.
  *
  */

#ifndef MANAGEDPROCESSDATA_H
#define MANAGEDPROCESSDATA_H

#include "ManagedProcessData.h"
#include "IManagedProcessData.h"
#include "ManagedProcessHelper.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Core
{
    class ManagedProcessData : public IManagedProcessData
    {

    public:

        ManagedProcessData(unsigned long systemControllerKey,
                             unsigned long managedProcessKey);

        virtual ~ManagedProcessData();

        /** 
          * getSystemControllerKey
          *
          * Returns the key of the System Controller for this entry
          * in the sn_managed_process table (ie the sncon_id column).
          *
          * @return The System Controller's key.
          *
          */
        virtual unsigned long getSystemControllerKey();

        /** 
          * getManagedProcessKey
          *
          * Returns the entity key of the managed process for this entry
          * in the sn_managed_process table.
          *
          * @return The managed process's entity key.
          *
          */
        virtual unsigned long getManagedProcessKey();

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
        virtual EOperationMode getDefaultOperatingMode();

		/** 
          * isStandbyProcess
          *
          * Return true if the managed process is standby process
          *
          */
		virtual bool isStandbyProcess();

		/** 
          * hasStandbyProcess
          *
          * Return true if the managed process has standby process
          *
          */
		virtual bool hasStandbyProcess();

		/** 
          * getMonitorGroupName
          *
          * Return the group name the managed process pertains to
          *
          */
		virtual std::string getMonitorGroupName();

		/** 
          * getStartLag
          *
          * Return the start lag of the managed process
          *
          */
		virtual int getStartLag();

    private:
        ManagedProcessHelper* m_helper;
    };

} // namespace TA_Base_Core

#endif // MANAGEDPROCESSDATA_H

