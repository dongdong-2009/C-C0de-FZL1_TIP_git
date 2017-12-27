/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundCategoryAccessFactory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * SoundCategoryAccessFactory is a singleton that is used
  * to create ISoundCategory and IConfigSoundCategory objects.
  *
  */

#ifndef SOUNDCATEGORYACCESSFACTORY_H
#define SOUNDCATEGORYACCESSFACTORY_H

#include <string>
#include <vector>

namespace TA_Base_Core
{
	class ISoundCategory;
	class IConfigSoundCategory;

	class SoundCategoryAccessFactory
	{

	public:

		/**
		  * Destructor
		  */
		virtual ~SoundCategoryAccessFactory() {};


		/**
		  * getInstance
		  *
		  * Creates and returns an instance of this object.
		  *
		  * @return A reference to an instance of a SoundFileAccessFactory object.
		  */
		static SoundCategoryAccessFactory& getInstance();


        /** 
          * getAllSoundCategories
          *
          * Returns a vector of pointers to ISoundCategory objects containing data about 
          * all sound categories.
          *
          * @return A vector of objects conforming to the ISoundCategory interface.
          *
          * @exception DataException      Data was not found, or is not unique.
          * @exception DatabaseException  Problem communicating with the database.
          */
		std::vector<ISoundCategory*> getAllSoundCategories();

        /**
          * getSoundCategory
          *
          * Returns a ISoundCategory object for the specified key.
          *
          * @param key The key of the category.
          *
          * @param ISoundCategory* The category object.
          */
        ISoundCategory* getSoundCategory( const unsigned long key );


        /**
          * createSoundCategory
          * 
          * Creates a new configuration SoundCategory object.
          *
          * @return The configuration SoundCategory object as a pointer to an IConfigSoundCategory object.
          *
          * @exception DatabaseException Thrown if there is a database problem.
          * @exception DataException Thrown if there is bad data.
          */
        IConfigSoundCategory* createSoundCategory();

    
    private:

		// Made private as this is a singleton.
        SoundCategoryAccessFactory() {};
		SoundCategoryAccessFactory(const SoundCategoryAccessFactory&);
		SoundCategoryAccessFactory& operator=(const SoundCategoryAccessFactory &);

        static SoundCategoryAccessFactory* m_instance;
	};

} // namespace TA_Base_Core

#endif // SOUNDCATEGORYACCESSFACTORY_H
