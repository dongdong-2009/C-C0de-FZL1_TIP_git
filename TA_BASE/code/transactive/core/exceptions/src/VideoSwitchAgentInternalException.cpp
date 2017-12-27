/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/VideoSwitchAgentInternalException.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * These are the exceptions thrown within the Video Switch Agent.
  * The app/cctv/VideoSwitchAgent/IDL/src/ExceptionsCorbaDef.idl
  * defines the exceptions that are thrown out of the Video Switch Agent.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "core/exceptions/src/VideoSwitchAgentInternalException.h"

namespace TA_Base_Core
{
    //
    // Constructor
    //
    VideoSwitchAgentInternalException::VideoSwitchAgentInternalException( const EVideoSwitchAgentFailType type ) throw()
		: TransactiveException( VideoSwitchAgentInternalException::getDefaultMessage( type ) ), m_type( type )
    {
    }



    //
    // Constructor
    //
    VideoSwitchAgentInternalException::VideoSwitchAgentInternalException( const std::string& msg ) throw()
		: TransactiveException( msg ), m_type( AGENT_ERROR )
    {
    }
    
    
    //
    // Constructor
    //
    VideoSwitchAgentInternalException::VideoSwitchAgentInternalException( const EVideoSwitchAgentFailType type, const std::string& msg ) throw()
        : TransactiveException( msg ), m_type( type )
    {
    }
 

    //
    // getType
    //
    VideoSwitchAgentInternalException::EVideoSwitchAgentFailType VideoSwitchAgentInternalException::getType()
    {
        return m_type;
    }


    //
    // getDefaultMessage
    //
    std::string VideoSwitchAgentInternalException::getDefaultMessage( const EVideoSwitchAgentFailType type )
    {
        switch( type )
        {
            case AGENT_ERROR:
                return "";
            case CONFIGURATION_ERROR:
                return "The Video Switch Agent failed to initialise because it was configured incorrectly.";
            case CREATION_FAILED:
                return "The required object could not be created by the appropriate factory.";
            case NOT_SUPPORTED:
                return "The requested functionality is not supported.";
            case INVALID_CONFIG:
                return "";
            case MAX_TEXT_LENGTH:
                return "The superimposed text is too long.";
            case INAPPROPRIATE_TEXT:
                return "The text is inappropriate.";
            case NOT_LOCK_OWNER:
                return "The user does not own the lock on this object.";
            case INVALID_TYPE:
                return "The video input or output type is unrecognised.";
            case AGENT_IN_MONITOR_MODE:
                return "The agent is in Monitor mode, so it cannot perform Control mode operations.";
            case INVALID_SPEED:
                return "The requested speed is invalid.";
            case INVALID_PRESET:
                return "The preset index is invalid.";
            case UNKNOWN_INPUT:
                return "The input cannot be switched to the output because it is unknown.";
            case SWITCH_LOCKED:
                return "The input is switch locked by the hardware.";
            case OUT_OF_SERVICE:
                return "The device is out of service.";
            case INSUFFICIENT_PRIVILEGES:
                return "The user does not have the required privileges for this operation.";
            default:
                return "";
        }
        return "";
    }
}
