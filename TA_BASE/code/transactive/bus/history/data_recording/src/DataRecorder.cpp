/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/DataRecorder.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Manages Data Recording for one specific type. Maintains
 * a set of DataCaches and uses an IDataWriter implementation
 * to persist the data as necessary.
 *
 */

#ifdef __WIN32__
#pragma warning (disable : 4786)
#endif

#include <sstream>
#include <memory>

#include "bus/history/data_recording/src/DataRecorder.h"
#include "bus/history/data_recording/src/Datum.h"
#include "bus/history/data_recording/src/DataRecordingException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/LivePropertyAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/LivePropertyData.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    DataRecorder::DataRecorder( IDataRecorderDbAccessPtr dataRecorderDbAccess, 
                                unsigned long typeKey, 
                                ERecordingType recordingType,
                                IDataWritePolicyPtr writePolicy )
    : m_terminated( false ), m_recording( false ), m_type( typeKey ), m_writer( (IDataWriter*)NULL ), 
      m_minimumQuality( 0 ), m_dbAccessor( dataRecorderDbAccess ), m_recordingType( recordingType ),
      m_writePolicy( writePolicy ), m_sampleRateMilliseconds( 0 )
    {
        //
        // Establish the name of this type 
        //
        std::map<unsigned long, std::string> entityTypes = TA_Base_Core::EntityAccessFactory::getInstance().getEntityTypes();
        std::map<unsigned long, std::string>::iterator typeIter = entityTypes.find( m_type );
        if ( typeIter == entityTypes.end() )
        {
            std::stringstream errorMessage;
            errorMessage << "Entity type key " << m_type << " not found";
            TA_ASSERT( false, errorMessage.str().c_str() );
        }
        m_typeName.assign( typeIter->second );

        //
        // Load LiveProperties for this type
        //
        m_config = m_dbAccessor->getRecordableDataByType( m_type );
        m_dataCache = DataCachePtr( new DataCache( m_typeName ) );

        std::stringstream logMessage;
        if ( ! m_config.empty() )
        {
            std::vector<TA_Base_Core::IRecordableDataPtr>::iterator dataIter = m_config.begin();
            for( ; dataIter != m_config.end(); ++dataIter )
            {
                TA_Base_Core::IRecordableDataPtr& dataItem = *dataIter;

                if ( dataItem->getStatus() == TA_Base_Core::IRecordableData::STATUS_AVAILABLE )
                {
                    // This should be the same value for each data recording property
                    if ( m_sampleRateMilliseconds > 0 && dataItem->getIntervalBetweenWrites() != m_sampleRateMilliseconds )
                    {
                        std::stringstream warningMessage;
                        warningMessage << "Write Interval for " << dataItem->getName() << "(" << dataItem->getIdentifier() 
                                       << ") has a different write interval (" << dataItem->getIntervalBetweenWrites()
                                       << ") to other Live Properties for type " << m_typeName;

                        LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, warningMessage.str().c_str() );
                    }
                    else
                    {
                        m_sampleRateMilliseconds = dataItem->getIntervalBetweenWrites();
                    }
                    m_dataCache->addField( dataItem );
                }
            }

            logMessage << "DataRecorder for type " << m_type << " created. Recording " << m_config.size() << " properties.";
            if ( m_recordingType == ASYNCHRONOUS )
            {
                logMessage << "Writing every " << m_sampleRateMilliseconds << " ms.";
            }
        }
        else
        {
            logMessage << "DataRecorder for type " << m_type << " created. No Data Recording objects found.";
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, logMessage.str().c_str() );
    }        


    DataRecorder::~DataRecorder()
    {
        if ( m_recording )
        {
            stopRecording();
        }
        if ( getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
        {
            terminateAndWait();
        }
    }


    void DataRecorder::startRecording()
    {
        FUNCTION_ENTRY( "startRecording" );

        TA_Base_Core::ThreadGuard guard( m_recorderLock );

        if ( ! m_recording )
        {
            std::vector<IDataSourcePtr>::iterator sourceIter = m_dataSources.begin();
            for( ; sourceIter != m_dataSources.end(); ++sourceIter )
            {
                (*sourceIter)->startRecording();
            }
            m_recording = true;
            m_condition.signal();
        }

        FUNCTION_EXIT;
    }


    void DataRecorder::stopRecording()
    {
        FUNCTION_ENTRY( "stopRecording" );

        TA_Base_Core::ThreadGuard guard( m_recorderLock );

        if ( m_recording )
        {
            std::vector<IDataSourcePtr>::iterator sourceIter = m_dataSources.begin();
            for( ; sourceIter != m_dataSources.end(); ++sourceIter )
            {
                (*sourceIter)->stopRecording();
            }
            m_recording = false;
            m_condition.signal();
        }

        FUNCTION_EXIT;
    }


    void DataRecorder::addDataSource( IDataSourcePtr source )
    {
        FUNCTION_ENTRY( "addDataSource" );

        TA_Base_Core::ThreadGuard guard( m_recorderLock );

        source->setDataSink( *this );
        m_dataSources.push_back( source );

        if ( m_recording )
        {
            source->startRecording();
        }

        FUNCTION_EXIT;
    }
        

    void DataRecorder::setDataWriter( IDataWriterPtr writer )
    {
        FUNCTION_ENTRY( "setDataWriter" );

        TA_Base_Core::ThreadGuard guard( m_recorderLock );
        m_writer = writer;

        FUNCTION_EXIT;
    }


    void DataRecorder::setMinimumAcceptableQuality( int quality )
    {
        FUNCTION_ENTRY( "setMinimumAcceptableQuality" );

        m_minimumQuality = quality;

        FUNCTION_EXIT;
    }


    void DataRecorder::notifyData( unsigned long id, time_t timestamp, const std::vector<Datum>& data, int quality )
    {
        FUNCTION_ENTRY( "notifyData" );
        TA_Base_Core::ThreadGuard guard( m_recorderLock );

        if ( m_recording )
        {
            std::stringstream logMessage;
            logMessage << "Received update for id " << id << ". Time=" << timestamp << ". Quality=" << quality;
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, logMessage.str().c_str() );

            if ( quality >= m_minimumQuality )
            {
                m_dataCache->updateCache( id, timestamp, data, quality );
                
                //
                // If recording in synchronous mode then data is written 
                // as it is received, and cleared regardless of whether the
                // write succeeded or not
                //
                if ( m_recordingType == SYNCHRONOUS )
                {
                    writeCache( m_dataCache );
                    m_dataCache->clearCache(); 
                }
            }
        }
        FUNCTION_EXIT;
    }
            
    
    void DataRecorder::run()
    {
        FUNCTION_ENTRY( "run" );

        while( ! m_terminated )
        {
            if ( m_recording && m_recordingType == ASYNCHRONOUS )
            {
                m_condition.timedWait( m_sampleRateMilliseconds );

                TA_Base_Core::ThreadGuard guard( m_recorderLock );
                if ( m_recording )
                {
                    writeCache( m_dataCache );
                }
            }
            else
            {
                m_condition.wait();
            }
        }

        FUNCTION_EXIT;
    }


    void DataRecorder::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

        m_terminated = true;
        m_recording = false;
        m_condition.signal();

        FUNCTION_EXIT;
    }


    void DataRecorder::writeCache( DataCachePtr cache )
    {
        FUNCTION_ENTRY( "writeCache" );

        if ( m_writer.get() != NULL )
        {
            try
            {
                m_writer->write( cache );

                if ( m_writePolicy.get() != NULL )
                {
                    m_writePolicy->notifyDataWriteSuccess();
                }
            }
            catch( TA_Base_Core::DataRecordingException& )
            {
                if ( m_writePolicy.get() != NULL )
                {
                    m_writePolicy->notifyDataWriteError( cache );
                }
            }
        }
        FUNCTION_EXIT;
    }
}

