#if !defined(ITSI_GROUP_H_INLCUDED)
#define ITSI_GROUP_H_INLCUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/ITSIGroup.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Action is an implementation of IITSIGroup. It holds the data specific to an action entry
  * in the database, and allows read-only access to that data.
  *
  */

#include "core/data_access_interface/radio/src/IITSIGroup.h"
#include "core/data_access_interface/src/ProfileGroupHelper.h"

namespace TA_Base_Core
{
    class ITSIGroup : public IITSIGroup
    {

    public:

		static const std::string ITSI_GROUP;

        /**
         * Constructor
         *
         * Construct an ITSIGroup class based around a key.
         *
         * @param key The key of this ITSIGroup in the database
         */
        ITSIGroup( const unsigned long key );

        /**
         * Constructor
         *
         * Construct an ITSIGroup class based around all data 
         *
         * @param key The key of this ITSIGroup in the database
         * @param name The name of this ITSIGroup in the database
         * @param description The description of this ITSIGroup in the database
         */
        ITSIGroup( const unsigned long key, const std::string& name );

		/**
		 * Destructor
		 *
		 */
		~ITSIGroup();

        
        /**
         * Refer to IITSIGroup.h for a description of this method.
         */
        virtual unsigned long getKey();

        /**
         * Refer to IITSIGroup.h for a description of this method.
         */
        virtual std::string getName();

        /**
         * Refer to IITSIGroup.h for a description of this method.
         */
        virtual void invalidate();

				/**
         * isGroupIsUsedInProfile
         *
         * Checks whether this profile group is used in profile table from the database. 
         * The calling application MUST NOT delete this object, as it will affect the
		 * constraint / relationship in the profile table.
         *
		 * @param key is the profile group primary key for check in profile table
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile group is to be deleted
         */
		virtual bool isGroupUsedInProfile(unsigned long key);

    private:

        // Assignment operator not used so it is made private
        ITSIGroup( const ITSIGroup&);            
		ITSIGroup& operator=(const ITSIGroup&);

        unsigned long m_key;
        std::string m_name;

		ProfileGroupHelper* m_ITSIGroupHelper;

    };
} // closes TA_Base_Core

#endif // !defined(ITSI_GROUP_H_INLCUDED)
