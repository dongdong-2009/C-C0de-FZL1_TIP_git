/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/IConfigSoundFile.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface contains methods that provide read and
  * write access to an entry in the so_sound_file table.
  *
  */

#ifndef ICONFIGSOUNDFILE_H
#define ICONFIGSOUNDFILE_H

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/sound_manager/src/ISoundFile.h"
#include <string>

namespace TA_Base_Core
{
    class IConfigSoundFile : virtual public ISoundFile, virtual public IConfigItem
    {

    public:

        /** 
          * setCategoryKey
          *
          * Sets the sound category that this sound belongs to.
          *
          * @param pkey  The primary key of the sound category.
          */
		virtual void setCategoryKey(unsigned long pkey) = 0;


        /**
         * getCategoryName
         *
         * Returns the name of the category for this sound file.
         *
         * @return std::string The category name.
         */
        virtual std::string getCategoryName() =0;

        /** 
          * setPriority
          *
          * Sets the priority associated with this sound.
          *
          * @param priority  The sound's priority.
          */
		virtual void setPriority(unsigned long priority) = 0;


        /**
          * setDescription
          *
          * Sets the description for this sound.
          *
          * @param description The description.
          */
        virtual void setDescription(std::string description) = 0;


        /** 
          * setSoundFile
          *
          * Sets the path and filename of the wave file containing
		  * this sound.
          *
          * @param file  The path to the wave file.
          */
		virtual void setSoundFile(std::string file) = 0;


        /**
          * deleteThisSoundFile
          *
          * Removes the current sound file from the database.
          */
        virtual void deleteThisSoundFile() = 0;
    };
    
} // namespace TA_Base_Core

#endif // ICONFIGSOUNDFILE_H
