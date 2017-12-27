/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ITISAgentEntityData.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ITISAgentEntityData provides read-write access to TISAgent entity data.
  */

#ifndef ITISAGENT_ENTITY_DATA_H
#define ITISAGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class ITISAgentEntityData : public virtual IEntityData
    {
    public:


		 /**
         * getCurrentTimeScheduleVersion
         *
         * Returns the current ISCS Train Time Schedule Version
         *
         * @return The integer value representing the version number
         */
        virtual std::string getCurrentTimeScheduleVersion() = 0;

       /**
         * getStatusPollInterval
         *
         * Returns the period of time (in milliseconds) that the TISAgent server should
		 * wait between sending Status Request messages to the STIS server
		 *  Predefined Message Library
         *
         * @return boolean value
         */
        virtual int getStatusPollInterval() = 0;

	virtual std::string getAssetName() = 0;
 
    };

} // namespace TA_Base_Core

#endif // #ifndef ITISAGENT_ENTITY_DATA_H

