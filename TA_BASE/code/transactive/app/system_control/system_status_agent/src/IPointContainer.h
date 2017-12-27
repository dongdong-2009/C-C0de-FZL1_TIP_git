/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/IPointContainer.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This simple interface just provides a box to put your datapoints in.
  * The AbstractThreadedDriver needs a way to access the points that drivers that 
  * impliment AbstractThreadedDriver own. This container allows attributes associated
  * with points to be stored by composition rather than deriving from DataPoint
  *
  */

#if !defined(AFX_IPOINTCONTAINER_H__A4353AF8_BD7D_4878_884B_A89C7C0BDF52__INCLUDED_)
#define AFX_IPOINTCONTAINER_H__A4353AF8_BD7D_4878_884B_A89C7C0BDF52__INCLUDED_

#include "bus/scada/datapoint_library/src/DataPoint.h"

namespace TA_Base_App
{
	class IPointContainer  
	{
	public:

		/*
		* getAddressIdentifier
		*
		* @return the raw IP address or hostname of the point.
		* Note that the address configured for the datapoint will probably contain more than
		* the above, e.g. mftStatus:192.168.70.60. This metod should just return 192.168.70.60 bit
		*/
		virtual std::string getAddressIdentifier() = 0;

		virtual TA_Base_Bus::DataPoint* const getDataPoint() = 0;
		virtual ~IPointContainer(){};

	protected:
		IPointContainer(){};


	};
} // end namespace TA_Base_App

#endif // !defined(AFX_IPOINTCONTAINER_H__A4353AF8_BD7D_4878_884B_A89C7C0BDF52__INCLUDED_)
