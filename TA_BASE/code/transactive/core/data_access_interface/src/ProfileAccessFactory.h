/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ProfileAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ProfileAccessFactory is a singleton that is used to retrieve Profile objects either from the
  * database or newly created. All Profile objects returned will adhear to the IProfile interface.
  *
  */



#if !defined(PROFILEACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PROFILEACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IDatabase.h"

namespace TA_Base_Core
{
    class IConfigProfile;
    class IProfile;
	class IDatabase;

	class ProfileAccessFactory
	{

	public:
        virtual ~ProfileAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ProfileAccessFactory object.
         */
		static ProfileAccessFactory& getInstance();


        /**
         * getProfile
         *
         * Retrieves the specified profile and returns the associated data as an 
         * object conforming to the IProfile interface.
         *
         * @param key The database key to the profile to retrieve
         *
         * @return A pointer to an object conforming to the IProfile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfile 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no profile
         *            matching the supplied PKEY, or if there is more than one profile
         *            matching the supplied PKEY. 
         */
		IProfile* getProfile(const unsigned long key, const bool readWrite = false);

        

        // TD 10468
        unsigned long getDeletedRawProfileType( unsigned long key);
        std::string getDeletedProfileName( unsigned long key);
        unsigned long getDeletedProfileGroup( unsigned long key);
        bool getDeletedProfileIsSystemProfile( unsigned long key);

        /** 
         * getProfilesByOperator
         *
         * Retreives all profiles belonging to an operator with the specified key and
		 * returns the associated data as a vector of objects conforming to the IProfile 
		 * interface.
         *
         * @return A vector of pointers to objects conforming to the IProfile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfile 
         * objects
         *
         * @param key        The pkey of the operator.
         * @param readWrite  True if write access is required.
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         *
         */
		std::vector<IProfile*> getProfilesByOperator(unsigned long key, const bool readWrite = false);

        /** 
         * getProfilesByOperator
         *
         * Retreives all profiles belonging to an operator with the specified name and
		 * returns the associated data as a vector of objects conforming to the IProfile 
		 * interface.
         *
         * @return A vector of pointers to objects conforming to the IProfile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfile 
         * objects
         *
         * @param name       The name of the operator.
         * @param readWrite  True if write access is required.
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         *
         */
        std::vector<IProfile*> getProfilesByOperator(const std::string& name, const bool readWrite = false);

        /**
         * getProfilesByLocation
         *
         * Retrieves all profiles that are enabled for the specified location.
         *
         * @return A vector of pointers to objects conforming to the IProfile interface that are assocated with 
         * the specified location.
         *
         * @param locationId The key of the location for which to retrieve profiles.
         * @param readWrite If true, then Config Profiles are returned.
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IProfile*> getProfilesByLocation(unsigned long locationId, const bool readWrite = false);

        /** 
         * getNonSystemProfilesByOperator
         *
         * Retreives all non-system profiles belonging to an operator with the specified key and
		 * returns the associated data as a vector of objects conforming to the IProfile 
		 * interface.
         *
         * @return A vector of pointers to objects conforming to the IProfile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfile 
         * objects
         *
         * @param key        The pkey of the operator.
         * @param readWrite  True if write access is required.
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         *
         */
		std::vector<IProfile*> getNonSystemProfilesByOperator(unsigned long key, const bool readWrite = false);
        
        /** 
         * getNonSystemProfilesByOperator
         *
         * Retreives all non-system profiles belonging to an operator with the specified name and
		 * returns the associated data as a vector of objects conforming to the IProfile 
		 * interface.
         *
         * @return A vector of pointers to objects conforming to the IProfile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfile 
         * objects
         *
         * @param name       The name of the operator.
         * @param readWrite  True if write access is required.
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         *
         */
        std::vector<IProfile*> getNonSystemProfilesByOperator(const std::string& name, const bool readWrite = false);
        
        /**
         * getAllProfiles
         *
         * Retrieves all specified profiles and returns the associated data as a vector of  
         * objects conforming to the IProfile interface.
         *
         * @return A vector of pointers to objects conforming to the IProfile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfile 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IProfile*> getAllProfiles( const bool readWrite = false );


        /**
         * getAllSystemProfiles
         *
         * Retrieves all system profiles and returns the associated data as a vector of  
         * objects conforming to the IProfile interface.
         *
         * @return A vector of pointers to objects conforming to the IProfile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfile 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IProfile*> getAllSystemProfiles( const bool readWrite = false );


        /**
         * getAllSystemProfiles
         *
         * Retrieves all system profiles and returns the associated data as a vector of  
         * objects conforming to the IProfile interface.
         *
         * @return A vector of pointers to objects conforming to the IProfile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfile 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IProfile*> getAllNonSystemProfiles( const bool readWrite = false );


        /**
         * createProfile
         * 
         * Creates an empty IConfigProfile object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigProfile object
         *
         * @return A pointer to an IConfigProfile object
         *
         */
		IConfigProfile* createProfile();


        /**
         * copyProfile
         * 
         * Creates a new configuration profile object using the profile passed in
         *
         * @param IConfigProfile* The profile to copy
         *
         * @return The configuration profile object as a pointer to an IConfigProfile object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigProfile* copyProfile(const IConfigProfile* profileToCopy);

        std::vector<std::string> getAllDistinctParameterNames();

        /**
         * getTransActiveSuperProfile
         *
         * Retrieves the super profile that behaves like a "God" profile. It bypasses
         * all the rights/duty rules.
         *
         * @return A pointer to an object conforming to the IProfile interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IProfile 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        IProfile* getTransActiveSuperProfile();

    private:
        //
        // These are private as this method is a singleton
        //
		ProfileAccessFactory() 
		{

		};
		ProfileAccessFactory( const ProfileAccessFactory& theProfileAccessFactory);
		ProfileAccessFactory& operator=(const ProfileAccessFactory &) 
		{ 
		};
		
		/**
         * getProfileBySql
         *
         * Retrieves profile by specified sql statement and returns the associated data as an 
         * object conforming to the IProfile interface.
         *
		 */
		void getProfileBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IProfile*>& profiles, const bool readWrite);

		static ProfileAccessFactory* m_instance;

        // Related to TD 8604: This is the name of the profile that acts like a "God"
        // profile. If this name is changed in the database scripts, it must be changed
        // here.
        static const std::string TRANSACTIVE_SUPER_PROFILE;
    };

} // closes TA_Base_Core

#endif // !defined(ProfileAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
