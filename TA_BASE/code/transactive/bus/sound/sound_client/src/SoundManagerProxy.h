/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/sound/sound_client/src/SoundManagerProxy.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/02/26 16:16:02 $
  * Last modified by:  $Author: Noel $
  *
  * This class acts as a wrapper to the Sound Manager
  * COM interface.
  *
  */

#ifndef SOUNDMANAGERPROXY_H
#define SOUNDMANAGERPROXY_H

#include <string>
#include <comdef.h>

struct ISoundInterface; // Forward declaration

namespace TA_Base_Bus
{
	class SoundInterfaceSink;  // Forward declarations
	class ISoundObserver; 

	class SoundManagerProxy
	{

	public:

		/**
		  * Constructor
		  *
		  * @param observer  The observer which is notified of events received
		  *					 from the Sound Manager. This can be set to NULL (the
		  *					 argument's default) if the client is not interested in
		  *					 receiving any events.
		  *
		  * @exception SoundPlayerException  Thrown if the COM interface cannot be
		  *									 initialised.
		  */
		SoundManagerProxy(ISoundObserver* observer = NULL);


		/**
		  * Destructor
		  */
		~SoundManagerProxy();


		/** 
		  * setPriority
		  *
		  * Sets the current highest priority for a category of sounds. The sound
		  * corresponding to that priority will be played by the Sound Manager.
		  *
		  * @param category  A string representing the category whose priority is
		  *					 changing.
		  * @param priority  The current highest priority for the indicated category.
		  *					 A priority of 0 indicates that no sound should be played.
		  *
		  * @exception SoundPlayerException  Thrown if setting the priority fails.
		  *
		  */
		void setPriority(std::string category, int priority);


		/** 
		  * setSilence
		  *
		  * Changes the silence status of a category of sounds.
		  *
		  * @param category    The category of sounds to silence or unsilence.
		  * @param isSilenced  True if the sound should be silenced, and false if it
		  *					   should be unsilenced.
		  *
  		  * @exception SoundPlayerException  Thrown if changing the silence status of
		  *									  a category fails.
		  */
		void setSilence(std::string category, bool isSilenced);


		/** 
		  * setSilenceAll
		  *
		  * Changes the silence status of all categories of sounds.
		  *
		  * @param isSilenced  True if all categories should be silenced (so no sound
		  *					   at all will be played), and false if all categories should
		  *					   be unsilenced (so all categories will play).
		  *
		  * @exception SoundPlayerException  Thrown if changing the silence status fails.
		  */
		void setSilenceAll(bool isSilenced);

		 
		/** 
		  * joinCategories
		  *
		  * Joins or links two sound categories so that only one sound is played for
		  * both categories, depending on which category has the higher priority. If
		  * both categories have the same priority, then the primaryCategory is played.
		  *
		  * @param primaryCategory    The name of the first category to link.
		  * @param secondaryCategory  The name of the second category to link.
		  *
		  * @exception SoundPlayerException  Thrown if joining the categories fails.
		  *
		  */
		void joinCategories(std::string primaryCategory, std::string secondaryCategory);


		/** 
		  * isSilenced
		  *
		  * Checks whether or not a particular category of sounds has been silenced.
		  *
		  * @param category  The category of sounds for which the silence status is being
		  *					 checked.
		  *
		  * @return True if the category has been silenced, and false otherwise.
		  *
   		  * @exception SoundPlayerException  Thrown if checking the silence status fails.
		  */
		bool isSilenced(std::string category) const;


		/** 
		  * isAllSilenced
		  *
		  * Checks whether or not all categories of sounds have been silenced.
		  *
		  * @return True if all categories of sounds have been silenced, and false
		  *			otherwise.
		  *
   		  * @exception SoundPlayerException  Thrown if checking the silence status fails.
		  */
		bool isAllSilenced() const;

		/** 
		  * stopAllSound
		  *
		  * stop all the sound from the sound manager.
		  *
		  * @exception SoundPlayerException  Thrown if checking the stop sound fails.
		  */
		void stopAllSound();


	private:

		ISoundInterface* m_soundInterface;
		SoundInterfaceSink* m_soundInterfaceSink;

        /**
          * True if OLE was initialised successfully, false otherwise.
          */
        bool m_isOleInitialised;

		/** 
		  * checkReturnValue
		  *
		  * Checks that the HRESULT value returned from a call to the Sound Manager indicates
		  * that the call succeeded. If the call did not succeed, then a SoundPlayerException
		  * is thrown.
		  *
		  * @param hresult  The HRESULT value to check.
		  *
		  * @exception SoundPlayerException  Thrown if the HRESULT does not indicate success
		  *									 (ie is a negative number).
		  */
		void checkReturnValue(HRESULT hresult) const;
	};

} // namespace TA_Base_Bus

#endif // SOUNDMANAGERPROXY_H
