#if !defined(PROFILE_GROUP_H_INLCUDED)
#define PROFILE_GROUP_H_INLCUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ProfileGroup.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Action is an implementation of IProfileGroup. It holds the data specific to an action entry
  * in the database, and allows read-only access to that data.
  *
  */

#include "core/data_access_interface/src/IProfileGroup.h"
#include "core/data_access_interface/src/ProfileGroupHelper.h"

namespace TA_Base_Core
{
    class ProfileGroup : public IProfileGroup
    {

    public:

		static const std::string PROFILE_GROUP;
		 
        /**
         * Constructor
         *
         * Construct an ProfileGroup class based around a key.
         *
         * @param key The key of this ProfileGroup in the database
         */
        ProfileGroup(const unsigned long key );

        /**
         * Constructor
         *
         * Construct an ProfileGroup class based around all data 
         *
         * @param key The key of this ProfileGroup in the database
         * @param name The name of this ProfileGroup in the database
         * @param description The description of this ProfileGroup in the database
         */
        ProfileGroup(const unsigned long key, const std::string& name );

		/**
		 * Constructor
		 *
		 * Construct a ProfileGroup class based around dataset
		 */
		ProfileGroup(const unsigned long row, TA_Base_Core::IData& data);

		/**
		 * Destructor
		 *
		 */
		~ProfileGroup();

        
        /**
         * Refer to IProfileGroup.h for a description of this method.
         */
        virtual unsigned long getKey();

        /**
         * Refer to IProfileGroup.h for a description of this method.
         */
        virtual std::string getName();

        /**
         * getType
         *
         * Returns the profile group type name for this profile group.
         * Currently it is PROFILE as ITSI group is implemented in Radio DAI
         *	
         * @return The profile group type name as a string
         */
		virtual std::string getType();

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
		bool isGroupUsedInProfile(unsigned long key);

        /**
         * Refer to IProfileGroup.h for a description of this method.
         */
        virtual void invalidate();

    private:

        // Assignment operator not used so it is made private
        ProfileGroup( const ProfileGroup&);            
		ProfileGroup& operator=(const ProfileGroup&);

        unsigned long m_key;
        std::string m_name;
		std::string m_type;

		ProfileGroupHelper* m_profileGroupHelper;

    };
} // closes TA_Base_Core

#endif // !defined(PROFILE_GROUP_H_INLCUDED)
