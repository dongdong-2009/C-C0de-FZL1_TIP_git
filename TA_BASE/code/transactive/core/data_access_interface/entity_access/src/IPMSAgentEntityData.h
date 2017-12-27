/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IPMSAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IPMSAgentEntityData provides read-only access to PMSAgent entity data.
  */

#ifndef IPMSAGENT_ENTITY_DATA_H
#define IPMSAGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IPMSAgentEntityData : public virtual IEntityData
    {
    public:

        /**
         * getScanTimeMSecs
         *
         * Returns the scan time in msecs for PMSAgent entity
         *
         * @return The integer value presenting the scan time in msecs
         */
        virtual int getScanTimeMSecs() = 0;


        /**
         * getStationName
         *
         * Returns the name of the Station the PMSAgent entity is configured to monitor
         *
         * @return The name of the Station the PMSAgent entity is configured to monitor
         */
        virtual const std::string getStationName() = 0;

       // TD16169 ++
		/**
         * getAssetName
         *
         * Returns the asset name of the PMSAgent entity is configured to monitor
         *
         * @return The asset name of the PMSAgent entity is configured to monitor
         */
		virtual std::string getAssetName() = 0;

		/**
         * getAnalogueDataPointUpdateInterval
         *
         * Returns the analogue datapoint update interval of the PMSAgent entity is configured to monitor
         *
         * @return The analogue datapoint update interval of the PMSAgent entity is configured to monitor
         */
		virtual int getAnalogueDataPointUpdateInterval() = 0;
       // ++TD16169
    };

} //close namespace TA_Base_Core

#endif

