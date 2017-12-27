/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/SoundPlayerException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#include "SoundPlayerException.h"

namespace TA_Base_Core
{
	SoundPlayerException::SoundPlayerException()
	{

	}

	SoundPlayerException::SoundPlayerException(const std::string& msg) 
		: TransactiveException(msg)
	{

	}
		

	SoundPlayerException::~SoundPlayerException()
	{

	}
}

