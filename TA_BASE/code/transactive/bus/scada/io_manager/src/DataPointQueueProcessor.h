/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/DataPointQueueProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_DATAPOINT_QUEUE_PROCESSOR_H_
#define SCADA_DATAPOINT_QUEUE_PROCESSOR_H_

#include <map>
#include <vector>

#include "boost/shared_ptr.hpp"

#include "IDataPointProcessor.h"
#include "core/threads/src/QueueProcessorPool.h"


namespace TA_Base_Bus
{

	/** define DataPoint pointer and DataPointState pair.
     *  
     */
	class DataPointAndValuePair
	{
	public:
		DataPointAndValuePair(DataPoint* dp, DataPointState& state)
			:
			dataPoint(dp),
			dpState(state)
		{}

		DataPoint* dataPoint;
		DataPointState dpState;
	};

	/** the queue processor will update datapoint field value.
     *  
     *  __NOTE__: the processor will process data asynchronously and for each datapoint sequentially.
     */
	class DataPointQueueProcessor :	public TA_Base_Core::QueueProcessorPoolCallback<DataPointAndValuePair>,
									public IDataPointProcessor
	{
	public:

		/** this function will update DataPoint with paired value.
         * this function implementation QueueProcessorPoolCallback call back interface.
         *
         * @param numberOfWorkers: the number define worker numbers.
         */
		DataPointQueueProcessor(int numberOfWorkers = 29);
		virtual ~DataPointQueueProcessor();

		void submitData(DataPoint*, DataPointState &);

		/** this function will update DataPoint with paired value.
         * this function implementation QueueProcessorPoolCallback call back interface.
         *
         * @param item: DataPointAndValuePair value.
         * @return void
         */
		void queueProcessorPoolCallback( boost::shared_ptr<DataPointAndValuePair> item );

	private:
		
		TA_Base_Core::QueueProcessorPool<DataPointAndValuePair, 
				TA_Base_Core::QueueProcessorWorker<DataPointAndValuePair> >*	m_updateProcessorPool;


	};

};

#endif
