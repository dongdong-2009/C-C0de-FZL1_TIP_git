/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundCategory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is an implementation of the ISoundCategory 
  * interface. It provides read-only access to an 
  * entry in the so_sound_category table.
  *
  */

#ifndef SOUNDCATEGORY_H
#define SOUNDCATEGORY_H

#include "ISoundCategory.h"

#include <string>

namespace TA_Base_Core
{

	class SoundCategoryHelper; // Forward declaration

    class SoundCategory : public ISoundCategory
    {

    public:

		/**
		  * Constructor
		  */
		SoundCategory(unsigned long categoryKey);


		/**
		  * Destructor
		  */
		~SoundCategory();


        /** 
          * getCategoryKey
          *
          * Returns the pkey of the category.
          *
          * @return The sound category's key.
          *
          */
        virtual unsigned long getCategoryKey();


        /** 
          * getName
          *
          * Returns the name of the category.
          *
          * @return The category's name.
          *
          */
        virtual std::string getName();


        /** 
          * getPosition
          *
          * Returns this category's position in the sequence of sounds. So, this
		  * specifies when the sound category is played.
          *
          * @return The sound category's position.
          *
          */
        virtual unsigned long getPosition();



	private:

		SoundCategoryHelper* m_helper;

    };
    
} // namespace TA_Base_Core

#endif // SOUNDCATEGORY_H
