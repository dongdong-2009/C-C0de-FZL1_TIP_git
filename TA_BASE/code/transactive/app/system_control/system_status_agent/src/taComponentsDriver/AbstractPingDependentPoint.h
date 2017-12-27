/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/AbstractPingDependentPoint.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This abstract class extends ISelfResolvingPointContainer
  * The ISelfResolvingPointContainer interface provides a box to put your datapoints in.
  * It also dictates that this container be able to update its point when told to do so
  *
  * This class AbstractPingDependentPoint provides a data point who's quality is dependent on
  * the state of a related PingablePoint. As example the SystemControllerPoint class, which extends
  * this class, gets the state of a System Controller CORBA object but then looks at the related
  * ping point and sets the quality of the status point to BAD if the related point is not
  * pingable.
  *
  * A class that impliments this class is responcible for determining the state that
  * the point should be set to without regard to the ping point. This class then basically
  * applies the related ping point as a mask - setting the point's quality to BAD if the ping
  * point is not readable
  *
  */

#if !defined(AFX_ABSTRACTPINGDEPENDENTPOINT_H__00325D5C_BE99_44AC_980C_8AD48DB29566__INCLUDED_)
#define AFX_ABSTRACTPINGDEPENDENTPOINT_H__00325D5C_BE99_44AC_980C_8AD48DB29566__INCLUDED_

#if defined(WIN32)
#pragma warning(disable:4786 4503)
#endif

#include "app/system_control/system_status_agent/src/taComponentsDriver/ISelfResolvingPointContainer.h"

#include "bus/scada/datapoint_library/src/BooleanDataPoint.h"

namespace TA_Base_App
{
	class PingableDataPointDriver;

	class AbstractPingDependentPoint : public ISelfResolvingPointContainer  
	{
	public:

		virtual ~AbstractPingDependentPoint();


		/**
		 * onInit
		 * 
		 * This is called for the first and only time when the mode changes
		 *
		 * This implimentation resolves the ping point and stores it as a member.
		 * If the related ping point can not be resolved this method will raise
		 * an TA_Base_Core::SystemStatusAgentAlarms::DataPointNotCreated alarm and the state
		 * of the point will have BAD quality for the life of the agent.
		 *
		 * The class that impliments this class should call this base class method from it's own
		 * onInit() implimentation, assuming this method is overwritten
		 *
		 * @param pEntityData contains the updated configuration data
		 *
		 */
		virtual void onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData);

		/**
		 * setPingDriver
		 * 
		 * This method is used to give ping dependent points a copy of the ping driver
		 * This method must be called prior to calling the onInit() method
		 *
		 * @param pEntityData contains the updated configuration data
		 *
		 */
		static void setPingDriver(PingableDataPointDriver* driver);

		/**
		  *
		  * updateDataPoint
		  *
		  * This method first calls updateDataPointBasedOnPing()
		  * to see if it should bother trying to update the point. If the ping is OK it then
		  * employs derived class methods to update the data point
		  *
		  * This method also ensures that isUpdating() returns true
		  * when this method is running - not thread-safe.
		  */
		virtual void updateDataPoint();

	   /**
		* isUpdating
		*
		* returns false if the point is currently processing a call
		* to updateDataPoint().
		* THIS METHOD IS NOT THREAD-SAFE! Hence it is assumed that a
		* single client thread is employing this method
		*/
		virtual bool isUpdating();

	protected:
		AbstractPingDependentPoint();

		/**
		  *
		  * THIS METHOD SHOULD NOT BE OVERRIDDEN
		  *
		  * updateDataPointBasedOnPing
		  *
		  * When this method is called update the datapoint returned by getDataPoint()
		  *
		  *
		  *@return true if the paired point is pingable. If it is pingable then
		  * this point's state is not modified. Return false and update the state
		  * of the point as indicated below if the paired point is not pingable.
		  *
		  * The intent is that the deriving class call this method in its updateDataPoint() method
		  * and not update the datapoint if this method returns false, given that you will
		  * not be able to resolve the object and this call has set the quality approriatly already.
		  *
		  * This class AbstractPingDependentPoint provides a data point who's quality is dependent on
		  * the state of a related PingablePoint. As example the SystemControllerPoint class, which extends
		  * this class, gets the state of a System Controller CORBA object but then looks at the related
		  * ping point and sets the quality of the status point to BAD if the related point is not
		  * pingable.
		  *
		  * If the related ping point could not be resolved then set TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED:
		  * If the ping point is low but has been high before set TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE
		  * If the ping point has never been high set TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED
		  *
		  * If you can't ping do not modify the value of the point, just the state
		  */
		bool updateDataPointBasedOnPing();

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
		virtual bool updatePointBasedOnProcessStatus() = 0;

	   /*
		* isResolved
		*
		*@return true if you feel that resolve() should not need not be called
		* prior to calling updateBasedOnProcessStatus(). Note that you can return true
		* even if it is possible (but unlikely) that the resolved object is no longer
		* valid
		* 
		*/
		virtual bool isResolved() = 0;

	   /**
		* setStateForNoProcessResolution
		*
		* update the state of the data point to an appropriate state given that you can not read the status of the
		* related process.
		*
		* @throw if you can not write to the datapoint
		* 
		*/
		virtual void setStateForNoProcessResolution() = 0;

	   /**
		* resolve
		*
		* resolve or re-resove (and store) the process or object that is used to 
		* determine the state that the datapoint should be set to
		*
		*@return true if the resolution is OK - else false
		* 
		*/
		virtual bool resolve() = 0;

		//used to determine if BAD_LAST_KNOWN_VALUE should be used
		bool m_pingPointHasEverBeenHigh;

		//This is the paired Lan Status point
		TA_Base_Bus::BooleanDataPoint* m_pingPoint;

	private:

		bool m_isUpdating;

		static PingableDataPointDriver* m_pingableDataPointDriver;

		/**
		  *
		  * updateDataPointWithoutFlag
		  *
		  * This method was created just so I could cleanly wrap the
		  * m_isUpdating flag around all the code in updateDataPoint()
		  * This code does exactly what updateDataPoint() comment say
		  * that method does, less the flag stuff
		  */
		void updateDataPointWithoutFlag();

 

	};
} // end namespace TA_Base_App

#endif // !defined(AFX_ABSTRACTPINGDEPENDENTPOINT_H__00325D5C_BE99_44AC_980C_8AD48DB29566__INCLUDED_)
