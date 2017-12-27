#ifndef NOTIFICATION_SERVICE_CONSTANTS_H
#define NOTIFICATION_SERVICE_CONSTANTS_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/NotificationServiceConstants.h $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/12/17 15:24:44 $
 * Last modified by:  $Author: shurui.liu $
 * 
 * Constant values for the NotificationServiceWrapper
 */


namespace TA_Base_App
{
	// Timeout for resolving the EventChannelFactory (secs)
	const int RESOLVE_CHANNELFACTORY_TIMEOUT	= 20;

	// path & name of the omniNotify config file
	#define OMNI_NOTIFY_CONFIG_FILE "NotifyConfigFile"
}

#endif // NOTIFICATION_SERVICE_CONSTANTS_H
