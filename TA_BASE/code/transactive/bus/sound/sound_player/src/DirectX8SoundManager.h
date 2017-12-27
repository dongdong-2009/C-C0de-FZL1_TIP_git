/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/sound/sound_player/src/DirectX8SoundManager.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * DirectX8SoundManager is a singleton wrapper for the CSoundManager class.
  */

#ifndef DIRECTX8SOUNDMANAGER_H
#define DIRECTX8SOUNDMANAGER_H

#include "dsutil.h"

namespace TA_Base_Bus
{
		class DirectX8SoundManager : public CSoundManager
		{
		public:

		  /**
		    * Destructor
		    * 
		    */
		    virtual ~DirectX8SoundManager();
          
          /**
		    * getInstance
		    * 
		    * Gets the one and only DirectX8SoundManager
		    *
		    */
            static DirectX8SoundManager& getInstance();
        
          /**
		    * initialise
		    * 
		    * Initialise the DirectX8SoundManager for normal cooperative operation. 
		    * The isStereo, frequence and bitRate parameters set up the primary buffer 
		    * and determine how the sounds will be played.
		    *
		    * @param window - this application's main window
		    * @param isStereo - stereo mode if true, mono if false
		    * @param frequency - the frequency that sounds will be played at
		    * @param bitRate - the bit rate of the played sounds
		    *
		    * @exception SoundPlayerException - thrown if the initialisation fails
		    *
		    */
            void initialise(HWND window, bool isStereo, int frequency, int bitRate);
            
          /**
		    * initialise
		    * 
		    * Initialise the DirectX8SoundManager for normal cooperative operation. 
		    * The isStereo, frequence and bitRate parameters set up the primary buffer 
		    * and determine how the sounds will be played. Use this method if your 
            * application does not have a main window (desktop window will be used instead)
		    *
		    * @param isStereo - stereo mode if true, mono if false
		    * @param frequency - the frequency that sounds will be played at
		    * @param bitRate - the bit rate of the played sounds
		    *
		    * @exception SoundPlayerException - thrown if the initialisation fails
		    *
		    */
            void initialise(bool isStereo, int frequency, int bitRate);

          /**
		    * initialise
		    * 
		    * Initialise the DirectX8SoundManager for normal cooperative operation. 
		    * The parameters to set up the primary buffer are set to the defaults: 
            * stereo, 22kHz, 16 bit
		    *
		    * @param window - this application's main window
		    *
		    * @exception SoundPlayerException - thrown if the initialisation fails
		    *
		    */
            void initialise(HWND window);
 
          /**
		    * initialise
		    * 
		    * Initialise the DirectX8SoundManager for normal cooperative operation. 
		    * The parameters to set up the primary buffer are set to the defaults: 
            * stereo, 22kHz, 16 bit
		    *
		    * @exception SoundPlayerException - thrown if the initialisation fails
		    *
		    */
            void initialise();

          /**
		    * isInitialised
		    * 
		    * Has the DirectX8SoundManager been initialised.
		    *
		    */
            bool isInitialised();
	
		
		private:
		  /**
		    * Constructor
		    * 
		    * Private constructor to ensure singletoness
		    *
		    */
		    DirectX8SoundManager();

            bool m_isInitialised;
		};
		
}
#endif // DIRECTX8SOUNDMANAGER_H
