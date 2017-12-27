/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/TaComponentsDriver.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class impliments IDataPointDriver.
  * It is responcible for montoring TransActive Components. Currently the components monitored include:
  *  - System Status Controllers
  *  - Control Stations
  *  - Database Server: NOT YET IMPLIMENTED
  * TODO update this list
  * 
  * This driver can be extended to monitor any type of datapoint
  * that can be "self-resolving" (see ISelfResolvingPointContainer). 
  * In short this driver uses a thread pool to update its points.
  *
  * To add a new component type to this driver do the following:
  * 1) Create a new ISelfResolvingPointContainer implimentation
  * 2) Add the new line in the convertToIPointContainer() method.
  *
  */

#if !defined(AFX_SSA_TACOMPDRIVER_H_INCLUDED_)
#define AFX_SSA_TACOMPDRIVER_H_INCLUDED_

// Hu Wenguang ++
// Bug 637 (TD10894)
#include "core/alarm/src/AlarmHelper.h"
// Bug 637 (TD10894)
// ++ Hu Wenguang
#ifdef __WIN32__
#pragma warning( disable : 4503 ) 
#endif

#include "app/system_control/system_status_agent/src/IDataPointDriver.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/DpUpdateThread.h"
#include "core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class SystemStatusAgentEntityData;
}

namespace TA_Base_App
{

	class ISelfResolvingPointContainer;

	class SystemStatusAgent;

	class TaComponentsDriver : public IDataPointDriver
	{
	public:

	   /**
		* TaComponentsDriver()
		* 
		*/
		TaComponentsDriver(SystemStatusAgent& agentUser);

	   /**
		* ~TaComponentsDriver()
		* 
		*/
		virtual ~TaComponentsDriver();

	//The following methods impliment the IDataPointDriver interface:

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
		virtual bool keepThisPointUpdated(TA_Base_Bus::DataPoint* const dataPoint);


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
		*/
		virtual void setControl();

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
		 * If going from INIT (see IDataPoint driver)
		 *		don't do anything
		 * If going from CONTROL to MONITOR
		 *		stop polling values
		 *		set quality to bad for all childpoints
		 * If going from MONITOR to MONITOR
		 *		don't do anything
		 *
		 * This call should not call deactivateServant() on the datapoints
		 * You should be able to call setControl() again after this call
		 */
		virtual void setMonitor();


		/**
		 * getMode
		 *
		 * @return string indicating mode
		 * possible values are:
		 *	IDataPointDriver::MONITOR,
		 *  IDataPointDriver::CONTROL
		 *  IDataPointDriver::INIT
		 *
		 * Driver should be in INIT mode until setControl() or
		 * setMonitor() has been called for the first time
		 */
		virtual std::string getMode();

		/**
		 * onTerminate
		 *
		 * Stop updating the points and set them to a state appropriate for
		 * no comms
		 * DO NOT call deactivateServant() on your points. The 
		 * AgentUser does this for you
		 * Do not return untill you are happy to have your points deactivated.
		 */
		virtual void onTerminate();		

		/**
		 * processOnlineAgentUpdate
		 *
		 * This method will be called when the agent configuration is changed
		 * online. Note that online datapoint configuration updates are
		 * not passed to driver.
		 *
		 * @param changedParams contains the names of the params that have changed
		 */
		virtual void processOnlineAgentUpdate(std::vector<std::string> changedParams);

		/**
		 * getNextDataPoint
		 * 
		 * 
		 * This thread-safe method iterates through the datapoints, starting
		 * at the beginning again when the end is reached
		 *
		 * @return the next ISelfResolvingPointContainer* or 0 if this driver owns no points or
		 * if the a request to stop the threads has been received.
		 *
		 * This method also impliments the polling rate by requesting that threads nap
		 * for any remaining time at the top of each loop
		 *
		 */
		ISelfResolvingPointContainer* getNextDataPoint();

