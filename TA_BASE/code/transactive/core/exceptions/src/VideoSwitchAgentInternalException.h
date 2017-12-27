#if !defined(VIDEO_SWITCH_AGENT_INTERNAL_EXCEPTION_H)
#define VIDEO_SWITCH_AGENT_INTERNAL_EXCEPTION_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/VideoSwitchAgentInternalException.h $
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
    class VideoSwitchAgentInternalException : public TA_Base_Core::TransactiveException  
	{
    public:
        // Enumeration of the types of failures this exception covers.
       enum EVideoSwitchAgentFailType
        {
            AGENT_ERROR                 = 0,
            CONFIGURATION_ERROR         = 1,    // Thrown if a database or data error occurs
            CREATION_FAILED             = 2,    // Thrown if the protocol dependent interface object cannot be created.
            NOT_SUPPORTED               = 3,    // Thrown if the operation is not supported
            INVALID_CONFIG              = 4,    // Thrown if configuration is detected to be invalid
            MAX_TEXT_LENGTH             = 5,    // Thrown if the superimposed text is too long.
            INAPPROPRIATE_TEXT          = 6,    // Thrown if the superimposed text contains inappropriate words.
            NOT_LOCK_OWNER              = 7,    // Thrown if the user doesn't hold the lock on the VideoInput.
            INVALID_TYPE                = 8,    // Thrown if the VideoInput/VideoOutput type is unrecognised.
            AGENT_IN_MONITOR_MODE       = 9,    // Thrown if the agent is in Monitor mode and a Control mode function is called.
            INVALID_SPEED               = 10,   // Thrown if the movement speed for a Camera is invalid.
            INVALID_PRESET              = 11,   // Thrown if the preset index for a Camera is invalid.
            UNKNOWN_OUTPUT              = 12,   // Thrown if the VideoOutput is unknown.
            UNKNOWN_INPUT               = 13,   // Thrown if the VideoInput is unknown.
            SWITCH_LOCKED               = 14,   // Thrown if the VideoInput is switch locked (e.g. by a codec).
            OUT_OF_SERVICE              = 15,   // Thrown if the VideoInput is out of service.
            OBJECT_ALREADY_EXISTS       = 16,   // Thrown if an object is added to the SwitchManager with an existing key.
            OBJECT_DOES_NOT_EXIST       = 17,   // Thrown if the requested action is for an object that does not exist
            NO_OUTPUT_ASSOCIATION       = 18,   // Thrown if a camera is controlled without a video output association
            INVALID_HARDWARE_RESPONSE   = 19,   // Thrown if the hardware returns a response that the agent does not understand
            NO_CONNECTIONS_AVAILABLE    = 20,   // Thrown if there were no free connections to the hardware for the agent to use
            CONFIGURATION_LIMIT         = 21,   // Thrown if the configured limit of an entity type is reached
            NO_COMMUNICATIONS_HANDLER   = 22,   // Thrown if the CommunicationsHandler could not be created
            PRESET_NAME_NOT_UNIQUE      = 23,   // Thrown if the specified preset is not unique
            INSUFFICIENT_PRIVILEGES     = 100   // Thrown if the user does not have sufficient privileges to perform an action.
        };

  
        /**
          * VideoSwitchAgentInternalException
          *
          * Standard constructor that only specifies which failure occurred.
          *
          * @param      const EVideoSwitchAgentFailType type
          *             The type of failure.
          */
        VideoSwitchAgentInternalException( const EVideoSwitchAgentFailType type ) throw();
        
    
		/**
          * VideoSwitchAgentInternalException
          *
          * Standard constructor that creates an exception of type AGENT_ERROR.
          * This should be used for any generic errors that may occur.
          *
          * @param      const std::string& msg
          *             Detail about the failure.
          */
        VideoSwitchAgentInternalException( const std::string& msg ) throw();
 
   
		/**
          * VideoSwitchAgentInternalException
          *
          * Standard constructor that specifies both which failure occurred
          * and more detail about the failure.
          *
          * @param      const EVideoSwitchAgentFailType type
          *             The type of failure.
          * @param      const std::string& msg
          *             More detail about the failure.
          */
        VideoSwitchAgentInternalException( const EVideoSwitchAgentFailType type, const std::string& msg ) throw();
       
        //
        // The default copy constructor and assignment operators are acceptable.
        //
      
		/**
          * ~VideoSwitchAgentInternalException
          *
          * Standard destructor.
          */
        virtual ~VideoSwitchAgentInternalException() throw() {};
      
    
		/**
          * getType
          *
          * Gets the type of failure.
          *
          * @return     EVideoSwitchAgentFailType
          *             The type of the exception.
          */
        EVideoSwitchAgentFailType getType();

    private:
        
		/** 
          * getDefaultMessage
          *
          * Gets the default message for the given failure type.
          *
          * @return     std::string
          *             The default message or "" if none exists.
          *
          * @param      const EVideoSwitchAgentFailType type
          *             The failure type.
          */
        static std::string getDefaultMessage( const EVideoSwitchAgentFailType type );            

        //
        // The failure type of this error message.
        //
        EVideoSwitchAgentFailType m_type;
    };    
}

#endif // !defined(VIDEO_SWITCH_AGENT_INTERNAL_EXCEPTION_H)
