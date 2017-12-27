/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundFile.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is an implementation of the ISoundFile 
  * interface. It provides read-only access to an 
  * entry in the so_sound_file table.
  *
  */

#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#include "ISoundFile.h"

#include <string>

namespace TA_Base_Core
{

	class SoundFileHelper; // Forward declaration

    class SoundFile : public ISoundFile
    {

    public:

		/**
		  * Constructor
		  */
		SoundFile(unsigned long categoryKey, unsigned long priority);


		/**
		  * Destructor
		  */
		~SoundFile();


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

	private:

		SoundFileHelper* m_helper;

    };
    
} // namespace TA_Base_Core

#endif // SOUNDFILE_H
