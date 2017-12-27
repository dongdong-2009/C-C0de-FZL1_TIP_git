/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/MessageResourceException.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file defines an exception that will be thrown by the Message Resource library. 
  * It is derived from TransactiveException, and will be used for all errors that can be raised by the
  * Message Resoruce library.
  */

#include "core\exceptions\src\MessageResourceException.h"
#include "core\utilities\src\TAAssert.h"
#include "core\utilities\src\DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    //////////////////////////////////////////////////////////////////////
    // Constants
    //////////////////////////////////////////////////////////////////////

    
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////


    MessageResourceException::MessageResourceException(const EMessageResourceError failType,const unsigned long messageID) 
        : m_failType(failType),
          m_messageID(messageID),
          TransactiveException()
    {
        logException();
    }	

    MessageResourceException::MessageResourceException(const std::string& what, const EMessageResourceError failType,
        const unsigned long messageID)
        : m_failType(failType),
          m_messageID(messageID),
          TransactiveException(what)
    {
        logException();
    }	



    MessageResourceException::~MessageResourceException()
    {

    }

    //////////////////////////////////////////////////////////////////////
    // Member Functions
    //////////////////////////////////////////////////////////////////////

    void MessageResourceException::logException()
    {
        switch(m_failType)
        {
            case ( DLL_NOT_FOUND ):
                LOG_EXCEPTION_DETAILS( SourceInfo, "MessageResourceException",
                    "The Message Resource DLL could not be found");
                break;

            case ( FORMATTING_ERROR ):
                LOG_EXCEPTION_DETAILS( SourceInfo,  "MessageResourceException", 
                    "The configured message string was incorrectly formatted");
                break;

            case ( LANGUAGE_ERROR ):
                LOG_EXCEPTION_DETAILS( SourceInfo, "MessageResourceException", 
                    "The message ID could not be found within the system language context");
                break;
                
            case ( RESOURCE_NOT_FOUND_ERROR ):
                LOG_EXCEPTION_DETAILS( SourceInfo, "MessageResourceException", 
                    "The message ID could not be found in the Message Resource DLL");
                break;
    
            case ( GENERAL_ERROR ):
                LOG_EXCEPTION_DETAILS( SourceInfo, "MessageResourceException", 
                    "An unspecified error has occured within the Message Resource library.");
                break;
    
            /*case ( CONTROL_ENTITY_NOT_UNIQUE ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "MessageResourceException", CONTROL_ENTITY_NOT_UNIQUE_STRING.c_str() );
                break;

            case ( COMMAND_LINE_INVALID ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "MessageResourceException", COMMAND_LINE_INVALID_STRING.c_str() );
                break;

            case ( NO_DATABASE_CONNECTION ):
                    LOG( SourceInfo, DebugUtil::ExceptionDetails, "MessageResourceException", NO_DATABASE_CONNECTION_STRING.c_str() );
                break;

            case ( COMMUNICATION_ERROR_WITH_CONTROL_STATION ):
                    LOG( SourceInfo, DebugUtil::ExceptionDetails, "MessageResourceException", COMMUNICATION_ERROR_WITH_CONTROL_STATION_STRING.c_str() );
                break;

            case ( INITIALISATION_ERRORS ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "MessageResourceException", INITIALISATION_ERRORS_STRING.c_str() );
                break;

            case ( ACCESS_DENIED ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "MessageResourceException", ACCESS_DENIED_STRING.c_str() );
                break;

            case ( UNKNOWN_ACCESS_RIGHTS ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "MessageResourceException", UNKNOWN_ACCESS_RIGHTS_STRING.c_str() );
                break;

            case ( NO_ACTIVE_SESSION ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "MessageResourceException", NO_ACTIVE_SESSION_STRING.c_str() );
                break;*/

            default:
                TA_ASSERT(false,"Unknown GenericGUI Exception Type");
        }
    }

} // End TA_Base_Core