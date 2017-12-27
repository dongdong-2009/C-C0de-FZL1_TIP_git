/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTrainDvaMessageAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PaTrainDvaMessageAccessFactory is a singleton that is used to retrieve PaTrainDvaMessage objects either from the
  * database or newly created. All PaTrainDvaMessage objects returned will adhere to the IPaTrainDvaMessage interface.
  */



#if !defined(PaTrainDvaMessageAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PaTrainDvaMessageAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/IPaTrainDvaMessage.h"
#include "core/data_access_interface/pa/src/IConfigPaTrainDvaMessage.h"



namespace TA_Base_Core
{

    

	class PaTrainDvaMessageAccessFactory
	{


	private:
        
        // Disable the following methods
		PaTrainDvaMessageAccessFactory() { };
		PaTrainDvaMessageAccessFactory( const PaTrainDvaMessageAccessFactory& thePaTrainDvaMessageAccessFactory);
		PaTrainDvaMessageAccessFactory& operator=(const PaTrainDvaMessageAccessFactory &);


	public:
        
        
        /** 
         * ~PaTrainDvaMessageAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaTrainDvaMessageAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaTrainDvaMessageAccessFactory object.
         */
		static PaTrainDvaMessageAccessFactory& getInstance();


        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleans up the members.  
         * Primarily used upon program termination (e.g. from main()) so that Purify does not 
         * consider this class and its members to be memory leaks.
         *
         */
        static void removeInstance();

        
        
        /**
         * getLibraryVersion
         *
         * Retrieves the global Library version number of the PA_TRAIN_DVA_MESSAGE table
         * stored in the table comments.  The comments string is expected to be of the 
         * form "LibraryVersion=[0-9]+"
         *
         *
         * @return  the Library version number
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if the table comment does not contain the version number.  
         */
        unsigned long getLibraryVersion();


        /**
         * setLibraryVersion
         *
         * Sets the global Library version number of the PA_TRAIN_DVA_MESSAGE table
         * stored in the table comments.  The comments string will be set to :
         * "This table contains the train DVA message configuration.  
         *  DO NOT MODIFY THIS COMMENT.  LibraryVersion=X"
         *
         * @param   libraryVersion the version number of the train DVA message library
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
        void setLibraryVersion(unsigned long libraryVersion);


        /**
         * getPaTrainDvaMessage
         *
         * Retrieves the specified PaTrainDvaMessage and returns the associated data as an 
         * object conforming to the IPaTrainDvaMessage interface.
         *
         * @param   key        the primary key/id of the PaTrainDvaMessage to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaTrainDvaMessage objects or set to false to return 
         *                     readonly IPaTrainDvaMessage objects.
         *
         * @return  A pointer to an object conforming to the IPaTrainDvaMessage interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaTrainDvaMessage object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaTrainDvaMessage matching the supplied key
         *          - if there is more than one PaTrainDvaMessage matching the supplied key. 
         *
         */
		IPaTrainDvaMessage* getPaTrainDvaMessage(const unsigned long key, const bool readWrite);


        /**
         * getAllPaTrainDvaMessages
         *
         * Retrieves all PaTrainDvaMessages and returns the associated data as a vector of  
         * objects conforming to the IPaTrainDvaMessage interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaTrainDvaMessage objects or set to false to return 
         *                     readonly IPaTrainDvaMessage objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaTrainDvaMessage interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaTrainDvaMessage object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaTrainDvaMessages getAllPaTrainDvaMessages( const bool readWrite = false );
        

        /**
         * createPaTrainDvaMessage
         * 
         * Creates an empty IConfigPaTrainDvaMessage object, and returns a pointer to it.
         * When creating new train messages, it is the clients responsibility to define
         *  the primary key to be used when adding the record to database
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaTrainDvaMessage object.
         *
         * @param primaryKey the primary key of this train DVA message (must
         *          be unique within database)
         *
         * @exception DataException if a train DVA message with the specified key
         *              already exists
         *
         * @return A pointer to an IConfigPaTrainDvaMessage object
         *
         */
		IConfigPaTrainDvaMessage* createPaTrainDvaMessage(unsigned long primaryKey);


        /**
         * copyPaTrainDvaMessage
         * 
         * Creates a new configuration PaTrainDvaMessage object using the PaTrainDvaMessage passed in
         *
         * @param  PaTrainDvaMessageToCopy the ConfigPaTrainDvaMessage to copy
         *
         * @return the new object as a pointer to an IConfigPaTrainDvaMessage object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaTrainDvaMessage* copyPaTrainDvaMessage(const IConfigPaTrainDvaMessage* paTrainDvaMessageGroupToCopy);


    private:

        static PaTrainDvaMessageAccessFactory* m_instance;

        /**
         * getPaTrainDvaMessagesReturnedFromStatement
         *
         * Retrieves all PaTrainDvaMessages that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the messages to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaTrainDvaMessage objects or set to false to return 
         *                     readonly IPaTrainDvaMessage objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaTrainDvaMessage interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaTrainDvaMessage object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaTrainDvaMessages getPaTrainDvaMessagesReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);
        // For DDB to get the location DB name
		std::string getLocalDatabaseName();

	};

} // closes TA_Base_Core

#endif // !defined(PaTrainDvaMessageAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
