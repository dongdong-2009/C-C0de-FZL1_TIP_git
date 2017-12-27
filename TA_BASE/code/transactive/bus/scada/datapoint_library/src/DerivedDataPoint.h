/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DerivedDataPoint.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	DerivedDataPoint, in SCADA term, is an equivalent of an enum.  It
  * combines several inputs into a single data point to represent a multi state
  * device.
  *
  */

#ifndef DERIVED_ANALOGUEDATAPOINT_H
#define DERIVED_ANALOGUEDATAPOINT_H

#include <vector>

#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DerivedState.h"

namespace TA_Base_Core
{
	class DerivedDataPointEntityData;
};

namespace TA_Base_Bus
{
    // forward declaration
	class InputAssociation;
	class DataPointState;

	class DerivedDataPoint : public DataPoint,
							 public IEntityUpdateEventProcessor
	{
	public:


		DerivedDataPoint ( TA_Base_Core::DataPointEntityDataPtr		            dataPointEntityAccess,
						   TA_Base_Bus::EDataPointType		                    dpType,
						   TA_Base_Bus::EDataPointDataType	                    dpDataType,
                           ScadaPersistencePtr                                  persistence,
                           TA_Base_Core::ScadaRootEntityData&                   scadaRootEntityData,
                           MmsScadaUtility&                                     mmsScadaUtility,
                           CommandProcessor&                                    commandProcessor );


		virtual ~DerivedDataPoint();


		//
		// operations specific to IEntityUpdateEventProcessor
		//


		virtual void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType);


		//
		// overloaded functions specific to DataPoint implementation
		//

        virtual const std::string & getStateLabel(unsigned long state) const;


		/**
  		 * DataPoint::getEngineeringUnits()
  		 *
		 * Modifies a DataPointCorbaDef structure to reflect the DataPoint's
		 * engineering units configuration. To be implemented by subclass.
  		 *
  		 * @param eng	the DataPointCorbaDef structure to be modified
		 *
  		 */
		virtual void getEngineeringUnits ( TA_Base_Bus::DataPointEngineeringUnits & eng );


	    /**
	      * getDataPointValueAsString
	      * 
	      * get the datapoint value as string
	      * 
	      * @return virtual std::string 
	      * @param : const DpValue & dpValue
	      * 
	      */
        virtual std::string getDataPointValueAsString( const DpValue & dpValue );

        /**
		  * setToControlMode
		  *
		  * This method can be called by the agent responsible for the entity in
		  * the event of its mode of operation changing to control. This enables the
		  * entity to perform tasks such as connecting to any field devices it is
		  * responsible for communicating with.
		  */
		virtual void setToControlMode();


		/**
		  * setToMonitorMode
		  *
		  * This method can be called by the agent responsible for the entity in
		  * the event of its mode of operation changing to monitor. This enables the
		  * entity to perform tasks such as disconnecting from any field devices it was
		  * communicating with.
		  */
		virtual void setToMonitorMode();


		/**
		 * DataPoint::updateFieldState()
		 *
		 * Determines whether a DataPointState represents a change for
		 * the DataPoint or not. Also performs engineering unit
		 * conversion and alarm condition testing.  The default operation will
		 * also generate and submit an event for the new state
		 *
		 * @param	newState			The new DataPointState
		 *
		 */
		virtual void updateFieldState( DataPointState & newState );


		/**
  		 * DataPoint::updateInServiceStatus
  		 *
  		 * Update the current in-service status of the data point with the
		 * specified indication.  Returns true to indicate if the operation is
		 * successfull, false otherwise
		 *
		 * @param indication [IN]	An indication if the data point is in service
		 *							or not.  True to indicate in-service, false
		 *							otherwise
		 *
  		 */
		virtual bool updateInServiceStatus ( bool inService );
        
         /**
	      * getCompleteDynamicUpdate
	      * 
	      * return the complete dynamic update
	      * 
	      * @return virtual boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> 
	      * 
	      */
        virtual boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> getCompleteDynamicUpdate() ;

