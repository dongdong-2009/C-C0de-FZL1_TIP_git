/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/sound_file/src/SoundFileDatabaseAccessor.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve alarm plan association information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/sound_file/src/StdAfx.h"
#include "app/configuration/config_plugins/sound_file/src/SoundFileDatabaseAccessor.h"

#include "core/data_access_interface/sound_manager/src/ISoundCategory.h"
#include "core/data_access_interface/sound_manager/src/SoundCategoryAccessFactory.h"
#include "core/data_access_interface/sound_manager/src/IConfigSoundFile.h"
#include "core/data_access_interface/sound_manager/src/SoundFileAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    SoundFileDatabaseAccessor::SoundFileDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    SoundFileDatabaseAccessor::~SoundFileDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for ( LoadedSoundFiles::iterator iter = m_soundFiles.begin(); iter != m_soundFiles.end(); ++iter )
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void SoundFileDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if ( m_isLoaded )
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the sound files and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for ( LoadedSoundFiles::iterator soundFilesToDelete = m_soundFiles.begin();
              soundFilesToDelete != m_soundFiles.end();
              soundFilesToDelete++ )
        {
            if( NULL != soundFilesToDelete->second )
            {
                delete soundFilesToDelete->second;
                soundFilesToDelete->second = NULL;
            }
        }
        m_soundFiles.clear();


        // Now reload all the sound files.
        std::vector<TA_Base_Core::ISoundFile*> soundFiles = TA_Base_Core::SoundFileAccessFactory::getInstance().getAllSoundFiles( true );

        // And insert the sound files into our map. At the same time cast them to IConfigSoundFile*
        // instead of ISoundFile*.
        for ( std::vector<TA_Base_Core::ISoundFile*>::iterator iter = soundFiles.begin();
             iter != soundFiles.end();
             iter++ )
        {
             TA_Base_Core::IConfigSoundFile* castItem = dynamic_cast<TA_Base_Core::IConfigSoundFile*>(*iter);
             if( castItem != NULL )
             {
                m_soundFiles.insert( LoadedSoundFiles::value_type( castItem->getUniqueIdentifier(), castItem ) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> SoundFileDatabaseAccessor::getItemNames( CProgressCtrl& progress )
    {
        TA_ASSERT(false, "This method cannot be called for sound files");

        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,SoundFileDatabaseAccessor::SoundFile> SoundFileDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,SoundFile> details; 

        // The maximum range is the number of sound files times by 2. This is because we want to step for each
        // sound file while it is loaded and then again while each sound file is added to the list.
        progress.SetRange( 0, m_soundFiles.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for ( LoadedSoundFiles::iterator iter = m_soundFiles.begin();
              iter != m_soundFiles.end();
              iter++ )
        {
            if ( NULL != iter->second )
            {
                SoundFile item;
                item.category = iter->second->getCategoryName().c_str();
                item.priority.Format( "%d", iter->second->getPriority() );
                item.description = iter->second->getDescription().c_str();
                item.soundfile = iter->second->getSoundFile().c_str();

                details.insert( std::map<unsigned long, SoundFile>::value_type( iter->second->getUniqueIdentifier(), item ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    SoundFileDatabaseAccessor::SoundFile SoundFileDatabaseAccessor::getItemDetails( unsigned long id )
    {
        FUNCTION_ENTRY("getItemDetails");

        SoundFile item;

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedSoundFiles::iterator matching = m_soundFiles.find( id );
        if ( matching == m_soundFiles.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the sound file with identifier %lu", id );
        }
        else
        {
            item.category  = matching->second->getCategoryName().c_str();
            item.priority.Format( "%d", matching->second->getPriority() );
            item.description = matching->second->getDescription().c_str();
            item.soundfile = matching->second->getSoundFile().c_str();
        }

        FUNCTION_EXIT;
        return item;
    }


    TA_Base_Core::IConfigItem* SoundFileDatabaseAccessor::getItem( unsigned long key )
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedSoundFiles::iterator matching = m_soundFiles.find( key );
        if ( matching == m_soundFiles.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the sound file with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void SoundFileDatabaseAccessor::deleteItem( unsigned long key )
    {
        TA_ASSERT(false, "Sound files cannot be deleted.");
    }


    TA_Base_Core::IConfigItem* SoundFileDatabaseAccessor::newItem()
    {
        TA_ASSERT(false, "Sound files cannot be created.");
        return NULL;
    }


    TA_Base_Core::IConfigItem* SoundFileDatabaseAccessor::copyItem( const unsigned long idOfItemToCopy )
    {
        TA_ASSERT(false, "Sound files cannot be copied.");
        return NULL;
    }


    bool SoundFileDatabaseAccessor::areCurrentChangesPending( std::vector<std::string>& itemsNotApplied )
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for ( LoadedSoundFiles::iterator iter = m_soundFiles.begin(); iter != m_soundFiles.end();  ++iter)
        {
            if( (iter->second != NULL) && (iter->second->hasChanged() || iter->second->isNew()) )
            {
                // We just add a single empty item if we know a change has been made. We don't list each
                // individual change for alarm plan associations
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }
}
