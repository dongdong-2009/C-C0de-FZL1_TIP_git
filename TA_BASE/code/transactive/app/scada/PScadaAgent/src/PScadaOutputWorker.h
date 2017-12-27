/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source: $File: //depot/3002_TIP/3002/transactive/app/scada/PScadaAgent/src/PScadaOutputWorker.h $
  * @author  Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  * The PScadaOutputWorker thread is responsible
  * for sending DataPointWriteRequest to the 104 Library
  *
  */

#ifndef PSCADAOUTPUTWORKER_H
#define PSCADAOUTPUTWORKER_H

#include <vector>

#include "bus/scada/common_library/src/EventProcessor.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"

namespace TA_Base_Core
{
    class SocketOptions;
}

namespace TA_Base_Bus
{
    class DataPoint;
}

namespace TA_IRS_App
{
    // Forward declaration
	class PScadaUnit;

    class PScadaOutputWorker : public EventProcessor< TA_Base_Bus::DataPointWriteRequest >
	{

	public:
		PScadaOutputWorker();
		virtual ~PScadaOutputWorker();

        void addPScadaUnit(PScadaUnit& pscadaUnit);
		
		void clearWriteQueue();

	private:

		/**
		 * processEvent()
		 *
		 * Generates an Event based on the request, and logs it
		 *
		 * @param newEvent	the request for the new event
		 *
		 */
        virtual void processEvent( TA_Base_Bus::DataPointWriteRequest * newDataPointWriteRequest );

    private:
        bool m_inService;
		std::vector<PScadaUnit*> m_pscadaUnits;
		TA_Base_Bus::DataPointWriteQueue * m_writeQ;
	};

}

#endif  // PSCADAOUTPUTWORKER_H