	    /**
	      * getCompleteStaticConfig
	      * 
	      * return the complete static config for the datapoint. it is caller's reposibility to delete the return struct pointer
	      * 
	      * @return DatapointCompleteStaticConfig 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual boost::shared_ptr<DatapointCompleteStaticConfig> getCompleteStaticConfig() ;
        
        /**
	      * setAlarmProperties
	      * 
	      * set alarm properties
	      * 
	      * @return virtual void 
	      * @param :  const std::string& sessionId
	      * @param : const TA_Base_Bus::AlarmPropertyVect & newAlarmProperties
	      * 
	      * @exception <exceptions> Optional
	      */
         virtual void setAlarmProperties( const std::string& sessionId, const TA_Base_Bus::AlarmProperty & newAlarmProperties);

         //******************************************
        // Synchronization method for the datapoint
        //******************************************
	    /**
	      * updateSynState
	      * 
	      * used by the monitor mode datapoint to synchronize the internal dynamic state with
          * the control mode datapoint
	      * 
	      * @return 
	      * @param : TA_Base_Bus::DpDynamicSyncUpdate toupdate
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void updateSynState(const TA_Base_Bus::DpDynamicSyncUpdate& toupdate);

		virtual void initAfterProxyFactoryReady();
	protected:

        //******************************************
        // Synchronization private method for the datapoint
        //******************************************

	    /**
	      * updateSynAlarmProperties
	      * 
	      * update syn alarm properties
	      * 
	      * @return virtual void 
	      * @param : const TA_Base_Bus::AlarmProperty & newAlarmProperties
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties);


        /**
        * DataPoint::getAlarmPropertiesVect
        * 
        *  get the alarm properties vector
        * 
        * @return virtual TA_Base_Bus::AlarmPropertyVect 
        * 
        */
        virtual TA_Base_Bus::AlarmPropertyVect getAlarmPropertiesVect();
        
		/**
		 * updateSpecificConfiguration
		 *
		 * Update configuration data specific to this DerivedDataPoint
		 *
		 * @param configChanges [IN]		The list of configuration changes
		 *
		 */
		virtual void updateSpecificConfiguration ( const std::vector< std::string > * configChanges );


	private:


		/**
		 * calculateCombinedValue
		 *
		 * Calculate the combined value based on the values provided via the list.  The
		 * combined value is calculated from the following formulae:
		 *
		 * combinedValue = DP(n-1)*2**(n-1) + .... + DP(1)*2**(1) + DP(0)*2**(0)
		 *
		 */
		unsigned long calculateCombinedValue();


		/**
		 * calculateCombinedStatus
		 *
		 * Calculate the combined status of the DerivedDataPoint.  The combined
		 * status should represent the worst quality status of the input data point
		 * entities
		 *
		 */
		void calculateCombinedStatus();


		/**
		 * createDerivedInputAssociation
		 *
		 * Create the input associated with this DerivedDataPoint based on data
		 * extracted from database
		 *
		 */
		void createDerivedInputAssociation();


		/**
		 * createDerivedState
		 *
		 * Create the derived state associated with this DerivedDataPoint based on data
		 * extracted from database
		 *
		 */
		void createDerivedState();


		/**
		 * checkCombinedValue
		 *
		 * Check the combined value, calculated from the input entities associated
		 * with this DerivedDataPoint, against the configured derived state.  Raise
		 * alarm and set output if neccessary.
		 *
		 * @param combinedValue		The combined value to be checked against the
		 *							derived state
		 *
		 */
		void checkCombinedValue ( unsigned long combinedValue );


		/**
		 * updateCurrentState
		 *
		 * Update the current state of this DerivedDataPoint with the specified value
		 * and description.
		 *
		 * @param stateValue		The new value
		 * @param stateDescription	The description of the value
		 *
		 */
		void updateCurrentState ( unsigned long stateValue, const std::string & stateDescription );

		void initialise();
		void cleanup();

		DerivedStatesMap						m_listOfDerivedStates;

		bool									m_initialised;
		DataPointState *						m_currentState;
		EQualityStatus							m_currentStatus;
		TA_Base_Core::DerivedDataPointEntityData *	m_derivedDataPointEntityData;
		TA_Base_Core::ReEntrantThreadLockable       m_ownLock;

		std::vector < InputAssociation * >  m_listOfDerivedInputAssociations;
	};

}

#endif

