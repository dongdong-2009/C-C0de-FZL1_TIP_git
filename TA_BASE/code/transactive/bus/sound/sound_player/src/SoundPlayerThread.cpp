/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/sound/sound_player/src/SoundPlayerThread.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * A SoundPlayerThread periodically plays a sequence of sounds. The sequence
  * of sounds is stored in a map of ISound objects that are keyed by their
  * position in the sequence.
  * 
  */

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif // _MSC_VER

#include "SoundPlayerThread.h"
#include "ISound.h"

#include "core/exceptions/src/SoundPlayerException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include <Mmsystem.h>

using TA_Base_Core::DebugUtil;


namespace TA_Base_Bus
{

    /**
      * The time, in milliseconds, to wait between sounds in different categories.
      */
    const int CATEGORY_INTERVAL_MSECS = 1000;

    /**
      * The time, in milliseconds, to wait after the last sound before playing the first
      * sound again.
      */
    const int REPEAT_INTERVAL_MSECS = 1000;

 
    SoundPlayerThread::SoundPlayerThread()
        : m_isRunning(false), m_hasMapChanged(false)
    {
    }


    SoundPlayerThread::~SoundPlayerThread()
    {
    }


    void SoundPlayerThread::addSound(int position, ISound* sound)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);

		SoundMap::iterator soundIter = m_sounds.find(position);
		
        // Insert the sound into the map if the position doesn't exist, or 
        // overwrite the old sound if the position does exist.
        m_sounds[position] = sound;

		// Indicate that the map has changed.
		m_hasMapChanged = true;
    }


    void SoundPlayerThread::removeSound(int position)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);

		SoundMap::iterator soundIter = m_sounds.find(position);
		if (soundIter != m_sounds.end())
		{
			// Stop the current sound at the same position.
			soundIter->second->stop();
		}

        // Delete the sound from the map.
        m_sounds.erase(position);

		// Indicate that the map has changed.
		m_hasMapChanged = true;
    }


    void SoundPlayerThread::run()
    {
        m_isRunning = true;

        try
        {
            while (m_isRunning)
            {
				SoundMap copy;

                {
					TA_Base_Core::ThreadGuard guard(m_lock);

					// Copy the sound map.
					copy = m_sounds;

					// Reset the flag saying the copy is out-of-date.
					m_hasMapChanged = false;
				}

				// Iterate over the copy of the sound map.
				SoundMap::iterator soundIter = copy.begin();
				while (soundIter != copy.end())
				{
					try
					{
						// Play the sound.
						soundIter->second->play();
					}
					catch(TA_Base_Core::SoundPlayerException& ex)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", ex.what());
					}

					TA_Base_Core::Thread::sleep(CATEGORY_INTERVAL_MSECS);

					TA_Base_Core::ThreadGuard guard(m_lock);

					// Check if we need to refresh the contents of the map we're iterating over.
					if (m_hasMapChanged)
					{
						// Get the current sound we're up to.
						int position = soundIter->first;

						// Copy the sound map again.
						copy = m_sounds;
						m_hasMapChanged = false;

						// Update the iterator - we want the next sound after the one we just played.
						soundIter = copy.lower_bound(position);
						if (soundIter == copy.end())
						{
							soundIter = copy.begin();	// Restart sequence of sounds.
						}
						else if (copy.find(position) != copy.end())
						{
							soundIter++;  // Current position still in map. Go to the next sound.
						}

						if (soundIter == copy.end())
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
								"Sound map changing while playing sound %d - restarting sequence of sounds",
								position);
						}
						else
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
								"Sound map changed while playing sound %d - continuing at sound %d", 
								position, soundIter->first);
					
						}
					}
					else
					{
						// No changes, so just increment the iterator.
						soundIter++;
					}
				}
		
				if (copy.empty())
				{
					TA_Base_Core::Thread::sleep(REPEAT_INTERVAL_MSECS);
				}
				else
				{
					// Have already slept for CATEGORY_INTERVAL_MSECS - so sleep for the difference.
					TA_Base_Core::Thread::sleep(REPEAT_INTERVAL_MSECS - CATEGORY_INTERVAL_MSECS);
				}
            }

        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception caught!");
        }
    }


    void SoundPlayerThread::terminate()
    {
        m_isRunning = false;
    }

} // namespace TA_Base_Bus
