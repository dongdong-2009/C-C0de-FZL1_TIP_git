/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/NamedObject.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2017/09/29 17:02:16 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This class provides an interface-specific wrapper for the
  * NamedObjectBase class, which just deals with CORBA::Objects
  *
  * It is templatised with the interface, ptr and var classes which are
  * generated from a CORBA interface
  *
  * Methods are provided to access the templatised type
  */

#ifndef NAMED_OBJECT_H
#define NAMED_OBJECT_H

#ifdef WIN32
    // disable "warning C4290: C++ Exception Specification ignored"
    #pragma warning(push)
    #pragma warning(disable : 4290)
#endif // WIN32

#include "core/naming/src/AbstractNamedObject.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

namespace TA_Base_Core
{
    template <typename CorbaDef, typename CorbaDef_ptr = typename CorbaDef::_ptr_type, typename CorbaDef_var = typename CorbaDef::_var_type, unsigned short defaultTimeout = 0>
    class NamedObject : public AbstractNamedObject
    {
    public:
        /**
          * NamedObject
          *
          * Create an empty NamedObject so that setCorbaName can be called
          * when necessary
          */
        NamedObject()
            : m_timeoutPeriod(defaultTimeout), m_varInvalid(true)
        {
            m_corbaDefVar = CorbaDef::_nil();
        }

        /**
          * NamedObject
          *
          * Create a new NamedObject with the provided agentName and objectName
          *
          * @param  std::string& agentName
          * @param std::string& objectName
          */
        NamedObject(const std::string& agentName, const std::string& objectName)
            : m_timeoutPeriod(defaultTimeout), m_varInvalid(true)
        {
            m_corbaDefVar = CorbaDef::_nil();

            // Set the name on NamedObjectBase
            setCorbaName(agentName, objectName);
        }


        // Bohong++, just do the same thing as
        // NamedObject( const std::string& agentName, const std::string& objectName )
        // cos if the second parameter passed in is a char*, it can't be converted
        // into a string internally
        NamedObject(const std::string& agentName, const char* objectName)
            : m_timeoutPeriod(defaultTimeout), m_varInvalid(true)
        {
            m_corbaDefVar = CorbaDef::_nil();
            std::string sObjectName = objectName;

            // Set the name on NamedObjectBase
            setCorbaName(agentName, sObjectName);
        }

        /**
          * NamedObject
          *
          * Create a new NamedObject with the provided corbaName
          *
          * @param  std::string& agentName
          * @param std::string& objectName
          */
        NamedObject(const CorbaName& corbaName)
            : m_timeoutPeriod(defaultTimeout), m_varInvalid(true)
        {
            m_corbaDefVar = CorbaDef::_nil();

            // Set the name on NamedObjectBase
            setCorbaName(corbaName);
        }

        /**
          * NamedObject
          *
          * Create a new NamedObject with the following entity.
          *
          * Note - this method will lookup the agentName for the given entity using the
          * EntityAccessFactory. If you are creating LOTS (>100's) of NamedObject's,
          * please do not use this method as it is not as fast as providing the necessary
          * details/CorbaName.
          *
          * @param std::string& entityName
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          */
        NamedObject(const std::string& entityName, const bool isAgentEntity)
            : m_timeoutPeriod(defaultTimeout), m_varInvalid(true)
        {
            // Lookup in the database
            CorbaName name = EntityAccessFactory::getInstance().getCorbaNameOfEntity(entityName, isAgentEntity);

            m_corbaDefVar = CorbaDef::_nil();

            setCorbaName(name);
        }

        /**
          * setEntityName
          *
          *
          * Note - this method will lookup the agentName for the given entity using the
          * EntityAccessFactory. If you are calling this on LOTS (>100's) of NamedObject's,
          * please do not use this method as it is not as fast as providing the necessary
          * details/CorbaName.
          *
          * @param std::string& entityName
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          */
        void setEntityName(const std::string& entityName, const bool isAgentEntity)
        {
            // Check to see if the entityName is the same as the existing entityName
            if (entityName.compare(getObjectName()) == 0)
            {
                // They are the same so we can just return without doing anything
                return;
            }

            // Lookup entity.
            CorbaName name = EntityAccessFactory::getInstance().getCorbaNameOfEntity(entityName, isAgentEntity);

            setCorbaName(name);
        }

