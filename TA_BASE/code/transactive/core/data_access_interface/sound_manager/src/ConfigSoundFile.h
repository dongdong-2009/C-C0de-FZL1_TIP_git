/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/ConfigSoundFile.h $
  * @author:  Justin Ebedes
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

#ifndef CONFIGSOUNDFILE_H
#define CONFIGSOUNDFILE_H

#include "core/data_access_interface/sound_manager/src/IConfigSoundFile.h"
#include <string>

namespace TA_Base_Core
{
    class SoundFileHelper;

    class ConfigSoundFile : public IConfigSoundFile
    {

    public:
        /**
         * Default Constructor
         *
         * Creates a new sound file entry.
         */
        ConfigSoundFile();


        /**
         * Constructor
         *
         * Loads an existing sound file.
         *
         * @param categoryKey The category to load the sound file for.
         * @param priority The priority to load the sound file for.
         */
        ConfigSoundFile( unsigned long categoryKey, unsigned long priority );


        /**
         * Copy Constructor
         *
         * Creates a copy of the current sound file.
         *
         * @param rhs The object to copy.
         */
        ConfigSoundFile( const ConfigSoundFile& rhs );

        /**
         * Destructor
         *
         * Cleans up the helper when no longer needed.
         */
        virtual ~ConfigSoundFile();

        ///////////////////////////////////
        // Methods from IConfigSoundFile //
        ///////////////////////////////////

        /** 
          * setCategoryKey
          *
          * Sets the sound category that this sound belongs to.
          *
          * @param pkey  The primary key of the sound category.
          */
		virtual void setCategoryKey(unsigned long pkey);


        /**
         * getCategoryName
         *
         * Returns the name of the category for this sound file.
         *
         * @return std::string The category name.
         */
        virtual std::string getCategoryName();


        /** 
          * setPriority
          *
          * Sets the priority associated with this sound.
          *
          * @param priority  The sound's priority.
          */
		virtual void setPriority(unsigned long priority);


        /**
          * setDescription
          *
          * Sets the description for this sound.
          *
          * @param description The description.
          */
        virtual void setDescription(std::string description);


        /** 
          * setSoundFile
          *
          * Sets the path and filename of the wave file containing
		  * this sound.
          *
          * @param file  The path to the wave file.
          */
		virtual void setSoundFile(std::string file);


        /**
          * deleteThisSoundFile
          *
          * Removes the current sound file from the database.
          */
        virtual void deleteThisSoundFile();

        /////////////////////////////
        // Methods from ISoundFile //
        /////////////////////////////

        /** 
          * getCategoryKey
          *
          * Returns the pkey of the category to which this sound belongs.
          *
          * @return The sound category's key.
          *
          */
	    virtual unsigned long getCategoryKey();


        /** 
          * getPriority
          *
          * Returns the priority associated with this sound.
          *
          * @return The sound's priority.
          *
          */
	    virtual unsigned long getPriority();


        /**
          * getDescription
          *
          * Returns the description for this sound.
          *
          * @return std::string The description.
          */
        virtual std::string getDescription();

        
        /** 
          * getSoundFile
          *
          * Returns the filename (including the full path) to the wave file
		  * containing the sound.
          *
          * @return The sound's path and filename.
          *
          */
	    virtual std::string getSoundFile();


        /**
          * getDateCreated
          *
          * Returns the date at which the sound file entry was created.
          *
          * @return time_t The creation time.
          */
        virtual time_t getDateCreated();


        /**
          * getDateModified
          *
          * Returns the date at which the sound file entry was last changed.
          *
          * @return time_t The modification time.
          */
        virtual time_t getDateModified();


        ////////////////////////
        // Methods from IItem //
        ////////////////////////

        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();
		

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


        //////////////////////////////
        // Methods from IConfigItem //
        //////////////////////////////

        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier();


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged();


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();

        
        /**
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name);


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();

    private:
        // Disable methods that aren't required.
        ConfigSoundFile& operator=( const ConfigSoundFile& );

        /**
         * updateChanges
         *
         * Modifies the change list whenever a new change is made.
         *
         * @param name The name of the attribute whose value has changed.
         * @param oldValue The value of the attribute before the change.
         * @param newValue The value of the attribute after the change.
         */
        void updateChanges( const std::string& name, const std::string& oldValue, const std::string& newValue );

        // Strings for naming changes.
        static const std::string CATEGORY;
        static const std::string PRIORITY;
        static const std::string DESCRIPTION;
        static const std::string SOUNDFILE;

        // The static member used to keep track of identifiers.
        static unsigned long s_nextIdentifier;

        // The unique number used to represent this sound file configuration item.
        unsigned long m_uniqueIdentifier;

        // The helper used to access the database.
        SoundFileHelper* m_helper;

        // The map that keeps track of changes to this sound file.
        ItemChanges m_soundFileChanges;
    };
    
} // namespace TA_Base_Core

#endif // CONFIGSOUNDFILE_H
