/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/ISystemControllerData.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/10/18 11:29:58 $
  * Last modified by:  $Author: Ouyang $
  *
  * This interface contains methods to access the data
  * in the sn_system_controller table.
  *
  */

#ifndef ISYSTEMCONTROLLER_DATA
#define ISYSTEMCONTROLLER_DATA

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class ISystemControllerData : virtual public IItem
    {

    public:

        /** 
          * getKey
          *
          * Returns the snscon_id primary key for this System Controller.
          *
          * @return The key as an unsigned long.
          *
          */
        virtual unsigned long getKey() = 0;

        /** 
          * getName
          *
          * Returns the hostname of this System Controller.
          *
          * @return The hostname.
          *
          */
        virtual std::string getName() = 0;

        /** 
          * getAgentPollPeriod
          *
          * Returns the period, in seconds, between polls to an agent
          * managed by this System Controller.
          *
          * @return The poll period, in seconds.
          *
          */
        virtual unsigned long getAgentPollPeriod() = 0;

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
        virtual unsigned long getHeartbeatSendPeriod() = 0;
        virtual unsigned long getHeartbeatSendNumber() = 0;

        /**
         *  getAgentResponseTimeout
         *
         *  Returns the time an agent has to respond to a status
         *  change request
         *  
         *  @return The time in seconds an agent has to respond when it
         *          is asked to change state
         */
        virtual unsigned long getAgentResponseTimeout() =0; 

        /**
          * getServerEntityKey
          *
          * Returns the entity key of the Server entity for this System
          * Controller.
          *
          * @return The entity key.
          */
        virtual unsigned long getServerEntityKey() = 0;

        /** 
          * getGroupId
          *
          * Returns the primary key (snscgr_id) of the group that this 
          * System Controller is in, or 0 if it is not in any group.
          *
          * @return The primary key of the group.
          *
          */
        virtual unsigned long getGroupId() = 0;

        /** 
          * getGroupByName
          *
          * Returns the name of the group that this System Controller is
          * in, or an empty string if it is not in any group.
          *
          * @return The name of the group, or "".
          *
          */
        virtual std::string getGroupByName() = 0;

        /** 
          * getDebugLogDir
          *
          * Returns the directory that the System Controller should put
          * log files in.
          *
          * @return The directory.
          *
          */
        virtual std::string getDebugLogDir() = 0;

        /** 
          * getBinDir
          *
          * Returns the directory that the System Controller can find its binaries
          * 
          *
          * @return The directory.
          *
          * @exception DatabaseException
          * @exception DataException
          */
        virtual std::string getBinDir() = 0;

        virtual ~ISystemControllerData() {};
    };

    typedef boost::shared_ptr<ISystemControllerData> ISystemControllerDataPtr;

} // namespace TA_Base_Core

#endif // ISYSTEMCONTROLLER_DATA
