#if !defined(MessageConfig_5CF214C5_C6A4_4d9c_849A_1E8C47518EFF__INCLUDED_)
#define MessageConfig_5CF214C5_C6A4_4d9c_849A_1E8C47518EFF__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/MessageConfig.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * MessageTypeTableAccessor provides read-only access to a subset of the columns in the 
  * ME_MESSAGE_TYPE table.  A cache is maintained for records that have been accessed.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>
#include "core/message/types/MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    // Searches s for parameter names in replacementList of the form "[paramName]", replacing 
    // occurrences with the corresponding parameter value.  
	std::string gFindAndReplaceParams( const std::string& s, const DescriptionParameters& replacementList);

    // Converts the given (name, value) pairs to a string of the form "name1:value1,name2:value2,..."
    // The last name-value pair ends with a trailing comma.
    // TODO LPP: This could be a toString overload for const DescriptionParameters& - but would 
    // this be too obscure?
	void gGetStringFromParameters(const DescriptionParameters& parameters, std::string& paramStr);

    // Converts the given string to a set of (name,value) pairs.  The string must have the same
    // form as specified above.	
    // NOTE: The caller takes ownership of the NameValuePair objects in 'parameters'.
	void gGetParametersFromString(const std::string& paramStr, DescriptionParameters& parameters);


	class IMessageTypeData;

	class MessageTypeTableAccessor 
	{
	public:
		/**
		* destructor
		*/
		virtual ~MessageTypeTableAccessor();

		/**
		* Returns a reference to this object.
		*/
		static MessageTypeTableAccessor& getInstance();

        // Cleans up the singleton instance.  Can be safely called multiple times.
        static void destroyInstance();

		/**
		* isLoggable
		*
		* Check to see if a particular Message Type should be logged
		*
		* @param    string    The message type key to check if logging is required
		* @return	bool	true if the message is loggable, false otherwise
		*/
		bool isLoggable(const MessageType& messageType);


		/**
		* isRealTimeEvent
		*
		* Check to see if a particular Message Type should be sent to EventAgent.(together with isLoggable)
		*
		* @param    string    The message type key to check if logging is required
		* @return	bool	true if the message is realTimeEvent, false otherwise
		*/
		bool isRealTimeEvent(const std::string& messageTypeKey);


		unsigned int getViewLevel(const std::string& messageTypeKey);

		unsigned long getActionId(const std::string& messageTypeKey);

		std::string getMessageTypeName(const std::string& messageTypeKey);

		/**
		* isLoggable
		*
		* Check to see if a particular Message Type should be logged
		*
		* @param    messageType    The message type to check if logging is required
        * @param    reload         to Reload from database for this message type
		* @return	bool	true if the message is loggable, false otherwise
		*/
        bool isLoggable(const std::string& messageTypeKey, bool reload = false);

		/**
		* isPrintable
		*
		* Check to see if a particular Message Type should be printed.
		*
		* @param    messageType    The message Type to check if printing is necessary
		* @return	bool	true if the message is printable, false otherwise
		*/
		bool isPrintable(const MessageType& messageType);

		/**
		* getPriority
		*
		* Get the configured priority of the message
		*
		* @param    messageType    The message Type to lookup
		* @return	short	the message priority
		*/
		short getPriority(const MessageType& messageType);

		/**
		* getExpandedDescription
		*
		* Get the description field for a particular messageType and replace 
        * substrings of the form "[name]" with the value in the corresponding 
        * (name, value) pair in 'parameters'.
		*/
		std::string getExpandedDescription(const MessageType& messageType, const DescriptionParameters& parameters);


	private:

		/**
		* getMessageTypeRecord
		*
		* Used to retrieve the messageTypeData object, or create a new one if it does not
		* exist
		*
		* @param	typeKey	the typekey of the MessageType object to load
		* @return	msgTypeData	pointer to the MessageTypeData object
		*/
		TA_Base_Core::IMessageTypeData* getMessageTypeRecord(const std::string& typeKey, bool reload = false);
		
		/**
		* constructor
		*
		*private as this is a singleton
		*/
		MessageTypeTableAccessor();

	private:

		/**
		* Store all the MessageType data info in this map
		* indexed by MessageTypeKey
		*/
		typedef std::map<std::string, TA_Base_Core::IMessageTypeData*> MsgTypeMap;
		MsgTypeMap m_msgTypeMap;
		
		/**
		* the one and only classInstance
		*/
		static MessageTypeTableAccessor* m_theClassInstance;

		/**
		* protect singleton creation
		*/
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		/**
		* protect the map
		*/
		TA_Base_Core::NonReEntrantThreadLockable m_mapLock;

		/**
		* protect the DAI code
		*/
		TA_Base_Core::NonReEntrantThreadLockable m_daiLock;

	};


}; // TA_Base_Core
#endif // !defined(MessageConfig_5CF214C5_C6A4_4d9c_849A_1E8C47518EFF__INCLUDED_)
