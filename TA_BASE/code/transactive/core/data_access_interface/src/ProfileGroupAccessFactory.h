/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ProfileGroupAccessFactory.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ProfileGroupAccessFactory is a singleton that is used to retrieve profile group objects either from the
  * database or newly created. All profile group objects returned will adhere to the IProfileGroup interface.
  * Data is primarily retrieved from the SE_PROFILE_GROUP table
  *
  */


#if !defined(PROFILEGROUPACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PROFILEGROUPACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class IConfigProfileGroup;
	class IProfileGroup;

	class ProfileGroupAccessFactory
	{

	public:
        virtual ~ProfileGroupAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ProfileGroupAccessFactory object.
         */
		static ProfileGroupAccessFactory& getInstance();

		
        /**
         * getProfileGroup
         *
         * Retrieves the profile group of the key provided.
         *
         * @return A pointer to an object conforming to the IProfileGroup interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfileGroup
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        IProfileGroup* getProfileGroup( const unsigned long ProfileGroupKey, const bool readWrite = false );

        /**
         * getAllProfileGroups
         *
         * Retrieves all specified profile groups and returns the associated data as a vector of  
         * objects conforming to the IProfileGroup interface.
         *
         * @return A vector of pointers to objects conforming to the IProfileGroup interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfileGroup
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IProfileGroup*> getAllProfileGroups(const bool readWrite = false);

        /**
         * createProfileGroup
         * 
         * Creates an empty IConfigProfileGroup object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigProfileGroup object
         *
         * @return A pointer to an IConfigProfileGroup object
         *
         */
		IConfigProfileGroup* createProfileGroup();

        /**
         * copyProfileGroup
         * 
         * Creates a new configuration profile group object using the profile passed in
         *
         * @param IConfigProfileGroup* The profile to copy
         *
         * @return The configuration profile group object as a pointer to an IConfigProfileGroup object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigProfileGroup* copyProfileGroup(const IConfigProfileGroup* profileGpToCopy);

    private:
        //
        // These are private as this method is a singleton
        //
		ProfileGroupAccessFactory() 
		{

		};
		ProfileGroupAccessFactory( const ProfileGroupAccessFactory& theProfileGroupAccessFactory);
		ProfileGroupAccessFactory& operator=(const ProfileGroupAccessFactory &);
		
		void getProfileGroupBySql(IDatabase* pDatabase,const SQLStatement& sql, std::vector<IProfileGroup*>& profileGroups, const bool readWrite);


		static ProfileGroupAccessFactory* m_instance;
	};

} // closes TA_Base_Core

#endif // !defined(ProfileGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
