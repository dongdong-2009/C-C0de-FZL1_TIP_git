/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/IDataPointProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_I_DATAPOINT_PROCESSOR_H_
#define SCADA_I_DATAPOINT_PROCESSOR_H_

#include <string>
#include "bus/scada/datapoint_library/src/DataPoint.h"

namespace TA_Base_Bus
{
	class DataPoint;
	class DataPointState;

	/** Interface for ProtocolUpdater submit datapoint updating data. 
	 * The derived processor will process those datas.
     *  
     */
	class IDataPointProcessor
	{
	public:
		virtual ~IDataPointProcessor(){};

		/**
         * submit the updating datas.
         *
         * @param dp: a pointer to DataPoint.
         * @param dpState: DataPointState reference.
         * @return void
         */
		virtual void submitData(DataPoint* dp, DataPointState& dpState) = 0;

	};

};

#endif