        /**
          * Destructor
          */
        virtual ~NamedObject()
        {
            //Naming::getInstance().deregisterNamingUser( this );
        }

        /**
          * corbaDefPtr
          *
          * Accessor for the m_dmIfVar member. Note: memory is not duplicated -
          * don't delete it (or assign it to a DmIf_var)!!!
          *
          * If the member is currently nil then attempt to resolve it
          *
          * @return CORBA Object reference. Never nil
          *
          * @exception ObjectResolutionException
          */
        virtual CorbaDef_var corbaDefPtr()
        {
            // Gaurd the corbaDefVar
            ThreadGuard guard(m_varGuard);

            if (m_varInvalid)
            {
                resolve();
            }

            return m_corbaDefVar;
        }

        /**
          * operator->
          *
          * Alternative mechanism to access to the m_corbaDefVar member. Allows
          * NamedObjects to be dereferenced as if they were normal object refs
          *
          * @return CORBA Object reference. Never nil
          *
          * @exception ObjectResolutionException
          */
        CorbaDef_var operator->()
        {
            return corbaDefPtr();
        }

        /**
          * setObjectTimeout
          *
          * This function will allow programmers to specify specific timeouts for
          * NamedObjects. If not called, it will default to the default timeout
          * period (20 secs).
          *
          * valid range: 0 - 300 ( 0 disables the timeout, the next call will default to the global timeout )
          *
          * @param unsigned short seconds, the timeout period in seconds
          */
        virtual void setObjectTimeout(unsigned short seconds)
        {
            //TA_ASSERT( ( seconds >= 0 ) && ( seconds <= 300 ),
            TA_ASSERT(seconds <= 300, "Object timeout must be in the range of 0 - 300 seconds");

            // apply the timeout to the object reference, the timeout is in milliseconds!
            // Note that if the value is 0, this will disable the timeout ( and default to the global timeout value )
            m_timeoutPeriod = seconds;

            // Make sure the obj reference is valid before calling. If not valid
            // cache the timeoutPeriod and apply once the object is successfully resolved
            if (! CORBA::is_nil(m_corbaDefVar))
            {
                omniORB::setClientCallTimeout(m_corbaDefVar, (m_timeoutPeriod * 1000));

                // the object is now valid
                m_varInvalid = false;
            }
        }

        /**
          * assignObject
          *
          * Assign a new object var as received from the NameRebind message
          *
          * @param	newObject	the new Object to use
          */
        virtual void assignObject(CORBA::Object_var newObject)
        {
            // Gaurd the corbaDefVar
            ThreadGuard guard(m_varGuard);

            try
            {
                m_corbaDefVar = CorbaDef::_narrow(newObject);
            }
            catch (const CORBA::SystemException&)
            {
                TA_Base_Core::ObjectResolutionException ex(TA_Base_Core::RESOLUTION_FAILED,
                                                           "NamedObjRef::resolve - error narrowing to templatised interface");
                TA_THROW(ex);
            }

            if (CORBA::is_nil(m_corbaDefVar))
            {
                TA_Base_Core::ObjectResolutionException ex(TA_Base_Core::WRONG_INTERFACE,
                                                           "NamedObjRef::resolve - narrowed object is nil");
                TA_THROW(ex);
            }

            // If we have a timeout set, apply it to the new object reference
            setObjectTimeout(m_timeoutPeriod);
        }

        virtual void assignObject(CorbaDef_var newObject)
        {
            // Gaurd the corbaDefVar
            ThreadGuard guard(m_varGuard);

            m_corbaDefVar = newObject;

            if (CORBA::is_nil(m_corbaDefVar))
            {
                TA_Base_Core::ObjectResolutionException ex(TA_Base_Core::WRONG_INTERFACE,
                                                           "NamedObjRef::resolve - narrowed object is nil");
                TA_THROW(ex);
            }

            // If we have a timeout set, apply it to the new object reference
            setObjectTimeout(m_timeoutPeriod);
        }

