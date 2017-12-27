/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/PingableDataPointDriver.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class extends AbstractThreadedDriver.
  * It sets the value of DI datapoints it owns by periodically pinging the address defined in the address
  * field of the datapoint configuration
  *
  * This class is also a thread which goes about pinging stuff
  *
  * Normally it is a drivers responcibilty to raise alarms resulting from datapoint 
  * bad quality, while the responcibility to raise alarms due to datapoint values is placed
  * on the datapoint library which utilises the alarm configuration for the datapoint.
  * Note however that this is not always the case: 
  * The PingableDataPointDriver as example, never raises bad quality
  * alarms, instead if it can not ping a point, for what ever reason, it simply sets that 
  * point's value to 0 via the Datapoint interface. The datapoint library will then raise
  * an alarm, assuming that the point has been configured so that a false value = alarm 
  *
  * Each instance of PingablePoint has a INetworkFailureAlarmPolicy* member
  * The class is designed so that, at runtime, a specific implimentation
  * of INetworkFailureAlarmPolicy can be specified (see CTOR) for each PingablePoint.
  * The current implentation however defaults all points to use ConsecutiveNetworkFailureAlarmPolicy.
  * Also, PingablePoint allows the number of retries to be set for each PingablePoint. The current 
  * implimentation of PingableDataPointDriver aplies the same value to all points however, the value
  * applied is a config param (PING_RETRIES_PARAM). This class and PingablePoint support
  * on line updates of this param
  */

#if !defined(AFX_PINGABLEDATAPOINTDRIVER_H__32191299_CB47_41C3_9E01_9A6FB86E878E__INCLUDED_)
#define AFX_PINGABLEDATAPOINTDRIVER_H__32191299_CB47_41C3_9E01_9A6FB86E878E__INCLUDED_



#include "app/system_control/system_status_agent/src/threadedDriver/AbstractThreadedDriver.h"
#include "core/sockets/src/RawDatagramSocket.h"

namespace TA_Base_App
{
	class PingableDataPointDriver : public AbstractThreadedDriver
	{
	public:

	   /**
		* PingableDataPointDriver()
		* 
		*/
		PingableDataPointDriver(SystemStatusAgent& agentUser);

	   /**
		* ~PingableDataPointDriver()
		* 
		*/
		virtual ~PingableDataPointDriver();

	//The following methods impliment the AbstractThreadedDriver abstract class:
	protected: //only the base class uses the following methods, hence they are protected

	   /**
		* convertToIPointContainer
		*
		* If the driver is willing to keep this point updated it should return
		* a IPointContainer* which contains this point. If the driver does not
		* want this point return 0
		* The IPointContainer* should be created on the heap. You do NOT need to
		* delete this point on destruction
		*/
		virtual IPointContainer* convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint);

		/**
		 * onInit
		 * 
		 * open a socket - throw if it can't
		 * get the current config params   
		 * 
		 * This is called for the first and only time when the mode changes
		 * from INIT to something else. It is not called in the CTOR
		 * because the gen_agent object does not exist when this object
		 * is created.
		 * @param pEntityData contains the updated configuration data
		 *
		 * @throw SocketFailedException if can't open socket on init
		 *
		 */
		virtual void onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData);

        /**
          * pollPoints
          *
          * This will ping each point that this driver owns, up to PING_RETRIES_PER_CYCLE
		  * times. The success or failure of the ping is reported to the point's
		  * alarm policy. The alarms are then processed outside of this call - see doPostPollUpdate()
          */
		virtual void pollPoints();

        /**
          * doPostPollUpdate
          *
		  *
		  *
          * This method is called following each call to pollPoints. Hence, you can either 
		  * update your datapoints pollPoints or retoactivly update them here.
          * This mechanism is provided because some driver implimentations may
          * not wish to serialy scan and update datapoints.
		  *
		  * the value of all points are updated based on what the alarm policy state 
		  * is for each point. Note that we do not raise alarms directly. Instead we just
		  * set the datapoint low and the alarm configuration for the point 
		  * will cause an alarm to be raised via the datapoint library.
		  * Note that for this driver we always set the quality of the point to good
		  * once we start pinging
          */
		virtual void doPostPollUpdate();

		/**
		 * 
		 * onlineConfigUpdate
		 *
		 * This method is called when processOnlineAgentUpdate is called. 
		 * You should process any parameters specific to your driver.
		 *
		 * This method will be called when the agent configuration is changed
		 * online. Note that online datapoint configuration updates are
		 * not passed to driver.
		 *
		 * @param changedParams contains the names of the params that have changed
		 */
		virtual void onlineConfigUpdate(std::vector<std::string> changedParams);

		/**
		 * 
		 * getPollingRateParamName
		 *
		 * @return std::string: This method must return the name of the the configuration 
		 * parameter used to specify the rate at which your points will be polled, i.e. the
		 * rate at which pollPoints will be called
		 *
		 *
		 * @param changedParams contains the names of the params that have changed
		 */
		virtual std::string getPollingRateParamName();

	private:
		//data members:

		//This value dictates how many times a point can be reported as bad
		//by pollPoints() before it is considered un-pingable
		// - see INetworkFailureAlarmPolicy class
		unsigned int m_pingRetries;

		// ICMP datagrams are "unreliable"...
		//This value dictates how many times during each ping cycle, i.e.
		//one call to pollPoints(), each point is, at a maximum, pinged.
		//It is impotant to note that the System Status Agent parameter:
		//SystemStatusAgentEntityData::PING_RETRIES_PARAM is NOT THIS PARAM;
		//see comments on m_pingRetries above
		static const unsigned int PING_RETRIES_PER_CYCLE;

		//This is how long we will wait for a responce from a ping
		//This value is set via a run param
		//Note that the polling period is not controlled by this class. It
		//is controlled in the AbstractThreadedDriver class
		unsigned int m_pingTimeoutSeconds;

		//This datamember is used to identify received datagrams as ones 
		//that we sent
		unsigned short m_pid;

		//The socket that we use to send pings on
		TA_Base_Core::RawDatagramSocket m_socket;

	private:
		//private methods:

		/**
		 * setRetries
		 *
		 * apply the specified retry setting
		 */
		void setRetries(unsigned int newRetrySetting);

		/**
		 * setTimeOut
		 *
		 * apply the specified timeout period
		 */
		void setTimeOut(unsigned int newTimeOut);

		//disable the copy CTOR and the assignment operator
		PingableDataPointDriver(const PingableDataPointDriver& obj);
		PingableDataPointDriver& operator=(const PingableDataPointDriver& obj);

	};

} // end namespace TA_Base_App
#endif // !defined(AFX_PINGABLEDATAPOINTDRIVER_H__32191299_CB47_41C3_9E01_9A6FB86E878E__INCLUDED_)
