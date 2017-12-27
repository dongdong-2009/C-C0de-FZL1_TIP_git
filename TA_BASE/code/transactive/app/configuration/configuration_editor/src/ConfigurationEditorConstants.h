/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorConstants.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This holds all constants used throughout the Configuration Editor. These constants
  * have been put in a special namespace (TA_Configuration) so they do not pollute the
  * TA_Base_App namespace.
  */

#include <string>

namespace TA_Base_App
{
    namespace TA_Configuration
    {
        // These are the number of pixels in the icons used in the application
        static const unsigned long LARGE_ICON_SIZE                      = 32;
        static const unsigned long SMALL_ICON_SIZE                      = 16;

        // This is the font typeface and font size used throughout the application
        static const CString TYPEFACE_FOR_CONTROLS                      = "ËÎÌו";
        static const int     FONTSIZE_FOR_CONTROLS                      = 90;

        // This is the format all dates should be shown in
        //static const CString DATE_FORMAT = "%A, %B %d, %Y  %H:%M:%S"; //TD12474

        static const std::string RPARAM_COMPONENT                       = "ComponentType";
        static const std::string RPARAM_CONFIGEDITOR_PKEY               = "ConfigEditorPkey";
        static const std::string RPARAM_HOSTNAME                        = "HostName";
        static const std::string RPARAM_PORTNUMBER                      = "PortNumber";
        static const std::string RPARAM_STARTUP_VIEW                    = "StartupView";
        static const std::string RPARAM_RUN_WITHOUT_CONTROL_STATION     = "RunWithoutControlStation";
        static const std::string RPARAM_LOCAL_TEST                      = "LocalTest";
        static const std::string RPARAM_DO_NOT_CHECK_ALREADY_RUNNING    = "DoNotCheckConfigurationEditorAlreadyRunning";

        // This is the maximum number of items that can be displayed in message boxes that list
        // a number of items. Once this number is reached then ...<more> will be shown.
        static const int MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE              = 10;
    }
}
