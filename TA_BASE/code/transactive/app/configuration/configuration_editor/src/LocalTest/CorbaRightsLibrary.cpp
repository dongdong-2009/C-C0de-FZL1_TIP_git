/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/src/CorbaRightsLibrary.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/21 10:20:13 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  Implementation of CorbaRightsLibrary.
  */

#ifdef _MSC_VER
    #pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "bus/security/rights_library/src/RightsLibraryInternal.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/foreach.hpp>

namespace
{
    const unsigned long INVALID_KEY(0xFFFFFFFF);
}

namespace TA_Base_Bus
{
    CorbaRightsLibrary::CorbaRightsLibrary()
        : m_rightsAgent(NULL),
          m_threadPoolManager(1),
          m_cache(m_sessionUpdates),
          m_subscriber(m_cache),
          m_superProfileKey(INVALID_KEY),
          m_cacheWoDuty(m_sessionUpdatesWoDuty),
          m_subscriberWoDuty(m_cacheWoDuty)
    {
    }

    CorbaRightsLibrary::~CorbaRightsLibrary()
    {
    }

    bool CorbaRightsLibrary::isActionPermittedOnResource(
        const std::string& sessionId, unsigned long resourceKey,
        unsigned long actionKey, std::string& reasonString,
        EDecisionModule& decisionModule)
    {
        if (actionKey == TA_Base_Bus::aca_RUN_APPLICATION)
        {
            return TA_Base_Core::getRunParamValue("AcaRunApplication", 1) != 0;
        }
        else if (actionKey == TA_Base_Bus::aca_CE_CONFIGURE_ACTION_GROUPS)
        {
            return TA_Base_Core::getRunParamValue("AcaCeConfigureActionGroups", 1) != 0;
        }
        else if (actionKey == TA_Base_Bus::aca_CE_CONFIGURE_OPERATORS)
        {
            return TA_Base_Core::getRunParamValue("AcaCeConfigureOperators", 1) != 0;
        }
        else if (actionKey == TA_Base_Bus::aca_CE_CONFIGURE_PROFILES)
        {
            return TA_Base_Core::getRunParamValue("AcaCeConfigureProfiles", 1) != 0;
        }
        else if (actionKey == TA_Base_Bus::aca_CE_CONFIGURE_RIGHTS)
        {
            return TA_Base_Core::getRunParamValue("AcaCeConfigureRights", 1) != 0;
        }

        return true;
    }

    void CorbaRightsLibrary::isActionPermittedOnResources(
        const std::string& sessionId, ResourceList& resourceKeys, ResourceList& returnResourceList,
        unsigned long actionKey, std::string& reasonString,
        EDecisionModule& decisionModule)
    {
    }

    bool CorbaRightsLibrary::isActionPermittedOnResourceAndLocSub(
        const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
        unsigned long actionKey, std::string& reasonString,
        EDecisionModule& decisionModule)
    {
        return true;
    }

    bool CorbaRightsLibrary::isActionPermittedOnResourceWithoutDuty(
        const std::string& sessionId, unsigned long resourceKey,
        unsigned long actionKey, std::string& reasonString,
        EDecisionModule& decisionModule)
    {
        return true;
    }

    void CorbaRightsLibrary::areActionsPermittedOnSubsystemAndLocSub(
        const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
        ActionList& actionKeys, ActionList& returnActionList, std::string& reasonString,
        EDecisionModule& decisionModule)
    {
    }

    bool CorbaRightsLibrary::isActionPermittedOnResourceAndLocSubWithoutDuty(
        const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
        unsigned long actionKey, std::string& reasonString,
        EDecisionModule& decisionModule)
    {
        return true;
    }

    void CorbaRightsLibrary::resetCache()
    {
    }

    void CorbaRightsLibrary::notifyObservers(std::string& sessionId, ERightsChangeType changeType, DutyChanges& changes)
    {
    }

    void CorbaRightsLibrary::registerObserver(IRightsChangeObserver& observer)
    {
    }

    void CorbaRightsLibrary::unregisterObserver(IRightsChangeObserver& observer)
    {
    }

    bool CorbaRightsLibrary::isSuperSession(const std::string& sessionId)
    {
        return true;
    }
}
