/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/configuration_updates/src/MessageObjectTypeMap.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class defines the interface that must be implemented
  * by GUIs that want to receive online updates
  */


#if !defined(MessageObjectTypeMap_h)
#define MessageObjectTypeMap_h

#include "core/configuration_updates/src/EObjectType.h"
#include <string>
#include <map>


namespace TA_Base_Core
{
    class MessageObjectTypeMap 
    {
    public:
        /** 
          * MessageObjectTypeMap
          *
          * Constructs the message - object type map.
          *
          */
        MessageObjectTypeMap();


        /** 
          * ~MessageObjectTypeMap
          *
          * Destructor does nothing.
          *
          */
        virtual ~MessageObjectTypeMap() {}


        /** 
          * isMessageValid
          *
          * Returns true if the message is in the map.  False otherwise.
          *
          * @param message The message to be determined.
          *
          * @return true if the message is in the map.  False otherwise.
          */
        bool isMessageValid(std::string& message) const;


        /** 
          * getObjectType
          *
          * Returns the object type corresponding to the message if the message is in the map.  
          * Asserts otherwise.
          *
          * @param message The message which object type is to be determined.
          *
          * @return the object type corresponding to the message.
          */
        EObjectType getObjectType(std::string& message) const;
    
    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        MessageObjectTypeMap( const MessageObjectTypeMap& map){ };
        MessageObjectTypeMap& operator=(const MessageObjectTypeMap &) { };

        // use this to easily map the key of message type to the object type
        std::map<std::string, EObjectType> m_objectTypeKey;
    };
}

#endif // !defined(MessageObjectTypeMap_h)
