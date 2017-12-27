/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/BindingHandler.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/09/02 14:28:39 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This is a locally created object that wrappers the sending
  * and receiving of BindingRequestMessages
  * The executing thread will block querying this object.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786 4819 4996)
#endif // defined (_MSC_VER)

#include "core/naming/src/BindingHandler.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"
#include "core/message/src/BindingRequestMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/NamingService_MessageTypes.h"
#include "core/message/types/NameRebindStateUpdate_MessageTypes.h"
#include "core/naming/IDL/src/NamedObjectRepositoryCorbaDef.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/NamingPortAccessFactory.h"
#include "core/naming/src/Naming.h"
#include "core/naming/src/NamingThreadPoolSingleton.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#define DEFAULT_TTL 300

namespace TA_Base_Core
{
    ObjectRepositoryInfo::ObjectRepositoryInfo( const std::string& agentName )
        : m_agentName(agentName),
          m_timeToLive(DEFAULT_TTL),
          m_bindingHandler(NULL),
          m_namingThreadPoolSingleton(NamingThreadPoolSingleton::getInstance()),
          m_resolving(false)
    {
        FUNCTION_ENTRY( "ObjectRepositoryInfo" );

        // Set up the inital expiry time
        resetExpiry(true);

        {
            // repository format: corbaloc::%s:%d/%sRepo
            static std::string repository = RunParams::getInstance().get("NamingRepository");
            std::string repository1 = RunParams::getInstance().get(m_agentName + "NamingRepository1");
            std::string repository2 = RunParams::getInstance().get(m_agentName + "NamingRepository2");

            if (RunParams::getInstance().isSet(m_agentName + "NamingRepository"))
            {
                repository1 = RunParams::getInstance().get(m_agentName + "NamingRepository");
            }

            try
            {
                std::vector<std::string> vs;

                if (repository1.size() || repository2.size())
                {
                    boost::split(vs, repository1, boost::is_any_of(":/"), boost::token_compress_on);

                    if (vs.size() == 4)
                    {
                        m_primaryNSHostname = vs[1];
                        m_primaryNSPort = boost::lexical_cast<unsigned long>(vs[2]);
                        m_primaryNSString = repository1;
                    }

                    boost::split(vs, repository2, boost::is_any_of(":/"), boost::token_compress_on);

                    if (vs.size() == 4)
                    {
                        m_secondaryNSHostname = vs[1];
                        m_secondaryNSPort = boost::lexical_cast<unsigned long>(vs[2]);
                        m_secondaryNSString = repository2;
                    }
                }
                else if (repository.size())
                {
                    boost::split(vs, repository, boost::is_any_of(":/"), boost::token_compress_on);

                    if (vs.size() == 4)
                    {
                        m_primaryNSHostname = vs[1];
                        m_primaryNSPort = boost::lexical_cast<unsigned long>(vs[2]);
                        m_primaryNSString = repository;
                    }
                }
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Bad NamingRepository for %s: repository=%s, repository1=%s, repository2=%s", m_agentName.c_str(), repository.c_str(), repository1.c_str(), repository2.c_str());
            }
        }

        if (m_primaryNSString.empty() && m_secondaryNSString.empty())
        {
		    try
		    {
                // yanrong++: CL-21230
			    m_primaryNSHostname     = NamingPortAccessFactory::getInstance().getNamingPort(m_agentName).getPrimaryHostname();
			    m_primaryNSPort         = NamingPortAccessFactory::getInstance().getNamingPort(m_agentName).getPrimaryPort();
			    m_secondaryNSHostname   = NamingPortAccessFactory::getInstance().getNamingPort(m_agentName).getSecondaryHostname();
			    m_secondaryNSPort       = NamingPortAccessFactory::getInstance().getNamingPort(m_agentName).getSecondaryPort();

                if ( m_primaryNSHostname.size() && m_primaryNSPort != 0 )
                {
                    m_primaryNSString = str(boost::format("corbaloc::%s:%d/%sRepo") % m_primaryNSHostname  % m_primaryNSPort % m_agentName);
                }

                if ( m_secondaryNSHostname.size() && m_secondaryNSPort != 0 )
                {
                    m_secondaryNSString = str(boost::format("corbaloc::%s:%d/%sRepo") % m_secondaryNSHostname  % m_secondaryNSPort % m_agentName);
                }
		    }
		    catch( const TA_Base_Core::DataException& ex )
		    {
			    LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
		    }
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "ObjectRepositoryInfo Ctor, agentName=%s, primaryNS=%s, secondaryNS=%s", agentName.c_str(), m_primaryNSString.c_str(), m_secondaryNSString.c_str());

        FUNCTION_EXIT;
    }


