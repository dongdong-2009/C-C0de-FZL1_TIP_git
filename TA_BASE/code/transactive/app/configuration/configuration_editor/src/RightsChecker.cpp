/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/RightsChecker.cpp $
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

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/RightsChecker.h"
#include "app/configuration/configuration_editor/src/DatabaseCache.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    RightsChecker& RightsChecker::getInstance()
    {
        static RightsChecker* instance = new RightsChecker;
        return *instance;
    }

    RightsChecker::RightsChecker()
        : m_configurationEditorResourceId(0)
    {
        FUNCTION_ENTRY("Constructor");

        RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

        m_sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        buildRightLibrary(m_sessionId);

        try
        {
            // We must figure out the entity key as we do not yet have access to the ConfigurationEditorGUI
            // object to retrieve the key.

            //TA_Base_Core::IEntityData* entity(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME)));
            //TA_Base_Core::IResourcePtr resource(TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(entity->getKey()));
            //m_configurationEditorResourceId = resource->getKey();
            m_configurationEditorResourceId = DatabaseCache::getInstance().getConfigurationEditorResourceId();
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Error", "Could not initialise ready to check rights. Therefore could not determine what components to display for this user.");

            using TA_Base_Core::GenericGUIException;
            TA_THROW(GenericGUIException(GenericGUIException::UNKNOWN_ACCESS_RIGHTS));
        }
    }

    bool RightsChecker::canPerformAction(unsigned long action)
    {
        FUNCTION_ENTRY("canPerformAction");

        TA_ASSERT(m_rightsLibrary != NULL, "The rights library was not constructed correctly so we cannot determine rights");

        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        try
        {
            bool canPerformAction = m_rightsLibrary->isActionPermittedOnResource(
                                        RunParams::getInstance().get(RPARAM_SESSIONID),
                                        m_configurationEditorResourceId,
                                        action,
                                        reason,
                                        decisionModule);

            if (!canPerformAction)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Cannot access component because %s", reason.c_str());//TD14337 yezh++
            }

            return canPerformAction;
        }
        catch (const TA_Base_Core::RightsException&)
        {
        }
        catch (...)
        {
        }

        FUNCTION_EXIT;
        return false;
    }

    void RightsChecker::buildRightLibrary(const std::string& sessionId)
    {
        TA_Base_Bus::RightsLibraryFactory rightsLibraryFactory;

        try
        {
            if (sessionId.empty())
            {
                // We are connected straight to the database so the user has permission to do anything
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "User is connected straight to the database so they are given rights to do anything");
                m_rightsLibrary.reset(rightsLibraryFactory.buildRightsLibrary(TA_Base_Bus::ALWAYS_TRUE_RL));
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "User is connected through the system so checks must be done with the rights agent.");
                m_rightsLibrary.reset(rightsLibraryFactory.buildRightsLibrary());
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Error", "Could not initialise ready to check rights. Therefore could not determine what components to display for this user.");
            using TA_Base_Core::GenericGUIException;
            TA_THROW(GenericGUIException(GenericGUIException::UNKNOWN_ACCESS_RIGHTS));
        }
    }

    void RightsChecker::onRunParamChange(const std::string& name, const std::string& value)
    {
        if (RPARAM_SESSIONID == name)
        {
            updateSession(value);
        }
    }

    void RightsChecker::updateSession(const std::string& sessionId)
    {
        if (m_sessionId != sessionId)
        {
            if (m_sessionId.empty()) // only use first session
            {
                m_sessionId = sessionId;
                buildRightLibrary(sessionId);
            }
        }
    }
}
