/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IRTUEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IRTUEntityData provides read-only access to RTU entity data.
  */

#ifndef IRTU_ENTITY_DATA_H
#define IRTU_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IRTUEntityData : public virtual IEntityData
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
         * getInServiceState
         *
         * Returns the InService state of RTU entity
         *
         * @return The boolean value to indicate if this entity is in service,
		 *		   TRUE if in service and FALSE otherwise
         */
        virtual bool getInServiceState() = 0;

        /**
         * getScanTimeMSecs
         *
         * Returns the scan time in msecs for RTU entity
         *
         * @return The integer value presenting the scan time in msecs
         */
        virtual int getScanTimeMSecs() = 0;

        /**
         * getRTUStatusTableStartAddress
         *
         * Returns the start address of the Status Table for RTU entity
         *
         * @return The starting address of the RTU Status table
         */
        virtual unsigned long getRTUStatusTableStartAddress() = 0;

        /**
         * getRTUStatusTableEndAddress
         *
         * Returns the end address of the Status Table for RTU entity
         *
         * @return The ending address of the RTU Status table
         */
        virtual unsigned long getRTUStatusTableEndAddress() = 0;

        /**
         * getRTUStatusTableServiceNumber
         *
         * Returns the service number of the Status Table for RTU entity
         *
         * @return The service number of the RTU Status table
         */
        virtual const std::string getRTUStatusTableServiceNumber() = 0;

        /**
         * getRTUCommandTableStartAddress
         *
         * Returns the start address of the Command Table for RTU entity
         *
         * @return The starting address of the RTU Command table
         */
        virtual unsigned long getRTUCommandTableStartAddress() = 0;

        /**
         * getRTUCommandTableEndAddress
         *
         * Returns the end address of the Command Table for RTU entity
         *
         * @return The ending address of the RTU Command table
         */
        virtual unsigned long getRTUCommandTableEndAddress() = 0;

        /**
         * getRTUCommandTableServiceNumber
         *
         * Returns the service number of the Command Table for RTU entity
         *
         * @return The service number of the RTU Command table
         */
        virtual const std::string getRTUCommandTableServiceNumber() = 0;

        /**
         * getRTUPollingTableStartAddress
         *
         * Returns the start address of the Polling Table for RTU entity
         *
         * @return The starting address of the RTU Polling table
         */
        virtual unsigned long getRTUPollingTableStartAddress() = 0;

        /**
         * getRTUPollingTableEndAddress
         *
         * Returns the end address of the Polling Table for RTU entity
         *
         * @return The ending address of the RTU Polling table
         */
        virtual unsigned long getRTUPollingTableEndAddress() = 0;

        /**
         * getRTUPollingTableServiceNumber
         *
         * Returns the service number of the Polling Table for RTU entity
         *
         * @return The service number of the RTU Polling table
         */
        virtual const std::string getRTUPollingTableServiceNumber() = 0;

        /**
         * getTcpNoDelay
         *
         * Returns whether to set the TCP_NODELAY socket option for the RTU entity
         *
         * @return whether to set the TCP_NODELAY socket option
         */
        virtual bool getTcpNoDelay() = 0;
        
	};

} //close namespace TA_Base_Core

#endif

