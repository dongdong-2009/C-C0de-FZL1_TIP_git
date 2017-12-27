/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/DataPointDriverHelper.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class creates DataPointDriver objects as required and provides interaction with
  * these drivers in a generic fashion. 
  * Basically, you pass a Datapoint pointer to this thing and it decides (based on the datapoint configuration, etc)
  * if there is a DataPointDriver implientation that can support the datapoint. If there is it creates a singleton
  * instance of the requred driver, if one does not already exist. That DataPointDriver then handles interaction with
  * that point
  *
  * To add your implimentation of a IDataPointDriver to the framework do the following:
  * 1) add one line to the addDrivers method
  * 2) add #include <yourDriver>.h to DataPointDriverHelper.cpp
  */

#if !defined(AFX_DATAPOINTDRIVERHELPER_H__B2023A6D_FBC2_41BE_AF4E_07A2490CCC1D__INCLUDED_)
#define AFX_DATAPOINTDRIVERHELPER_H__B2023A6D_FBC2_41BE_AF4E_07A2490CCC1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __WIN32__
#pragma warning(disable:4786 4503)
#endif

#include <vector>

namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_Base_App
{

	class SystemStatusAgent;

	class IDataPointDriver;	

	class DataPointDriverHelper  
	{
	public:

		/**
		 * DataPointDriverHelper
		 *
		 * CTOR is private - this is a singleton
		 */
		DataPointDriverHelper(SystemStatusAgent& agentUser);


        /**
          * setMonitor
          *
          * This call propagates down to the IDatapoint driver implimentations when
		  * whenever GenericAgent receives a request from the Process Monitor to change
		  * its operating state to monitor.
		  *
          * Note that this agent does not need to sync between agents. Hence it does
		  * not need to register or derigister for messages
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
		  */
		void setMonitor();

        /**
          * setControl
          *
          * This call propagates down to the IDatapoint driver implimentations when
		  * whenever GenericAgent receives a request from the Process Monitor to change
		  * its operating state to control.
		  *
          * Note that this agent does not need to sync between agents. Hence it does
		  * not need to register or derigister for messages
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
		void setControl();

        /**
          * onTerminate
          *
		  * This method will be called when terminate is called on the agent.
		  * It just calls onTerminate on all your data point drivers.
          */
		void onTerminate();

		/**
		 * processOnlineAgentUpdate
		 *
		 * This method will be called when the agent configuration is changed
		 * online. Note that online datapoint configuration updates are
		 * not passed to driver.
		 * Call processOnlineAgentUpdate() on all your drivers
		 *
		 * @param changedParams contains the names of the params that have changed
		 */
		virtual void processOnlineAgentUpdate(std::vector<std::string> changedParams);

		/**
		 * ~DataPointDriverHelper()
		 *
		 */		
		virtual ~DataPointDriverHelper();

        /**
          * giveDataPointToADriver
          *
		  * @precondition the param dataPoint is already registered as a
		  * CORBA object. All drivers must be in init mode
		  *
          * @postcondition Assuming the method does not throw, datapoint you pass 
		  * will be kept updated by some IDataPointDriver implimentation. If this
		  * method does throw the datapoint will be unchanged
          *
          * @param dataPoint is DataPoint* that is already registered as a
		  * CORBA object.
		  *
		  * @throw SystemStatusAgentConfigException if there is no IDataPointDriver
		  * avaiable that can own this point
          */
		void giveDataPointToADriver(TA_Base_Bus::DataPoint* const dataPoint);

	private:

		SystemStatusAgent& m_agentUser;		

		/**
		  * addDrivers
		  *
		  * This method adds the driver types to the list of drivers that will
		  * be used to handle datapoints
		  *
		  * Modify the implimentation of this method to add your new DriverType.
		  * You just add one line to this method and add an include to the cpp
		  * for your IDataPointDriver implimentation
		  */
		void addDrivers();

		std::vector<IDataPointDriver*> m_drivers;
		typedef std::vector<IDataPointDriver*>::iterator driverIter;

		//disable the copy CTOR and the assignment operator
		DataPointDriverHelper(const DataPointDriverHelper&);
		DataPointDriverHelper& operator=(const DataPointDriverHelper&);
	};
} // end namespace TA_Base_App

#endif // !defined(AFX_DATAPOINTDRIVERHELPER_H__B2023A6D_FBC2_41BE_AF4E_07A2490CCC1D__INCLUDED_)
