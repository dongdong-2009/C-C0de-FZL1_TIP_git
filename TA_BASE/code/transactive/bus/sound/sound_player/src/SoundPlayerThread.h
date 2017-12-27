/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/sound/sound_player/src/SoundPlayerThread.h $
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

#ifndef SOUNDPLAYERTHREAD_H
#define SOUNDPLAYERTHREAD_H

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <string>
#include <map>


namespace TA_Base_Bus
{
    class ISound;

	class SoundPlayerThread : public TA_Base_Core::Thread
    {

	public:

	    /**
		  * Constructor
		  */
		SoundPlayerThread();


        /**
		  * Destructor
		  */
		virtual ~SoundPlayerThread();


        /** 
          * addSound
          *
          * Adds a sound in the given position to the sequence of sounds to be
          * played.
          *
          * @param position  The position of this sound.
          * @param sound     The sound to play.
          *
          */
        virtual void addSound(int position, ISound* sound);


        /** 
          * removeSound
          *
          * Removes a sound from the sequence of sounds being played.
          *
          * @param position  The position of the sound to remove.
          *
          */
        virtual void removeSound(int position);
	   

        /**
          * run
          *
          * Periodically plays a sequence of sounds.
          *
          */
    	virtual void run();


        /**
          * terminate
          *
          * Terminates this thread.
          *
          */
    	virtual void terminate();


	private:

        /**
          * A map of ISound objects, keyed by the order in which to play the sounds, that the
          * SoundPlayerThread will play.
          */
        typedef std::map<int, ISound*> SoundMap;
        SoundMap m_sounds;
               
        /**
          * True if the thread is running, and false otherwise.
          */
        volatile bool m_isRunning;

        /**
          * Protects access to the map of sounds.
          */
        TA_Base_Core::NonReEntrantThreadLockable m_lock;

		/**
		  * Set to true if the map is changed while the SoundPlayerThread is iterating over it.
		  * This allows us to refresh the changes made to the map in the thread, so that the new
		  * sounds are picked up immediately.
		  */
		volatile bool m_hasMapChanged;
	};

} // namespace TA_Base_Bus

#endif // SOUNDPLAYERTHREAD_H
