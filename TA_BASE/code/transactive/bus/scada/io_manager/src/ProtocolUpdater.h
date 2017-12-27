/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolUpdater.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_PROTOCOL_UPDATER_H_
#define SCADA_PROTOCOL_UPDATER_H_

#include "ProtocolIOBase.h"

namespace TA_Base_Bus
{
	/** this class update IDeviceData submitted by ProtocolDevice to DataPoint.
	 * derived from ProtocolIOBase.
     *  
     */
	class ProtocolUpdater : public ProtocolIOBase
	{
	public:

		/** constructor.
         *
         * @param name: object name.
         */
		ProtocolUpdater(std::string name);
		virtual ~ProtocolUpdater();

		virtual bool isDataPointMatched(DataPoint* dp) = 0;
		virtual void finishedAddDataPoint(){};

		virtual void setOperationMode(TA_Base_Core::EOperationMode mode){};

		virtual void submitDeviceData(IDeviceData*) = 0;

		/** update DataPoint field value asynchronously. 
		 * this function will insert the request to IDataPointProcessor.
         *
         * @param dp: DataPoint pointer.
         * @return dpState: DataPointState, the new value.
         */
		void updateDPFieldValue(DataPoint* dp, DataPointState& dpState);

	protected:
	};
	
};

#endif
