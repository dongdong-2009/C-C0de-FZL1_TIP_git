/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/sound/sound_manager/src/CategoryManager.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class maintains all of the categories of sounds
  * (eg alarms, calls, etc). It contains a map of SoundCategory
  * objects that are keyed by a string containing the name
  * of the category.
  *
  */

#if !defined(CategoryManager_D422F016_AC71_4df8_93C3_4ED7FACB2F4A__INCLUDED_)
#define CategoryManager_D422F016_AC71_4df8_93C3_4ED7FACB2F4A__INCLUDED_

#include "bus/sound/sound_player/src/SoundPlayer.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "SoundCategory.h"

#include <string>
#include <map>

namespace TA_Base_App
{

	class CategoryManager : public TA_Base_Core::IOnlineUpdatable
	{

	public:
		/** 
		  * Constructor
		  *
		  * Constructs a CategoryManager object.
		  *
		  */
		CategoryManager();


		/** 
		  * Destructor
		  */
		virtual ~CategoryManager();


		/** 
		  * setPriority
		  *
		  * Sets the highest priority of a particular category.
		  *
		  * @param category  The category to set the priority for.
		  * @param priority  The category's highest priority. 0 indicates that no
		  *					 sound should be played for the category.
		  *
		  */
		void setPriority(std::string category, int priority);


		/** 
		  * setSilence
		  *
		  * Silences or unsilences a category.
		  *
		  * @param category	   The category whose silence state should be changed.
		  * @param isSilenced  True if the category should be silenced, and false
		  *					   otherwise.
		  *
		  */
		void setSilence(std::string category, bool isSilenced);


		/** 
		  * setSilenceAll
		  *
		  * Silences or unsilences all categories.
		  *
		  * @param isSilenced  True if all categories should be silenced, and false
		  *					   otherwise.
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
          */
		void joinCategories(std::string primaryCategory, std::string secondaryCategory);


        /** 
          * getJoinedCategory
          *
          * Returns the name of the category that is linked with the given category. If
          * the category is not joined to any other category, then "" is returned.
          *
          * @param category  The name of the category.
          *
          * @return  The name of the category that is linked to the given category, or
          *          the empty string if no category is linked.
          */
        std::string getJoinedCategory(std::string category);


		/** 
		  * isSilenced
		  *
		  * Returns the silence state of a particular category.
		  *
		  * @param category  The category to check the silence state for.
		  *
		  * @return  True if the category is silenced, and false otherwise.
		  */
		bool isSilenced(std::string category);


		/** 
		  * isAllSilenced
		  *
		  * Checks if all categories are silenced.
		  *
		  * @return True if all categories are silenced, and false otherwise.
		  */
		bool isAllSilenced();


		/** 
		  * processUpdate
		  *
		  * Processes an online update. Allows the filenames of sounds to be changed
		  * while the Sound Manager is running.
		  *
		  * @param updateEvent  Information about the online update.
		  */
		virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


	private:

		/**
		  * A map of SoundCategory objects that are keyed by the category name.
		  */
		typedef std::map<std::string, SoundCategory*> SoundCategoryMap;
		SoundCategoryMap m_categories;

		/**
		  * The sound player.
		  */
		TA_Base_Bus::SoundPlayer m_soundPlayer;

		/**
		  * Protect access to each category.
		  */
		TA_Base_Core::NonReEntrantThreadLockable m_lock;

	};

} // namespace TA_Base_App

#endif // !defined(CategoryManager_D422F016_AC71_4df8_93C3_4ED7FACB2F4A__INCLUDED_)
