/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainAgentAccess.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of TrainAgentAccess.h - see header for class description.
  *
  */

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"

#include "TrainAgentAccess.h"
#include "TrainAgentAccessException.h"

namespace
{
    static const short TRAIN_AGENT_TIMEOUT(60); // secs
}

using namespace TA_Base_App;


TrainAgentAccess* TrainAgentAccess::s_trainAgentAccess = NULL;
TA_Base_Core::NonReEntrantThreadLockable TrainAgentAccess::s_singletonLock;


TrainAgentAccess& TrainAgentAccess::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    TA_Base_Core::ThreadGuard guard(s_singletonLock);

    if (s_trainAgentAccess == NULL)
    {
        s_trainAgentAccess = new TrainAgentAccess();
    }

    FUNCTION_EXIT;
    return *s_trainAgentAccess;
}


TrainAgentAccess::TrainAgentAccess()
{
    FUNCTION_ENTRY( "TrainAgentAccess" );

    // Get the corba name of the OCC Train Agent. Only that agent will have information about ALL trains in the system.
    const std::string trainAgentType(TA_Base_Core::TrainAgentEntityData::getStaticType());
    const unsigned long trainAgentLocationKey(TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey());

    TA_Base_Core::CorbaNameList trainAgentNames(
        TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(trainAgentType, trainAgentLocationKey,true));

    // Make sure there's exactly one Train Agent entity configured for this location.
    if (trainAgentNames.empty())
    {
        std::ostringstream msg;
        msg << "There are no entities of type " << trainAgentType << " configured for OCC.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());

        TA_THROW(TrainAgentAccessException(msg.str()));
    }
    else if (trainAgentNames.size() > 1)
    {
        std::ostringstream msg;
        msg << "There are multiple entities of type " << trainAgentType << " configured for OCC.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());

        TA_THROW(TrainAgentAccessException(msg.str()));
    }

    // Set the named objects' corba name so that the Train Agent can be resolved correctly.
    m_trainInfo.setCorbaName(trainAgentNames[0]);
    m_trainTisOps.setCorbaName(trainAgentNames[0]);

    // Extend the default corba timeout.
    m_trainInfo.setObjectTimeout(TRAIN_AGENT_TIMEOUT);
    m_trainTisOps.setObjectTimeout(TRAIN_AGENT_TIMEOUT);

    FUNCTION_EXIT;
}


TrainAgentAccess::~TrainAgentAccess()
{
    FUNCTION_ENTRY( "~TrainAgentAccess" );

    TA_Base_Core::ThreadGuard guard(s_singletonLock);
    s_trainAgentAccess = NULL;

    FUNCTION_EXIT;
}


TrainAgentAccess::TrainIdSet TrainAgentAccess::getInboundTrainIds()
{
    FUNCTION_ENTRY( "getInboundTrainIds" );

    initialiseTrainIdMap();

    TA_Base_Core::ThreadGuard guard(m_trainIdMapLock);
    TrainIdSet inboundTrainIds;

    TrainIdMap::const_iterator mapIt(m_trainIdMap.find(TA_Base_Bus::ITrainInformationCorbaDef::Track_Inbound));
    if (mapIt != m_trainIdMap.end())
    {
        inboundTrainIds = mapIt->second;
    }

    FUNCTION_EXIT;
    return inboundTrainIds;
}


TrainAgentAccess::TrainIdSet TrainAgentAccess::getOutboundTrainIds()
{
    FUNCTION_ENTRY( "getOutboundTrainIds" );

    initialiseTrainIdMap();

    TA_Base_Core::ThreadGuard guard(m_trainIdMapLock);
    TrainIdSet outboundTrainIds;

    TrainIdMap::const_iterator mapIt(m_trainIdMap.find(TA_Base_Bus::ITrainInformationCorbaDef::Track_Outbound));
    if (mapIt != m_trainIdMap.end())
    {
        outboundTrainIds = mapIt->second;
    }

    FUNCTION_EXIT;
    return outboundTrainIds;
}


TrainAgentAccess::TrainIdSet TrainAgentAccess::getAllTrainIds()
{
    FUNCTION_ENTRY( "getAllTrainIds" );

    initialiseTrainIdMap();

    TA_Base_Core::ThreadGuard guard(m_trainIdMapLock);
    TrainIdSet allTrainIds;

    for (TrainIdMap::const_iterator mapIt(m_trainIdMap.begin()); mapIt != m_trainIdMap.end(); mapIt++)
    {
        const TrainIdSet& trackSpecificTrainIds = mapIt->second;

        for (TrainIdSet::const_iterator idIt(trackSpecificTrainIds.begin()); idIt != trackSpecificTrainIds.end(); idIt++)
        {
            allTrainIds.insert(*idIt);
        }
    }

    FUNCTION_EXIT;
    return allTrainIds;
}


unsigned short TrainAgentAccess::getTisLibraryVersion(TrainId trainId)
{
    FUNCTION_ENTRY( "getTisLibraryVersion" );

    try
    {
        unsigned short libraryVersion = 0;
        CORBA_CALL_RETURN( libraryVersion, m_trainTisOps, getLibraryVersion, ( trainId ) );//limin

        FUNCTION_EXIT;
        return libraryVersion;
    }
    catch (const TA_Base_Bus::ITISTrainCommandsCorbaDef::InvalidParameter& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). Caught InvalidParameter exception: %s", ex.details.in());

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent.", ex.details.in()));
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). Caught OperationModeException: %s", ex.reason.in());

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent.", ex.reason.in()));
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). Caught ObjectResolutionException: %s", ex.what());

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent.", ex.what()));
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). Caught TransactiveException: %s", ex.what());

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent.", ex.what()));
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent.",
                                            std::string("CORBA::Exception: ") +
                                            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)));
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). Caught unknown exception.");

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent."));
    }

    FUNCTION_EXIT;
    return 0;
}


void TrainAgentAccess::initialiseTrainIdMap()
{
    FUNCTION_ENTRY( "initialiseTrainIdMap" );

    TA_Base_Core::ThreadGuard guard(m_trainIdMapLock);

    // Get train details from the Train Agent only if we haven't already got them.
    if (!m_trainIdMap.empty())
    {
        FUNCTION_EXIT;
        return;
    }

    try
    {
        TA_Base_Bus::ITrainInformationCorbaDef::TrainDetailsList_var trainList;
        CORBA_CALL_RETURN( trainList, m_trainInfo, getTrainList, () );//limin

        // Arrange the train IDs into separate sets, one set per track.
        for (unsigned long i(0); i < trainList->length(); i++)
        {
            TA_Base_Bus::ITrainInformationCorbaDef::TrainDetailsNotification& trainDetails = trainList[i];
            m_trainIdMap.insert(std::make_pair(trainDetails.track, TrainIdSet())); // insertion occurs only if the keyed
                                                                                   // element does not already exist
            TrainIdMap::iterator mapIt(m_trainIdMap.find(trainDetails.track));     // get a reference to the relevant ID set
            mapIt->second.insert(trainDetails.trainID);                            // pop the given train ID into the set
        }
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). Caught OperationModeException: %s", ex.reason.in());

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent.", ex.reason.in()));
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). Caught ObjectResolutionException: %s", ex.what());

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent.", ex.what()));
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). Caught TransactiveException: %s", ex.what());

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent.", ex.what()));
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent.",
                                            std::string("CORBA::Exception: ") +
                                            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)));
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Train Agent access failure in getTrainList(). Caught unknown exception.");

        TA_THROW(TrainAgentAccessException("Failed to retrieve train details from Train Agent."));
    }

    FUNCTION_EXIT;
}


