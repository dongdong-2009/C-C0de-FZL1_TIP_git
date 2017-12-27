
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/OperatorPasswordAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/10/27 16:19:08 $
  * Last modified by:  $Author: hui.wang $
  * 
  * SessionAccessFactory is a singleton that is used to retrieve Session objects either from the
  * database or newly created. All Session objects returned will adhear to the ISession interface.
  */



#if !defined(OperatorPasswordAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define OperatorPasswordAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IOperatorPassword.h"

namespace TA_Base_Core
{

	class OperatorPasswordAccessFactory
	{

	public:
		virtual ~OperatorPasswordAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an SessionAccessFactory object.
         */
		
		static OperatorPasswordAccessFactory& getInstance();

        /**
         * getSession
         *
         * Retrieves the specified Session and returns the associated data as an 
         * object conforming to the ISession interface.
         *
         * @param sessionId The sessionId to the session to retrieve
         *
         * @return A pointer to an object conforming to the ISession interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISession 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Session
         *            matching the supplied PKEY, or if there is more than one Session
         *            matching the supplied PKEY. 
         */
		IOperatorPassword* getOperatorPassword(unsigned long operatorkey,bool changePassword, bool rightPassword);

		IOperatorPassword* CreateOperatorPassword(unsigned long operatorkey);

    private:
        //
        // These are private as this method is a singleton
        //
		OperatorPasswordAccessFactory() { };
		OperatorPasswordAccessFactory( const OperatorPasswordAccessFactory& theSessionAccessFactory);
		OperatorPasswordAccessFactory& operator=(const OperatorPasswordAccessFactory &);

		

        static OperatorPasswordAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(SessionAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
