/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/MessageTypeAccessFactory.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * MessageTypeAccessFactory is a singleton that is used to retrieve MessageTypeData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */


#if !defined(MessageTypeAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_)
#define MessageTypeAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IMessageTypeData.h"
#include "core/data_access_interface/src/MessageTypeEventData.h"

namespace TA_Base_Core
{
	class MessageTypeAccessFactory
	{

		public:
            virtual ~MessageTypeAccessFactory() {};

			/**
			 * getInstance
			 *
			 * Creates and returns an instance of this object.
			 *
			 * @return A reference to an instance of an MessageTypeAccessFactory object.
			 */
			static MessageTypeAccessFactory& getInstance();

			

			/**
			 * getMessageType
			 *
			 * Returns the message type associated with the specified key as a pointer
			 * to an object conforming to the IMessageTypeData interface.
			 *
			 * @param key The database key to the message type to retrieve
			 *
			 * @return A pointer to an object conforming to the IMessageTypeData interface.
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there are
			 *            any problems in communicating with the database. The possilble 
			 *            reasons are:
			 *            1) Invalid connection string (as stored in RunParams)
			 *            2) Database is not/cannot be opened
			 *            3) Error while attempting to execute a database query
			 * @exception DataException A DataException will be thrown if there is no message type
			 *            matching the supplied key, or if there is more than one message type
			 *            matching the supplied key. 
			 */
			IMessageTypeData* getMessageTypeByKey(const std::string& key, const bool readWrite);

			/**
			 * getMessageType
			 *
			 * Returns the message type associated with the specified type name as a pointer
			 * to an object conforming to the IMessageTypeData interface.
			 *
			 * @param typeName The name of the message type to retrieve
			 *
			 * @return A pointer to an object conforming to the IMessageTypeData interface.
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there are
			 *            any problems in communicating with the database. The possilble 
			 *            reasons are:
			 *            1) Invalid connection string (as stored in RunParams)
			 *            2) Database is not/cannot be opened
			 *            3) Error while attempting to execute a database query
			 * @exception DataException A DataException will be thrown if there is no message type
			 *            matching the supplied name, or if there is more than one message type
			 *            matching the supplied name. 
			 */
			IMessageTypeData* getMessageType(const std::string& typeName, const bool readWrite);
			

            /**
			 * getAllMessageTypes
			 *
			 * Returns all message types in the database as a vector of
             * IMessageTypeData objects. The calling class is responsible for
             * deleting these objects.
			 *
             * @param readWrite      true if a writable object should be supplied,
             *                       false if a read-only object should be supplied.
			 * @param eventChannels  a vector of names of Channels that can be used to
             *                       filter the results. Only message types in these
             *                       channels will be returned. If the vector is empty,
             *                       no filtering will be performed.
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there are
			 *            any problems in communicating with the database. The possilble 
			 *            reasons are:
			 *            1) Invalid connection string (as stored in RunParams)
			 *            2) Database is not/cannot be opened
			 *            3) Error while attempting to execute a database query
			 * @exception DataException A DataException will be thrown if there is
             *                          a problem with the format of the data in the DB.
			 */
            std::vector<IMessageTypeData*> getAllMessageTypes(
                const bool readWrite,
                const std::vector<std::string>& eventChannels);

            /**
			 * getAllMessageTypesAndAttributes
			 *
			 * Retrieves all message types and their attributes using one efficient SQL query.
             * Returns all message types in the database as a vector of
             * IMessageTypeData objects. The calling class is responsible for
             * deleting these objects.
			 *
             * @param eventChannels  a vector of names of Channels that can be used to
             *                       filter the results. Only message types in these
             *                       channels will be returned. If the vector is empty,
             *                       no filtering will be performed.
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there are
			 *            any problems in communicating with the database. The possilble 
			 *            reasons are:
			 *            1) Invalid connection string (as stored in RunParams)
			 *            2) Database is not/cannot be opened
			 *            3) Error while attempting to execute a database query
			 * @exception DataException A DataException will be thrown if there is
             *                          a problem with the format of the data in the DB.
			 */
            std::vector<IMessageTypeData*> getAllMessageTypesAndAttributes(
                const std::vector<std::string>& eventChannels);


		private:
            MessageTypeAccessFactory() {};
			MessageTypeAccessFactory( const MessageTypeAccessFactory& theMessageTypeAccessFactory);
			MessageTypeAccessFactory& operator=(const MessageTypeAccessFactory &);
			
			/**
			 * getMessageType
			 *
			 * Returns the message type associated with the specified sql statement as a pointer
			 * to an object conforming to the IMessageTypeData interface.
			 * 
			 * @param databaseConnection The database connection to retrieve
			 * @param sql The sql statement of the message type to retrieve
			 *
			 * @return A pointer to an object conforming to the IMessageTypeData interface.
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there are
			 *            any problems in communicating with the database. The possilble 
			 *            reasons are:
			 *            1) Invalid connection string (as stored in RunParams)
			 *            2) Database is not/cannot be opened
			 *            3) Error while attempting to execute a database query
			 * @exception DataException A DataException will be thrown if there is no message type
			 *            matching the supplied name, or if there is more than one message type
			 *            matching the supplied name. 
			 */
			void getMessageTypeBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IMessageTypeData*>& messageTypes, const bool readWrite);

            static MessageTypeAccessFactory* m_instance;

	};
} // closes TA_Base_Core

#endif // !defined(MessageTypeAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_)