    ObjectRepositoryInfo::~ObjectRepositoryInfo()
    {
        FUNCTION_ENTRY( "~ObjectRepositoryInfo" );

        // Clean up the handler
        if ( m_bindingHandler )
        {
            m_bindingHandler->deactivateAndDeleteServant();
            m_bindingHandler = NULL;
        }

        TA_Base_Core::NamingThreadPoolSingleton::removeInstance( m_namingThreadPoolSingleton );

        FUNCTION_EXIT;
    }


    void ObjectRepositoryInfo::invalidate()
    {
        FUNCTION_ENTRY( "invalidate" );

        TA_THREADGUARD( m_lock );

        // re-resolve asap
        m_expiryTime = 0;

        // Set back to default TTL
        m_timeToLive = DEFAULT_TTL;

        // Free up memory and set the repository to nil
        m_resolvedRepository = NamedObjectRepositoryCorbaDef::_nil();

        m_corbaloc = m_ior = m_hostname = "";

        Naming::getInstance().invalidateAgent(m_agentName);

        FUNCTION_EXIT;
    }


    CORBA::Object_var ObjectRepositoryInfo::getObject( const std::string& objectName )
    {
        FUNCTION_ENTRY( "getObject" );

        //chenlei++ CL-21230
        //guarantee that all name resolving requests are serial
        m_tss->setResolving(m_resolving);
        ThreadGuard repoInfoGuard( m_repoInfoLock );
        //++chenlei CL-21230

        if (!m_tss->isResolving())
        {
            // Resolve the repository if necessary
            m_resolving = true;
            resolveObjectRepository();
            m_resolving = false;
        }

        TA_THREADGUARD( m_lock );

        // if the repository is NULL we have not been able to resolve the object
        if ( CORBA::is_nil( m_resolvedRepository ) )
        {
            TA_THROW( TA_Base_Core::ObjectResolutionException( TA_Base_Core::NO_AGENT, m_agentName, objectName ) );
        }

        FUNCTION_EXIT;
        return m_resolvedRepository->getObject( objectName.c_str() );
    }


    void ObjectRepositoryInfo::setObjectRepository( NamedObjectRepositoryCorbaDef_var repository, unsigned long timeToLive )
    {
        FUNCTION_ENTRY( "setObjectRepository" );

        TA_THREADGUARD( m_lock );

        // insert new reference
        m_resolvedRepository = repository;
        m_timeToLive = timeToLive;

        // update the expiry time
        resetExpiry();

        //Bohong++ for more infomation when resolve
        CORBA::String_var iorstring = CorbaUtil::getInstance().getOrb().object_to_string(repository);
        m_ior = iorstring;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ObjectRepositoryInfo::setObjectRepository: %s, timeToLive=%lu", getURI().c_str(), timeToLive);

        FUNCTION_EXIT;
    }


    void ObjectRepositoryInfo::setObjectRepository_r( NamedObjectRepositoryCorbaDef_var repository, unsigned long timeToLive )
    {
        FUNCTION_ENTRY( "setObjectRepository_r" );

        setObjectRepository( repository, timeToLive );

        FUNCTION_EXIT;
    }


    void ObjectRepositoryInfo::resolveObjectRepository()
    {
        FUNCTION_ENTRY( "resolveObjectRepository" );

        // Check to make sure the object has not expired
        if ( time(NULL) < m_expiryTime )
        {
            TA_THREADGUARD( m_lock );

            // Check to see if the object is valid
            if ( !CORBA::is_nil( m_resolvedRepository ) )
            {
                // First we want to call poll on the object to see if it is valid, and in control mode
                // If the call succeeds we don't want to expire it
                try
                {
                    // Call fails, flag the need for re-resolution
                    m_resolvedRepository->poll();
                    // set expiry to specific ttl
                    resetExpiry();

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Cached NamedObjectRepository on [ %s ][ %s ] is valid.", m_agentName.c_str(), getURI().c_str());
                    return;
                }
                catch ( const OperationModeException& )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Cached NamedObjectRepository on [ %s ][ %s ] not in Control Mode. Cache cleared.", m_agentName.c_str(), getURI().c_str());
                }
                catch ( const CORBA::Exception& )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Cached NamedObjectRepository on [ %s ][ %s ] unavailable. Cache cleared.", m_agentName.c_str(), getURI().c_str());
                }
            }

