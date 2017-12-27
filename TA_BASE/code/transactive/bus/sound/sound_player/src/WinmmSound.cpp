/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/sound/sound_player/src/WinmmSound.cpp $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Plays a sound using the Windows MultiMedia library.
  */



#include <fstream>
#include <windows.h>
#include <Mmsystem.h>

#include "WinmmSound.h"

#include "core/exceptions/src/SoundPlayerException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	WinmmSound::WinmmSound(const std::string& wavFile)
		:m_wavFile(wavFile)
	{
		//check that file can be opened
		std::ifstream inFile(wavFile.c_str());
		if(!inFile)
		{
			TA_THROW(TA_Base_Core::SoundPlayerException("Couldn't open file '" + m_wavFile + "'"));
		}
	}
	
	
	WinmmSound::~WinmmSound()
	{
	}
	
	
	void WinmmSound::play()
	{
		if(sndPlaySound(m_wavFile.c_str(), SND_ASYNC) == FALSE)
		{
			TA_THROW(TA_Base_Core::SoundPlayerException("Couldn't play sound in file '" + m_wavFile + "'"));
		}
	}
	
	
	void WinmmSound::stop()
	{
		sndPlaySound(NULL, SND_ASYNC);
	}
	
	
	void WinmmSound::setFile(std::string filename)
	{
		m_wavFile = filename;
	}
}
