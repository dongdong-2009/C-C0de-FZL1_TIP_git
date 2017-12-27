/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/IDataPointObserverNotification.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	Description
  *
  */

#ifndef IDATAPOINT_OBSERVER_NOTIFICATION_H
#define IDATAPOINT_OBSERVER_NOTIFICATION_H


#include "bus/scada/common_library/src/CommonDefs.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class IDataPointObserverNotification
	{
	public:

		/**
		 * processDataPointUpdate
		 *
		 * Call back for processing of the new data point update details
		 *
		 * @param 	pkey			The pkey of the data point
		 * @param 	updateDetails	The new updates of the data point specified by the pkey
		 *
		 */
		virtual void processDataPointUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails ) = 0;

	};
}

#endif

