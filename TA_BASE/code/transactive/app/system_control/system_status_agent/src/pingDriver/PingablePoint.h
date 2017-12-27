/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/PingablePoint.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class provides a datapoint with a bunch of parameters attached to facilitate
  * pinging.
  *
  * Each instance of this class has a INetworkFailureAlarmPolicy* member
  * The class is designed so that, at runtime, a specific implimentation
  * of INetworkFailureAlarmPolicy can be specified (see CTOR).
  * The current implentation however defaults to a ConsecutiveNetworkFailureAlarmPolicy.
  *
  */

#if !defined(AFX_PINGABLEPOINT_H__79A8E911_C6D4_46E2_A818_60CB94DF1BC9__INCLUDED_)
#define AFX_PINGABLEPOINT_H__79A8E911_C6D4_46E2_A818_60CB94DF1BC9__INCLUDED_

#include "core/sockets/src/InetAddress.h"
#include "app/system_control/system_status_agent/src/IPointContainer.h"

namespace TA_Base_Bus
{
	class BooleanDataPoint;
}

namespace TA_Base_App
{
	class INetworkFailureAlarmPolicy;

	class PingablePoint : public IPointContainer
	{
	public:

		enum FailurePolicyEnum{PERCENT, CONSECUTIVE};

        /** 
          * PingablePoint
          *
          * @param BooleanDataPoint* const datapoint is the point
		  * to update. Precondition the point is a registered CORBA object  
		  *
		  * @param const std::string ipAddress is a IP address of
		  * the form xxx.xxx.xxx.xxx where xxx is 0 to 255
		  *
		  * FailurePolicyEnum policy: specifies the type of failure policy to
		  * use when determining if this object is 'non-pinable'. Currently this 
		  * param must be set to CONSECUTIVE
		  *
		  * Note that currently only the CONSECUTIVE policy is avaiable 
		  * someday having the percentage policy avaiable would be good.
          */
		PingablePoint( 
			TA_Base_Bus::BooleanDataPoint* const datapoint,
			const std::string ipAddress,
			FailurePolicyEnum policyType = CONSECUTIVE
		);

		virtual ~PingablePoint();

		/*
		* getAddressIdentifier
		*
		* @return the raw IP address or hostname of the point.
		* Note that the address configured for the datapoint will probably contain more than
		* the above, e.g. mftStatus:192.168.70.60. This metod should just return 192.168.70.60 bit
		*/
		virtual std::string getAddressIdentifier();

		virtual TA_Base_Bus::DataPoint* const getDataPoint();

        /** 
          * getInternetAddressRef
          *
          * @return a modifiable reference to this points internet address
          */
		TA_Base_Core::InetAddress& getInternetAddressRef();

		INetworkFailureAlarmPolicy* const getAlarmPolicy();

		const bool hasBeenProcessed() const;

		void setProcessed(bool state);

        /** 
          * setMaxConsecutiveFails
          *
          * apply a new number of allowable consecutive ping failures
          *
          * @param unsigned int consecutiveFails: number of allowable
		  * consecutive ping failures
          */
		void setMaxConsecutiveFails(unsigned int consecutiveFails);

	private:
		TA_Base_Bus::BooleanDataPoint* const m_datapoint;
		TA_Base_Core::InetAddress m_ipAddress;
		INetworkFailureAlarmPolicy* m_alarmPolicy;

		//This member is used in the PingableDataPointDriver::pingNetwork() call
		//to mark this point as needing no further processing, for that call 
		//to pollPoints()
		bool m_hasBeenProcessed;
	};
} //end namespace TA_Base_App

#endif // !defined(AFX_PINGABLEPOINT_H__79A8E911_C6D4_46E2_A818_60CB94DF1BC9__INCLUDED_)
