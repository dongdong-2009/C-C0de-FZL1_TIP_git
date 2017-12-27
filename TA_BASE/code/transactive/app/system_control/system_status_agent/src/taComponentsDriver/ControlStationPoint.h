/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/ControlStationPoint.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class provides a datapoint representing a Control Station
  * The boolean datapoint is high if the Control Station is contactable
  * and responds to polling - else low
  * Note that this implimenation does not raise alarms on low. It is
  * assumed that the datapoint will be configured so that alarms are 
  * raised on low datapoint value
  *
  */

#if !defined(AFX_CONTROL_STATION_POINT_H__79A8E911_C6D4_INCLUDED_)
#define AFX_CONTROL_STATION_POINT_H__79A8E911_C6D4_INCLUDED_

#include "app/system_control/system_status_agent/src/taComponentsDriver/AbstractPingDependentPoint.h"
#include "core/process_management/IDL/src/IControlStationCorbaDef.h"

namespace TA_Base_App
{
	class ControlStationPoint : public AbstractPingDependentPoint
	{
	public:

		/**
		 * ControlStationPoint	
		 * 
		 * DTOR
		 *
		 */
		virtual ~ControlStationPoint();

		/**
		 * getDataPoint	
		 * 
		 * @return the datapoint owned by this point
		 *
		 */
		virtual TA_Base_Bus::DataPoint* const getDataPoint();

		/*
		* getAddressIdentifier
		*
		* @return the raw IP address or hostname of the point.
		* Note that the address configured for the datapoint will probably contain more than
		* the above, e.g. mftStatus:192.168.70.60. This metod should just return 192.168.70.60 bit
		*/
		virtual std::string getAddressIdentifier();

		/**
		 * onInit
		 * 
		 * This is called for the first and only time when the mode changes
		 * from INIT to something else (either monitor or control) 
		 * on the driver that owns this point. Hence, you should do any
		 * value/state/alarm manipulation required here. Note that when the
		 * agent starts the point will already have a quality of:
		 * TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED
		 *
		 * @param pEntityData contains the updated configuration data
		 *
		 * This implimentation attempts to resolve and store a pointer
		 * to the System Controller. It does not throw is this can not be 
		 * accomplished
		 * It also sets the corba time out and port number via run params.
		 */
		virtual void onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData);

		/**
		 * 
		 * onlineConfigUpdate
		 *
		 * This method is called when onlineConfigUpdate is called on the TaComponentsDriver. 
		 * You should process any parameters specific to your point.
		 *
		 * This method will be called when the agent configuration is changed
		 * online. Note that online datapoint configuration updates are
		 * not passed to driver.
		 *
		 * This implimentation of this method will apply the port number and corba
		 * timeout if specified. Note that new params will not be applied unless
		 * the Control Station becomes uncontactable
		 *
		 * @param changedParams contains the names of the params that have changed
		 */
		virtual void onlineConfigUpdate(std::vector<std::string> changedParams);

	


	   /**
		* convertToIPointContainer
		*
		* If this ISelfResolvingPointContainer can keep this dataPoint updated it should return
		* a ISelfResolvingPointContainer* which contains this point. If the ISelfResolvingPointContainer
		* does not want this point return 0
		* The IPointContainer* should be created on the heap. You do NOT need to
		* delete this point on destruction
		*/
		static ISelfResolvingPointContainer* convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint);

	protected:
	   /**
		* updatePointBasedOnProcessStatus
		*
		* update the state of the point based on the status of the related object. 
		* You should set an appropriate state if even you can not read the status of the
		* related process (but return false). Ignore the posibliltiy of Lan failure, e.g. if you could
		* not reach a Control Station process set low, GOOD quality
		*
		*@precondition: isResolved() returns true
		* 
		*@return false if you can not resolve the process/object required to read
		* the status. If you update the point successfully return true.
		*
		* @throw if you can not write to the datapoint
		*
		*/
		virtual bool updatePointBasedOnProcessStatus();

	   /*
		* isResolved
		*
		*@return true if you feel that resolve() should not need not be called
		* prior to calling updateBasedOnProcessStatus(). Note that you can return true
		* even if it is possible (but unlikely) that the resolved object is no longer
		* valid
		* 
		*/
		virtual bool isResolved();

	   /**
		* setStateForNoProcessResolution
		*
		* update the state of the data point to an appropriate state given that you can not read the status of the
		* related process.
		*
		* @throw if you can not write to the datapoint
		* 
		*/
		virtual void setStateForNoProcessResolution();

	   /**
		* resolve
		*
		* resolve or re-resove (and store) the process or object that is used to 
		* determine the state that the datapoint should be set to
		*
		*@return true if the resolution is OK - else false
		* 
		*/
		virtual bool resolve();	
		
	private:

        // A pointer to the Control Station CORBA object
        TA_Base_Core::IControlStationCorbaDef_var m_controlStation;

		TA_Base_Bus::BooleanDataPoint* const m_datapoint;
		const std::string m_hostName;

		TA_Base_Core::SystemStatusAgentEntityDataPtr m_entityData;

		//These two values are set via run params
		unsigned int m_portNumber;
		unsigned int m_corbaTimeOutSec;



	private:
		//private methods:

        /** 
          * ControlStationPoint
          *
          * @param DataPoint* const datapoint is the point
		  * to update. Precondition the point is a registered CORBA object  
		  *
		  * @param const std::string hostName is the hostname where the CS is running
          */
		ControlStationPoint( 
			TA_Base_Bus::BooleanDataPoint* const datapoint,
			const std::string& hostName
		);

	};
} //end namespace TA_Base_App

#endif // !defined(AFX_CONTROL_STATION_POINT_H__79A8E911_C6D4_INCLUDED_)
