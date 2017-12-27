/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OperatorAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * OperatorAccessFactory is a singleton that is used to retrieve operator objects either from the
  * database or newly created. All operator objects returned will adhear to the IOperator interface.
  */



#if !defined(OPERATORACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define OPERATORACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/IConfigOperator.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

	class OperatorAccessFactory
	{

	public:
        virtual ~OperatorAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an OperatorAccessFactory object.
         */
		static OperatorAccessFactory& getInstance();

	  /**
	   * getInstanceNoInit
	   *
	   * This function is for getting an instance, 
	   * and meanwhile avoid calling initializeOperatorProfile() and initializeOperatorRegion() in constructure, 
	   * which would otherwise spend a lot of time.
	   *		
	   * In some situation, like in application EventViewer, we use this OperatorAccessFactory 
	   * only for loading operators' simple info(id and name),
	   * and we don't care the profile/region infomation. 
	   * In this case, instead of getInstance(),we can call this funtion to get a "Simple" instance 
	   * and then load what we need.
	   * We can improve the performance by this way.
	   */
		static OperatorAccessFactory& getInstanceNoInit();

        /**
         * getOperator
         *
         * Retrieves the specified operator and returns the associated data as an 
         * object conforming to the IOperator interface.
         *
         * @param key The database key to the operator to retrieve
         *
         * @return A pointer to an object conforming to the IOperator interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IOperator 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no operator
         *            matching the supplied PKEY, or if there is more than one operator
         *            matching the supplied PKEY. 
         */
		IOperator* getOperator(const unsigned long key, const bool readWrite);

		//TD15069
		std::string getDeletedOperatorName( unsigned long key);
		bool getDeletedOperatorIsSystemOperator( unsigned long key);


        /**
         * getOperator
         *
         * Retrieves the specified operator and returns the associated data as an 
         * object conforming to the IOperator interface.
         *
         * @param name The name of the operator in the database
         *
         * @return A pointer to an object conforming to the IOperator interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IOperator 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no operator
         *            matching the supplied PKEY, or if there is more than one operator
         *            matching the supplied PKEY. Addtionally, a DataException will be thrown
         *            if the entry in the PKEY column matching this operator NAME cannot be
         *            converted to an unsigned long
         */
		IOperator* getOperator(const std::string& name, const bool readWrite);

        /**
         * getOperatorFromSession
         *
         * Retrieves the operator associated with the specified session id, and returns
         * the associated data as an ojbect conforming to the IOperator interface.
         *
         * @param sessionID the uuid of the session as stored in the database
         *
         * @return A pointer to an object conforming to the IOperator interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IOperator 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no session
         *            matching the supplied key, or if there is more than one session
         *            matching the supplied key, and in the same situations for the operator
         *            key retrieved form the session table.
         */
		IOperator* getOperatorFromSession(const std::string& sessionId, const bool readWrite);

        /**
         * getAllOperators
         *
         * Retrieves all operators and returns the associated data as a vector of  
         * objects conforming to the IObject interface.
         * The operator data does not contain assigned regions.
         *
         * @return A vector of pointers to objects conforming to the IOperator interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IOperator
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
        std::vector<IOperator*> getAllOperators( const bool readWrite = false );
		std::vector<IOperator*> getAllOperatorsInfo(  );

        /**
         * getAllOperatorsAndRegions
         *
         * Retrieves all operators and returns the associated data (including assigned regions)
         * as a vector of objects conforming to the IObject interface.
         *
         * @return A vector of pointers to objects conforming to the IOperator interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IOperator
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
        std::vector<IOperator*> getAllOperatorsAndRegions( const bool readWrite = false );

        /**
         * getAllOperatorsAndAttributes
         *
         * Retrieves all operators and all operator attributes using a single and more effiecient
         * SQL query. However the associated profiles are not populated.
         * It returns the associated data as a vector of objects conforming to the IObject interface.
         *
         * @return A vector of pointers to objects conforming to the IOperator interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IOperator
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
        std::vector<IOperator*> getAllOperatorsAndAttributes();

        /**
         * getAllNonSystemOperators
         *
         * Retrieves all operators that are not system operators and returns the associated
         * data as a vector of objects conforming to the IObject interface.
         *
         * @param getProfiles True by default - Retrieves the profiles associated with each operator.
         * @param getRegions  False by default - Retrieves the regions associated with each operator.
         *
         * @return A vector of pointers to objects conforming to the IOperator interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IOperator
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
        std::vector<IOperator*> getAllNonSystemOperators( const bool readWrite = false, 
                                                          const bool getProfiles = true,
                                                          const bool getRegions = false );

        
        /**
         * createOperator
         * 
         * Creates an empty IConfigOperator object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigOperator object
         *
         * @return A pointer to an IConfigOperator object
         *
         */
		IConfigOperator* createOperator();


        /**
         * copyOperator
         * 
         * Creates a new configuration operator object using the operator passed in
         *
         * @param IConfigOperator* The operator to copy
         *
         * @return The configuration operator object as a pointer to an IConfigOperator object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigOperator* copyOperator(const IConfigOperator* operatorToCopy);

		void init();
		
		void getProfileByOperator(unsigned long operatorId, std::vector<unsigned long>& profiles);
		void getRegionByOperator(unsigned long operatorId, std::vector<unsigned long>& regions);

	private:

		OperatorAccessFactory();

		/**
		 * just to differentiate from OperatorAccessFactory(); 
		 * in this constructor, we don't have to initialize m_operatorToProfileMap and m_operatorToRegionMap
		 *
		 */
		OperatorAccessFactory(int type);

		OperatorAccessFactory( const OperatorAccessFactory& theOperatorAccessFactory);
		OperatorAccessFactory& operator=(const OperatorAccessFactory &);

		void initializeOperatorProfile();
		void initializeOperatorRegion();

		void getOperatorBySql( 
			    IDatabase* pDatabase,
				const SQLStatement& sql,
				std::vector<IOperator*>& operators,
				const bool readWrite= false,
				const bool getCfgProfiles= false,
				const bool getCfgRegions = false,
				const bool getProfiles= false, 
				const bool getRegions = false);

		void getOperatorInfoBySql( 
			IDatabase* pDatabase,
			const SQLStatement& sql,
				std::vector<IOperator*>& operators);


		typedef std::map< unsigned long, std::vector<unsigned long> > OperatorMap;

		bool		m_operatorToProfileLoad;
		OperatorMap m_operatorToProfileMap;
		NonReEntrantThreadLockable m_operatorToProfileLock;
		bool		m_operatorToRegionLoad;
		OperatorMap m_operatorToRegionMap;
		NonReEntrantThreadLockable m_operatorToRegionLock;

        static OperatorAccessFactory* m_instance;
	};

} // closes TA_Base_Core

#endif // !defined(OperatorAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
