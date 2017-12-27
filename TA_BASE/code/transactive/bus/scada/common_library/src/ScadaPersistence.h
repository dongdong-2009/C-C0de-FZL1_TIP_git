#ifndef SCADA_PERSISTENCE_H
#define SCADA_PERSISTENCE_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/ScadaPersistence.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include <memory>
#include <map>
#include <sstream>
#include <vector>
#include <string>

#include "bus/history/data_recording/src/DataRecorder.h"
#include "bus/history/data_recording/src/Datum.h"
#include "bus/history/data_recording/src/DbDataWriter.h"
#include "bus/history/data_recording/src/IDataRecorderDbAccess.h"
#include "bus/history/data_recording/src/IDataWritePolicy.h"
#include "bus/history/data_recording/src/DataRecordingException.h"
#include "bus/scada/common_library/src/EPersistedItem.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/data_recording_dai/src/PersistedState.h"
#include "core/data_access_interface/data_recording_dai/src/PersistedStateAccessFactory.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include "ace/RW_Mutex.h"
#include "ace/Guard_T.h"

namespace TA_Base_Bus
{
    class ScadaPersistence
    {

    public:

        enum EScadaPersistenceType
        {
            DATAPOINT,
            DATANODE
        };

        //
        // Use these to create a scada persistence instance
        // Factory type functions were used, because obviously
        // you can't override constructor names
        // and agent key and location key have the same type
        // so overloading isnt possible
        //

        /** 
          * createScadaPersistence
          *
          * Creates a new ScadaPersistence.
          * Preloads nothing. All states are retrieved on demand.
          *
          * @param type the type of persistence, datanode or datapoint.
          */
        static ScadaPersistence* createScadaPersistence( EScadaPersistenceType type );


        /** 
          * createScadaPersistenceForAgent
          *
          * Creates a new ScadaPersistence.
          * Preloads all states of the given type for the given agent.
          *
          * @param type the type of persistence, datanode or datapoint.
          * @param agentKey the agent to load for. Will assert on 0.
          */
        static ScadaPersistence* createScadaPersistenceForAgent( EScadaPersistenceType type, unsigned long agentKey );


        /** 
          * createScadaPersistenceForLocation
          *
          * Creates a new ScadaPersistence.
          * Preloads all states of the given type for the given location.
          *
          * @param type the type of persistence, datanode or datapoint.
          * @param locationKey the location to load for. Will assert on 0.
          */
        static ScadaPersistence* createScadaPersistenceForLocation( EScadaPersistenceType type, unsigned long locationKey );


        /**
         * Destructor
         *
         */
        ~ScadaPersistence();


        /**
         * readPersistedValue
         *
         * @param item          the persisted item to be read
         * @param entityKey     the entity to retrieve the value for
         * @param value         the persisted value will be copied to this variable
         *
         * Implementation must be included here rather than in the .cpp due to VC6's
         * crap support of templates.
         *
         */
        template<typename T> 
        void readPersistedValue( EPersistedItem item, unsigned long entityKey, T& value )
        {
            try
            {
                std::string dataRecordingId = mapEPersistedItem2DataRecordingId( item );

                //TA_THREADGUARD( m_lock );
				ACE_Read_Guard<ACE_RW_Mutex> guard(m_rwMutex);
                StateMap::iterator stateIter = m_states.find( entityKey );
                if ( stateIter == m_states.end() )
                {
                    TA_Base_Core::PersistedStateAccessFactory& persistedAccess = TA_Base_Core::PersistedStateAccessFactory::getInstance();
                    TA_Base_Core::PersistedStatePtr state = persistedAccess.getPersistedState( entityKey, m_typeKey );
                    state->getValue( dataRecordingId, value );
                }
                else
                {
                    stateIter->second->getValue( dataRecordingId, value );
                }
            }
            catch( const TA_Base_Core::DataException& )
            {
                // Ignore and go with default value
            }
        }


        /**
         * readDataNodeState
         *
         * @param entityKey     pkey of the DataNode to retrieve state for
         * @param state         this variable will be updated with the persisted state
         *
         */
        void readDataNodeState( unsigned long entityKey, TA_Base_Bus::DnCompleteDynamicSyncState& state );


