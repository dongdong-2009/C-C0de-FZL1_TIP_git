/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/ISelfResolvingPointContainer.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This interface extends to the IPointContainer interface
  * This simple interface just provides a box to put your datapoints in.
  * It also dictates that this container be able to update its point when told to do so
  * Updating a point would typically include the following steps:
  * 1) scan the physical entity for value and state
  * 2) write the new value and or state the datapoint
  * 3) raise bad_quality alarms as required on the point. Note that typically you will not
  * raise alarms on the value of a datapoint because the datapoint library will do that for
  * you, based on the alarm config of the point. The datapoint library leaves raising alarms
  * related to quality up to you however.
  * Also note that not all types of datapoins allow alarm configuration, TextDataPoint as
  * example.
  *
  */

#if !defined(AFX_ISELFRESPOINTCONTAINER_H_INCLUDED_)
#define AFX_ISELFRESPOINTCONTAINER_H_INCLUDED_

#include "core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.h"
#include "app/system_control/system_status_agent/src/IPointContainer.h"
#include "core/utilities/src/TAAssert.h"
#include <vector>



namespace TA_Base_App
{
	class ISelfResolvingPointContainer : public IPointContainer
	{
	public:

		
	   /**
		* convertToIPointContainer
		*
		* If this IPointContainer can keep this dataPoint updated it should return
		* a IPointContainer* which contains this point. If the IPointContainer
		* does not want this point return 0
		* The IPointContainer* should be created on the heap. You do NOT need to
		* delete this point on destruction
		*
		* STATIC METHODS CAN NOT BE DECLARED VIRTUAL. Hence I have placed this here as a 
		* reminder that you must impliment a method with this signature in your
		* implimentation of ISelfResolvingPointContainer
		*/
		static IPointContainer* convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint)
		{
            TA_ASSERT(false, "This method should never be called. It is here to remind you to do it.");
            return NULL;
        }

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
		 */
		virtual void onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData) = 0;

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
		 * @param changedParams contains the names of the params that have changed
		 */
		virtual void onlineConfigUpdate(std::vector<std::string> changedParams) = 0;

		/**
		  *
		  * updateDataPoint
		  *
		  * When this method is called update the datapoint returned by getDataPoint()
		  *
		  * Updating a point would typically include the following steps:
	      * 1) scan the physical entity for value and state
	      * 2) write the new value and or state the datapoint
	      * 3) raise bad_quality alarms as required on the point. Note that typically you will not
	      * raise alarms on the value of a datapoint because the datapoint library will do that for
	      * you, based on the alarm config of the point. The datapoint library leaves raising alarms
	      * related to quality up to you however.
		  */
		virtual void updateDataPoint() = 0; 

	   /**
		* isUpdating
		*
		* returns false if the point is currently processing a call
		* to updateDataPoint().
		* THIS METHOD IS NOT THREAD-SAFE! Hence it is assumed that a
		* single client thread is employing this method
		*/
		virtual bool isUpdating() = 0;

		virtual ~ISelfResolvingPointContainer(){};

	protected:
		ISelfResolvingPointContainer(){};


	};
} // end namespace TA_Base_App

#endif // !defined(AFX_ISELFRESPOINTCONTAINER_H_INCLUDED_)
