/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrintCfgAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * OnlinePrintCfgAccessFactory is a singleton that is used to retrieve OnlinePrintCfg objects either from the
  * database or newly created. All OnlinePrintCfg objects returned will adhear to the IOnlinePrintCfg interface.
  */



#if !defined(OnlinePrintCfgAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define OnlinePrintCfgAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IOnlinePrintCfg.h"
#include "core/data_access_interface/src/IConfigOnlinePrintCfg.h"




namespace TA_Base_Core
{

	class OnlinePrintCfgAccessFactory
	{
	public:
        typedef struct LocationRecord_s
        {
            std::string name;
            unsigned long key;
        } LocationRecord;


	private:

        // Disable the following methods
        OnlinePrintCfgAccessFactory() { };
		OnlinePrintCfgAccessFactory( const OnlinePrintCfgAccessFactory& theOnlinePrintCfgAccessFactory);
		OnlinePrintCfgAccessFactory& operator=(const OnlinePrintCfgAccessFactory &);
		void getOnlinePrintCfgBySql(IDatabase* pDatabase, const SQLStatement& sql, IOnlinePrintCfgs& OnlinePrintCfgs, const bool readWrite);


	public:

      
        /** 
         * ~OnlinePrintCfgAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~OnlinePrintCfgAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an OnlinePrintCfgAccessFactory object.
         */
		static OnlinePrintCfgAccessFactory& getInstance();


        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleans up the members.  
         * Primarily used upon program termination (e.g. from main()) so that Purify does not 
         * consider this class and its members to be memory leaks.
         *
         */
        static void removeInstance();

        std::vector<LocationRecord> getUnusedLocations();
        
        /**
         * getOnlinePrintCfg
         *
         * Retrieves the specified OnlinePrintCfg and returns the associated data as an 
         * object conforming to the IOnlinePrintCfg interface.
         *
         * @param   idKey      the primary key/id of the OnlinePrintCfg to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigOnlinePrintCfg objects or set to false to return 
         *                     readonly IOnlinePrintCfg objects.
         *
         * @return  A pointer to an object conforming to the IOnlinePrintCfg interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IOnlinePrintCfg object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no OnlinePrintCfg matching the supplied key
         *          - if there is more than one OnlinePrintCfg matching the supplied key. 
         *
         */
		IOnlinePrintCfg* getOnlinePrintCfg(const unsigned long idKey, const bool readWrite = false);

		
        /**
         * getAllOnlinePrintCfgs
         *
         * Retrieves all OnlinePrintCfgs and returns the associated data as a vector of  
         * objects conforming to the IOnlinePrintCfg interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigOnlinePrintCfg objects or set to false to return 
         *                     readonly IOnlinePrintCfg objects.
         *
         * @return  a vector of pointers to objects conforming to the IOnlinePrintCfg interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IOnlinePrintCfg object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no OnlinePrintCfg matching the supplied key
         *          - if there is more than one OnlinePrintCfg matching the supplied key. 
         *
         */
        IOnlinePrintCfgs getAllOnlinePrintCfgs(const bool readWrite = false );



        /**
         * createOnlinePrintCfg
         * 
         * Creates an empty IConfigOnlinePrintCfg object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigOnlinePrintCfg object.
         *
         * @return A pointer to an IConfigOnlinePrintCfg object
         *
         */
		IConfigOnlinePrintCfg* createOnlinePrintCfg();


        /**
         * copyOnlinePrintCfg
         * 
         * Creates a new configuration OnlinePrintCfg object using the OnlinePrintCfg passed in
         *
         * @param  OnlinePrintCfgToCopy the ConfigOnlinePrintCfg to copy
         *
         * @return the new object as a pointer to an IConfigOnlinePrintCfg object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigOnlinePrintCfg* copyOnlinePrintCfg(const IConfigOnlinePrintCfg* OnlinePrintCfgToCopy);


    private:
        static OnlinePrintCfgAccessFactory* m_instance;
	};

} // closes TA_Core

#endif // !defined(OnlinePrintCfgAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
