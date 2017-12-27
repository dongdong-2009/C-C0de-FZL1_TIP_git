/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/ScadaPersistence.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Class for managing the persistence of DataPoint / DataNode states
 *
 */

#if defined(_MSC_VER)
#pragma warning(disable : 4006)
#pragma warning(disable : 4503)
#endif // defined _MSC_VER

#include "bus/scada/common_library/src/ScadaPersistence.h"
#include "bus/history/data_recording/src/ExceptionOnWriteErrorPolicy.h"
#include "bus/history/data_recording/src/DataRecorderDbDataAccess.h"
#include "bus/scada/datanode_library/src/DataNodeStates.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "boost/lexical_cast.hpp"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{


    ScadaPersistence* ScadaPersistence::createScadaPersistence( EScadaPersistenceType type )
    {
        FUNCTION_ENTRY( "createScadaPersistence" );
        FUNCTION_EXIT;
        return new ScadaPersistence(type);
    }


    ScadaPersistence* ScadaPersistence::createScadaPersistenceForAgent( EScadaPersistenceType type, unsigned long agentKey )
    {
        FUNCTION_ENTRY( "createScadaPersistenceForAgent" );
        FUNCTION_EXIT;
        return new ScadaPersistence(type, agentKey);
    }


    ScadaPersistence* ScadaPersistence::createScadaPersistenceForLocation( EScadaPersistenceType type, unsigned long locationKey )
    {
        FUNCTION_ENTRY( "createScadaPersistenceForLocation" );
        FUNCTION_EXIT;
        return new ScadaPersistence(type, false, locationKey);
    }



    ScadaPersistence::ScadaPersistence( EScadaPersistenceType type )
    : m_typeKey( 0 ), m_typeName("")
    {
        FUNCTION_ENTRY( "ScadaPersistence( type )" );
        
        initialise(type);

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
            "ScadaPersistence created for type '%s' (pkey=%d) - Loaded 0 persisted states",
            m_typeName.c_str(), m_typeKey );

        FUNCTION_EXIT;
    }


    ScadaPersistence::ScadaPersistence( EScadaPersistenceType type, unsigned long agentKey )
    : m_typeKey( 0 ), m_typeName("")
    {
        FUNCTION_ENTRY( "ScadaPersistence( type, agentKey )" );

        TA_ASSERT( agentKey != 0, "Agent key cannot be 0 - use the other constructor" );
        
        initialise(type);

        //
        // Retrieve all persisted states for this type and agent
        //
        m_states = TA_Base_Core::PersistedStateAccessFactory::getInstance().getPersistedStatesOfTypeForAgent( m_typeKey, agentKey );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
            "ScadaPersistence created for type '%s' (pkey=%d) and agent %d - Loaded %d persisted states",
            m_typeName.c_str(), m_typeKey, agentKey, m_states.size() );

        FUNCTION_EXIT;
    }

    ScadaPersistence::ScadaPersistence( EScadaPersistenceType type, bool dummy, unsigned long locationKey )
    : m_typeKey( 0 ), m_typeName("")
    {
        FUNCTION_ENTRY( "ScadaPersistence( type, dummy, locationKey )" );
        
        TA_ASSERT( locationKey != 0, "Location key cannot be 0 - use the other constructor" );

        initialise(type);

        //
        // Retrieve all persisted states for this type and location
        //
        m_states = TA_Base_Core::PersistedStateAccessFactory::getInstance().getPersistedStatesOfTypeAtLocation( m_typeKey, locationKey );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
            "ScadaPersistence created for type '%s' (pkey=%d) and location %d - Loaded %d persisted states",
            m_typeName.c_str(), m_typeKey, locationKey, m_states.size() );

        FUNCTION_EXIT;
    }


    ScadaPersistence::~ScadaPersistence()
    {
        // Nothing
    }


    void ScadaPersistence::initialise( EScadaPersistenceType type )
    {
        //
        // Retrieve pkey of DataPoint/DataNode entity type
        //
        TA_Base_Core::EntityAccessFactory& entityAccess = TA_Base_Core::EntityAccessFactory::getInstance();
        std::map<unsigned long, std::string> typeMap = entityAccess.getEntityTypes();
        
        switch( type )
        {
            case DATAPOINT :
                m_typeName.assign( TA_Base_Core::DataPointEntityData::getStaticType() );
                break;

            case DATANODE :
                m_typeName.assign( TA_Base_Core::DataNodeEntityData::getStaticType() );
                break;

            default :
                TA_ASSERT( false, "Unknown EScadaPersistenceType value" );
                break;
        }

        std::map<unsigned long, std::string>::const_iterator typeIter = typeMap.begin();
        for( ; typeIter != typeMap.end(); ++typeIter )
        {
            if ( typeIter->second == m_typeName )
            {
                m_typeKey = typeIter->first;
                break;
            }
        } 

        TA_ASSERT( m_typeKey != 0, "Could not locate SCADA type in entity types map" );
        
        //
        // Initialise Data Recording components
        //
        m_dataRecorderDbAccess = IDataRecorderDbAccessPtr( new DataRecorderDbDataAccess() );
        m_dataRecorderWritePolicy = IDataWritePolicyPtr( new ExceptionOnWriteErrorPolicy() );
        m_dataRecorder = std::auto_ptr<DataRecorder>( new DataRecorder( m_dataRecorderDbAccess, m_typeKey, 
                                                                        DataRecorder::SYNCHRONOUS, m_dataRecorderWritePolicy ) );

        m_databaseWriter = IDataWriterPtr( new DbDataWriter() );

        m_dataRecorder->setDataWriter( m_databaseWriter );
        m_dataRecorder->startRecording();
    }


    void ScadaPersistence::readDataNodeState( unsigned long entityKey, TA_Base_Bus::DnCompleteDynamicSyncState& state )
    {
        TA_Base_Core::PersistedStatePtr persistedState;

        StateMap::iterator stateIter = m_states.find( entityKey );
        if ( stateIter == m_states.end() )
        {
            persistedState = TA_Base_Core::PersistedStateAccessFactory::getInstance().getPersistedState( entityKey, m_typeKey );
        }
        else
        {
            persistedState = stateIter->second;
        }
        
        
        std::string tempString( "" );

        // Tag State
        persistedState->getValue( mapEPersistedItem2DataRecordingId( DN_TAG_STATUS ), tempString );
        state.tagState.tagState = stringToTagState( tempString );
        // Tag Output Inhibit
        tempString = "";
        persistedState->getValue( mapEPersistedItem2DataRecordingId( DN_TAG_OUTPUT_INHIBIT ), tempString );
        state.tagState.tagInhibitState.outputInhibit = stringToTagOutputInhibitState( tempString );
        // Tag Input Inhibit
        tempString = "";
        persistedState->getValue( mapEPersistedItem2DataRecordingId( DN_TAG_INPUT_INHIBIT ), tempString );
        state.tagState.tagInhibitState.inputInhibit = stringToTagInputInhibitState( tempString );
        // PTW Id
        persistedState->getValue( mapEPersistedItem2DataRecordingId( DN_TAG_PTW ), state.tagState.ptwId );
        // PTW Comment
        persistedState->getValue( mapEPersistedItem2DataRecordingId( DN_PTW_COMMENTS ), state.tagState.comment );
        // Last Operator Session ID
        persistedState->getValue( mapEPersistedItem2DataRecordingId( DN_LAST_OPERATOR_SESSION_ID ), state.lastOperator.sessionID );
        // Last Operator Timestamp
        persistedState->getValue( mapEPersistedItem2DataRecordingId( DN_LAST_OPERATOR_TIMESTAMP ), state.lastOperator.timeStamp.time );
        // MMS Inhibit
        persistedState->getValue( mapEPersistedItem2DataRecordingId( DN_MMS_INHIBIT_STATE ), state.inhibitMMSState );
        // Notes
        persistedState->getValue( mapEPersistedItem2DataRecordingId( DN_NOTES ), state.notes ); 
    }

    void ScadaPersistence::resetPersistedState( unsigned long entityKey )
    {
        //TA_THREADGUARD( m_lock );
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_rwMutex);
        StateMap::iterator stateIter = m_states.find( entityKey );
        if ( stateIter != m_states.end() )
        {
            stateIter->second->invalidate();
        }
    }


    void ScadaPersistence::removePersistedState( unsigned long entityKey )
    {
        //TA_THREADGUARD( m_lock );
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_rwMutex);
		m_states.erase( entityKey );
    }


    void ScadaPersistence::processOnlineUpdate( TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        FUNCTION_ENTRY( "processOnlineUpdate" );
        
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_rwMutex);
        StateMap::iterator stateIter = m_states.find( updateEvent.getKey() );
        if ( stateIter != m_states.end() )
        {
            //TA_THREADGUARD( m_lock );
            stateIter->second->invalidate();
        }

        FUNCTION_EXIT;
    }        
    
}
