/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VideoEnquirer.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Light-weight singleton class to communicate with the VideoSwitchAgent
  * for selected enquiry functions only :
  *     - get camera name
  *
  */

#if !defined(AFX_VIDEO_ENQUIRER_H__C08A7DFF_F0FD_4409_BDE5_EB600F0B2A96__INCLUDED_)
#define AFX_VIDEO_ENQUIRER_H__C08A7DFF_F0FD_4409_BDE5_EB600F0B2A96__INCLUDED_

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET) || defined (STEPTYPE_VIDEOWALL)

#include <string>
#include "bus/cctv/video_switch_agent/IDL/src/MovementControlCorbaDef.h"


namespace TA_Base_App
{
    class VideoEnquirer
    {
    public:
        VideoEnquirer() {};
        ~VideoEnquirer() {};

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

        /**
          * getCameraName
          *
          * Gets the name of the Camera with the specified key.
          *
          * @return     std::string
          *             The name of the Camera.
          *
          * @param      unsigned long cameraEntityKey
          *             The entity key of the Camera.
          *
          * @exception  TransactiveException
          *             Thrown if:
          *             1. The VideoSwitchAgent cannot be contacted
          *             2. The Camera with the given entity key was not found.
          *             3. The Camera name cannot be retrieved from the database.
          */

        std::string getCameraName( unsigned long cameraEntityKey );

#endif

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

        /**
          * getMonitorName
          *
          * Gets the name of the Monitor with the specified key.
          *
          * @return     std::string
          *             The name of the Monitor.
          *
          * @param      unsigned long monitorEntityKey
          *             The entity key of the Monitor.
          *
          * @exception  TransactiveException
          *             Thrown if:
          *             1. The VideoSwitchAgent cannot be contacted
          *             2. The Monitor with the given entity key was not found.
          *             3. The Monitor name cannot be retrieved from the database.
          */

        std::string getMonitorName( unsigned long monitorEntityKey );

#endif

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

        /**
          * getPresetName
          *
          * Gets the name of the Camera Preset with the specified id for
          * the given camera.
          *
          * @return     std::string
          *             The name of the Preset.
          *
          * @param      unsigned long cameraEntityKey
          *             The entity key of the Camera.
          * @param      unsigned long presetKey
          *             The unique key of the Preset.
          *
          * @exception  TransactiveException
          *             Thrown if:
          *             1. The VideoSwitchAgent cannot be contacted.
          *             2. The Camera with the given entity key was not found.
          *             3. The Preset with the given id was not found.
          *             4. The Preset name cannot be retrieved.
          */

        std::string getPresetName( unsigned long cameraEntityKey, unsigned long presetKey );

        /**
          * getPresets
          *
          * Gets the presets for a particular camera.
          *
          * @return     MovementControlCorbaDef::CameraPresetSequence*
          *             The camera's presets.
          *
          * @param      unsigned long cameraEntityKey
          *             The entity key of the Camera.
          *
          * @exception  TransactiveException
          *             Thrown if:
          *             1. The VideoSwitchAgent cannot be contacted.
          *             2. The Camera with the given entity key was not found.
          *             3. The Camera presets cannot be retrieved.
          */

        TA_Base_Bus::MovementControlCorbaDef::CameraPresetSequence* getPresets( unsigned long cameraEntityKey );

#endif

#if defined (STEPTYPE_VIDEOWALL)

        /**
          * getWallboardName
          *
          * Gets the name of the Wallboard with the specified key.
          *
          * @return     std::string
          *             The name of the Wallboard.
          *
          * @param      unsigned long wallboardEntityKey
          *             The entity key of the Wallboard.
          *
          * @exception  TransactiveException
          *             Thrown if:
          *             1. The VideoSwitchAgent cannot be contacted
          *             2. The Wallboard with the given entity key was not found.
          *             3. The Wallboard name cannot be retrieved from the database.
          */

        std::string getWallboardName( unsigned long wallboardEntityKey );

        /**
          * getLayoutName
          *
          * Gets the name of the Layout with the specified id (for wallboards).
          *
          * @return     std::string
          *             The name of the Layout.
          *
          * @param      unsigned long layoutId
          *             The entity key of the Layout.
          *
          * @exception  TransactiveException
          *             Thrown if:
          *             1. The database cannot be contacted.
          *             2. The Layout with the given id was not found.
          *             3. The Layout name cannot be retrieved from the database.
          */

        std::string getLayoutName( unsigned long layoutId );

#endif

        // KTTODO Add all copy ctr/ass ops.
    };
}

#endif // STEPTYPE_ASSIGN_CAMERA_TO_MONITOR || STEPTYPE_ACTIVATE_CAMERA_PRESET || STEPTYPE_VIDEOWALL

#endif // AFX_VIDEO_ENQUIRER_H__C08A7DFF_F0FD_4409_BDE5_EB600F0B2A96__INCLUDED_
