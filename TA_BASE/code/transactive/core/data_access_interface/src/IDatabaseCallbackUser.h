/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IDatabaseCallbackUser.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * IDatabaseCallbackUser is a callback interface for use with any DatabaseAccessInterface class that allows for progress updates
 * while loading large numbers of events. As some tables can have over 1 million entries, this will allow
 * system users to be updated with the progress of their application while the code "stuck" in the database loading method.
 */


#if !defined(IDATABASE_CALLBACK_USER_H)
#define IDATABASE_CALLBACK_USER_H

#include <string>

namespace TA_Base_Core
{

   
	class IDatabaseCallbackUser
	{

	public:

		/**
         * setCount
         *
         * Sets the total number of database items that the AccessFactory will load. This number cannot be guaranteed to be 
         * accurate, and therefore should only be used when calculating increments for user notification.
         *
         * @param count The (approximate) number of database items the factory will process to load.
         */
        virtual void setCount(const unsigned long count) =0;

        /**
         * notifyLoad
         * 
         * Send a notification of the number of database items having been loaded
         *
         * @param count The number of database items the factory has loaded since the previous call to this method, or since
         * the operation began (which ever was most recent).
         */
        virtual void notifyLoad(const unsigned long count = 500) =0;

        /**
         * limitEnforced
         *
         * Notifies the callback user that the pre-set database maximum is being enforced.
         */
        virtual void limitEnforced() = 0;

        virtual ~IDatabaseCallbackUser() {};

		

	};

} //close namespace TA_Base_Core

#endif // !defined(IDATABASE_CALLBACK_USER_H)
