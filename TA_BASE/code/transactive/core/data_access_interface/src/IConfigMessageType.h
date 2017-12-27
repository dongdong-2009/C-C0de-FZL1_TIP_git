/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigMessageType.h $
 * @author Nick Jardine
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by: $Author: CM $
 * 
 * IConfigMessageType is an interface to a message type object that supports changing the data.
 * It provides methods for retrieving, setting, and writing data.
 * The set methods in this interface do not set the values in the database
 * straight away. They only set the values in the object after validating that
 * they are correct. The data must be explicitly written to the database.
 */


#if !defined(IConfigMessageType_24660863_93AA_444a_9EF9_E18CA21CE00A__INCLUDED_)
#define IConfigMessageType_24660863_93AA_444a_9EF9_E18CA21CE00A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IMessageTypeData.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_Base_Core
{

	class IConfigMessageType : public IMessageTypeData, public IConfigItem
	{
    public:

        /**
         * setDescription
         *
         * Sets the description of this message type. This should only be used by the ConfigMessageType class
         *
         * @param description The description to give this message type
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
         *
         * pre: This MessageType has not been deleted
         */
        virtual void setDescription(const std::string& description) =0;

		/**
		 * setPriority
		 *
		 * Sets the prioirity level of this message type. This should only be used by the ConfigMessageType class
		 *
		 * @param priority The priority level to give this message type. This MUST be between 0 and 9 - this is tested
		 * by an assert.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 *
		 * pre: 0 <= priority <= 9
		 */
		virtual void setPriority(const unsigned int priority) =0;

		/**
		 * setPrintMessageStaus
		 *
		 * Locally sets whether or not this message type should be printed.
		 *
		 * @param printMessage Set to true if this message type should be printed.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		virtual void setPrintMessageStatus(const bool printMessageStatus) =0;

		/**
		 * setLogMessageStatus
		 *
		 * Locally sets whether or not this message type should be written to the database.
		 *
		 * @param logMessageStatus Set to true if this message type should be written to the database.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		virtual void setLogMessageStatus(const bool logMessageStatus) =0;

		/**
		 * persistMessageStatus
		 *
		 * Locally sets whether or not this message type should be added to a persistent data structure
		 *
		 * @param persistMessageStatus Set to true if this message type should be stored in a persistent data structure
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		virtual void setPersistMessageStatus(const bool persistMessageStatus) =0;

        /**
         * setViewLevel
         *
         * Sets the view level required to see this message in the event viewer.
         *
         * @param viewLevel The view level.
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
        virtual void setViewLevel( const unsigned int viewLevel ) =0;

        /**
         * deleteThisObject
         *
         * Removes this message type from the database. 
         * The calling application MUST then delete this message type object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: - The writeMessageTypeData() method has already been called
         *      - This MessageType has not been deleted
         */
        virtual void deleteThisObject() =0;

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the MessageType object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This MessageType has not been deleted
         */
        virtual void applyChanges() =0;

        virtual ~IConfigMessageType() {};
	};

} //close namespace TA_Base_Core

#endif // !defined(IConfigMessageType_24660863_93AA_444a_9EF9_E18CA21CE00A__INCLUDED_)
