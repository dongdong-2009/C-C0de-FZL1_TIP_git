/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/threadedDriver/AbstractThreadedDriver.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class impliments IDataPointDriver as well as extending TA_Base_Core::Thread.
  *
  * This class provides a way to start a thread (in setControl()), in which a 
  * loop repeatedly calls the abstract methods pollPoints() and doPostPollUpdate(). 
  * A driver that extends this class is expected to scan the field equipment in pollPoints()
  * It can then either update the related Datapoints at that time or do it retroactivly
  * in doPostPollUpdate()
  * 
  * This abstract class is intended to be implimented by any datapoint driver that can
  * keep its datapoints updated by running around in a loop in a seperate thread, collecting info
  * used to determine what values their datapoints should be set to then updating the points as
  * required. 
  *
  * Normally it is a drivers responcibilty to raise alarms resulting from datapoint 
  * bad quality, while the responcibility to raise alarms due to datapoint values is placed
  * on the datapoint library which utilises the alarm configuration for the datapoint.
  * Note however that this is not always the case: 
  * The PingableDataPointDriver as example, never raises bad quality
  * alarms, instead if it can not ping a point, for what ever reason, it simply sets that 
  * point's value to 0 via the Datapoint interface. The datapoint library will then raise
  * an alarm, assuming that the point has been configured so that a false value = alarm 
  */

#if !defined(AFX_SSA_ABSTHREADEDDRIVER_H_)
#define AFX_SSA_ABSTHREADEDDRIVER_H_

#if defined(WIN32)
#pragma warning(disable:4786 4503)
#endif

#include "app/system_control/system_status_agent/src/IDataPointDriver.h"
#include "core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Core
{
	class SystemStatusAgentEntityData;
}


namespace TA_Base_App
{
	class IPointContainer;

	class SystemStatusAgent;

	class AbstractThreadedDriver 
			: public IDataPointDriver, public virtual TA_Base_Core::Thread
	{

	public:

	//The following methods impliment the IDataPointDriver interface:

		/**
		  * keepThisPointUpdated
		  *
		  * This method returns true if this driver agrees to keep this
		  * datapoint updated and false if it does not.
		  *
		  * @preconditions: Results not defined if:
		  * 1) the param dataPoint is NOT already registered as a CORBA object. 
		  * 2) this driver is NOT stopped when this method is called
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
		 * @throw SocketFailedException if can't open socket on init
		 *
		 * If going from INIT to CONTROL
		 *      open a socket - throw if it can't
		 *      get the current config params      
		 *		start polling thread
		 *		return
		 *		Thread will:
		 *			set quality and values A/R
		 *			re-init alarms by sending alarm updates for all points reqarless of state
		 *
		 * If going from MONITOR to CONTROL
		 *
		 * If going from CONTROL to CONTROL 
		 *		do nothing 
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
		 * Driver should be in init mode until setControl() or
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
		 *
		 * This uses terminateAndWait()
		 */
		virtual void onTerminate();		

		/**
		 * processOnlineAgentUpdate
		 *
		 * This method will be called when the agent configuration is changed
		 * online. Note that online datapoint configuration updates are
		 * not passed to driver.
		 *
		 * This method will process a change to the polling rate for this driver. The
		 * name of this parameter is specified by getPollingRateParamName()
		 *
		 * @param changedParams contains the names of the params that have changed
		 */
		virtual void processOnlineAgentUpdate(std::vector<std::string> changedParams);

	//******************end of IDataPointDriver interface implimentation*****************

	//Implimentation of TA_Thread:

        /**
          * run
          *
          * Implementation of the TA_Thread run method
          */
        virtual void run();

        /**
          * terminate
          *
          * Implementation of the TA_Thread terminate method
		  *
		  * causes the run() method to return
		  *
		  * do not call this method directly - use terminateAndWait()
		  * 
          */
        virtual void terminate();

	//******************end of TA_Thread interface implimentation*****************

	//abstract methods to be implimented by AbstractThreadedDriver derivative:
	protected: //only this class uses the following methods, hence they are protected

	   /**
		* convertToIPointContainer
		*
		* If the driver is willing to keep this point updated it should return
		* a IPointContainer* which contains this point. If the driver does not
		* want this point return 0
		* The IPointContainer* should be created on the heap. You do NOT need to
		* delete this point on destruction - AbstractThreadedDriver does that for you
		*/
		virtual IPointContainer* convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint) = 0;

