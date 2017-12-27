/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/IPIDSEntityData.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * IPIDSEntityData provides read only access to PIDS entity data.
  */

#ifndef IPIDS_ENTITY_DATA_H
#define IPIDS_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IPIDSEntityData : public virtual IEntityData
    {
    public:

       /**
         * getServerIPAddress
         *
         * Returns the IP address of the local PIDS server
         *
         * @return A string representing the IP address
         */
        virtual std::string getServerIPAddress() = 0;
 
	   /**
         * getMessageTimeout
         *
         * Returns the length of time (in milliseconds) the TISAgent should wait after 
		 * sending a message before it decides that a timeout condition has occurred
         *
         * @return Integer value of the timeout
         */
        virtual int getMessageTimeout() = 0;

	   /**
         * getMessageRetries
         *
         * Returns the number of times a message should be sent to the PIDS server
		 * without receiving a response, before giving up
         *
         * @return Integer number of retries
         */
        virtual int getMessageRetries() = 0;

      /**
         * getServerPort
         *
         * Returns the TCP port the PIDS server is listening
		 * on
         *
         * @return An representing the TCP port
         */
        virtual int getServerPort() = 0;


       /**
         * getVetting
         *
         * Returns the vetting state
		 * 
         *
         * @return the vetting state
         */
        virtual bool getVetting() = 0;


        /**
         * getVetting
         *
         * Sets the vetting state
		 * 
         */
        virtual void setVetting(const bool vetting) = 0;
    };

} // namespace TA_Base_Core

#endif // #ifndef IPIDS_ENTITY_DATA_H

