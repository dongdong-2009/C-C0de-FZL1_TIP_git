/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/sound/sound_player/src/ISound.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ISound is an interface for sounds. Sounds may be played or stopped from 
  * playing.
  */

#ifndef ISOUND_H
#define ISOUND_H

#include <string>

namespace TA_Base_Bus
{

    class ISound
    {

    public:

      /**
        * Destructor
        * 
        * Virtual destructor 
        *
        */
        virtual ~ISound(){};

     /**
        * play
        * 
        * Plays the sound
        *
        * @exception SoundPlayerException - thrown when the sound cannot be played
        */
        virtual void play() = 0;

      /**
        * stop
        * 
        * Stops the sound from playing
        *
        */
        virtual void stop() = 0;

      /**
        * setFile
        *
        * Sets the filename for this sound.
        *
        * @param filename  The filename containing the sound to be played.
        */
        virtual void setFile(std::string filename) = 0;
    };

}

#endif // ISOUND_H
