/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/SystemControllerData.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/10/18 11:29:58 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class is an implementation of the ISystemControllerData
  * interface. It provides access to the data stored in a single
  * entry in the sn_system_controller table.
  *
  */

#ifndef SYSTEMCONTROLLERDATA_H
#define SYSTEMCONTROLLERDATA_H

#include "ISystemControllerData.h"
#include "SystemControllerHelper.h"

namespace TA_Base_Core
{
    class SystemControllerData : public ISystemControllerData
    {

    public:

        /** 
          * Constructor
          *
          * Constructs a new SystemControllerData object.
          *
          * @param key  The primary key of the System Controller.
          */
        SystemControllerData(unsigned long key);

        /**
          * Destructor
          */
        virtual ~SystemControllerData();
 
        /** 
          * getKey
          *
          * Returns the snscon_id primary key for this System Controller.
          *
          * @return The key as an unsigned long.
          *
          */
        virtual unsigned long getKey();

        /** 
          * getName
          *
          * Returns the hostname of this System Controller.
          *
          * @return The hostname.
          *
          */
        virtual std::string getName();

        /** 
          * getAgentPollPeriod
          *
          * Returns the period, in seconds, between polls to an agent
          * managed by this System Controller.
          *
          * @return The poll period, in seconds.
          *
          */
        virtual unsigned long getAgentPollPeriod();

        /** 
          * getHeartbeatSendPeriod
          *
          * Returns the interval, in seconds, between heartbeats that this
          * System Controller sends to peers and other System Controllers
          * monitoring the group it is in (if any).
          *
          * @return The interval between heartbeats, in seconds.
          *
          */
        virtual unsigned long getHeartbeatSendPeriod();
        virtual unsigned long getHeartbeatSendNumber();

        /**
         *  getAgentResponseTimeout
         *
         *  Returns the time an agent has to respond to a status
         *  change request
         *  
         *  @return The time in seconds an agent has to respond when it
         *          is asked to change state
         */
        virtual unsigned long getAgentResponseTimeout(); 

        /**
          * getServerEntityKey
          *
          * Returns the entity key of the Server entity for this System
          * Controller.
          *
          * @return The entity key.
          */
        virtual unsigned long getServerEntityKey();

        /** 
          * getGroupId
          *
          * Returns the primary key (snscgr_id) of the group that this 
          * System Controller is in, or 0 if it is not in any group.
          *
          * @return The primary key of the group.
          *
          */
        virtual unsigned long getGroupId();

        /** 
          * getGroupByName
          *
          * Returns the name of the group that this System Controller is
          * in, or an empty string if it is not in any group.
          *
          * @return The name of the group, or "".
          *
          */
        virtual std::string getGroupByName();

        /** 
          * getDebugLogDir
          *
          * Returns the directory that the System Controller should put
          * log files in.
          *
          * @return The directory.
          *
          */
        virtual std::string getDebugLogDir();

        /** 
          * getBinDir
          *
          * Returns the directory that the System Controller can find agent binarys
          *
          * @return The directory.
          *
          */
        virtual std::string getBinDir();

        /**
         * invalidate
         *
         * Make the data contained by this System Controller as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
        SystemControllerHelper* m_helper;
    };

} // namespace TA_Base_Core

#endif // SYSTEMCONTROLLERDATA_H
