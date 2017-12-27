/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/OutputCommandSender.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__OUTPUT_COMMAND_SENDER_H_
#define SNMP__OUTPUT_COMMAND_SENDER_H_

#include <map>
#include "IProtocolIOBase.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/common_library/src/EventProcessor.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"


namespace TA_Base_Bus
{
    class DataPoint;
}

namespace TA_Base_Bus
{
	/** sending DataPointWriteRequest and update DataPoint control state.
	 * Implementing processEvent interface.
	 *
	 * __NOTE__: this class using a singleton instance, so you'd better just construct one instance of this class.
     */
    class OutputCommandSender : public EventProcessor< TA_Base_Bus::DataPointWriteRequest >
	{

	public:		
		OutputCommandSender();
		virtual ~OutputCommandSender();

		void clearWriteQueue();

		/** set the pairs of DataPoint key and IProtocolIOBase to the this object.
         *
         * @param senderMap: DataPoint key to IProtocolIOBase map.
		 * @return void:
         */
		void setDataPointSenderMap(const std::map<unsigned long, IProtocolIOBase*>& senderMap);

	private:

		/** process DataPointWriteRequest
         *
         * @param newDataPointWriteRequest: write request.
		 * @return void:
         */
        virtual void processEvent( TA_Base_Bus::DataPointWriteRequest * newDataPointWriteRequest );

    private:
        bool m_inService;
		TA_Base_Bus::DataPointWriteQueue * m_writeQ;

		std::map<unsigned long, IProtocolIOBase*> m_dpToSenderMap;
	};

}

#endif  // RTUOUTPUTWORKER_H
