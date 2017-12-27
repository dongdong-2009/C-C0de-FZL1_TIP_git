/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/sound/sound_player/src/SoundPlayer.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * The SoundPlayer is responsible for playing sounds. It takes requests for changes
  * to the sounds being played and passes them on to its SoundPlayerThread.
  * 
  */

#include "SoundPlayer.h"
#include "SoundPlayerThread.h"
#include "DirectX8SoundManager.h"
#include "ISound.h"

#include "core/exceptions/src/SoundPlayerException.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

    SoundPlayer::SoundPlayer()
    {
		try
		{
			DirectX8SoundManager::getInstance().initialise();
		}
		catch(TA_Base_Core::SoundPlayerException&)
		{
			// UE-10020
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				   "SoundPlayerException", "Unable to initialise the sound card. No sounds will be played.");

			//::MessageBox(NULL, "Unable to initialise the sound card. No sounds will be played.",
			//       "Sound Manager", MB_ICONSTOP);
		}

		m_soundPlayerThread.start();
    }


    SoundPlayer::~SoundPlayer()
    {
		//To terminate the SoundPlayerThread - Fix for Server Busy Issue.
		m_soundPlayerThread.terminateAndWait();
    }


    void SoundPlayer::playSound(int position, ISound* sound)
    {
        m_soundPlayerThread.addSound(position, sound);
    }


    void SoundPlayer::stopSound(int position)
    {
        m_soundPlayerThread.removeSound(position);
    }
}
