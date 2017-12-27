/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/IDataPointDriver.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * The contract of a class that impliments this IDataPointDriver interface is basically
  * to update the value of the datapoints it owns. 
  * It must also deregerister the related CORBA object when the driver is terminated.
  *
  * It is my intent that a new IDataPointDriver implimentation will be
  * created when a new datapoint source is indentified as a requirement. As example, we
  * have a requirement to expose the "pingability" of a Workstation on the network as 
  * a datapoint. Hence, it is my intent to create a PingableDataPointDriver that impliments
  * IDataPointDriver.
  *
  * Note: When you create a new IDataPointDriver implentation you must add it to
  * the DataPointDriverHelper or the System Status Agent will not employ your driver 
  * - see comments in that class
  *
  * Note that the framework does not dictate that your driver run in its own thread. 
  * BUT YOUR METHODS MUST BE NON-BLOCKING - other than time to init.
  * Don't make me write code to enforce this!
  * I would think that most IDataPointDriver implimentations would start a thread but
  * this is not required. As example, you could register for messages as a way 
  * of getting info related to the datapoints you own.
  *
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
  */

#if !defined(AFX_IDATAPOINTDRIVER_H__7679A413_A17B_4FC5_804C_A4379853FFA9__INCLUDED_)
#define AFX_IDATAPOINTDRIVER_H__7679A413_A17B_4FC5_804C_A4379853FFA9__INCLUDED_

#include <string>
#include <vector>

namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_Base_App
{
	class IDataPointDriver
	{
	public:

		/**
		  * keepThisPointUpdated
		  *
		  * This method returns true if this driver agrees to keep this
		  * datapoint updated and false if it does not.
		  *
		  * @preconditions: Results not defined if:
		  * 1) the param dataPoint is NOT already registered as a CORBA object. 
		  * 2) this driver is NOT in INIT mode when this method is called
		  *
          * @postcondition if method returns true the datapoint will be kept
		  * updated by the driver while the driver is in control mode.
		  *
		  * Calling this method repeatedly on the same datapoint will invalidate all
		  * previous calls, i.e. only the return value of the last call is relavent.
          *
		  * @return true if the driver has taken ownership - false if this driver
		  * has no clue how to keep this point updated and refuses to have anything
		  * to do with it
		  * 
          * @param dataPoint is a DataPoint* to the point you wish this driver to
		  * keep updated. Note that dataPoint must already be registered as a
		  * CORBA object.
		  *
		  * Note that Agent will initialize the quality to TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED
		  * indicating that it has not been initialized. Hence, you don't have to
		  * worry about it untill setControl is called
		  *
		 */
		virtual bool keepThisPointUpdated(TA_Base_Bus::DataPoint* const dataPoint) = 0;


		/**
		 * setControl
		 *
		 * This method will be called when the agent is started in, or
		 * moves to control mode
		 *
		 * When this is called you should start updating all the points you 
		 * have agreed to update - see keepThisPointUpdated()
		 *
		 * This call should only block while it is initializing.
		 * It should return and then carry on the business of updating
		 * its datapoints as required
		 *
		 * Note that setting the mode of child entities is handled by the gen_agent framework and
		 * should not be done in this method.
		 *
		 * If going from INIT or MONITOR to CONTROL
		 *		start polling
		 *		set quality and values A/R
		 *		re-init alarms by sending alarm updates for all points reqarless of state
		 * If going from CONTROL to CONTROL 
		 *		do nothing 
		*/
		virtual void setControl() = 0;

		/**
		 * setMonitor
		 *
		 * This method will be called when the agent is started in, or
		 * moves to monitor mode
		 *
		 * When this is called you should stop updating all the points you 
		 * have agreed to update (see keepThisPointUpdated()) and set their
		 * quality to bad
		 *
		 * Note that setting the mode of child entities is handled by the gen_agent framework and
		 * should not be done in this method
		 *
		 * If going from INIT (see IDataPoint driver) to MONITOR
		 *		set quality to bad for all childpoints
		 * If going from CONTROL to MONITOR
		 *		stop polling values
		 *		set quality to bad for all childpoints
		 * If going from MONITOR to MONITOR
		 *		don't do anything
		 *
		 * This call should not call deactivateServant() on the datapoints
		 * You should be able to call setControl() again after this call
		 */
		virtual void setMonitor() = 0;

		//Note that there is a cpp file for this class so that these
		//members can be initialized
		static const std::string IDataPointDriver::MONITOR;
		static const std::string IDataPointDriver::CONTROL;
		static const std::string IDataPointDriver::INIT;

		/**
		 * getMode
		 *
		 * @return string indicating mode
		 * possible values are:
		 *	IDataPointDriver::MONITOR,
		 *  IDataPointDriver::CONTROL
		 *  IDataPointDriver::INIT
		 *
		 * Driver should be in init mode until setControl() or
		 * setMonitor() has been called for the first time
		 */
		virtual std::string getMode() = 0;

		/**
		 * onTerminate
		 *
		 * Stop updating the points and set them to a state appropriate for
		 * no comms
		 * DO NOT call deactivateServant() on your points. The 
		 * AgentUser does this for you
		 * Do not return untill you are happy to have your points deactivated.
		 */
		virtual void onTerminate() = 0;		

		/**
		 * processOnlineAgentUpdate
		 *
		 * This method will be called when the agent configuration is changed
		 * online. Note that online datapoint configuration updates are
		 * not passed to driver.
		 *
		 * @param changedParams contains the names of the params that have changed
		 */
		virtual void processOnlineAgentUpdate(std::vector<std::string> changedParams) = 0;


		virtual ~IDataPointDriver(){}

	};
} // end namespace TA_Base_App

#endif // !defined(AFX_IDATAPOINTDRIVER_H__7679A413_A17B_4FC5_804C_A4379853FFA9__INCLUDED_)
