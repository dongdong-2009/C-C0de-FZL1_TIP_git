/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/MeteredDataPoint.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  * Metered DataPoints are accumulators that are updated periodically
  * to provide a totalised meter unit count.
  *
  */

#ifndef METERED_DATAPOINT_H
#define METERED_DATAPOINT_H

#include <vector>

#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/datapoint_library/src/AnalogueDataPoint.h"

namespace TA_Base_Bus
{
    // forward declaration
	class InputAssociation;
	class DataPointState;

	class MeteredDataPoint : public AnalogueDataPoint,
							 public IEntityUpdateEventProcessor
	{
	public:


		MeteredDataPoint ( TA_Base_Core::DataPointEntityDataPtr	            	dataPointEntityAccess,
						   TA_Base_Bus::EDataPointType		                    dpType,
						   TA_Base_Bus::EDataPointDataType	                    dpDataType,
                           ScadaPersistencePtr                                  persistence,
                           TA_Base_Core::ScadaRootEntityData&                   scadaRootEntityData,
                           MmsScadaUtility&                                     mmsScadaUtility,
                           CommandProcessor&                                    commandProcessor );

		virtual ~MeteredDataPoint();


		//
		// operations specific to IEntityUpdateEventProcessor
		//


		virtual void processEntityUpdateEvent ( unsigned long entityKey, ScadaEntityUpdateType updateType );


		//
		// overloaded functions specific to DataPoint implementation
		//


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
	      * setMeteredCorrectionValue 
	      * 
	      *  set the metered correction value
	      * 
	      * @return virtual void 
	      * @param :  const std::string& sessionId
	      * @param : double newValue
	      * 
	      */
        virtual void setMeteredCorrectionValue ( const std::string& sessionId, double newValue );

        
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


        // override function of AnalogueDataPoint to check the pass in parameter
        virtual void updateFieldState( DataPointState & newState );

		virtual void initAfterProxyFactoryReady();
	protected:


		/**
		 * updateSpecificConfiguration
		 *
		 * Update configuration data specific to this DerivedDataPoint
		 *
		 * @param configChanges		The list of configuration changes
		 *
		 */
		virtual void updateSpecificConfiguration ( const std::vector< std::string > * configChanges );


	private:

		/**
		 * updateInputAssociation
		 *
		 * Update the input data point associated with this Metered DataPoint
		 *
		 * @param inputKey		The pkey of the associated input data point 
		 *
		 */
		void updateInputAssociation ( unsigned long inputKey );


		/**
		 * cleanup
		 *
		 * Clean up all objects created by this Metered DataPoint
		 *
		 */
		void cleanup();
		void initialise();


		/**
		 * createDerivedInputAssociation
		 *
		 * Create the input data point associated with this Metered DataPoint
		 *
		 */
		void createDerivedInputAssociation();


		/**
		 * calculateValue
		 *
		 * Calculate the value of this Metered DataPoint
		 *
		 */
		void calculateValue();


		/**
		 * updateCurrentState
		 *
		 * Update the current state of this Metered DataPoint with the
		 * specified value.
		 *
		 * @param stateValue	The new value to be updated
		 *
		 */
		void updateCurrentState ( double stateValue );


		/**
		 * totalising
		 *
		 * Calculate and returned the totalised value based on the specified
		 * input value.
		 *
		 * @param inputValue	The new value to be totalised
		 *
		 */
		double totalising ( double inputValue );


		/**
		 * persistLastInputValue
		 *
		 * Persist the specified value to the LastInputValue parameter
		 * of the Metered DataPoint.
		 *
		 * @param lastInputValue	The value to be persisted
		 *
		 */
		void persistLastInputValue ( double lastInputValue );


		/**
		 * persistCorrectionValue
		 *
		 * Persist the specified value to the CorrectionValue parameter
		 * of the Metered DataPoint.
		 *
		 * @param correctionValue	The value to be persisted
		 *
		 */
		void persistCorrectionValue ( double correctionValue );


		/**
		 * persistAccumulatedValue
		 *
		 * Persist the current acculated value of the Metered DataPoint
		 * 
		 * return false when failed.
		 */
		bool persistAccumulatedValue();

		/**
		 * correct
		 *
		 * Correct the meter
		 *
		 * return false when failed persist;
		 */
		bool correct ( double newValue, std::string sessionId );


		bool						m_initialised;
		std::string					m_EngineeringLabel;
		double						m_lastInputValue;
		unsigned long				m_inputDependencyPKey;
		double				        m_rolloverConstant;
		double						m_accumulatedInputValue;
		double						m_meterFactor;
		TA_Base_Core::ReEntrantThreadLockable		m_ownLock;
		EQualityStatus				m_currentStatus;
		DataPointState *			m_currentState;	
		InputAssociation *			m_inputAssociation;
	};
}

#endif