        /**
         * writePersistedValue
         *
         * @param item          the persisted item to write
         * @param entityKey     the pkey of the entity that the value pertains to
         * @param value         the new value to persist
         * @param quality       the 'quality' of the new value [optional]
         *
         * Implementation must be included here rather than in the .cpp due to VC6's
         * crap support of templates.
         *
         */
        template<typename T>
        bool writePersistedValue( EPersistedItem item, unsigned long entityKey, const T& value )
        {
            //
            // VC6 didn't like setting a default parameter value for quality :-(
            // 
            return writePersistedValue( item, entityKey, value, QUALITY_GOOD_NO_SPECIFIC_REASON );
        }

        template<typename T>
        bool writePersistedValue( EPersistedItem item, unsigned long entityKey, const T& value, EQualityStatus quality )
        {
            std::string dataRecordingId = mapEPersistedItem2DataRecordingId( item );

            std::vector<Datum> data;
            data.push_back( Datum( dataRecordingId, value ) );
            try
            {
                //
                // DataRecorder::notify() is synchronised so no need to guard the
                // ThreadLockable here
                //
                m_dataRecorder->notifyData( entityKey, time( NULL ), data, quality );
            }
            catch( const TA_Base_Core::DataRecordingException& )
            {
                return false;
            }

            //
            // Update cached value if necessary
            //
            //TA_THREADGUARD( m_lock );
			ACE_Write_Guard<ACE_RW_Mutex> guard(m_rwMutex);
            StateMap::iterator stateIter = m_states.find( entityKey );
            if ( stateIter != m_states.end() )
            {
                stateIter->second->setValue( dataRecordingId, value );
            }
            return true;
        }


        /**
         * resetPersistedState
         *
         * If the persisted state for the specified entity has been cached, 
         * the cached state is invalidated.
         *
         * @param   entityKey   pkey of the entity, which state shall be reset
         *
         */
        void resetPersistedState( unsigned long entityKey );


        /**
         * removePersistedState
         *
         * If the persisted state for the specified entity has been cached, 
         * the cached state is removed.
         *
         * @param   entityKey   pkey of the entity, which state shall be removed
         *
         */
        void removePersistedState( unsigned long entityKey );


        /**
         * processOnlineUpdate
         *
         * Handles online configuration update events.
         * 
         * @param updateEvent   an online configuration update event
         *
         */
        void processOnlineUpdate( TA_Base_Core::ConfigUpdateDetails& updateEvent );        
    

    private:

        /** 
          * ScadaPersistence
          *
          * Constructor
          * Preloads nothing. All states are retrieved on demand.
          *
          * @param type the type of persistence, datanode or datapoint.
          */
        ScadaPersistence( EScadaPersistenceType type );


        /** 
          * ScadaPersistence
          *
          * Constructor
          * Preloads all states of the given type for the given agent.
          *
          * @param type the type of persistence, datanode or datapoint.
          * @param agentKey the agent to load for. Will assert on 0.
          */
        ScadaPersistence( EScadaPersistenceType type, unsigned long agentKey );

        /** 
          * ScadaPersistence
          *
          * Constructor
          * Preloads all states of the given type for the given location.
          *
          * @param type the type of persistence, datanode or datapoint.
          * @param dummy does nothing, used to overload the above constructor
          *              (agent key and location key have the same type)
          * @param locationKey the location to load for. Will assert on 0.
          */
        ScadaPersistence( EScadaPersistenceType type, bool dummy, unsigned long locationKey );

        /** 
          * initialise
          *
          * common initialisation for all constructors
          *
          * @param type the type of persistence, datanode or datapoint.
          */
        void initialise( EScadaPersistenceType type );

        typedef std::map<unsigned long, TA_Base_Core::PersistedStatePtr> StateMap;

        unsigned long                               m_typeKey;
        std::string                                 m_typeName;
        std::auto_ptr<DataRecorder>                 m_dataRecorder;
        IDataWriterPtr                              m_databaseWriter;
        IDataRecorderDbAccessPtr                    m_dataRecorderDbAccess;
        IDataWritePolicyPtr                         m_dataRecorderWritePolicy;        
        StateMap                                    m_states;         
        //TA_Base_Core::NonReEntrantThreadLockable    m_lock;
		ACE_RW_Mutex 								m_rwMutex;

    };

    typedef boost::shared_ptr<ScadaPersistence> ScadaPersistencePtr;
}
#endif // SCADA_PERSISTENCE_H

