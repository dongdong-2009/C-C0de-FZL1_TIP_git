/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundFileAccessFactory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * SoundFileAccessFactory is a singleton that is used
  * to create ISoundFile and IConfigSoundFile objects.
  *
  */

#ifndef SOUNDFILEACCESSFACTORY_H
#define SOUNDFILEACCESSFACTORY_H

#include <string>
#include <vector>

namespace TA_Base_Core
{
	class ISoundFile;
	class IConfigSoundFile;

	class SoundFileAccessFactory
	{

	public:

		/**
		  * Destructor
		  */
		virtual ~SoundFileAccessFactory() {};


		/**
		  * getInstance
		  *
		  * Creates and returns an instance of this object.
		  *
		  * @return A reference to an instance of a SoundFileAccessFactory object.
		  */
		static SoundFileAccessFactory& getInstance();


        /**
          * getAllSoundFiles
          *
          * Returns a vector of pointers to ISoundFile objects for all rows in the
          * SO_SOUND_FILE table.
          *
          * @param readWrite Specifies whether the objects returned support writing.
          *
          * @return A vector of objects conforming to the ISoundFile interface.
          *
          * @exception DataException      Data was not found, or is not unique.
          * @exception DatabaseException  Problem communicating with the database.
          */
        std::vector<ISoundFile*> getAllSoundFiles( bool readWrite=false );


        /** 
          * getSoundFileByCategory
          *
          * Returns a vector of pointers to ISoundFile objects containing data about 
          * the sound file in the given category.
          *
          * @return A vector of objects conforming to the ISoundFile interface.
          *
          * @param categoryKey  The pkey of the category.
          *
          * @exception DataException      Data was not found, or is not unique.
          * @exception DatabaseException  Problem communicating with the database.
          */
		std::vector<ISoundFile*> getSoundFileByCategory(unsigned long categoryKey);


        /**
          * createSoundFile
          * 
          * Creates a new configuration SoundFile object.
          *
          * @return The configuration SoundFile object as a pointer to an IConfigSoundFile object.
          *
          * @exception DatabaseException Thrown if there is a database problem.
          * @exception DataException Thrown if there is bad data.
          */
        IConfigSoundFile* createSoundFile();

    
        /**
          * copySoundFile
          *
          * Creates a new sound file with the same data as the one provided.
          *
          * @param original A sound file object.
          *
          * @return IConfigSoundFile* A new sound object that has yet to be written to the db.
          *
          * @exception DatabaseException Thrown if there is a database problem.
          * @exception DataException Thrown if there is bad data.
          */
        IConfigSoundFile* copySoundFile( const IConfigSoundFile* original );

    private:

		// Made private as this is a singleton.
        SoundFileAccessFactory() {};
		SoundFileAccessFactory(const SoundFileAccessFactory&);
		SoundFileAccessFactory& operator=(const SoundFileAccessFactory &);

        static SoundFileAccessFactory* m_instance;
	};

} // namespace TA_Base_Core

#endif // SOUNDFILEACCESSFACTORY_H
