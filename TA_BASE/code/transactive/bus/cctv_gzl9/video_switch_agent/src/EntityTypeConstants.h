#if !defined ENTITY_TYPE_CONSTANTS_H
#define ENTITY_TYPE_CONSTANTS_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/cctv/video_switch_agent/src/EntityTypeConstants.h $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Defines the possible VideoInput and VideoOutput types that can
  * be returned from getType().
  */

namespace TA_Base_Bus_GZL9
{
    namespace TA_Visual
    {
        // VideoInputs
        
        static const std::string VIDEO_MONITOR      = "VideoMonitor";
        static const std::string RECORDING_UNIT     = "RecordingUnit";
            
        // VideoOutputs
        
        static const std::string CAMERA             = "Camera";
        static const std::string SEQUENCE           = "Sequence";
        static const std::string QUAD               = "Quad";
        // Recording units are also VideoInputs, so that is where they are defined.
        // static const std::string RECORDING_UNIT = "RecordingUnit";

        // VideoOutputGroup
        
        static const std::string VIDEO_OUTPUT_GROUP = "VideoOutputGroup";

        // VideoOutputGroup size values for the different VideoOutput types.

        static const std::string SMALL              = "Small";
        static const std::string MEDIUM             = "Medium";
        static const std::string LARGE              = "Large";

    } // TA_Visual

} // TA_Base_App

#endif // ENTITY_TYPE_CONSTANTS_H
