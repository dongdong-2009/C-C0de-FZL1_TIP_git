/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundCategoryHelper.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class contains the data associated with one
  * entry in the so_sound_category table.
  *
  */

#ifndef SOUNDCATEGORYHELPER_H
#define SOUNDCATEGORYHELPER_H

#include <string>

namespace TA_Base_Core
{
    class SoundCategoryHelper
    {

    public:

        /**
          * Constructor
          *
          * Creates an object for an existing SoundCategory.
          */
        SoundCategoryHelper(unsigned long categoryKey);


        /**
          * Constructor
          *
          * Creates an object for a new SoundCategory.
          */
        SoundCategoryHelper();


        /**
          * Copy Constructor
          *
          * Creates an object of a new SoundCategoryHelper copied from an existing one.
          * This should only be used by configuration SoundCategoryHelper objects.
          */
        SoundCategoryHelper( const SoundCategoryHelper& );


		/**
          * Destructor
		  */
        virtual ~SoundCategoryHelper();
  

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

        
        /** 
          * setCategoryKey
          *
          * Sets the pkey of this category.
          *
          * @param pkey  The primary key of the sound category.
          */
		virtual void setCategoryKey(unsigned long pkey);


        /** 
          * setName
          *
          * Sets the name of this category.
          *
          * @param name  The sound category's name.
          */
		virtual void setName(std::string name);


        /** 
          * setPosition
          *
          * Sets the sound category's position in the sequence of sounds.
		  * Used to specify the order in which the categories are played.
          *
          * @param position  The sound category's position.
          */
		virtual void setPosition(unsigned long position);


        /**
          * deleteThisSoundCategory
          *
          * Removes this SoundCategory from the database. The calling application MUST then delete
          * this SoundCategory object, as it makes no sense to keep it any longer.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a 
          *            problem writing the data to the database.
          *
          * pre: Either - this system controller was initially loaded from the database
          *      OR     - applyChanges() has already been called
          *      This SoundCategory has not been deleted
          */
        void deleteThisSoundCategory();


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


    private:

		SoundCategoryHelper& operator=( const SoundCategoryHelper& );

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
        std::string m_name;
		unsigned long m_position;

    };

} // namespace TA_Base_Core

#endif // SOUNDCATEGORYHELPER_H
