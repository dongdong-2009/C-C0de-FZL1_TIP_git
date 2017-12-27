/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_GenericGUI_QT/TA_BASE/transactive/core/corba/src/ServantBase.cpp $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/10/09 15:42:35 $
  * Last modified by:  $Author: CM $
  *
  * This class is used to nicely activate & de-activate
  * CORBA servants (interface implementations)
  */
#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <vector>
#include "core/corba/src/ServantBase.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/naming/src/Naming.h"

namespace TA_Base_Core
{
    ServantBase::ServantBase()
        : m_deactivated(true),
          m_explicitlyActivated(false),
          m_nameRegistered(false)
    {
    }

    ServantBase::~ServantBase()
    {
        try
        {
            TA_ASSERT(m_deactivated, "Servant has not been deactivated");
        }
        catch (...)
        {
            // Do nothing. This is just here to stop an exception being thrown in test rigs were
            // the TA_ASSERT macro behaviour is set to exception.
        }
    }

    void ServantBase::activateServant()
    {
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ServantBase::activateServant(): %s activating", gPtrToStr(this).c_str());

        try
        {
            // Get the POA we want
            PortableServer::POA_var poa = this->_default_POA();

            // Activate the object
            PortableServer::ObjectId_var oid = poa->activate_object(this);

            // The servant is activated
            m_deactivated = false;

            // Explicit activation
            m_explicitlyActivated = true;

            CORBA::Object_var obj = poa->servant_to_reference(this);
            CORBA::String_var s = TA_Base_Core::CorbaUtil::getInstance().getOrb().object_to_string(obj);
            LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ServantBase::activateServant(): %s activated %s", gPtrToStr(this).c_str(), s.in());
        }
        catch (const PortableServer::POA::ServantAlreadyActive&)
        {
            // The servant has already been implicitly activated
            m_deactivated = false;

            // Explicit activation
            m_explicitlyActivated = true;
        }
        catch (...)
        {
            TA_ASSERT(true, "Unknown exception caught while activating servant. Make sure the orb is activated!");
        }
    }

    void ServantBase::activateServantWithName(const std::string& objectName)
    {
        // Activate the servant
        activateServant();

        // register the name
        registerNamedObject(objectName);
    }

    void ServantBase::deactivateServant()
    {
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ServantBase::deactivateServant(): %s deactivating", gPtrToStr(this).c_str());

        // If the object is de-activated we have to remove the name registration
        removeNamedObjectRegistration();

        // deactivate the servant on all the other POAs
        for (unsigned int i = 0; i < m_POAs.size(); i++)
        {
            try
            {
                PortableServer::ObjectId_var id = m_POAs[i]->servant_to_id(this);
                m_POAs[i]->deactivate_object(id.in());
            }
            catch (const PortableServer::POA::ObjectNotActive&)
            {
                // The servant has already been implicitly de-activated
                // Or never activated
            }
            catch (...)
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown exception", "caught while deactivating servant.");
            }
        }

        m_POAs.clear();

        // deactivate the servant on the default POA
        try
        {
            // Get the default POA
            PortableServer::POA_var poa = this->_default_POA();

            // Get the object ID
            PortableServer::ObjectId_var id = poa->servant_to_id(this);

            // Deactivate the object
            poa->deactivate_object(id.in());

            if (!hasDeactivated())
            {
                LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Servant deletion timeout. Object is not fully deactivated");
            }
        }
        catch (const PortableServer::POA::ObjectNotActive&)
        {
            // The servant has already been implicitly de-activated
            // Or never activated
        }
        catch (...)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown exception", "thrown while deactivating servant.");
        }

        m_deactivated = true;
    }

    void ServantBase::deactivateAndDeleteServant()
    {
        if (m_deactivated && m_explicitlyActivated)
        {
            delete this;
            return;
        }

        // Deactivate the servant
        deactivateServant();

        // Clean up
        delete this;
    }

    void ServantBase::registerNamedObject(const std::string& objectName)
    {
        // Store the name
        m_objectName = objectName;

        // Activate the object if not already activated
        if (!m_explicitlyActivated)
        {
            activateServant();
        }

        // Get a reference to the poa
        PortableServer::POA_var poa = this->_default_POA();

        // Store the object in the TDNS, using the poa to get the object ref
        Naming::registerName(poa->servant_to_reference(this), m_objectName);

        // The name has been registered
        m_nameRegistered = true;
    }

    void ServantBase::removeNamedObjectRegistration(void)
    {
        // Check to see if the name is registered before doing anything
        if (m_nameRegistered)
        {
            // Remove the registration
            Naming::removeNameRegistration(m_objectName);

            // flag the name as un-registered
            m_nameRegistered = false;
        }
    }

    void ServantBase::setDeactivated()
    {
        // Guard the call
        TA_Base_Core::ThreadGuard guard(m_deactivationLock);

        // Set to deactivated
        m_deactivated = true;
    }

    bool ServantBase::hasDeactivated()
    {
        // Wait for the callback to indicate the servant is actually de-activated
        // Give it a max of 10 secs
        for (unsigned int i = 0; i < 2000; i++)
        {
            // Sleep for a bit
            TA_Base_Core::Thread::sleep(5);

            // Check deactivation
            TA_Base_Core::ThreadGuard guard(m_deactivationLock);

            if (m_deactivated)
            {
                return true;
            }
        }

        // never deactivated
        return false;
    }

    void ServantBase::addPOA(PortableServer::POA_var poa)
    {
        for (size_t i = 0; i < m_POAs.size(); ++i)
        {
            if (m_POAs[i]->_is_equivalent(poa))
            {
                return;
            }
        }

        m_POAs.push_back(poa);
    }

    PortableServer::POA_ptr ServantBase::_default_POA()
    {
        return CorbaUtil::getInstance().getPOA();
    }
}; // namespace TA_Base_Core
