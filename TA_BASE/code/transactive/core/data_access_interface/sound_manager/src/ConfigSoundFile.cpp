/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/ConfigSoundFile.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is an implementation of the IConfigSoundFile 
  * interface. It provides read and write access to an 
  * entry in the so_sound_file table.
  *
  */

#include "core/data_access_interface/sound_manager/src/ConfigSoundFile.h"
#include "core/data_access_interface/sound_manager/src/SoundFileHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    const std::string ConfigSoundFile::CATEGORY( "Category" );
    const std::string ConfigSoundFile::PRIORITY( "Priority" );
    const std::string ConfigSoundFile::DESCRIPTION( "Description" );
    const std::string ConfigSoundFile::SOUNDFILE( "Sound File" );

    unsigned long ConfigSoundFile::s_nextIdentifier = 0;

    ConfigSoundFile::ConfigSoundFile() :
        m_uniqueIdentifier(++s_nextIdentifier),
        m_helper(NULL)
    {
        m_helper = new SoundFileHelper();
    }


    ConfigSoundFile::ConfigSoundFile( unsigned long categoryKey, unsigned long priority ) :
        m_uniqueIdentifier(++s_nextIdentifier),
        m_helper(NULL)
    {
        m_helper = new SoundFileHelper( categoryKey, priority );
    }


    ConfigSoundFile::ConfigSoundFile( const ConfigSoundFile& rhs ) :
        m_uniqueIdentifier(++s_nextIdentifier),
        m_helper( new SoundFileHelper( *rhs.m_helper ) )
    {
    }


    ConfigSoundFile::~ConfigSoundFile()
    {
        delete m_helper;
        m_helper = NULL;
    }


    ///////////////////////////////////
    // Methods from IConfigSoundFile //
    ///////////////////////////////////

	void ConfigSoundFile::setCategoryKey( unsigned long key )
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");

        // Convert the categories to strings.
        std::string oldCategory( getCategoryName() );
        std::string newCategory( m_helper->retrieveCategoryName( key ) );

        // Keep track of the changes.
        updateChanges( CATEGORY, oldCategory, newCategory );

        // Update the stored value.
        m_helper->setCategoryKey( key );        
    }


    std::string ConfigSoundFile::getCategoryName()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->retrieveCategoryName( m_helper->getCategoryKey() );
    }


	void ConfigSoundFile::setPriority( unsigned long priority )
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");

        // Convert the priorities to strings.
        std::stringstream oldPriority;
        oldPriority << m_helper->getPriority();
        std::stringstream newPriority;
        newPriority << priority;

        // Keep track of the changes.
        updateChanges( PRIORITY, oldPriority.str(), newPriority.str() );

        // Update the stored value.
        m_helper->setPriority( priority );
    }


    void ConfigSoundFile::setDescription(std::string description)
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");

        // Conver the sound files to strings.
        std::string oldDescription( m_helper->getDescription() );
        std::string newDescription( description );

        // Keep track of the changes.
        updateChanges( DESCRIPTION, oldDescription, newDescription );

        // Update the stored value.
        m_helper->setDescription( description );
    }


	void ConfigSoundFile::setSoundFile(std::string file)
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");

        // Conver the sound files to strings.
        std::string oldSoundFile( m_helper->getSoundFile() );
        std::string newSoundFile( file );

        // Keep track of the changes.
        updateChanges( SOUNDFILE, oldSoundFile, newSoundFile );

        // Update the stored value.
        m_helper->setSoundFile( file );
    }


    void ConfigSoundFile::deleteThisSoundFile()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        m_helper->deleteThisSoundFile();
    }

    /////////////////////////////
    // Methods from ISoundFile //
    /////////////////////////////

	unsigned long ConfigSoundFile::getCategoryKey()
	{
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getCategoryKey();
	}


	unsigned long ConfigSoundFile::getPriority()
	{
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getPriority();
	}


    std::string ConfigSoundFile::getDescription()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getDescription();
    }


	std::string ConfigSoundFile::getSoundFile()
	{
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getSoundFile();
	}


    time_t ConfigSoundFile::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getDateCreated();
    }


    time_t ConfigSoundFile::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getDateModified();
    }


    ////////////////////////
    // Methods from IItem //
    ////////////////////////


    unsigned long ConfigSoundFile::getKey()
    {
		// Fix TD15687 
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getCategoryKey();
		// ajlaredo++ 
    }


    std::string ConfigSoundFile::getName()
    {
        // Nothing to do here - Sound files don't have a name.
        return "";
    }
		

    void ConfigSoundFile::invalidate()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");

        // Invalidate the data and reset the change list.
        m_helper->invalidate();
        m_soundFileChanges.clear();
    }
    

    //////////////////////////////
    // Methods from IConfigItem //
    //////////////////////////////

    unsigned long ConfigSoundFile::getUniqueIdentifier()
    {
        return m_uniqueIdentifier;
    }


    ItemChanges ConfigSoundFile::getAllItemChanges()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");

        return m_soundFileChanges;
    }

        
    bool ConfigSoundFile::hasChanged()
    {
        return !m_soundFileChanges.empty();
    }


    bool ConfigSoundFile::isNew()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->isNew();
    }

        
    void ConfigSoundFile::setName(const std::string& name)
    {
        // Nothing to do here - Sound Files don't have a 'name'.
    }


    void ConfigSoundFile::applyChanges()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");

        // Apply the changes and then reset the change list.
        m_helper->applyChanges();
        m_soundFileChanges.clear();
    }

    void ConfigSoundFile::updateChanges( const std::string& name, const std::string& oldValue, const std::string& newValue )
    {
        FUNCTION_ENTRY("updateChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_soundFileChanges.find( name );

        if ( matching != m_soundFileChanges.end() )
        {
            // We already have this attribute name in the list.
            if ( newValue == matching->second.oldValue )
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_soundFileChanges.erase( matching );
            }
            else
            {
                // Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if ( oldValue != newValue )
        {
            // If the old and new values are different then we can add this item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_soundFileChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

} // namespace TA_Base_Core
