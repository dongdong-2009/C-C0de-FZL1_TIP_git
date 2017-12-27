/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/IManagedProcessData.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface contains methods to access an entry
  * in the sn_managed_process table.
  *
  */

#ifndef IMANAGEDPROCESSDATA_H
#define IMANAGEDPROCESSDATA_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace TA_Base_Core
{
    class IManagedProcessData
    {

    public:

        /** 
          * getSystemControllerKey
          *
          * Returns the key of the System Controller for this entry
          * in the sn_managed_process table (ie the sncon_id column).
          *
          * @return The System Controller's key.
          *
          */
        virtual unsigned long getSystemControllerKey() = 0;

        /** 
          * getManagedProcessKey
          *
          * Returns the entity key of the managed process for this entry
          * in the sn_managed_process table.
          *
          * @return The managed process's entity key.
          *
          */
        virtual unsigned long getManagedProcessKey() = 0;

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
        virtual EOperationMode getDefaultOperatingMode() = 0;

		/** 
          * isStandbyProcess
          *
          * Return true if the managed process is standby process
          *
          */
		virtual bool isStandbyProcess() = 0;

		/** 
          * hasStandbyProcess
          *
          * Return true if the managed process has standby process
          *
          */
		virtual bool hasStandbyProcess() = 0;

		/** 
          * getMonitorGroupName
          *
          * Return the group name the managed process pertains to
          *
          */
		virtual std::string getMonitorGroupName() = 0;

		/** 
          * getStartLag
          *
          * Return the start lag of the managed process
          *
          */
		virtual int getStartLag() = 0;
    };
    
    typedef std::vector<IManagedProcessData*> IManagedProcessDataList;
    typedef boost::shared_ptr<IManagedProcessData> IManagedProcessDataPtr;
    typedef std::vector<IManagedProcessDataPtr> IManagedProcessDataPtrList;
} // namespace TA_Base_Core

#endif // IMANAGEDPROCESSDATA_H
