#if !defined(PRINTALARMSENDER_H_INCLUDED)
#define PRINTALARMSENDER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintAlarmSender.h $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Sends Online Printing Agent alarm messages.
  * 
  */

namespace TA_Base_App
{
	class PrintAlarmSender
	{
    // Operations
	public:
		PrintAlarmSender();
		virtual ~PrintAlarmSender();

        /**
         * sendPrintFailureAlarm
         *
         * Sends an Online Printing Agent alarm notifying of print job failure.
         */ 
		void sendPrintFailureAlarm(const std::string& printer, const std::string& additionalDetails);


	};
}

#endif // !defined(PRINTALARMSENDER_H_INCLUDED)
