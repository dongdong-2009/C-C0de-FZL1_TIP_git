/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundFileHelper.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class contains the data associated with one
  * entry in the so_sound_file table.
  *
  */

#ifndef SOUNDFILEHELPER_H
#define SOUNDFILEHELPER_H

#include <string>

namespace TA_Base_Core
{
    class SoundFileHelper
    {

    public:

        /**
          * Constructor
          *
          * Creates an object for an existing SoundFile.
          */
        SoundFileHelper(unsigned long categoryKey, unsigned long priority);


        /**
          * Constructor
          *
          * Creates an object for a new SoundFile.
          */
        SoundFileHelper();


        /**
          * Copy Constructor
          *
          * Creates an object of a new SoundFile copied from an existing one.
          * This should only be used by configuration SoundFile objects.
          */
        SoundFileHelper( const SoundFileHelper& );


		/**
          * Destructor
		  */
        virtual ~SoundFileHelper();
  

        /**
         * isNew
         * 
         * @return Whether or not this object is new (meaning it has not yet been applied to the database).
         */
        bool isNew() const
        {
            return m_isNew;
        }


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
          * Returns the description associated with this sound.
          *
          * @return std::string The description of what the sound represents.
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
        
        /** 
          * setCategoryKey
          *
          * Sets the sound category that this sound belongs to.
          *
          * @param pkey  The primary key of the sound category.
          */
		virtual void setCategoryKey(unsigned long pkey);


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
          * Sets the description associated with the sound file.
          *
          * @param description The new description.
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
          * Removes this SoundFile from the database. The calling application MUST then delete
          * this SoundFile object, as it makes no sense to keep it any longer.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a 
          *            problem writing the data to the database.
          *
          * pre: Either - this system controller was initially loaded from the database
          *      OR     - applyChanges() has already been called
          *      This SoundFile has not been deleted
          */
        void deleteThisSoundFile();


        /**
          * applyChanges
          *
          * This will apply all changes made to the database.
          *
          * @exception DatabaseException Thrown if there is a database error.
          * @exception DataException Thrown if a parameter name cannot be found, or if
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
        void applyChanges();


        /**
          * invalidate
          *
          * Mark the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */
        void invalidate();


        /**
         * retrieveCategoryName
         *
         * Retrieves the name for the specified category.
         *
         * @param key The key of the category.
         *
         * @return std::string The name of the category.
         */
        std::string retrieveCategoryName( const unsigned long key );


    private:

		SoundFileHelper& operator=( const SoundFileHelper& );

        /**
          * reload()
          *
          * This method reloads the data from the database. It is called when a get
          * method is called and the data state is not valid.
          *
          * @exception DatabaseException
          * @exception DataException
          */
        void reload();


        /**
          * modifyExisting
          *
          * This will update an existing SoundFile in the database with the settings currently saved here.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a 
          *            problem writing the data to the database.
          * @exception DataConfigurationException If the data contained in the system controller object
          *            is not sufficent to create an entry in the database, a 
          *            DataConfigurationException will be thrown. This is thrown as an exception
          *            and not tested as an assert as it can be directly linked to GUI components
          *            and may be the result of a user action that occurs prior to filling in 
          *            all sections.
          */
        void modifyExisting();

        
        /**
          * addNew
          *
          * This will add a new SoundFile in the database with the settings currently saved here.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a 
          *            problem writing the data to the database.
          * @exception DataConfigurationException If the data contained in the system controller object
          *            is not sufficent to create an entry in the database, a 
          *            DataConfigurationException will be thrown. This is thrown as an exception
          *            and not tested as an assert as it can be directly linked to GUI components
          *            and may be the result of a user action that occurs prior to filling in 
          *            all sections.
          */
        void addNew();



        bool m_isValidData;
        bool m_isNew;

        unsigned long m_categoryKey;
		unsigned long m_priority;
        std::string m_description;
        std::string m_soundFile;
        time_t m_dateCreated;
        time_t m_dateModified;
    };

} // namespace TA_Base_Core

#endif // SOUNDFILEHELPER_H
