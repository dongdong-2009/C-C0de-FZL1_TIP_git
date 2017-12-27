/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IMessageTypeData.h $
 * @author Nick Jardine
 * @version $Revision: #2 $
 * Last modification: $DateTime: 2015/01/26 14:07:36 $
 * Last modified by: $Author: huirong.luo $
 * 
 * IMessageTypeData is an interface to a message type object. It allows the message type object implementation
 * to be changed without changing the code that uses this interface.
 */


#if !defined(IMessageTypeData_412CCE29_A960_4da6_9EA9_D2418C31B5C2__INCLUDED_)
#define IMessageTypeData_412CCE29_A960_4da6_9EA9_D2418C31B5C2__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IMessageTypeData: public IItem
	{
	public:

        /**
         * getDescription
         *
         * Returns the description for this MessageType.
         *
         * @return The description for this MessageType as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
         */
        virtual std::string getDescription() =0;

		/**
         * getName
         *
         * Returns the name of this MessageType. If this is the first time data for this MessageType
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this MessageType as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
         */
		virtual std::string getName(){return "";};

		/**
		 * getPriority
		 *
		 * Returns the prioirity level of this message type.
		 *
		 * @return The priority level of the message type as an unsigned int. This value will be between 0 and 9.
		 * any other value will cause an exception.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
		 *
		 * post: 0 <= return value <= 9
		 */
		virtual unsigned int getPriority() =0;

		/**
		 * toBePrinted
		 *
		 * Returns whether or not this message type should be printed.
		 *
		 * @return true if this message type should be printed.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		virtual bool toBePrinted() =0;

		/**
		 * toBeLogged
		 *
		 * Returns whether or not this message type should be written to the database.
		 *
		 * @return true if this message type should be written to the database.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		virtual bool toBeLogged() =0;

		/**
		 * toBePersisted
		 *
		 * Returns whether or not this message type should be added to a persistent data structure
		 *
		 * @returns true if this message type should be stored in a persistent data structure
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		virtual bool toBePersisted() =0;


        /**
         * getDateCreated
         *
         * Returns the time at which the message type was created.
         *
         * @return time_t The creation time.
         *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the time at which the message type was last modified.
         *
         * @return time_t The modification time.
         *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
         */
        virtual time_t getDateModified() =0;


        /**
         * getViewLevel
         *
         * Returns the view level required to see this message in the event viewer.
         *
         * @return unsigned int The view level.
         *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
         */
        virtual unsigned int getViewLevel() =0;

		virtual std::string getDisplayName() = 0;
        /**
         * getActionId
         *
         * Returns the ActionID of this message(join with table se_action), if no record find in se_action table,return 0
         *
         * @return unsigned long The action ID.
         *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
         */
		virtual unsigned long getActionId() {return 0;};


		/**
		 * getIsEnableRealTimeEvent
		 *
		 * Returns whether or not this message type is a real time message.
		 *
		 * @return true if this message type is a real-time message.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		virtual bool getIsEnableRealTimeEvent() {return true;};

        virtual ~IMessageTypeData() {};
	};

} //close namespace TA_Base_Core

#endif // !defined(IMessageTypeData_412CCE29_A960_4da6_9EA9_D2418C31B5C2__INCLUDED_)
