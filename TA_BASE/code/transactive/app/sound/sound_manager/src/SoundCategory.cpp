/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/sound/sound_manager/src/SoundCategory.cpp $
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

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

#include "SoundCategory.h"

#include "core/utilities/src/DebugUtil.h"
#include "bus/sound/sound_player/src/DirectX8Sound.h"

#include "core/data_access_interface/sound_manager/src/SoundFileAccessFactory.h"
#include "core/data_access_interface/sound_manager/src/ISoundFile.h"

#include "core/exceptions/src/SoundPlayerException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"


namespace TA_Base_App
{

	SoundCategory::SoundCategory(TA_Base_Bus::SoundPlayer& soundPlayer, std::string categoryName, 
                                    unsigned long categoryKey, int position)
	: m_soundPlayer(soundPlayer), m_categoryName(categoryName), m_categoryKey(categoryKey), m_position(position), 
	  m_isSilenced(false), m_highestPriority(0), m_linkedCategory(NULL), m_isPrimary(false)
	{
		try
		{
			std::vector<TA_Base_Core::ISoundFile*> soundData = TA_Base_Core::SoundFileAccessFactory::getInstance().
														getSoundFileByCategory(categoryKey);

			for (std::vector<TA_Base_Core::ISoundFile*>::iterator iter = soundData.begin(); iter != soundData.end(); iter++)
			{
				try
				{
					m_sounds[(*iter)->getPriority()] = new TA_Base_Bus::DirectX8Sound((*iter)->getSoundFile());
				}
				catch(TA_Base_Core::SoundPlayerException& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", ex.what());
				}

				delete *iter;
				*iter = NULL;
			}
		}
		catch(TA_Base_Core::DataException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
		}
		catch(TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
		}
	}


	SoundCategory::~SoundCategory()
	{
		// Clean up the ISound objects.
		for (SoundMap::iterator iter = m_sounds.begin(); iter != m_sounds.end(); iter++)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}


	void SoundCategory::setPriority(int priority)
	{
		SoundMap::iterator iter;

		if (priority == 0)
		{
			m_highestPriority = priority;
		}
		else
		{
			// Check that we have an ISound object for the requested priority.
			iter = m_sounds.find(priority);
			if (iter != m_sounds.end())
			{
				m_highestPriority = priority;
			}
			else
			{
				TA_THROW(TA_Base_Core::SoundPlayerException("Priority does not exist"));
			}
		}

		if (m_linkedCategory == NULL)
		{
			// No linked category. Just play a sound according to this category's
			// highest priority.
			playCurrentPriority();
		}
		else
		{
			// Check if the linked category's highest priority is higher than our priority.
			bool isOurPriorityHigher = (m_highestPriority > m_linkedCategory->m_highestPriority)
										|| (m_highestPriority == m_linkedCategory->m_highestPriority
											&& m_isPrimary);

			if (isOurPriorityHigher)
			{
				// Stop the linked category's sound.
				m_linkedCategory->m_soundPlayer.stopSound(m_linkedCategory->m_position);

				// Play our highest priority.
				playCurrentPriority();
			}
			else
			{
				// Stop our sound.
				m_soundPlayer.stopSound(m_position);

				// Tell the linked category to play its sound.
				m_linkedCategory->playCurrentPriority();
			}
		}
	}


	void SoundCategory::setSilence(bool isSilenced)
	{
		m_isSilenced = isSilenced;
		updateSilenceState();
		
		// If we're linked to a sound category, change its silence state too.
		if (m_linkedCategory != NULL)
		{
			m_linkedCategory->m_isSilenced = isSilenced;
			m_linkedCategory->updateSilenceState();
		}
	}


	bool SoundCategory::isSilenced() const
	{
		return m_isSilenced;
	}


	void SoundCategory::setFile(int priority, std::string filename)
	{
		// Check that we have an ISound object for the requested priority.
		SoundMap::iterator iter = m_sounds.find(priority);
		if (iter != m_sounds.end())
		{
			iter->second->setFile(filename);
		}
		else
		{
			TA_THROW(TA_Base_Core::SoundPlayerException("Priority does not exist"));
		}
	}


	void SoundCategory::linkToPrimaryCategory(SoundCategory* linkedCategory)
	{
		m_linkedCategory = linkedCategory;
		m_isPrimary = false;
	}


	void SoundCategory::linkToSecondaryCategory(SoundCategory* linkedCategory)
	{
		m_linkedCategory = linkedCategory;
		m_isPrimary = true;
	}


    SoundCategory* SoundCategory::getJoinedCategory() const
    {
        return m_linkedCategory;
    }


	void SoundCategory::playCurrentPriority()
	{
		if (m_highestPriority == 0)
		{
			// Stop the sound playing.
			m_soundPlayer.stopSound(m_position);
		}
		else
		{
			if (!m_isSilenced)
			{
				// Check that we have an ISound object for the current priority.
				SoundMap::iterator iter = m_sounds.find(m_highestPriority);
				if (iter != m_sounds.end())
				{
					// Tell the SoundPlayer to play the sound.
					m_soundPlayer.playSound(m_position, iter->second);	
				}
			}
		}
	}


	void SoundCategory::updateSilenceState()
	{
		// Silence state has changed. Update the SoundPlayer.
		if (m_isSilenced)
		{
			m_soundPlayer.stopSound(m_position);
		}
		else
		{
			setPriority(m_highestPriority);
		}
	}

} // namespace TA_Base_App
