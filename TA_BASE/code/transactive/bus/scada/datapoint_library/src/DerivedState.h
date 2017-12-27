/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DerivedState.h $
  * @author  HoaVu
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2015/01/21 14:56:06 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	DerivedState represents a configured state of the Derived DataPoint.  A DerivedState
  * also has a reference to an output DataPoint entity
  *
  */

#ifndef DERIVED_STATE_H
#define DERIVED_STATE_H

#include <string>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"

namespace TA_Base_Core
{
    // forward declaration
	class ConfiguredDerivedState;
	class DerivedOutputAssociation;
}

namespace TA_Base_Bus
{
    // forward declaration
	class DataPointState;
	class DataPoint;
	typedef struct tagTriggeringEntityDetail TriggeringEntityDetail;

	//TD15649, To use datapoint proxy of DDO/DAO/TDO/TAO to send output command instead of to use datapoint corba object
	//To receive control state notification event and forward to DDO/DAO/TDO/TAO's client.
	class DerivedState: public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	public:

		DerivedState ( TA_Base_Core::ConfiguredDerivedState * configuredDerivedState,
					   DataPoint * theDerivedDataPoint );
		virtual ~DerivedState();

		/**
		 * getStateValue
		 *
		 * Get the configured value of the derived state associated with the
		 * Derived DataPoint entity
		 *
		 */
		unsigned long getStateValue() const;

		/**
		 * getStateDescription
		 *
		 * Get the description of the derived state associated with the
		 * Derived DataPoint entity
		 *
		 */
		const std::string & getStateDescription() const;

		/**
		 * getStateReturnCondition
		 *
		 * Get the ReturnCondition of the derived state associated with the
		 * Derived DataPoint entity
		 *
		 */
		const std::string & getStateReturnCondition() const;

		/**
		 * getAlarmEnabled
		 *
		 * Get the indication if the alarm is enabled for the derived state
		 *
		 */
		bool getAlarmEnabled() const;

		/*
		 * setAlarmEnabled
		 *
		 * Enable or disable the alarm associated with the DerivedState
		 * 
		 * @param newEnabled 	the new enabled setting
		 *
		 */
		void setAlarmEnabled( bool newEnabled );

		/**
		 * getAlarmDelay
		 *
		 * Get the alarm delay of the derived state
		 *
		 */
		int getAlarmDelay() const;

		/*
		 * setAlarmDelay
		 *
		 * Set the delay for the alarm associated with the DerivedState
		 * 
		 * @param newDelay 	the new delay setting
		 *
		 */
		void setAlarmDelay ( int newDelay );

		/**
		 * getAlarmSeverity
		 *
		 * Get the alarm severity of the derived state
		 *
		 */
		int getAlarmSeverity() const;

		/*
		 * setAlarmSeverity
		 *
		 * Set the severity for the alarm associated with the DerivedState
		 * 
		 * @param newSeverity 	the new severity setting
		 *
		 */
		void setAlarmSeverity ( int newSeverity );

		/**
		 * getAlarmMessage
		 *
		 * Get the alarm message of the derived state
		 *
		 */
		const std::string & getAlarmMessage() const;

		/*
		 * setAlarmMessage
		 *
		 * Set the alarm message for the alarm associated with the DerivedState
		 * 
		 * @param newMessage 	the new alarm message setting
		 *
		 */
		void setAlarmMessage ( const std::string & newMessage );

		/**
		 * getMMSAlarmConfiguration
		 *
		 * Get the MMS alarm configuration of the derived state
		 *
		 */
		const std::string & getMMSAlarmConfiguration() const;

		/*
		 * setMMSAlarmConfiguration
		 *
		 * Set the MMS alarm configuration for the alarm associated with the DerivedState
		 * 
		 * @param newConfiguration 	the new MMS alarm configuration
		 *
		 */
		void setMMSAlarmConfiguration ( const std::string & newConfiguration );

		/**
		 * setOutputEntities
		 *
		 * Set all output datapoint entities associated with the DerivedState to 
		 * the predefined values.  Return true to indicate the operation is successfull,
		 * false to indicate the operation has failed to set at least one output datapoint.
		 *
		 */
		bool setOutputEntities ( const std::string& sessionId,
								 bool auditEventLoggingEnabled,
                                 const TA_Base_Bus::DataPointControl& trigDp,
								 const EControlOperation operation = CONTROL_NO_OPERATION);

		/**
  		 * setOutputInhibitState
  		 *
  		 * Update the current control inhibited status of the data point with the
		 * specified indication.
		 *
		 * @param indication [IN]	An indication if the control is inhibited, true
		 *							to inhibit the data point's control ability
		 *
  		 */
		virtual void setOutputInhibitState ( const std::string& sessionId, TA_Base_Bus::EOutputInhibitState inhibitstate  );

		//
		// operations specific to IEntityUpdateEventProcessor
		//
		virtual void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType);
			
	private:
		void clearChildProxy();

		TA_Base_Core::ConfiguredDerivedState * m_configuredDerivedState;
		std::vector < TA_Base_Core::DerivedOutputAssociation * > m_listOfOutputEntities;
		std::vector<TA_Base_Bus::DataPointProxySmartPtr*> m_listOfDataPointProxies; //TD15649

		DataPoint *				m_derivedDataPoint;

		DerivedState();
		DerivedState ( const DerivedState & theInstance );
		DerivedState & operator= ( const DerivedState & rhs );
	};

	typedef std::map < unsigned long, DerivedState * > DerivedStatesMap;
}

#endif
