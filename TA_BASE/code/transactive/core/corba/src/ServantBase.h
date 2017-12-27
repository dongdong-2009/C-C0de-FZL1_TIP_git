/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_GenericGUI_QT/TA_BASE/transactive/core/corba/src/ServantBase.h $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/10/09 15:42:35 $
  * Last modified by:  $Author: CM $
  *
  * This class is used to nicely activate & de-activate
  * CORBA servants (interface implementations)
  */

#ifndef SERVANT_BASE_H
#define SERVANT_BASE_H

#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <vector>
#include "core/corba/src/CorbaUtil.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "omniORB4/CORBA.h"

namespace TA_Base_Core
{
    class ServantBase : public virtual PortableServer::ServantBase
    {
    public:

        /**
          * declare friend classes to access private functions
          */
        friend class ServantActivator;
        friend class CorbaUtil;

        /**
          * constructor
          *
          * Constructs the ServantBase class
          */
        ServantBase();

        /**
          * destructor
          *
          * Makes sure the programmer does not do anything nasty
          */
        virtual ~ServantBase();

        /**
          * activateServant
          *
          * This method will ativate your servant (start accepting incoming calls). You can
          * activate & deactivate your servant a number of times
          *
          * NOTE: You will NOT need to call _this() to activate your
          *         servant
          */
        void activateServant();

        /**
          * activateServantWithName
          *
          * This method will ativate your servant (start accepting incoming calls). You can
          * activate & deactivate your servant a number of times
          *
          * It will also add the object to the TDNS with the given name
          *
          * NOTE1: If this object is registered in the TDNS it will be automatically un-registered
          *
          * NOTE2: You will NOT need to call _this() to activate your
          *         servant
          */
        void activateServantWithName(const std::string& objectName);

        /**
          * deactivateServant
          *
          * This method will deactivate the servant with all POAs. You can deactivate your servant
          * and latter re-activate it using this method
          *
          * NOTE1: You will need to delete your servant after you have called this
          *
          * NOTE2: If this object is registered in the TDNS it will be automatically un-registered
          */
        void deactivateServant();

        /**
          * deactivateAndDeleteServant
          *
          * This method will deactivate the servant with the POA
          *
          * NOTE1: This method will delete your servant for you!
          *
          * NOTE2: If this object is registered in the TDNS it will be automatically un-registered
          *
          * DO NOT: delete your servant after you have called this it will already be deleted.
          *            This should be the last thing you do to your servant
          */
        void deactivateAndDeleteServant();

        /**
          * removeNamedObjectRegistration
          *
          * Remove your object from the TDNS.
          *
          * @param void
          */
        void removeNamedObjectRegistration(void);

        /**
          * _default_POA
          *
          * Return a reference to the ServantManaged POA that lives
          * in CorbaUtil
          *
          * @return the correct POA to use
          */
        virtual PortableServer::POA_ptr _default_POA();

    private:

        /**
          * registerNamedObject
          *
          * Register your object in the TDNS, with the provided name
          *
          * @param std::string& objectName
          */
        void registerNamedObject(const std::string& objectName);

        /**
          * setDeactivated
          *
          * Used to by the ServantActivator::etherealize function
          * to ensure the servant is deactivated
          */
        void setDeactivated();

        /**
          * hasDeactivated
          *
          * This call will wait a given amount of time before returning
          *
          * @return true    if the object has been deactivated, false otherwise
          */
        bool hasDeactivated();

        /**
          * addPOA
          *
          * Used to add additional POAs to the list of POAs
          * primarily for the bootstrapping POA
          *
          * @param  poa pointer to the poa to add
          */
        void addPOA(PortableServer::POA_var poa);

        /**
          * Make sure the object is cleaned up correctly
          */
        volatile bool m_deactivated;

        /**
          * check to see if the servant is explicitly activated
          */
        bool m_explicitlyActivated;

        /**
          * guard the deactivation callback
          */
        TA_Base_Core::NonReEntrantThreadLockable m_deactivationLock;

        /**
          * the POAs currently active
          */
        std::vector<PortableServer::POA_var> m_POAs;

        /**
          * Store the registered name, for ease of de-registration
          */
        std::string m_objectName;

        /**
          * Keep track of the name registration
          */
        bool m_nameRegistered;
    };
}; // namespace TA_Base_Core

#endif // EVENT_LISTENER_H
