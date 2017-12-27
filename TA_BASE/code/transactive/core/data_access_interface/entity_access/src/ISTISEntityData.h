/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ISTISEntityData.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ISTISEntityData provides read only access to STIS entity data.
  */

#ifndef ISTIS_ENTITY_DATA_H
#define ISTIS_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class ISTISEntityData : public virtual IEntityData
    {
    public:

       /**
         * getServerIPAddress
         *
         * Returns the IP address of the local STIS server
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
         * Returns the number of times a message should be sent to the STIS server
		 * without receiving a response, before giving up
         *
         * @return Integer number of retries
         */
        virtual int getMessageRetries() = 0;

      /**
         * getServerPort
         *
         * Returns the TCP port the STIS server is listening
		 * on
         *
         * @return An representing the TCP port
         */
        virtual int getServerPort() = 0;

        virtual std::string getCCTIPAddress() = 0;

        virtual int getCCTPort() = 0;

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

#endif // #ifndef ISTIS_ENTITY_DATA_H