        /**
          * clear
          *
          * Clear the cache, so the reference returns to the uninitialised state
          */
        virtual void clear()
        {
            // Gaurd the corbaDefVar
            //ThreadGuard guard( m_varGuard );

            LOG_GENERIC(SourceInfo,
                        TA_Base_Core::DebugUtil::DebugDebug,
                        "NamedObject::clear() called for [ %s, %s ]",
                        getAgentName().c_str(),
                        getObjectName().c_str());

            // Clear our cache
            // m_corbaDefVar = CorbaDef::_nil();
            // We no longer need to lock as we are merely flagging this object as invalid. The next
            // call on the object will force it to re-resolve
            m_varInvalid = true;
            setHostname("");
        }

    protected:
        /**
          * resolve
          *
          * Force immediate resolution of the object reference from the name and
          * narrow to the templatised class
          *
          * @exception ObjectResolutionException
          */
        virtual void resolve()
        {
            // Narrow to the templatised interface class
            try
            {
                m_corbaDefVar = CorbaDef::_narrow(resolveObject());
            }
            catch (const CORBA::SystemException&)
            {
                TA_THROW(TA_Base_Core::ObjectResolutionException(TA_Base_Core::WRONG_INTERFACE,
                                                                 getAgentName(),
                                                                 getObjectName(),
                                                                 "Exception while narrowing resolve Object to templatised interface"));
            }

            if (CORBA::is_nil(m_corbaDefVar))
            {
                TA_THROW(TA_Base_Core::ObjectResolutionException(TA_Base_Core::WRONG_INTERFACE,
                                                                 getAgentName(),
                                                                 getObjectName(),
                                                                 "Narrowing of resolved Object failed, object is nil"));
            }

            // if we have a timeout set, apply it to the new object reference
            setObjectTimeout(m_timeoutPeriod);
        }

        /**
        * Use the _var type to store the narrowed object reference. This looks
        * after memory management for us
        */
        CorbaDef_var m_corbaDefVar;

        /**
         * Store the timeout period. If it is 0, there is no timeout set (it is disabled)
         */
        unsigned short m_timeoutPeriod;

        /**
         * keep track of whether or not the var is actually invalid
         */
        volatile bool m_varInvalid;

    private:

        /**
        * constructor
        *
        * Copy constructor hidden
        */
        NamedObject(const NamedObject&);

        /**
         * hide the assignment operator
         */
        NamedObject& operator=(const NamedObject&);

        /**
         * Protect the m_corbaDefVar against MT access
         */
        NonReEntrantThreadLockable m_varGuard;
    };
}; // namespace TA_Base_Core

#define CORBACALL(statement, namedObject)                                                                                                                           \
    try                                                                                                                                                             \
    {                                                                                                                                                               \
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugCORBA, "CORBACALL BEGIN [%s]: %s", namedObject.getDescription().c_str(), #statement);                 \
        statement;                                                                                                                                                  \
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugCORBA, "CORBACALL END [%s]: %s", namedObject.getDescription().c_str(), #statement);                   \
    }                                                                                                                                                               \
    catch (TA_Base_Core::OperationModeException& e)                                                                                                                 \
    {                                                                                                                                                               \
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );                                                                   \
        namedObject.clear();                                                                                                                                        \
        statement;                                                                                                                                                  \
    }                                                                                                                                                               \
    catch (CORBA::TIMEOUT& e)                                                                                                                                       \
    {                                                                                                                                                               \
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::TIMEOUT", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str());                                     \
        throw;                                                                                                                                                      \
    }                                                                                                                                                               \
    catch (CORBA::SystemException& e)                                                                                                                               \
    {                                                                                                                                                               \
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str());                             \
        namedObject.clear();                                                                                                                                        \
        statement;                                                                                                                                                  \
    }

#define CORBA_CALL_RETURN(variable, namedObject, corba_function, parameters) CORBACALL((variable = namedObject -> corba_function parameters), namedObject)
#define CORBA_CALL(namedObject, corba_function, parameters ) CORBACALL((namedObject -> corba_function parameters), namedObject)


#ifdef WIN32
    #pragma warning(pop)
#endif // WIN32

#endif // NAMED_OBJECT_H