		/**
		 * onInit
		 * 
		 * This method is called when init() is called.
		 * 
		 * Do initialization stuff, e.g update run params etc  
		 * 
		 * This is called for the first and only time when the mode changes
		 * from INIT to monitor or control. It is not called in the CTOR
		 * because the gen_agent object does not exist when this object
		 * is created.
		 *
		 * @param pEntityData contains the updated configuration data
		 *
		 * @throw SocketFailedException if can't open socket on init
		 *
		 */
		virtual void onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData) = 0;

        /**
          * pollPoints
          * 
          * for all the points you own check the related state and value
          * of field equipment. Note that doPostPollUpdate() 
          * is called following each call to this method. Hence, you can either update your
		  * datapoints in this call or retoactivly update them in the doPostPollUpdate()
          * call. This mechanism is provided because some driver implimentations may
          * not wish to serialy scan and update datapoints.
		  *
		  * This method is called each loop through the thread
          */
		virtual void pollPoints() = 0;

        /**
          * doPostPollUpdate
          *
          * This method is called following each call to pollPoints. Hence, you can either 
		  * update your datapoints pollPoints or retoactivly update them here.
          * This mechanism is provided because some driver implimentations may
          * not wish to serialy scan and update datapoints.
          */
		virtual void doPostPollUpdate() = 0;

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
		virtual void onlineConfigUpdate(std::vector<std::string> changedParams) = 0;

		/**
		 * 
		 * getPollingRateParamName
		 *
		 * @return std::string: This method must return the name of the the configuration 
		 * parameter used to specify the rate at which your points will be polled, i.e. the
		 * rate at which pollPoints will be called
		 *
		 * Note that you will want to add a new polling rate param, as well as a default value 
		 * to SystemStatusAgentEntityData.h
		 *
		 * @param changedParams contains the names of the params that have changed
		 */
		virtual std::string getPollingRateParamName() = 0;


	//******************end abstract methods**********************

	//The following methods are intended to be employed by the deriving class
	//but not overridden:
	protected:

	   /**
		* AbstractThreadedDriver()
		* 
		*/
		AbstractThreadedDriver(SystemStatusAgent& agentUser);

	   /**
		* ~AbstractThreadedDriver()
		* 
		*/
		virtual ~AbstractThreadedDriver();
	public:
		//A static helper method for all to enjoy:

		/**
		 * convertAddressFieldToIpAddress
		 *
		 * @param stringToConvert is expected to have the form
		 *		"<prefix>xxx.xxx.xxx.xxx"
		 * where xxx is an interger 0 to 255
		 *
		 * @param prefix is an identifier used to determine how this point is resolved,
		 * e.g. ping:xxx.xxx.xxx.xxx where the prefix would be "ping:"
		 *
		 * @param disableAddressValidation: if true then the method will just check that
		 * the param addressMixedCase starts with the param prefix, assuming it does
		 * it will then overwrite the addressMixedCase param with whatever remains 
		 * after the prefix is removed
		 * if false (the default) then full validation is performed (see above for required format)
		 *
		 * @return true if the param conforms to the format specified above
		 * else return false. This is not case sensitive.
		 *
		 * if return is true the param stringToConvert is modified so that it 
		 * contains xxx.xxx.xxx.xxx if false the param is not modified.
		 */
		static bool convertAddressFieldToIpAddress(std::string& addressMixedCase, 
			const std::string& prefix, bool disableAddressValidation = false);

		/**
		 * getDataPointByAddress
		 *
		 * @ return a pointer to the DataPoint with the specified "addressIdentifier" (see param)
		 * Returns 0 if no datapoint has this addressIdentifier.
		 *
		 * @param addressIdentifier is expected to be extracted from the configured address. 
		 * Note that the address configured for the datapoint will probably contain more than
		 * what you should pass in this parameter, e.g. if the configured address is 
		 * mftStatus:192.168.70.60 then this param should be set to 192.168.70.60
		 * The comparision performed does the same for the data points that are compared.
		 *
		 * Note that the motivation for this is that you are probably trying to find a point
		 * with a configured address of the form ping:<IP or hostname> so that you can pair
		 * this ping point with your, let's say, sysConStatus<IP or hostname> point. Hence,
		 * the bit we are using for comparison is the <IP or hostname> portion of the configured
		 * address, and that is what you are expected to pass in to this method
		 */
		TA_Base_Bus::DataPoint* getDataPointByAddressIdentifier(const std::string& addressIdentifier);

	protected: //these data members are protected because I'm too lazy to supply the required
		//get and set methods, etc - particulary for the vector

		//The DataPoints that this driver owns are stored in the following vector
		//Note that IPointContainer contains a Datapoint
		typedef std::vector<IPointContainer*> DataPoints;
		typedef DataPoints::iterator IterToDp;
		DataPoints m_dataPoints;

		TA_Base_Core::SystemStatusAgentEntityDataPtr m_agentEntityData;

		//set to true to stop the thread
		bool m_tellThreadToStop;

	private:
		//data members:
		std::string m_mode;
		SystemStatusAgent& m_agentUser;
		bool m_isRunning;
		unsigned int m_pollingRateSeconds; 

	private:
		//private methods:

		/**
		 * setPingRate
		 *
		 * apply the specified ping rate
		 */
		void setPingRate(unsigned int newRate);

		/**
		 * init
		 * 
		 * 
		 * This is called for the first and only time when the mode changes
		 * from INIT to something else. It is not called in the CTOR
		 * because the gen_agent object does not exist when this object
		 * is created. Calls onInit() after updating the params for this
		 * agent
		 *
		 * @throw SocketFailedException if can't open socket on init
		 *
		 */
		void init();

		/**
		 * setPointFromControlToMonitorState
		 * 
		 * 
		 * Set the quality of the point to TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE
		 * The old value is maintained
		 */
		//void setPointFromControlToMonitorState(TA_Base_Bus::DataPoint* const dataPoint);

		//disable the copy CTOR and the assignment operator
		AbstractThreadedDriver(const AbstractThreadedDriver& obj);
		AbstractThreadedDriver& operator=(const AbstractThreadedDriver& obj);

	};

} // end namespace TA_Base_App
#endif // !defined(AFX_SSA_ABSTHREADEDDRIVER_H_)

