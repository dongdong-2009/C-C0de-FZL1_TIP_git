/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/RightsChecker.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is used to determine if a user has access rights for a certain item. This could be modified
  * later to be used in the config_plugin_helper by making it print out nice error messages to the user
  * (as well as logging) before returning the result
  */

#ifndef CE_RIGHTS_CHECKER
#define CE_RIGHTS_CHECKER
#include <boost/shared_ptr.hpp>
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Bus
{
    class RightsLibrary;
    typedef boost::shared_ptr<RightsLibrary> RightsLibraryPtr;
}

namespace TA_Base_App
{
    class RightsChecker : public TA_Base_Core::RunParamUser
    {
    public:

        static RightsChecker& getInstance();
        bool canPerformAction(unsigned long action);
        void updateSession(const std::string& sessionId);
        virtual void onRunParamChange(const std::string& name, const std::string& value);

    private:

        RightsChecker();
        void buildRightLibrary(const std::string& sessionId);

    private:

        TA_Base_Bus::RightsLibraryPtr m_rightsLibrary;
        unsigned long m_configurationEditorResourceId;
        std::string m_sessionId;
    };
}

#endif CE_RIGHTS_CHECKER
