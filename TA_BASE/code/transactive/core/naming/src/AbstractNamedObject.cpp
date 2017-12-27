/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/AbstractNamedObject.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class wrappers a CORBA object reference and associates it with
  * an agent and a name. The agent will be used to resolve the agent object
  * reference via the TDNS. Once the agent is resolved this will allow us
  * to make direct calls on the agent to resolve the necessary objects
  * 
  * The NamedObject also listens for NameRebindMessages that are sent by
  * agents when they enter control mode. This will allow the NamedObject
  * to attain a new object reference without actually needing to re-resolve
  * anything
  *
  * The NamedObject template specialises this class to deal with a single
  * interface
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 4250 )
#endif // defined( WIN32 )

#include "core/naming/src/AbstractNamedObject.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/naming/src/Naming.h"
#include "core/naming/IDL/src/RebindNameCorbaDef.h"
#include "core/naming/IDL/src/NamedObjectRepositoryCorbaDef.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/naming/src/BindingHandler.h"

namespace TA_Base_Core
{
    AbstractNamedObject::AbstractNamedObject()
    {
        Naming::getInstance().registerNamingUser( this );
    }


    AbstractNamedObject::~AbstractNamedObject()
    {
        Naming::getInstance().deregisterNamingUser( this );
    }


    std::string AbstractNamedObject::getAgentName( void ) const
    {
        return m_agentName;
    }


    std::string AbstractNamedObject::getObjectName( void ) const
    {
        return m_objectName;
    }


    CorbaName AbstractNamedObject::getCorbaName( void ) const
    {
        return CorbaName( m_agentName, m_objectName );
    }


    void AbstractNamedObject::setCorbaName( const CorbaName& corbaName )
    {
        setCorbaName( corbaName.getAgentName(), corbaName.getObjectName() );
    }


    void AbstractNamedObject::setCorbaName( const std::string& agentName, const std::string& objectName )
    {
        TA_ASSERT( !agentName.empty(), "The agent name used in setName was empty" );
        TA_ASSERT( !objectName.empty(), "The object name used in setName was empty" );

        LOG( SourceInfo,
            TA_Base_Core::DebugUtil::GenericLog, 
            TA_Base_Core::DebugUtil::DebugDebug,
            "AbstractNamedObject::setCorbaName() called for [ %s, %s ]",
            agentName.c_str(),
            objectName.c_str() );

        if ( m_objectName.empty() && m_agentName.empty() )
        {
            // first time in subscribing to messages
            m_objectName = objectName;
            m_agentName = agentName;

            return;
        }

        // Check to see if the name has changed
        if ( m_objectName.compare( objectName ) != 0 )
        {
            clear();
            m_objectName = objectName;
        }

        // Check to see if the context has changed
        if ( m_agentName.compare( agentName ) != 0 )
        {
            clear();

            // Store the new context
            m_agentName = agentName;
        }    
    }


    CORBA::Object_var AbstractNamedObject::resolveObject()
    {
        TA_ASSERT( !m_agentName.empty(), "The agent name is empty, please use setName() prior to using this NamedObject!" );
        TA_ASSERT( !m_objectName.empty(), "The object name is empty, please use setName() prior to using this NamedObject!" );

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "AbstractNamedObject::resolve() called for [ %s, %s ]",
            m_agentName.c_str(), m_objectName.c_str() );

        // If ResolveName() fails then we will get a nil objref, but we can't
        // actually distinguish between "no naming service" and "no binding" and
        // "resolution failed" at this stage
        CORBA::Object_var obj;
        try
        {
            obj = TA_Base_Core::Naming::getInstance().resolveName(m_agentName, m_objectName);
        }
        catch ( const TA_Base_Core::ObjectResolutionException& )
        {
            // Pass this on to the client
            throw;
        }
        catch ( const TA_Base_Core::OperationModeException& e )
        {
            TA_THROW( TA_Base_Core::ObjectResolutionException( TA_Base_Core::OPERATION_MODE,
                m_agentName,
                m_objectName,
                e.reason.in() ) );
        }
        catch( ... )
        {
            TA_THROW( TA_Base_Core::ObjectResolutionException( TA_Base_Core::UNKNOWN_ERROR,
                m_agentName,
                m_objectName,
                "Unknown Exception Caught" ) );
        }

        if ( CORBA::is_nil( obj ) )
        {
            TA_THROW( TA_Base_Core::ObjectResolutionException( TA_Base_Core::RESOLUTION_FAILED,
                m_agentName,
                m_objectName,
                "Resolved object is nil" ) );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "AbstractNamedObject::resolve <%s, %s> - resolution successful", m_agentName.c_str(), m_objectName.c_str() );

        m_hostname = TA_Base_Core::Naming::getInstance().getHostname(m_agentName);

        // Return the object
        return obj;
    }


    void AbstractNamedObject::handleNamingUpdate( const std::string& agentName, 
                                                  const NameObjectPairSeq& namedObjectSeq )
    {
        if (m_agentName != agentName )
        {
            return;
        }

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            "AbstractNamedObject::handleNamingUpdate() - clearing object reference for [ %s, %s ], namedObjectSeq.length=%d",
            m_agentName.c_str(), m_objectName.c_str(), namedObjectSeq.length() );

        // Clear the reference
        clear();

        if (0 == namedObjectSeq.length())
        {
            NamedObjectRepositoryCorbaDef_var repository = Naming::getInstance().getObjectRepositoryCorbaDef(agentName);

            if (!CORBA::is_nil(repository))
            {
                try
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                        "AbstractNamedObject::handleNamingUpdate() - get object reference for [ %s, %s ]",
                        m_agentName.c_str(), m_objectName.c_str() );

                    CORBA::Object_var obj = repository->getObject(m_objectName.c_str());
                    assignObject(obj);
                    return;
                }
                catch( const CORBA::Exception& ex )
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "AbstractNamedObject::handleNamingUpdate - Caught CORBA::Exception: %s", CorbaUtil::getInstance().exceptionToString(ex).c_str());
                }
            }
        }

        // If this message has an NameObjectPairSeq then we can go through and find
        // the name of the object that we currently have a pointer to.
        for ( unsigned int i = 0; i < namedObjectSeq.length(); i++ )
        {
            // If We find the name, we need to assign the new object reference
            if ( m_objectName.compare( namedObjectSeq[i].objectName ) == 0 )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                    "AbstractNamedObject::handleNamingUpdate() - Assigning new object reference to [ %s, %s ]",
                    m_agentName.c_str(), m_objectName.c_str() );

                // Assign the new object so that the next call will use it!
                assignObject( namedObjectSeq[i].objectReference );
                
                return;
            }
        }
    }


    void AbstractNamedObject::invalidateObject( const std::string& agentName )
    {
        // If the agent name is for this NamedObject clear the cache
        if (m_agentName == agentName)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "AbstractNamedObject::invalidateObject() - clearing object reference for [ %s, %s ]",
                m_agentName.c_str(), m_objectName.c_str() );

            // Clear the reference on the Derived class, will in turn clear on this class
            clear();
        }
        
        // This object isn't a part of this agent so just ignore it
    }


    std::string AbstractNamedObject::getDescription() const
    {
        std::stringstream strm;
        strm << "[" << m_agentName << "," << m_objectName;

        if (!m_hostname.empty())
        {
            strm << "," << m_hostname;
        }

        strm << "]";
        return strm.str();
    }

} //namespace TA_Base_Core
