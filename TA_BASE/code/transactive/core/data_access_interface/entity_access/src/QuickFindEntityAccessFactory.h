/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/QuickFindEntityAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * QuickFindAccessFactory is a singleton that is used to retrieve entity objects which should be available
  * for quick launch in the Control Station.
  * All objects returned will adhear to the ISession interface.
  */



#if !defined(QuickFindEntityAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define QuickFindEntityAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>


namespace TA_Base_Core
{
    class IQuickFindEntity;

	class QuickFindEntityAccessFactory
	{

	public:
        virtual ~QuickFindEntityAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an QuickFindEntityAccessFactory object.
         */
		static QuickFindEntityAccessFactory& getInstance();


        /**
         * getEntity
         *
         * Retrieves the specified entity and returns the associated data as an 
         * object conforming to the IQuickFindEntity interface.
         *
         * @param key The pkey of the entity to retrieve
         *
         * @return A pointer to an object conforming to the IQuickFindEntity interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IQuickFindEntity 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no entity
         *            matching the supplied PKEY, or if there is more than one entity
         *            matching the supplied PKEY. 
         */
		IQuickFindEntity* getEntity(const unsigned long key);


        /**
         * getAllQuickFindEntities
         *
         * Retrieves the all entities with quick find attributes and returns the associated data as a
         * vector of objects conforming to the IQuickFindEntity interface
         *
         * @return A vector of objects conforming to the IQuickFindEntity interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IQuickFindEntity 
         * objects in the vector
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no entity
         *            matching the supplied PKEY, or if there is more than one entity
         *            matching the supplied PKEY. 
         */
        std::vector<IQuickFindEntity*> getAllQuickFindEntities();


    private:
        //
        // These are private as this method is a singleton
        //
		QuickFindEntityAccessFactory() { };
		QuickFindEntityAccessFactory( const QuickFindEntityAccessFactory& theQuickFindEntityAccessFactory);
		QuickFindEntityAccessFactory& operator=(const QuickFindEntityAccessFactory &);

        static QuickFindEntityAccessFactory* s_instance;
    };

} // closes TA_Base_Core

#endif // !defined(QuickFindEntityAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