		/**
		 * reportPointMissingPingPoint	
		 * 
		 * Call this method to tell the driver that you were expecting to find a datapoint 
		 * configured as a ping point for the point specifed in the param. See 
		 * AbstractPingDependentPoint for more information.
		 *
		 * Note that the driver will raise an alarm during init if one or more points
		 * are missing
		 *
		 *@param address: the pkey of the point that is missing the paired ping ping point
		 *
		 */
		static void reportPointMissingPingPoint(unsigned long pkey);

	private:
		//data members:

		//stores a list of missing ping points so that the problem can be reported during init
		static std::vector<unsigned long> m_pkeysOfPointsMissingPingPoints;
		
		//The DataPoints that this driver owns are stored in the following vector
		//Note that IPointContainer contains a Datapoint
		typedef std::vector<ISelfResolvingPointContainer*> DataPoints;
		typedef DataPoints::iterator IterToDp;
		DataPoints m_dataPoints;
		IterToDp m_iterator;

		//This vector of threads acts as a thread pool
		std::vector<DpUpdateThread*> m_threads;

		//The number of threads in the pool
		const int NUM_THREADS;

		//provides a thread lock
		TA_Base_Core::ReEntrantThreadLockable m_lock;

		//indicates which was called last, terminateAllThreads() or startAllThreads()
		bool m_terminateAllThreads;

		/*
		 *	IDataPointDriver::MONITOR,
		 *  IDataPointDriver::CONTROL
		 *  IDataPointDriver::INIT
		 */
		std::string m_mode;


		SystemStatusAgent& m_agentUser;

		TA_Base_Core::SystemStatusAgentEntityDataPtr m_agentEntityData;

		//the polling rate
		unsigned int m_pollingRateSec;
		//the time last polling cycle started
		time_t m_lastTime;

	private:


		//private methods:

		/**
		 * raiseAlarmOnMissingPingPoints
		 * 
		 * 
		 * raises a single alarm ON THE AGENT if any of the expected
		 * paired ping points are missing - see AbstractPingDependentPoint for
		 * more information
		 *
		 */
		void raiseAlarmOnMissingPingPoints();


		/**
		 * terminateAllThreads
		 * 
		 * 
		 * starts all DpUpdate threads
		 *
		 */
		void startAllThreads();

		/**
		 * terminateAllThreads
		 * 
		 * 
		 * Stops all DpUpdate threads before returning
		 *
		 */
		void terminateAllThreads();


		/**
		 * convertToIPointContainer
		 * 
		 * 
		 * Will create a new IPointContainer on the heap (you own the memory) if the
		 * dataPoint is a type we support - else return 0
		 *
		 * @return a IPointContainer* that owns the point or 0 if param dataPoint
		 * is not a type we support
		 *
		 */
		ISelfResolvingPointContainer* convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint);

		/**
		 * init
		 * 
		 * 
		 * This is called for the first and only time when the mode changes
		 * from INIT to something else. It is not called in the CTOR
		 * because the gen_agent object does not exist when this object
		 * is created.
		 * 
		 * This implimentation:
		 *		calls onInit on each ISelfResolvingPointContainer
		 *		creates the threads in the thread pool
		 *		inits the iterator to the datapoints
		 *
		 */
		void init();

		/**
		 * setPointFromControlToMonitorState
		 * 
		 * Set the quality of the point to TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE
		 * The old value is maintained
		 */
		//void setPointFromControlToMonitorState(TA_Base_Bus::DataPoint* const dataPoint);


		//disable the copy CTOR and the assignment operator
		TaComponentsDriver(const TaComponentsDriver& obj);
		TaComponentsDriver& operator=(const TaComponentsDriver& obj);

        // Hu Wenguang ++
        // Bug 637 (TD10894)
        TA_Base_Core::AlarmHelper* m_alarmHelper;
        // Bug 637 (TD10894)
        // ++ Hu Wenguang

	};

} // end namespace TA_Base_App
#endif // !defined(AFX_SSA_TACOMPDRIVER_H_INCLUDED_)