            invalidate();
        }

        // We need to resolve the repository
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "NamedObjectRepository on [ %s ] is expired - attempting resolution.", m_agentName.c_str());

        // yanrong++: CL-21230
        if (m_primaryNSString.size() || m_secondaryNSString.size())
        {
            nsResolve();

            static bool useMessaging = TA_Base_Core::RunParams::getInstance().isSet("NamingUseMessaging");

            if (!useMessaging)
            {
                FUNCTION_EXIT;
                return;
            }
        }
        // ++yanrong

        if ( NULL == m_bindingHandler )
        {
            m_bindingHandler = new BindingHandler; // gongzong++ CL-21311
        }

        m_bindingHandler->resolveObjectRepository(this);

        FUNCTION_EXIT;
    }


    std::string ObjectRepositoryInfo::getAgentName() const
    {
        FUNCTION_ENTRY( "getAgentName" );
        FUNCTION_EXIT;
        return m_agentName;
    }


    void ObjectRepositoryInfo::resetExpiry(bool isExpired)
    {
        FUNCTION_ENTRY( "resetExpiry" );

        if ( !isExpired )
        {
            // reset the expiry time
            m_expiryTime = m_timeToLive + time( NULL );
        }
        else
        {
            // for fast recovery, if resolve failed, set expiry to be 30 seconds.
            static unsigned long s_failedTimeToLive = 5;
            // set the expiry time to current system time.
            //m_expiryTime = time( NULL ) + s_failedTimeToLive;
            m_expiryTime = 0; // resolve immediately
        }

        FUNCTION_EXIT;
    }


    // gongzong++ CL-21311
    IWorkItemPtr ObjectRepositoryInfo::queueNSResovleWorkItem(const std::string& corbaloc, unsigned long resolveTimeout, unsigned long& result)
    {
        FUNCTION_ENTRY( "queueNSResovleWorkItem" );

        if ( !corbaloc.empty() )
        {
            IWorkItemPtr item( new NSResovleWorkItem(*this, corbaloc.c_str(), resolveTimeout, result) );

            m_namingThreadPoolSingleton->queueWorkItem(item);

            FUNCTION_EXIT;
            return item;
        }
        else
        {
            FUNCTION_EXIT;
            return IWorkItemPtr();
        }
    }


    // yanrong++: CL-21230
    bool ObjectRepositoryInfo::nsResolve()
    {
        FUNCTION_ENTRY( "nsResolve" );

        // if we can't find NS URL then we needn't resolve it
        if ( m_primaryNSString.empty() && m_secondaryNSString.empty() )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "nsResolve return false because of no nsstring. agentName=%s", m_agentName.c_str());

            FUNCTION_EXIT;
            return false;
        }

        bool result = false;
        static unsigned long resolveTimeout = TA_Base_Core::getRunParamValue(RPARAM_RESOLVE_TIMEOUT_MS, 2000);
        static unsigned long failoverResolveTimeout = TA_Base_Core::getRunParamValue(RPARAM_FAILOVER_RESOLVE_TIMEOUT_MS, 5000);
        TA_Base_Core::MonotonicTimer timer;
        unsigned long long elapsed = 0;

        while (true)
        {
            m_primaryResolveResult = m_secondaryResolveResult = ResolveFailed;
            m_resolveFailedCounter = (!m_primaryNSString.empty()) + (!m_secondaryNSString.empty());
            IWorkItemPtr primaryWorkItem   = queueNSResovleWorkItem(m_primaryNSString.c_str(), resolveTimeout, m_primaryResolveResult);
            IWorkItemPtr secondaryWorkItem = queueNSResovleWorkItem(m_secondaryNSString.c_str(), resolveTimeout, m_secondaryResolveResult);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "nsResolve wait condition, timeout=%lu, failover-timeout=%lu, agentName=%s", resolveTimeout, failoverResolveTimeout, m_agentName.c_str());

            m_nsResolveCondition.timedWait(resolveTimeout);

            if (primaryWorkItem)   { m_namingThreadPoolSingleton->removeWorkItem(primaryWorkItem); }
            if (secondaryWorkItem) { m_namingThreadPoolSingleton->removeWorkItem(secondaryWorkItem); }
            elapsed = timer.elapsed();

            if (((m_primaryResolveResult | m_secondaryResolveResult) == ResolveOperationModeException) && (elapsed < failoverResolveTimeout))
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "nsResolve failover, timeout=%lu, failover-timeout=%lu, agentName=%s, elapsed=%llu", resolveTimeout, failoverResolveTimeout, m_agentName.c_str(), elapsed);
                TA_Base_Core::Thread::sleep(100);
            }
            else
            {
                result = ((ResolveSucceed == m_primaryResolveResult) || (ResolveSucceed == m_secondaryResolveResult));
                break;
            }
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "nsResolve finished, result=%s, timeout=%lu, failover-timeout=%lu, elapsed=%llu, agentName=%s",
            (result ? "true" : "false"), resolveTimeout, failoverResolveTimeout, elapsed, m_agentName.c_str());

        FUNCTION_EXIT;
        return result;
    }


    void ObjectRepositoryInfo::nsResolveProcess(const std::string& corbaloc, unsigned long& result)
    {
        FUNCTION_ENTRY( "nsResolveProcess" );

        TA_ASSERT(!corbaloc.empty(), "An empty ns string to be used");

        result = ResolveFailed;

        {
            // NS resolve timeout default use 2000 milliseconds
            static unsigned long resolveTimeout = getRunParamValue(RPARAM_RESOLVE_TIMEOUT_MS, 2000);
            try
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "corba stringToObject, string = %s", corbaloc.c_str()); //[CL-21230] 
                CORBA::Object_var obj = TA_Base_Core::CorbaUtil::getInstance().stringToObject(corbaloc.c_str());
                omniORB::setClientCallTimeout(obj, resolveTimeout);
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Narrow CorbaObject to NamedObjectRepositoryCorbaDef Object");
                NamedObjectRepositoryCorbaDef_var objectRepository = TA_Base_Core::NamedObjectRepositoryCorbaDef::_narrow(obj);

                if ( !CORBA::is_nil(objectRepository) )
                {
                    try
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Set poll timeout = %lu", resolveTimeout); //[CL-21311]
                        omniORB::setClientCallTimeout(objectRepository, resolveTimeout);
                        objectRepository->poll();
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "nsResolve successfully, corbaloc=%s", corbaloc.c_str());

                        setObjectRepository(objectRepository, m_timeToLive);
                        m_corbaloc = corbaloc;
                        m_hostname = (corbaloc == m_primaryNSString ? m_primaryNSHostname : m_secondaryNSHostname );
                        result = ResolveSucceed;
                        m_nsResolveCondition.signal(); //gongzong++ CL-21311

                        // got here means process successfully.
                        FUNCTION_EXIT;
                        return;
                    }
                    catch ( const OperationModeException& ex)
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Caught OperationModeException: %s, corbaloc=%s", ex.reason.in(), corbaloc.c_str());
                        result = ResolveOperationModeException;
                    }
                    catch ( const CORBA::Exception& ex )
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                            "Caught COBAException: %s, corbaloc=%s", CorbaUtil::getInstance().exceptionToString(ex).c_str(), corbaloc.c_str());
                    }
                    catch( ... )
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Caught unknown exception, corbaloc=%s", corbaloc.c_str());
                    }
                }
            }
            catch( const CORBA::Exception& ex )
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Caught COBAException: %s, corbaloc=%s", CorbaUtil::getInstance().exceptionToString(ex).c_str(), corbaloc.c_str());
            }
            catch( ... )
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Caught unknown exception, m_currentNSString=%s", corbaloc.c_str());
            }
        }

        {
            ThreadGuard guard(m_nsResolveCondition.getLinkedThreadLockable());

            if ( --m_resolveFailedCounter <= 0 )
            {
                m_nsResolveCondition.signal();
            }
        }
        // got here means some unexpected error happen
        // pushFailedNsString(corbaloc);

        FUNCTION_EXIT;
    }


    // void ObjectRepositoryInfo::pushFailedNsString(const std::string& nsstring)
    // {
    //     ThreadGuard guard(m_failedNsLock);
    //     m_failedNsStrings[nsstring] = false;
    //
    //     // if all the NS resolving is failed, the signal the waiting thread
    //     if(m_failedNsStrings.size() == m_nsStringCount)
    //     {
    //         ThreadGuard conditionGuard(m_nsResolveCondition.getLinkedThreadLockable());
    //         m_nsResolveCondition.signal();
    //     }
    // }
    //
    // void ObjectRepositoryInfo::clearFailedNsString()
    // {
    //     ThreadGuard guard(m_failedNsLock);
    //     m_failedNsStrings.clear();
    // }
    //
    // unsigned long ObjectRepositoryInfo::getFailedNsStringCount()
    // {
    //     ThreadGuard guard(m_failedNsLock);
    //     return (unsigned long)m_failedNsStrings.size();
    // }
    // ++yanrong

    /////////////////////////////////////////////////////////////
    // yanrong++: CL-21230
    NSResovleWorkItem::NSResovleWorkItem(ObjectRepositoryInfo& callback, const std::string& corbaloc, unsigned long resolveTimeout, unsigned long& result)
        : m_callback(callback),
          m_corbaloc(corbaloc),
          m_timeout(resolveTimeout),
          m_result(result)
    {
        FUNCTION_ENTRY( "NSResovleWorkItem" );
        FUNCTION_EXIT;
    }


    void NSResovleWorkItem::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        unsigned long long elapsed = m_timer.elapsed();

        if (elapsed < m_timeout)
        {
            m_callback.nsResolveProcess(m_corbaloc.c_str(), m_result);
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "NSResovleWorkItem corbaloc=%s, elapsed=%llu", m_corbaloc.c_str(), elapsed);
        }

        FUNCTION_EXIT;
    }


    // ++yanrong

    /////////////////////////////////////////////////////////////

    BindingHandler::BindingHandler()
        : m_reposInfo( NULL ),
          m_bindingRequestSender(NULL)
    {
        FUNCTION_ENTRY( "BindingHandler" );

        activateServant();

        m_bindingRequestSender = std::auto_ptr<BindingRequestMessageSender>(
            TA_Base_Core::MessagePublicationManager::getInstance().getBindingRequestMessageSender(
                TA_Base_Core::NamingService::BindName) );

        TA_ASSERT( NULL != m_bindingRequestSender.get(), "Unable to get the BindingRequestMessageSender" );

        FUNCTION_EXIT;
    }


    void BindingHandler::resolveObjectRepository( ObjectRepositoryInfo* reposInfo )
    {
        FUNCTION_ENTRY( "resolveObjectRepository" );

        TA_ASSERT( reposInfo != NULL, "ObjectRepositoryInfo should not be NULL" );
        const unsigned long DEFAULT_RESOLVE_TIMEOUT_MS = 2000; // 2 seconds
        static unsigned long s_resolveTimeoutMs = getRunParamValue(RPARAM_RESOLVE_TIMEOUT_MS, DEFAULT_RESOLVE_TIMEOUT_MS);

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingHandler::resolveObjectRepository: agentname=%s, try to gain condition lock", reposInfo->getAgentName().c_str());

        ThreadGuard guard(m_condition.getLinkedThreadLockable());

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingHandler::resolveObjectRepository: gained condition lock, is sending binding request message for naming, agentname=%s, resolveTime=%d, timewait begin",
            reposInfo->getAgentName().c_str(), s_resolveTimeoutMs );

        // Store the repository for the callback
        m_reposInfo = reposInfo;

        m_bindingRequestSender->sendBindingRequest( TA_Base_Core::NamingService::BindName,
                                                    m_reposInfo->getAgentName(),
                                                    this->_this() );

        // timedWait() unlocks the lock object so callback() can proceed
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingHandler::resolveObjectRepository: sent binding request message for naming, agentname=%s, resolveTime=%d, timewait begin",
            m_reposInfo->getAgentName().c_str(), s_resolveTimeoutMs );

        m_condition.timedWait( s_resolveTimeoutMs );

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingHandler::resolveObjectRepository: sent binding request message for naming, agentname=%s, resolveTime=%d, timewait end",
            m_reposInfo->getAgentName().c_str(), s_resolveTimeoutMs );

        FUNCTION_EXIT;
    }


    void BindingHandler::callback( NamedObjectRepositoryCorbaDef_ptr requestedObject, CORBA::ULong timeToLive )
    {
        FUNCTION_ENTRY( "callback" );

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BindingHandler::callback begin" );

        ThreadGuard guard(m_condition.getLinkedThreadLockable());

        TA_Assert(!CORBA::is_nil(requestedObject));

        static unsigned long resolveTimeout = getRunParamValue(RPARAM_RESOLVE_TIMEOUT_MS, 2000);
        omniORB::setClientCallTimeout(requestedObject, resolveTimeout);

        m_reposInfo->setObjectRepository( NamedObjectRepositoryCorbaDef::_duplicate(requestedObject), timeToLive );

        m_condition.signal();

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BindingHandler::callback return, resolved successful" );

        FUNCTION_EXIT;
    }


}; // TA_Base_Core
