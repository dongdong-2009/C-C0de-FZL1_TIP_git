/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/GenericGUIException.cpp $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * GenericGUI. It is derived from TransactiveException.
  * This exception class will encompass all GenericGUI errors that
  * can possibly occur.
  */

#include "core\exceptions\src\GenericGUIException.h"
#include "core\utilities\src\TAAssert.h"
#include "core\utilities\src\DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    //////////////////////////////////////////////////////////////////////
    // Constants
    //////////////////////////////////////////////////////////////////////

    const std::string GenericGUIException::ENTITY_CONFIGURATION_INVALID_STRING = "Configuration for the entity passed was invalid";
    const std::string GenericGUIException::ENTITY_DOES_NOT_EXIST_STRING = "The entity specified on the command line does not exist in the database";
    const std::string GenericGUIException::ENTITY_NOT_UNIQUE_STRING = "The entity specified on the command line was not unique in the database";
    const std::string GenericGUIException::CONTROL_ENTITY_CONFIGURATION_INVALID_STRING = "The entity specified for this GUI to control/view has invalid configuration in the database";
    const std::string GenericGUIException::CONTROL_ENTITY_DOES_NOT_EXIST_STRING = "The entity specified for this GUI to control/view does not exist in the database";
    const std::string GenericGUIException::CONTROL_ENTITY_NOT_UNIQUE_STRING = "The entity specified for this GUI to control/view is not unique in the database";
    const std::string GenericGUIException::COMMAND_LINE_INVALID_STRING = "The command line passed to the application is invalid";
    const std::string GenericGUIException::NO_DATABASE_CONNECTION_STRING = "Could not connect to the database to retrieve entity details";
    const std::string GenericGUIException::COMMUNICATION_ERROR_WITH_CONTROL_STATION_STRING = "Could not communicate with the Control Station in some way";
    const std::string GenericGUIException::INITIALISATION_ERRORS_STRING = "A component could not be initialised";
    const std::string GenericGUIException::ACCESS_DENIED_STRING = "Insufficient privilege to launch the application";
    const std::string GenericGUIException::UNKNOWN_ACCESS_RIGHTS_STRING = "Cannot determine the rights of the operator - could be indeterminate, N//A or unable to resolve rights agent";
    const std::string GenericGUIException::NO_ACTIVE_SESSION_STRING = "No operator is currently logged into the Control Station";

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////


    GenericGUIException::GenericGUIException(const EGenericGUIFail failType) 
        : m_failType(failType),
          TransactiveException()
    {
        logException();
    }	

    GenericGUIException::GenericGUIException(const std::string& what, const EGenericGUIFail failType)
        : m_failType(failType),
          TransactiveException(what)
    {
        logException();
    }	



    GenericGUIException::~GenericGUIException()
    {

    }

    //////////////////////////////////////////////////////////////////////
    // Member Functions
    //////////////////////////////////////////////////////////////////////

    void GenericGUIException::logException()
    {
        switch(m_failType)
        {
            case ( ENTITY_CONFIGURATION_INVALID ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException",ENTITY_CONFIGURATION_INVALID_STRING.c_str() );
                break;

            case ( ENTITY_DOES_NOT_EXIST ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", ENTITY_DOES_NOT_EXIST_STRING.c_str() );
                break;

            case ( ENTITY_NOT_UNIQUE ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", ENTITY_NOT_UNIQUE_STRING.c_str()  );
                break;
                
            case ( CONTROL_ENTITY_CONFIGURATION_INVALID ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", CONTROL_ENTITY_CONFIGURATION_INVALID_STRING.c_str()  );
                break;
    
            case ( CONTROL_ENTITY_DOES_NOT_EXIST ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", CONTROL_ENTITY_DOES_NOT_EXIST_STRING.c_str() );
                break;
    
            case ( CONTROL_ENTITY_NOT_UNIQUE ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", CONTROL_ENTITY_NOT_UNIQUE_STRING.c_str() );
                break;

            case ( COMMAND_LINE_INVALID ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", COMMAND_LINE_INVALID_STRING.c_str() );
                break;

            case ( NO_DATABASE_CONNECTION ):
                    LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", NO_DATABASE_CONNECTION_STRING.c_str() );
                break;

            case ( COMMUNICATION_ERROR_WITH_CONTROL_STATION ):
                    LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", COMMUNICATION_ERROR_WITH_CONTROL_STATION_STRING.c_str() );
                break;

            case ( INITIALISATION_ERRORS ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", INITIALISATION_ERRORS_STRING.c_str() );
                break;

            case ( ACCESS_DENIED ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", ACCESS_DENIED_STRING.c_str() );
                break;

            case ( UNKNOWN_ACCESS_RIGHTS ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", UNKNOWN_ACCESS_RIGHTS_STRING.c_str() );
                break;

            case ( NO_ACTIVE_SESSION ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "GenericGUIException", NO_ACTIVE_SESSION_STRING.c_str() );
                break;

            default:
                TA_ASSERT(false,"Unknown GenericGUI Exception Type");
        }
    }

} // End TA_Base_Core
