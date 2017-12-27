/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrinterAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * OnlinePrinterAccessFactory is a singleton that is used to retrieve OnlinePrinter objects either from the
  * database or newly created. All OnlinePrinter objects returned will adhear to the IOnlinePrinter interface.
  */



#if !defined(OnlinePrinterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define OnlinePrinterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IOnlinePrinter.h"
#include "core/data_access_interface/src/IConfigOnlinePrinter.h"




namespace TA_Base_Core
{

	class OnlinePrinterAccessFactory
	{
	public:
        typedef struct SubsystemRecord_s
        {
            std::string name;
            unsigned long key;
        } SubsystemRecord;


	private:

        // Disable the following methods
        OnlinePrinterAccessFactory() : m_isSubsystemsLoaded(false) { };
		OnlinePrinterAccessFactory( const OnlinePrinterAccessFactory& theOnlinePrinterAccessFactory);
		OnlinePrinterAccessFactory& operator=(const OnlinePrinterAccessFactory &);

		void getOnlinePrinterBySql(IDatabase* pDatabase, const SQLStatement& sql, IOnlinePrinters& OnlinePrinters, const bool readWrite);
	public:

      
        /** 
         * ~OnlinePrinterAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~OnlinePrinterAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an OnlinePrinterAccessFactory object.
         */
		static OnlinePrinterAccessFactory& getInstance();


        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleans up the members.  
         * Primarily used upon program termination (e.g. from main()) so that Purify does not 
         * consider this class and its members to be memory leaks.
         *
         */
        static void removeInstance();

		

        std::vector<SubsystemRecord> getSubsystems();

        /**
         * getOnlinePrinter
         *
         * Retrieves the specified OnlinePrinter and returns the associated data as an 
         * object conforming to the IOnlinePrinter interface.
         *
         * @param   idKey      the primary key/id of the OnlinePrinter to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigOnlinePrinter objects or set to false to return 
         *                     readonly IOnlinePrinter objects.
         *
         * @return  A pointer to an object conforming to the IOnlinePrinter interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IOnlinePrinter object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no OnlinePrinter matching the supplied key
         *          - if there is more than one OnlinePrinter matching the supplied key. 
         *
         */
		IOnlinePrinter* getOnlinePrinter(const unsigned long idKey, const bool readWrite = false);

        /**
         * getAllOnlinePrinters
         *
         * Retrieves all OnlinePrinters and returns the associated data as a vector of  
         * objects conforming to the IOnlinePrinter interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigOnlinePrinter objects or set to false to return 
         *                     readonly IOnlinePrinter objects.
         *
         * @return  a vector of pointers to objects conforming to the IOnlinePrinter interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IOnlinePrinter object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no OnlinePrinter matching the supplied key
         *          - if there is more than one OnlinePrinter matching the supplied key. 
         *
         */
        IOnlinePrinters getAllOnlinePrinters(unsigned long pLocation, const bool readWrite = false );



        /**
         * createOnlinePrinter
         * 
         * Creates an empty IConfigOnlinePrinter object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigOnlinePrinter object.
         *
         * @return A pointer to an IConfigOnlinePrinter object
         *
         */
		IConfigOnlinePrinter* createOnlinePrinter();


        /**
         * copyOnlinePrinter
         * 
         * Creates a new configuration OnlinePrinter object using the OnlinePrinter passed in
         *
         * @param  OnlinePrinterToCopy the ConfigOnlinePrinter to copy
         *
         * @return the new object as a pointer to an IConfigOnlinePrinter object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigOnlinePrinter* copyOnlinePrinter(const IConfigOnlinePrinter* OnlinePrinterToCopy);


    private:
        static OnlinePrinterAccessFactory* m_instance;

        std::vector<SubsystemRecord> m_subsystems;
        bool m_isSubsystemsLoaded;
	};

} // closes TA_Core

#endif // !defined(OnlinePrinterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
