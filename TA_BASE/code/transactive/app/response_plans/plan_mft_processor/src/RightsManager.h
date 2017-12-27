/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/RightsManager.h $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  **/

#if !defined(AFX_RIGHTSMANAGER_H__INCLUDED_)
#define AFX_RIGHTSMANAGER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <string>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Bus
{
    class AuthenticationLibrary;
    class RightsLibrary;
}

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanAgentUpdateProcessor

    class RightsManager : TA_Base_Core::RunParamUser
    {
    // Operations
    public:
        ~RightsManager();

        static RightsManager &getInstance();
        static void destroyInstance();

        virtual void onRunParamChange(const std::string& name, const std::string& value);

        void init();
        void uninit();
        bool isProfileMatched( const unsigned long ulProfileId );
        bool isRightsPermittedOnResource( const unsigned long ulEntityKey, const unsigned long ulActionId );
        bool isDutyPermittedOnResource( const unsigned long ulEntityKey, const unsigned long ulActionId );

    private:
        RightsManager();
        RightsManager(const RightsManager &other);
        RightsManager &operator=(const RightsManager &other);

        void updateRights( const std::string& strSessionId );
        void rightsCleared();

    // Attributes
    private:
        static RightsManager *s_instance;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

        TA_Base_Bus::AuthenticationLibrary* m_authLibrary;
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;
        std::string m_strSessionId;
        std::set<unsigned long> m_setProfileIds;

        TA_Base_Core::NonReEntrantThreadLockable m_lckProfiles;
    };
}

#endif // !defined(AFX_RIGHTSMANAGER_H__INCLUDED_)
