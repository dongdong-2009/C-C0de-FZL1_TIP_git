/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ISchedulingAgentEntityData.h $
  * @author Dirk McCormick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ISchedulingAgentEntityData provides read-only access to the
  * Scheduling Agent's entity data.
  */

#ifndef I_SCHEDULING_AGENT_ENTITY_DATA_H
#define I_SCHEDULING_AGENT_ENTITY_DATA_H

#include <vector>
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class ISchedulingAgentEntityData : public virtual IEntityData
    {
    public:

        /**
         *  getOperatorID
         *
         *  Returns the operator ID used by the Scheduling Agent.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual unsigned long getOperatorID() = 0;


        /**
         *  getPassword
         *
         *  Returns the password used by the Scheduling Agent.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual std::string getPassword() = 0;
        
        /**
         *  getPlanAgentLocationKey
         *
         *  Returns the location ID of the Plan Agent that the
         *  Scheduling Agent communicates with.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual unsigned long getPlanAgentLocationKey() = 0;

        /**
         *  getPlanAgentSubsystemKey
         *
         *  Returns the subsystem ID used of the Plan Agent that the
         *  Scheduling Agent communicates with.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual unsigned long getPlanAgentSubsystemKey() = 0;

        /**
         *  getPlanAgentEntityKey
         *
         *  Returns the entity key of the Plan Agent that the
         *  Scheduling Agent communicates with.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual unsigned long getPlanAgentEntityKey() = 0;

		virtual std::string getAssetName() = 0;

    };

} //close namespace TA_Base_Core

#endif // I_SCHEDULING_AGENT_ENTITY_DATA_H

