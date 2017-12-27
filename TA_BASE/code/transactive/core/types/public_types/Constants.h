#ifndef Public_Constants_01_03_2005
#define Public_Constants_01_03_2005

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/types/public_types/Constants.h $
  * @author:  
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/02/16 09:11:29 $
  * Last modified by:  $Author: dhanshri $
  *
  * Contains public constants
  */

#include <sys/timeb.h>

namespace TA_Base_Core
{
    /**
     * The default time to use on alarms and events.
     */
    const timeb defaultTime = {0,0,-1,-1};

    enum EDataPointQuality
    {
	    /**
	     * Mapped to DataPointStatus - QUALITY_GOOD_NO_SPECIFIC_REASON
	     */
	    ITA_QUALITY_GOOD = 192,
	    /**
	     * Mapped to DataPointStatus - QUALITY_BAD_NO_SPECIFIC_REASON.
	     */
		ITA_QUALITY_BAD = 0,				// 00000000 00000000
		// Updated for all quality statuses with EQualityStatus
		ITA_QUALITY_BAD_CONFIGURATION_ERROR = 4,			// 00000000 00000100
	    ITA_QUALITY_BAD_NOT_CONNECTED = 8,					// 00000000 00001000
	    ITA_QUALITY_BAD_DEVICE_FAILURE = 12,				// 00000000 00001100
	    ITA_QUALITY_BAD_SENSOR_FAILURE = 16,				// 00000000 00010000
	    ITA_QUALITY_BAD_LAST_KNOWN_VALUE = 20,				// 00000000 00010100
	    ITA_QUALITY_BAD_COMM_FAILURE = 24,					// 00000000 00011000
	    ITA_QUALITY_BAD_OUT_OF_SERVICE = 28,				// 00000000 00011100

	    ITA_QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON = 64,	// 00000000 01000000
	    ITA_QUALITY_UNCERTAINTY_LAST_USABLE_VALUE = 68,		// 00000000 01000100
	    ITA_QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE = 80,	// 00000000 01010000
	    ITA_QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED = 84,		// 00000000 01010100
	    ITA_QUALITY_UNCERTAINTY_SUB_NORMAL = 88,			// 00000000 01011000
	    ITA_QUALITY_GOOD_LOCAL_OVERRIDE = 216				// 00000000 11011000

    };

}

#endif //Public_Type_Defs_01_03_2005
