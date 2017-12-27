/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/SubsystemStateAccessFactory.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * SubsystemStateAccessFactory is a singleton that is used to retrieve subsystem state objects either from the
  * database or newly created. All subsystem state objects returned will adhere to the ISubsystemState interface.
  * Data is primarily retrieved from the SE_SUBSYSTEM_STATE table
  *
  */


#if !defined(SubsystemStateAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define SubsystemStateAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class ISubsystemState;

	class SubsystemStateAccessFactory
	{

	public:
        virtual ~SubsystemStateAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an SubsystemStateAccessFactory object.
         */
		static SubsystemStateAccessFactory& getInstance();


        /**
         * getSubsystemState
         *
         * Retrieves the subsystem state of the key provided.
         *
         * @return A pointer to an object conforming to the ISubsystemState interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubsystemState
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
        ISubsystemState* getSubsystemState( const unsigned long subsystemStateKey );


        /**
         * getAllSubsystemStates
         *
         * Retrieves all specified subsystem states and returns the associated data as a vector of  
         * objects conforming to the ISubsystemState interface.
         *
         * @return A vector of pointers to objects conforming to the ISubsystemState interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubsystemState
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
        std::vector<ISubsystemState*> getAllSubsystemStates( );


    private:
        //
        // These are private as this method is a singleton
        //
		SubsystemStateAccessFactory();
		SubsystemStateAccessFactory( const SubsystemStateAccessFactory& theSubsystemStateAccessFactory);
		SubsystemStateAccessFactory& operator=(const SubsystemStateAccessFactory &);
    };

} // closes TA_Base_Core

#endif // !defined(SubsystemStateAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
