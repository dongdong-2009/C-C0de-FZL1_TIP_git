/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/ISoundFile.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface contains methods that provide read-only 
  * access to an entry in the so_sound_file table.
  *
  */

#ifndef ISOUNDFILE_H
#define ISOUNDFILE_H

#include "core/data_access_interface/src/IItem.h"
#include <string>

namespace TA_Base_Core
{
    class ISoundFile : public IItem
    {

    public:

        /** 
          * getCategoryKey
          *
          * Returns the pkey of the category to which this sound belongs.
          *
          * @return The sound category's key.
          *
          */
        virtual unsigned long getCategoryKey() = 0;


        /** 
          * getPriority
          *
          * Returns the priority associated with this sound.
          *
          * @return The sound's priority.
          *
          */
        virtual unsigned long getPriority() = 0;


        /**
          * getDescription
          *
          * Returns the description for this sound.
          *
          * @return std::string The description.
          */
        virtual std::string getDescription() = 0;


        /** 
          * getSoundFile
          *
          * Returns the filename (including the full path) to the wave file
		  * containing the sound.
          *
          * @return The sound's path and filename.
          *
          */
        virtual std::string getSoundFile() = 0;


        /**
          * getDateCreated
          *
          * Returns the date at which the sound file entry was created.
          *
          * @return time_t The creation time.
          */
        virtual time_t getDateCreated() = 0;


        /**
          * getDateModified
          *
          * Returns the date at which the sound file entry was last changed.
          *
          * @return time_t The modification time.
          */
        virtual time_t getDateModified() = 0;

    };
    
} // namespace TA_Base_Core

#endif // ISOUNDFILE_H
