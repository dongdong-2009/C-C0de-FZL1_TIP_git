/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/sound/sound_manager/src/SoundCategory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents a category of sounds. It mains which
  * sound in the category has the highest priority, and allows
  * the category to have its priority changed, to be silenced,
  * etc.
  *
  */

#if !defined(SoundCategory_C409C916_D15B_474f_8BE1_402476CDD2B6__INCLUDED_)
#define SoundCategory_C409C916_D15B_474f_8BE1_402476CDD2B6__INCLUDED_

#include "bus/sound/sound_player/src/ISound.h"
#include "bus/sound/sound_player/src/SoundPlayer.h"

#include <map>
#include <string>

namespace TA_Base_App
{

	class SoundCategory
	{

	public:

		/** 
		  * Constructor
		  *
		  * @param soundPlayer  A reference to a SoundPlayer that the category can use
		  *						to play sounds.
          * @param categoryName The name of the category.
		  * @param categoryKey	The primary key of the category.
		  * @param position		The position (order), in the sequence of sound categories,
		  *						that sets when this category will be played.
		  *
		  */
        SoundCategory(TA_Base_Bus::SoundPlayer& soundPlayer, std::string categoryName, 
                        unsigned long categoryKey, int position);


		/**
		  * Copy constructor
		  */
		SoundCategory(const SoundCategory& soundCategory);


		/** 
		  * Destructor
		  */
		virtual ~SoundCategory();


		/**
		  * getCategoryKey
		  *
		  * Returns the primary key of this SoundCategory.
		  */
		unsigned long getCategoryKey() const
		{
			return m_categoryKey;
		}


        /** 
          * getCategoryName
          *
          * Returns the name of this SoundCategory.
          */
        std::string getCategoryName() const
        {
            return m_categoryName;
        }


		/** 
		  * setPriority
		  *
		  * Sets the highest priority of this category.
		  *
		  * @param priority  The category's highest priority. 0 indicates no sound should
		  *					 be played.
		  */
		void setPriority(int priority);


		/** 
		  * setSilence
		  *
		  * Silences or unsilences this category.
		  *
		  * @param isSilenced  True if the category should be silenced, and false otherwise.
		  */
		void setSilence(bool isSilenced);


		/** 
		  * isSilenced
		  *
		  * Returns the silence state of the category.
		  *
		  * @return True if the category is silenced, and false otherwise.
		  */
		bool isSilenced() const;


		/**
		  * setFile
		  *
		  * Changes the filename of a sound in this category.
		  *
		  * @param priority  The priority of the sound you wish to change.
		  * @param filename  The new filename (including the path) of the sound.
		  */
		void setFile(int priority, std::string filename);


        /** 
          * linkToPrimaryCategory
          *
          * Links this SoundCategory to a primary SoundCategory. This SoundCategory
		  * becomes the secondary SoundCategory.
          *
          * @param linkedCategory  A pointer to the SoundCategory to link to this
		  *						   category.
          */
		void linkToPrimaryCategory(SoundCategory* linkedCategory);


        /** 
          * linkToSecondaryCategory
          *
          * Links this SoundCategory to a secondary SoundCategory. This SoundCategory
		  * becomes the primary SoundCategory.
          *
          * @param linkedCategory  A pointer to the SoundCategory to link to this
		  *						   category.
          */
		void linkToSecondaryCategory(SoundCategory* linkedCategory);


        /** 
          * getJoinedCategory
          *
          * Returns the sound category joined to this category, or NULL if this
          * category is not linked to any category.
          *
          * @return The sound category joined to this category, or NULL.
          */
        SoundCategory* getJoinedCategory() const;

	private:

        /** 
          * playCurrentPriority
          *
          * Tells the SoundPlayer to play our current highest priority.
          */
		void playCurrentPriority();


		/**
		  * updateSilenceState
		  *
		  * Responds to a change in the SoundCategory's silence state by updating the
		  * SoundPlayer with the sound to be played, if any.
		  */
		void updateSilenceState();


		/**
		  * A map of ISound objects containing all of the sounds for this category,
		  * keyed by their priorities.
		  */
		typedef std::map<int, TA_Base_Bus::ISound*> SoundMap;
		SoundMap m_sounds;

        /**
          * The category's name.
          */
        std::string m_categoryName;

		/**
		  * The SoundCategory's key.
		  */
		unsigned long m_categoryKey;

		/**
		  * True if this category is silenced, and false otherwise.
		  */
		bool m_isSilenced;

		/**
		  * The current highest priority of this category.
		  */
		int m_highestPriority;

		/**
		  * The position of this category in the sequence of sound categories
		  */
		int m_position;

		/**
		  * The sound player
		  */
		TA_Base_Bus::SoundPlayer& m_soundPlayer;

		/**
		  * The sound category that is linked to this category. NULL if no category
		  * is linked to this category.
		  */
		SoundCategory* m_linkedCategory;

		/**
		  * True if this SoundCategory is linked to another SoundCategory, and this
		  * SoundCategory is the primary SoundCategory. If so, its sound is played
		  * when the two SoundCategory's have the same priority.
		  */
		bool m_isPrimary;

	};

} // namespace TA_Base_App

#endif // !defined(SoundCategory_C409C916_D15B_474f_8BE1_402476CDD2B6__INCLUDED_)
