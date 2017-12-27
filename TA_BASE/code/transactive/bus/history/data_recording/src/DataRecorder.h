#ifndef DATA_RECORDER_H
#define DATA_RECORDER_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/DataRecorder.h $
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

#include <vector>
#include <map>
#include <string>

#include "bus/history/data_recording/src/IDataSink.h"
#include "bus/history/data_recording/src/DataCache.h"
#include "bus/history/data_recording/src/IDataSource.h"
#include "bus/history/data_recording/src/IDataWriter.h"
#include "bus/history/data_recording/src/IDataWritePolicy.h"
#include "bus/history/data_recording/src/IDataRecorderDbAccess.h"
#include "core/data_access_interface/data_recording_dai/src/IRecordableData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"
#include "core/threads/src/Thread.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
    class Datum;

    class DataRecorder : public IDataSink, public TA_Base_Core::Thread
    {

    public:

        enum ERecordingType
        {
            SYNCHRONOUS,        // Data will be written as it is received
            ASYNCHRONOUS        // Data will be cached and written periodically
        };
            

        /**
         * Constructor
         *
         * @param   dataRecorderDbAccess    access to the DataRecorder configuration
         * @param   typeKey                 the LiveProperty type for this Recorder 
         * @param   recordingType           defines how data will be written
         * @param   writePolicy             specialised data writing behaviour
         *
         */
        DataRecorder( IDataRecorderDbAccessPtr dataRecorderDbAccess, 
                      unsigned long typeKey, 
                      ERecordingType recordingType,
                      IDataWritePolicyPtr writePolicy );


        /**
         * Destructor
         *
         */
        virtual ~DataRecorder();


        /**
         * startRecording
         *
         * Enables the DataRecorder's writing of cached data. Also
         * calls startRecording on each IDataSource stored by the 
         * Recorder.
         *
         */
        void startRecording();


        /**
         * stopRecording
         *
         * Disables the DataRecorder's writing of cached data. Also
         * calls stopRecording on each IDataSource stored by the 
         * Recorder.
         *
         */
        void stopRecording();
    

        /**
         * addDataSource
         *
         * Adds a new IDataSource implementation to the Recorder. The
         * DataRecorder will add all Recordable Items to the IDataSource,
         * call IDataSource::setDataSink and IDataSource::startRecording
         * (if recording is currently enabled)
         *
         */
        void addDataSource( IDataSourcePtr source );


        /**
         * setDataWriter
         *
         * Assigns the IDataWriter implementation for the DataRecorder
         * to use for persisting cached data.
         *
         * @param   writer  the IDataWriter to use
         * 
         */
        void setDataWriter( IDataWriterPtr writer );



        /***********************************************************************
         *
         * IDataSink functions
         *
         ***********************************************************************/


        /**
         * notifyData
         *
         * Callback for IDataSource implementations to update the data being
         * recorded for one Item.
         *
         * @param   id          unique identifier for Item
         * @param   timestamp   source time for data
         * @param   data        the data 
         * @param   quality     confidence level in integrity of data
         *
         */
        virtual void notifyData( unsigned long id, time_t timestamp, const std::vector<Datum>& data, int quality );


        /**
         * setMinimumAcceptableQuality
         *
         * @param   quality     the minimum quality for data to be
         *                      stored by the Sink. Data of lesser quality
         *                      will be ignored.
         *
         */
        virtual void setMinimumAcceptableQuality( int quality );

        
        /***********************************************************************
         *
         * Thread functions
         *
         ***********************************************************************/


        /**
         * run
         *
         * If data recording is enabled and the recording type is ASYNCHRONOUS
         * the DataRecorder periodically persists it's DataCache's via
         * the IDataWriter.
         * 
         */
        virtual void run();


        /**
         * terminate
         *
         * Signals the run() loop to complete
         *
         */
        virtual void terminate();


    private:


        /** 
         * Prevent copying
         *
         */
        DataRecorder( const DataRecorder& copyMe );
        DataRecorder& operator=( const DataRecorder& copyMe );


        /**
         * writeCache
         *
         * Passes the DataCache to IDataWriter::write(). Catches
         * DataRecordingInternalException's and raises alarms for
         * write errors.
         *
         */
        void writeCache( DataCachePtr cache );


        bool volatile                                   m_terminated;
        bool volatile                                   m_recording;
        unsigned long                                   m_type;
        std::string                                     m_typeName;
        IDataWriterPtr                                  m_writer;
        std::vector<IDataSourcePtr>                     m_dataSources;
        DataCachePtr                                    m_dataCache;
        std::vector<TA_Base_Core::IRecordableDataPtr>   m_config;
        TA_Base_Core::NonReEntrantThreadLockable        m_recorderLock;
        TA_Base_Core::Condition                         m_condition;
        int                                             m_minimumQuality;
        IDataRecorderDbAccessPtr                        m_dbAccessor;
        ERecordingType                                  m_recordingType;
        IDataWritePolicyPtr                             m_writePolicy;
        unsigned long                                   m_sampleRateMilliseconds;
    };

    typedef boost::shared_ptr<DataRecorder> DataRecorderPtr;

}

#endif // DATA_RECORDER_H
