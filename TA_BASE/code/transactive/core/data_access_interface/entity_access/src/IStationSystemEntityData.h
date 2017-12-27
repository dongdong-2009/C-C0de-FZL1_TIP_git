/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IStationSystemEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IStationSystemEntityData provides read-only access to StationSystem entity data.
  */

#ifndef ISTATIONSYSTEM_ENTITY_DATA_H
#define ISTATIONSYSTEM_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IStationSystemEntityData : public virtual IEntityData
    {
    public:

        /**
         * getSlaveID
         *
         * Returns the slave ID of the RTU entity
         *
         * @return The integer value presenting the slave ID 
         */
        virtual int getSlaveID() = 0;

        /**
         * getOutputStartAddress
         *
         * Returns the start of the output address of this Station System
         *
         * @return The start of the output address
         */
        virtual unsigned long getOutputStartAddress() = 0;


        /**
         * getOutputEndAddress
         *
         * Returns the end of the output address of this Station System
         *
         * @return The end of the output address
         */
        virtual unsigned long getOutputEndAddress() = 0;

        /**
         * getPollingStartAddress
         *
         * Returns the start of the polling address of this Station System
         *
         * @return The start of the polling address
         */
        virtual unsigned long getPollingStartAddress() = 0;


        /**
         * getPollingEndAddress
         *
         * Returns the end of the polling address of this Station System
         *
         * @return The end of the polling address
         */
        virtual unsigned long getPollingEndAddress() = 0;

        /**
         * getOutputServiceNumber
         *
         * Returns the service number for outputing data to this Station System
         *
         * @return The service number for outputing data
         */
        virtual const std::string getOutputServiceNumber() = 0;
    };

} //close namespace TA_Base_Core

#endif

