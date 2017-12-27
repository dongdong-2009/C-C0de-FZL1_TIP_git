/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/ITSIGroupAccessFactory.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ITSIGroupAccessFactory is a singleton that is used to retrieve radio login group objects either from the
  * database or newly created. All radio login group objects returned will adhere to the IITSIGroup interface.
  * Data is primarily retrieved from the RA_ITSI_GROUP table
  *
  */


#if !defined(ITSIGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ITSIGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class IITSIGroup;
	class IConfigITSIGroup;

	class ITSIGroupAccessFactory
	{

	public:
        virtual ~ITSIGroupAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ITSIGroupAccessFactory object.
         */
		static ITSIGroupAccessFactory& getInstance();


        /**
         * getITSIGroup
         *
         * Retrieves the radio login group of the key provided.
         *
         * @return A pointer to an object conforming to the IITSIGroup interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IITSIGroup
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
        IITSIGroup* getITSIGroup( const unsigned long ITSIGroupKey, const bool readWrite = false );


        /**
         * getAllITSIGroups
         *
         * Retrieves all specified radio login groups and returns the associated data as a vector of  
         * objects conforming to the IITSIGroup interface.
         *
         * @return A vector of pointers to objects conforming to the IITSIGroup interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IITSIGroup
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
        std::vector<IITSIGroup*> getAllITSIGroups(const bool readWrite = false);

        /**
         * createITSIGroup
         * 
         * Creates an empty IConfigITSIGroup object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigITSIGroup object
         *
         * @return A pointer to an IConfigITSIGroup object
         *
         */
		IConfigITSIGroup* createITSIGroup();

        /**
         * copyITSIGroup
         * 
         * Creates a new configuration ITSI group object using the profile passed in
         *
         * @param IConfigITSIGroup* The profile to copy
         *
         * @return The configuration profile group object as a pointer to an IConfigITSIGroup object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigITSIGroup* copyITSIGroup(const IConfigITSIGroup* itsiGpToCopy);

    private:
        //
        // These are private as this method is a singleton
        //
		ITSIGroupAccessFactory();
		ITSIGroupAccessFactory( const ITSIGroupAccessFactory& theITSIGroupAccessFactory);
		ITSIGroupAccessFactory& operator=(const ITSIGroupAccessFactory &);
    };

} // closes TA_Base_Core

#endif // !defined(ITSIGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
