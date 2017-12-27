/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASpeedDialButtonAccessFactory.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * RASpeedDialButtonAccessFactory is a singleton that is used to retrieve RASpeedDialButton objects either from the
  * database or newly created. All RASpeedDialButton objects returned will adhear to the IRASpeedDialButton interface.
  */



#if !defined(RASpeedDialButtonAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define RASpeedDialButtonAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/radio/src/IRASpeedDialButton.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_Core
{

	class RASpeedDialButtonAccessFactory
	{

    public:
        virtual ~RASpeedDialButtonAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static RASpeedDialButtonAccessFactory& getInstance();


        /**
         * getRASpeedDialButton
         *
         * Retrieves the specified RASpeedDialButton and returns the associated data as an 
         * object conforming to the IRASpeedDialButton interface.
         *
         * @param RASpeedDialButtonKey The RASUBS_ID to the RASpeedDialButton to retrieve
         *
         * @return A pointer to an object conforming to the IRASpeedDialButton interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRASpeedDialButton 
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
        IRASpeedDialButton* getRASpeedDialButtonDetails(unsigned long key);

        /**
         * realizes IRadioSessionCorbaDef::getSpeedDialSet()
         */
        std::vector<IRASpeedDialButton*> getSpeedDialSetByName(const std::string& setName);

        
        IRASpeedDialButton* createSpeedDialButton(IRASpeedDialButton* x);
        void deleteSpeedDialButton(IRASpeedDialButton* x);


    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        RASpeedDialButtonAccessFactory() { };
        RASpeedDialButtonAccessFactory& operator=(const RASpeedDialButtonAccessFactory &) { };
        RASpeedDialButtonAccessFactory( const RASpeedDialButtonAccessFactory& ) {};

        static RASpeedDialButtonAccessFactory* s_instance;
        static NonReEntrantThreadLockable m_singletonLock;	};

} // closes TA_Base_Core

#endif // !defined(RASpeedDialButtonAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
