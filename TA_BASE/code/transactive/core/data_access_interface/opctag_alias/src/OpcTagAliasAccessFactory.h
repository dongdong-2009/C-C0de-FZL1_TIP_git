/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/OpcTagAliasAccessFactory.h $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  * 
  * OpcTagAliasAccessFactory is a singleton that is used to retrieve OpcTagAlias objects either from the
  * database or newly created. All OpcTagAlias objects returned will adhear to the IOpcTagAlias interface.
  * Data is primarily retrieved from the CO_OPC_MAP table
  *
  */



#if !defined(OpcTagAliasAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define OpcTagAliasAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class IOpcTagAlias;
    class IConfigOpcTagAlias;

	class OpcTagAliasAccessFactory
	{

	public:
        virtual ~OpcTagAliasAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an OpcTagAliasAccessFactory object.
         */
		static OpcTagAliasAccessFactory& getInstance();


        /**
         * getOpcTagAlias
         *
         * Retrieves the OpcTagAlias of the key provided.
         *
         * @return A pointer to an object conforming to the IOpcTagAlias interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IOpcTagAlias 
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
        IOpcTagAlias* getOpcTagAlias( const unsigned long opcTagAliasKey, const bool readWrite = false );


        /**
         * getAllOpcTagAliass
         *
         * Retrieves all specified OpcTagAliass and returns the associated data as a vector of  
         * objects conforming to the IOpcTagAlias interface.
         *
         * @return A vector of pointers to objects conforming to the IOpcTagAlias interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IOpcTagAlias 
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
        std::vector<IOpcTagAlias*> getAllOpcTagAliases( const bool readWrite = false );


        /**
         * createOpcTagAlias
         * 
         * Creates a new configuration OpcTagAlias object with the entitykey and alias name
         *
         * @return The configuration OpcTagAlias object as a pointer to an IConfigOpcTagAlias object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigOpcTagAlias* createOpcTagAlias(const unsigned long entitykey, const std::string& name);

        /**
         * createOpcTagAlias
         * 
         * Creates a new configuration OpcTagAlias object.
         *
         * @return The configuration OpcTagAlias object as a pointer to an IConfigOpcTagAlias object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigOpcTagAlias* createOpcTagAlias();

        /**
         * copyOpcTagAlias
         * 
         * Creates a new configuration OpcTagAlias object using the OpcTagAlias passed in
         *
         * @param IConfigOpcTagAlias* The OpcTagAlias to copy
         *
         * @return The configuration OpcTagAlias object as a pointer to an IConfigOpcTagAlias object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigOpcTagAlias* copyOpcTagAlias(const IConfigOpcTagAlias* opcTagAliasToCopy);



    private:
        //
        // These are private as this method is a singleton
        //
		OpcTagAliasAccessFactory();
		OpcTagAliasAccessFactory( const OpcTagAliasAccessFactory& theOpcTagAliasAccessFactory);
		OpcTagAliasAccessFactory& operator=(const OpcTagAliasAccessFactory &);
    };

} // closes TA_Base_Core

#endif // !defined(OpcTagAliasAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
