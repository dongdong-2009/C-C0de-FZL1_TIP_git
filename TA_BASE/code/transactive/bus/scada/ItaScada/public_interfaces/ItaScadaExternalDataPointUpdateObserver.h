/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/ItaScada/public_interfaces/ItaScadaExternalDataPointUpdateObserver.h $
  * @author  Sean Liew
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  * Implementation of the Interface ItaScadaExternalDataPointUpdateObserver
  *
  */

#if !defined(ItaScadaExternalDataPointUpdateObserver_6F7DA565_77E9_418c_8842_1125F6E1BBD2__INCLUDED_)
#define ItaScadaExternalDataPointUpdateObserver_6F7DA565_77E9_418c_8842_1125F6E1BBD2__INCLUDED_

/**
 * Interface to allow the observation of datapoint value and quality updates for
 * an externally hosted datapoint.
 * @version 1.0
 * @created 04-Mar-2005 10:31:29 AM
 */


#include "core/types/public_types/Constants.h"

namespace TA_Base_Bus
{
	class DpValue;

	class ItaScadaExternalDataPointUpdateObserver
	{

	public:
		/**
		 * Method to receive the datapoint updates
		 * @param name    Name of updated external datapoint
		 * @param value    The updated datapoint value.
		 * @param quality    The updated datapoint quality
		 * 
		 */
		virtual void onExternalDataPointUpdate(const std::string& name, const TA_Base_Bus::DpValue& dpValue, TA_Base_Core::EDataPointQuality quality) =0;

	};
}

#endif // !defined(ItaScadaExternalDataPointUpdateObserver_6F7DA565_77E9_418c_8842_1125F6E1BBD2__INCLUDED_)