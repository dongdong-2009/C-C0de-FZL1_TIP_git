/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/sound/sound_manager/src/CategoryManager.cpp $
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

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

#include "CategoryManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/data_access_interface/sound_manager/src/SoundCategoryAccessFactory.h"
#include "core/data_access_interface/sound_manager/src/ISoundCategory.h"
#include "core/data_access_interface/sound_manager/src/SoundFileAccessFactory.h"
#include "core/data_access_interface/sound_manager/src/ISoundFile.h"

#include "core/exceptions/src/SoundPlayerException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"

#include <string>
#include <map>
#include <vector>

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

	// The parameter name for the ConfigUpdateDetails object.
	static const std::string SOUND_FILE = "Sound File";


	CategoryManager::CategoryManager()
	{
		try
		{
			std::vector<TA_Base_Core::ISoundCategory*> categories = 
				TA_Base_Core::SoundCategoryAccessFactory::getInstance().getAllSoundCategories();

			std::vector<unsigned long> keys;
			for (std::vector<TA_Base_Core::ISoundCategory*>::iterator iter = categories.begin(); iter != categories.end(); iter++)
			{
				m_categories.insert( SoundCategoryMap::value_type((*iter)->getName(), 
									 new SoundCategory(m_soundPlayer, (*iter)->getName(), 
                                                        (*iter)->getCategoryKey(), (*iter)->getPosition())) );

				keys.push_back((*iter)->getCategoryKey()); // Fixed TD#15687++ 
				
				delete *iter;
				*iter = NULL;
			}

			// Fixed TD#15687++ 
			TA_Base_Core::OnlineUpdateListener::getInstance().registerInterests(TA_Base_Core::SOUND_FILE, *this,keys);
			// ajlaredo--

		}
		catch(TA_Base_Core::DataException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());

			LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "Unable to retrieve sound categories from database. Exiting.");
			exit(1);
		}
		catch(TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());

			LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "Unable to retrieve sound categories from database. Exiting.");
			exit(1);
		}
	}


	CategoryManager::~CategoryManager()
	{
		// Nothing to clean up.
	}


	void CategoryManager::setPriority(std::string category, int priority)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		if (strcmpi(category.c_str(), "ALL") == 0)
		{	// We added All category internally so we can enable the stopping of the sound
			// for all category
			// Iterate through all SoundCategory's and set their silence state.
			for (SoundCategoryMap::iterator iter = m_categories.begin(); iter != m_categories.end(); iter++)
			{
				try
				{
					iter->second->setPriority(priority);
				}
				catch(...)
				{
					std::string errorMsg = "Error occurred while trying to set all category";
					TA_THROW(TA_Base_Core::SoundPlayerException(errorMsg));
				}
			}
		}
		else
		{
			// Find the SoundCategory, and tell it to change it's priority.
			SoundCategoryMap::iterator iter = m_categories.find(category);
			if (iter != m_categories.end())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting highest priority of %s category to %d",
					category.c_str(), priority);

				iter->second->setPriority(priority);
			}
			else
			{
				std::string errorMsg = "Category " + category + " does not exist";
				TA_THROW(TA_Base_Core::SoundPlayerException(errorMsg));
			}
		}
		
	}


	void CategoryManager::setSilence(std::string category, bool isSilenced)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		// Find the SoundCategory, and tell it to change its silence state.
		SoundCategoryMap::iterator iter = m_categories.find(category);
		if (iter != m_categories.end())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting silence state of %s category to %s",
				category.c_str(), (isSilenced ? "true" : "false"));

			iter->second->setSilence(isSilenced);
		}
		else
		{
			std::string errorMsg = "Category " + category + " does not exist";
			TA_THROW(TA_Base_Core::SoundPlayerException(errorMsg));
		}
	}


	void CategoryManager::setSilenceAll(bool isSilenced)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		bool errorOccurred = false;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting the silence state of all categories to %s",
			(isSilenced ? "true" : "false"));

		// Iterate through all SoundCategory's and set their silence state.
		for (SoundCategoryMap::iterator iter = m_categories.begin(); iter != m_categories.end(); iter++)
		{
			try
			{
				iter->second->setSilence(isSilenced);
			}
			catch(...)
			{
				// Catch any exceptions here, as we want to set the silence state of as many
				// categories as possible.
				errorOccurred = true;
			}
		}

		if (errorOccurred)
		{
			TA_THROW(TA_Base_Core::SoundPlayerException("Error occurred while setting the silence state of all categories"));
		}
	}


	void CategoryManager::joinCategories(std::string primaryCategory, std::string secondaryCategory)
	{
		// Find both SoundCategory's.
		SoundCategoryMap::iterator primaryIter = m_categories.find(primaryCategory);
		SoundCategoryMap::iterator secondaryIter = m_categories.find(secondaryCategory);

		// Check that they were found.
		if (primaryIter == m_categories.end())
		{
			std::string errorMsg = primaryCategory + " category was not found.";
			TA_THROW(TA_Base_Core::SoundPlayerException(errorMsg));
		}
		else if (secondaryIter == m_categories.end())
		{
			std::string errorMsg = secondaryCategory + " category was not found.";
			TA_THROW(TA_Base_Core::SoundPlayerException(errorMsg));
		}

		// Both categories were found, so link them together.
		primaryIter->second->linkToSecondaryCategory(secondaryIter->second);
		secondaryIter->second->linkToPrimaryCategory(primaryIter->second);
	}


    std::string CategoryManager::getJoinedCategory(std::string category)
    {
        SoundCategoryMap::iterator iter = m_categories.find(category);
        if (iter == m_categories.end())
        {
            // Category not found, just return empty string.
            return "";
        }
        else
        {
            SoundCategory* joinedCategory = iter->second->getJoinedCategory();
            if (joinedCategory == NULL)
            {
                return "";
            }
            else
            {
                return joinedCategory->getCategoryName();
            }
        }
    }


	bool CategoryManager::isSilenced(std::string category)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		bool isSilencedBool = false;

		// Find the SoundCategory, and ask it its silence state.
		SoundCategoryMap::const_iterator iter = m_categories.find(category);
		if (iter != m_categories.end())
		{
			isSilencedBool = iter->second->isSilenced();

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Silence state of category %s is %s",
				category.c_str(), (isSilencedBool ? "true" : "false"));
		}
		else
		{
			std::string errorMsg = "Category " + category + " does not exist";
			TA_THROW(TA_Base_Core::SoundPlayerException(errorMsg));
		}

		return isSilencedBool;
	}


	bool CategoryManager::isAllSilenced()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		bool isAllSilencedBool = true;
		bool errorOccurred = false;

		// Iterate through all SoundCategory's and get their silence state.
		for (SoundCategoryMap::const_iterator iter = m_categories.begin(); iter != m_categories.end(); iter++)
		{
			try
			{
				isAllSilencedBool = ( isAllSilencedBool && iter->second->isSilenced() );
			}
			catch(...)
			{
				// Catch any exceptions here, as we want to set the silence state of as many
				// categories as possible.
				errorOccurred = true;
			}
		}

		if (errorOccurred)
		{
			TA_THROW(TA_Base_Core::SoundPlayerException("Error occurred while fetching the silence state of all categories"));
		}

		return isAllSilencedBool;
	}


	void CategoryManager::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("processUpdate");
		
		// TODO: Update object type. // ajlaredo @comment this line for TD15694
		/*if (updateEvent.getType() != TA_Base_Core::ALARM_SEVERITY)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
				"Not interested in update of this object type");
			return;
		}*/
		
		switch (updateEvent.getModifications())
		{
		case TA_Base_Core::Create:
		case TA_Base_Core::Delete:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
				"Alarm severities cannot be created or deleted");
			break;
			
		case TA_Base_Core::Update:

			TA_Base_Core::ThreadGuard guard(m_lock);

			const std::vector<std::string> changes = updateEvent.getChangedParams();
			if (changes.empty())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No updates. Ignoring");
				return;
			}
			
			for (unsigned int i = 0; i < changes.size(); i++)
			{
				std::string param = changes[i];

				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Online Update Param = %s",param.c_str());
				
				if (param.compare(SOUND_FILE) == 0)
				{
					// The sound filename changed for a particular sound.
					// Get the changes from the database.
					try
					{
						unsigned long categoryKey = updateEvent.getKey();

						// Get the name of the category with the given key.
						std::string categoryName;
						for (SoundCategoryMap::iterator catIter = m_categories.begin(); 
							catIter != m_categories.end(); catIter++)
						{
							if (catIter->second->getCategoryKey() == categoryKey)
							{
								categoryName = catIter->first;
							}
						}

						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Processing update for category %lu (%s)",
							categoryKey, categoryName.c_str());
						
						TA_ASSERT(categoryName != "", "Category not found with given key");

						// Find the category in the map.
						SoundCategoryMap::iterator category = m_categories.find(categoryName);
						TA_ASSERT(category != m_categories.end(), "Category not found!");
						
						// Get all sounds in the category.
						std::vector<TA_Base_Core::ISoundFile*> soundData = TA_Base_Core::SoundFileAccessFactory::getInstance().
														getSoundFileByCategory(categoryKey);

						// Loop through all sounds in the category. One of them has changed, but we don't know
						// which one.
						for (std::vector<TA_Base_Core::ISoundFile*>::iterator soundIter = soundData.begin();
							soundIter != soundData.end(); soundIter++)
						{
							// Get the priority and filename of this sound.
							unsigned long priority = (*soundIter)->getPriority();
							std::string filename = (*soundIter)->getSoundFile();

							// Clean up the ISoundFile object
							delete (*soundIter);
							*soundIter = NULL;

							try
							{
								// In case the sound file has changed, tell the SoundCategory about it.
								category->second->setFile(priority, filename);
							}
							catch(TA_Base_Core::SoundPlayerException& ex)
							{
								LOG(SourceInfo, DebugUtil::ExceptionCatch,
									"SoundPlayerException", ex.what());
							}
						}
					}
					catch(TA_Base_Core::DataException&)
					{
						LOG(SourceInfo, DebugUtil::ExceptionCatch,
							"DataException", "Error retreiving sound file configuration.");
					}
					catch(TA_Base_Core::DatabaseException&)
					{
						LOG(SourceInfo, DebugUtil::ExceptionCatch,
							"DataException", "Error retreiving sound file configuration.");
					}
				}
			}
			break;
		}
		
		FUNCTION_EXIT;
	}

} // namespace TA_Base_App
