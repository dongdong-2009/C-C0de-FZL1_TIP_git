/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/constants.h $
  * @author:  C. DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Just a few constant declarations that the Banner app uses
  */


#if !defined(BANNER_CONST_H__3A7A2_7A1A_4539_AA8B_9F1D15675D56__INCLUDED_)
#define BANNER_CONST_H__3A7A2_7A1A_4539_AA8B_9F1D15675D56__INCLUDED_

#include <string>

namespace BANNER_CONST
{
    // command lines to register and deregister the COM server
    static const std::string REGISTER_CMD = "--install";
    static const std::string UNREGISTER_CMD = "--remove";

	//changing this runparam indicates that the alarms associated with this
	//asset should be displayed in the banner
	static const std::string RPARAM_ASSETALARM = "AssetAlarm";
	
	//this required run param specifies the tab that will be selected
	//when the Banner starts. It can be set to one of the following tab name values
	//note that we do not listen for changes to this param. It only has effect
	//during init
	static const std::string RPARAM_START_TAB = "StartTab";

	static const COLORREF CCL_GOLD = RGB(255, 213, 0);

    // Get a unique user defined message id for set window position message.
    static UINT WM_SET_FLASH_STATE = RegisterWindowMessage("SetFlashState");

    // Constant required by the ATS COM IDL.
    static const unsigned int ATS = 130; // Dialog ID of the ATS page.

	// 4669Pre-FAT TD#13762
    static const COLORREF CCL_RED = RGB(255, 0, 0);
}

#endif //!defined(BANNER_CONST_H__3A7A2_7A1A_4539_AA8B_9F1D15675D56__INCLUDED_)