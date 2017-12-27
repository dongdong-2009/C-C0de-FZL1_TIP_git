#ifndef DATAPOINTPROXY_H
#define DATAPOINTPROXY_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointProxy.h $
  * @author:  Darren Sampson
  * @version: $Revision: #4 $
  * Last modification: $DateTime: 2016/12/30 18:22:55 $
  * Last modified by:  $Author: wang.wang $
  * 
  * This class provides access to a datapoint
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sys/timeb.h>
#include <vector>
#include <map>

#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataPointAccess.h"
#include "bus/scada/proxy_library/src/ProxyObserverNotifier.h"

#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

namespace TA_Base_Bus
{
   class IEntityUpdateEventProcessor;
   class DataPointSyncManager;
   class DataPointProxyManager;

	class DataPointProxy : public IDataPointProxy, public ProxyObserverNotifier				   
	{
		friend class DataPointProxyManager;

		public:

			DataPointProxy(TA_Base_Core::DataPointEntityDataPtr dpEntityData,
                DataPointSyncManager& syncManager);

			virtual ~DataPointProxy();

			// 
			// Accessors for Data Point Value State
			//
			/**
			  * getEntityKey
			  *
			  * This method gets the entity key of the data point.
			  *
			  * @return The entity key of the data point
			  */
			unsigned long	getEntityKey() const;
            unsigned long	getSubSystemKey() const;
            unsigned long	getPhysicalSubSystemKey() const;
            std::string 	getEntityName() const;
			unsigned long	getParentKey() const;
            unsigned long	getLocationKey() const;
			const std::string& 	getDescription() const;
			std::string  getAgentName() const;

			virtual bool isAlarmPropertiesValid() const; //wenching++

			virtual bool isConfigValid() const;
			/**
			  * getStatus
			  *
			  * This method gets the quality status of the Data point.
			  *
			  * @return The quality status of the data point
			  *
			  */
			unsigned long	getStatus();

			/**
			  * getQualityAsString
			  *
			  * This method gets the quality status of the Data point as a string.
			  *
			  * @return The quality status of the data point as a string
			  *
			  */
			const char* const getQualityAsString() const;

			/**
			  * isBoolean
			  *
			  * This method returns true if the data point type is boolean.
			  *
			  * @return Returns true if the datapoint type is boolean, false otherwise
			  */
			bool			isBoolean();

			/**
			  * isNumber
			  *
			  * This method returns true if the data point type is numeric.
			  *
			  * @return Returns true if the datapoint type is numeric, false otherwise
			  */
			bool			isNumber();

			/**
			  * isText
			  *
			  * This method returns true if the data point type is text.
			  *
			  * @return Returns true if the datapoint type is text, false otherwise
			  */
			bool			isText();

			/**
			  * isDerived
			  *
			  * This method returns true if the data point type is derived.
			  *
			  * @return Returns true if the datapoint type is derived, false otherwise
			  */
			bool			isDerived();

			/**
			  * getValue
			  *
			  * This method gets the current value of the Data point.
			  * If the data point is overriden, this will be the override value,
			  * otherwise, this will be the field value.
			  *
			  * @return The value of the data point
			  *
			  */
			DpValue	getValue();

			/**
			  * getValueAsString
			  *
			  * This method gets the current value of the Data point as a string.
			  * If the data point is overriden, this will be the override value,
			  * otherwise, this will be the field value.
			  *
			  * @return The value of the data point
			  *
			  */
			std::string		getValueAsString();

			/**
			  * getValueAsBool
			  *
			  * This method gets the current value of the Data point as a bool.
			  * If the data point is overriden, this will be the override value,
			  * otherwise, this will be the field value.
			  *
			  * @return The value of the data point
			  *
			  * @exception ScadaProxyException raised if not a boolean type.
			  */
			bool		getValueAsBool();

			/**
			  * getValueAsDouble
			  *
			  * This method gets the current value of the Data point as a double.
			  * If the data point is overriden, this will be the override value,
			  * otherwise, this will be the field value.
			  *
			  * @return The value of the data point
			  *
			  * @exception ScadaProxyException raised if not a numeric or derived type.
			  */
			double		getValueAsDouble();

			/**
			  * getTimestamp
			  *
			  * This method gets the current timestamp (in secs since the start of UTC epoch)
			  * of the Data point.
			  *
			  * @return The timestamp of the data point
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			long getTimestamp();

			/**
			  * getTimestampStructure
			  *
			  * This method gets the current timestamp (in secs and miliseconds since the start of UTC epoch)
			  * of the Data point.
			  *
			  * @return The timestamp of the data point as a timeb structure
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  *
			  */
			virtual const timeb & getTimestampStructure();

			/**
			  * getLastStatusChangeSessionId
			  *
			  * This method returns the full name of the operator who
			  * last performed any status change on a datapoint
			  *
			  * @return Full name of the operator
			  */
			std::string getLastStatusChangeSessionId();

			/**
			  * getLastStatusChangeTimestamp
			  *
			  * This method returns the timestamp when a datapoint's status
			  * has been last modified.
			  *
			  * @return timestamp 
			  */
			unsigned long getLastStatusChangeTimestamp();


			/**
			  * getFieldValue
			  *
			  * This method gets the current field value of the Data point.
			  * If the data point is not overriden, this will return an empty string
			  *
			  * @return The field value of the data point if it is overriden, empty string otherwise
			  */
			std::string		getFieldValue();

			/**
			*
			* getLaunchingConditionResultString
			*
			* Returns a string containing the result of the Launching Condition in
			* the following format:
			*
			* [PointDescription1] is [Value1], [PointDescription2] is [Value2], ...
			*
			*/
			std::string getLaunchingConditionResultString();

			/**
			  * setValue
			  *
			  * This method sets the value of the Data point.
			  * This method is valid for control data points and for overriding information data points.
			  *
			  * @param The value to set
			  *
			  * @exception ScadaProxyException raised if setting the data point fails.
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			void			setValue(std::string value, std::string sessionId = "", unsigned long triggeringEntityKey = 0, char * triggeringEntityValue = NULL, bool auditEventLoggingEnabled = true); //TD15649

			virtual void	setControlValue(std::string value, std::string sessionId = "",EControlOperation controlOperation = CONTROL_NO_OPERATION,  unsigned long triggeringEntityKey = 0, char * triggeringEntityValue = NULL, bool auditEventLoggingEnabled = true); 
			/**
			  * setBoolValue
			  *
			  * This method sets the value of the Data point. 
			  * This method is valid for control data points and for overriding information data points.
			  *
			  * @param The value to set
			  *
			  * @exception ScadaProxyException raised if setting the data point fails or if datapoint is not a bool
			  */
			virtual void			setBoolValue(bool value, std::string sessionId);

			/**
			  * setNumberValue
			  *
			  * This method sets the value of the Data point. 
			  * This method is valid for control data points and for overriding information data points.
			  *
			  * @param The value to set
			  *
			  * @exception ScadaProxyException raised if setting the data point fails or if datapoint is not an analogue type (int/float etc)
			  */
			virtual void			setAnalogueValue(double value, std::string sessionId);

			/**
			  * setMeteredCorrectionValue
			  *
			  * This method adjusts the metering value of the Data point.
			  * This method is valid for meter data points.
			  *
			  * @param The adjustment value
			  *
			  * @exception ScadaProxyException raised if adjusting the data point fails.
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			void			setMeteredCorrectionValue(double adjustment, std::string sessionId = "");

			/**
			  * isOverrideEnabled
			  *
			  * This method returns true if the data point override is enabled.
			  *
			  * @return Returns true if the data point override is enabled, false otherwise
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			bool			isOverrideEnabled();

			/**
			  * setOverrideEnableStatus
			  *
			  * This method sets the override enable status of the data point.
			  * This method is valid for information data points.
			  *
			  * @param The status to set
			  *
			  * @exception ScadaProxyException raised if setting fails.
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			void			setOverrideEnableStatus(bool enable, std::string sessionId = "");

			/**
			  * setManuallyOverrideValue
			  *
			  * This method forces the data point to manually override state and then sets its
			  * value to the specified value
			  *
			  * @param	value		The manually entered value to be set
			  * @param	sessionId	The operator's session id
			  *
			  * @exception ScadaProxyException raised if setting fails.
			  */
			virtual void	setManuallyOverrideValue ( const std::string & value, const std::string & sessionId );

			/**
			  * setAlarmProperties
			  *
			  * This method changes the alarm properties for the datapoint 
			  *
			  * @param	newAlarmProperty	The new alarm property of the data point
			  * @param	sessionId			The operator's session id
			  *
			  * @exception ScadaProxyException raised if setting fails.
			  *
			  */
			virtual void setAlarmProperties ( const AlarmProperty & newAlarmProperty, const std::string & sessionId );

			//
			// Accessors for Data point Inhibit State
			//

			/**
			  * isControlInhibited
			  *
			  * This method returns true if data point control is inhibited.
			  *
			  * @return Returns true if data point control is inhibited, false otherwise
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			bool			isControlInhibited();

			/**
			  * setOutputInhibitStatus
			  *
			  * This method sets the control inhibit status of the data point.
			  * This method is valid for control data points.
			  *
			  * @param The status to set
			  *
			  * @exception ScadaProxyException raised if setting fails.
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			void			setOutputInhibitStatus(EOutputInhibitState inhibit, std::string sessionId = "");

			/**
			  * isScanInhibited
			  *
			  * This method returns true if data point scanning is inhibited.
			  *
			  * @return Returns true if data point scanning is inhibited, false otherwise
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			bool			isScanInhibited();

			/**
			  * isAlarmInhibited
			  *
			  * This method returns true if data point alarms are inhibited.
			  *
			  * @return Returns true if data point alarms are inhibited, false otherwise
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			bool			isAlarmInhibited();

			/**
			  * isMmsAlarmInhibited
			  *
			  * This method returns true if data point mms alarms are inhibited.
			  *
			  * @return Returns true if data point mms alarms are inhibited, false otherwise
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			bool			isMmsAlarmInhibited();

			/**
			  * setScanInhibitStatus
			  *
			  * This method sets the scan inhibit status of the data point.
			  * This method is valid for information data points.
			  *
			  * @param The status to set
			  *
			  * @exception ScadaProxyException raised if setting fails.
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			void			setInputInhibitStatus(EInputInhibitState inhibit, std::string sessionId = "");

			//
			// Accessors for Data point Control State
			//

			/**
			  * getControlState
			  *
			  * This method gets the current control state of the Data point.
			  *
			  * @return The control state of the data point
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			EDataPointControlState getControlState();

			/**
			  * getControlTriggeringEntity
			  *
			  * This method gets the entity key of the entity that triggered the current 
			  * control state of the Data point.
			  *
			  * @return The entity key of the triggering entity
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			unsigned long getControlTriggeringEntity();

			/** TD15649 ++
			  * getControlTriggeringValue
			  *
			  * This method gets the entity value of the entity that triggered the current 
			  * control state of the Data point.
			  *
			  * @return The entity value of the triggering entity
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			std::string getControlTriggeringValue();

			//
			// Accessors for Alarm State
			//

			/**
			  * isInAlarmState
			  *
			  * This method returns true if data point has an open alarm.
			  *
			  * @return Returns true if data point has an open alarm, false otherwise
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			bool			isInAlarmState();

			/**
			  * getAlarmStateShortString
			  *
			  * This method returns the short string representing the alarm state.
			  *
			  * @return Returns the short string representing the alarm state
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			std::string		getAlarmStateShortString();

							
			//
			// Accessors for Data point configuration
			//

			/**
			  * isWriteable
			  *
			  * This method returns true if data point is writeable.
			  *
			  * @return Returns true if data point is writeable, false otherwise
			  */
			bool			isWriteable();

			/**
			  * getType
			  *
			  * This method gets the type of the data point
			  *
			  * @return The type of the data point
			  */
			EDataPointType	getType();

			/**
			  * getTypeAsString
			  *
			  * This method gets the type of the data point as a string.
			  *
			  * @return The type of the data point as a string
			  */
			std::string		getTypeAsString();

			/**
			 *
			 * getShortNameOfType
			 *
			 * This method gets the short name of the type of datapoint (eg. DI, AI, DDI)
			 *
			 * @return The short name of the type of datapoint
			 */
			std::string getShortNameOfType();

			/**
			  * getDisplayOrder
			  *
			  * This method gets the display order number of the data point.
			  * A negative number indicates that the data point should not be displayed
			  *
			  * @return The display order number of the data point
			  */
			int		getDisplayOrder();


			/**
			  * getUnit
			  *
			  * This method gets the units of the data point
			  *
			  * @return The units of the data point
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			std::string		getUnit();

			/**
			  * getLimit
			  *
			  * This method gets a limit for the data point
			  *
			  * @param The limit to retrieve - true retrieves the low limit, false the high limit
			  *
			  * @return A limit for the data point
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			double			getLimit(bool lowLimit);
			
			/**
			  * getBooleanLabel
			  *
			  * This method gets a boolean for the data point
			  *
			  * @param The label to retrieve - true retrieves the true label, false the false label
			  *
			  * @return A boolean label for the data point
			  *
			  * @exception ScadaProxyException raised if not a boolean type.
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			std::string		getBooleanLabel(bool bTrue);

			/**
			  * getDerivedStatesAsMap
			  *
			  * This method gets all the derived states of the data point as a map
			  *
			  * @return Derived states as a map
			  *
			  * @exception ScadaProxyException raised if not a derived type.
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			DpvEnumLabelsMap getDerivedStatesAsMap();

			/**
			  * getDerivedStateAsString
			  *
			  * This method gets a derived state of the data point as a string
			  *
			  * @param The state to retrieve
			  *
			  * @return A derived state as specified by the parameter as a string
			  *
			  * @exception ScadaProxyException raised if not a derived type.
			  * @exception ScadaProxyException raised if not a valid stateValue
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			std::string		getDerivedStateAsString(unsigned long stateValue);

			/**
			  * getDerivedStateAsDouble
			  *
			  * This method gets a derived state of the data point as a double
			  *
			  * @param The state to retrieve
			  *
			  * @return A derived state as specified by the parameter as a double
			  *
			  * @exception ScadaProxyException raised if not a derived type.
			  * @exception ScadaProxyException raised if not a valid stateValue
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			unsigned long 	getDerivedStateAsUnsignedLong(std::string stateValue);

			/**
			  * getAlarmProperties
			  *
			  * This method returns the alarm properties for the datapoint 
			  *
			  * @return a vector of alarm properties
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			AlarmPropertyVect getAlarmProperties();

			/**
			  * isAlarmEnabled
			  *
			  * This method returns true if the alarm type is enabled 
			  *
			  * @param The alarm type
			  *
			  * @return true if the Alarm type is enabled, false otherwise.
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			bool isAlarmEnabled(std::string alarmType);

			/**
			  * isGeneralAlarmEnabled
			  *
			  * This method returns true if the general alarm is enabled 
			  *
			  * @param The alarm value
			  *
			  * @return true if the Alarm is enabled, false otherwise.
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			bool isGeneralAlarmEnabled(unsigned long value);

			/**
			  * getAnalogueAlarmLimit
			  *
			  * This method gets the alarm limit for the specified alarm
			  *
			  * @return The alarm limits for the specified alarm
			  *
			  * @exception ScadaProxyException raised if not a valid alarm type
			  * @exception ScadaProxyException raised if no alarm limit.
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			double getAnalogueAlarmLimit(std::string alarmType);

            /**
			  * getAlarmAckState
			  *
			  * This method returns the alarm ack state.
			  *
			  * @return Returns the alarm ack state
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			virtual EScadaAlarmAckState getAlarmAckState();

			//Using for IEC104 Protocol
			/**
			* DataPoint::getPScadaFlag()
			*
			* Returns the PScada Flag of this Data Point
			*
			* @return	true indicate this is a data point used in PSCADA.
			*
			*/
			virtual bool getPScadaFlag();

			//Using for IEC104 Protocol
			/**
			* DataPoint::getRelatedInputDataPoint()
			*
			* Returns the Related input data point key for this Data Point
			*
			*/
			virtual unsigned long getRelatedInputDataPoint();

            // Using for WuFang IEC104
			/**
			* DataPoint::getRelatedWfDataPoint()
			*
			* Returns the Related WuFang data point key for this Data Point
			*
			*/
			virtual unsigned long getRelatedWfDataPoint();
            
            
			/**
			* getEventMessage
			*
			* Returns the Event Message of the DataPoint entity
			*
			* @return the Event Message of the DataPoint entity as a std::string
			*/
			virtual std::string getEventMessage();
		protected:

			//
			// Functions used to update the state of the datapoint when notifications received
			//

			void			updateAgentNotAvailableStatus();
			void			updateInternalState(const TA_Base_Bus::DpCompleteDynamicSyncState& internalState);
			void			updateValueStatus(const TA_Base_Bus::DataPointValue& valueState);
            void			updateAlarmStatus(const TA_Base_Bus::EDataPointAlarms& alarmState);
            void            updateAlarmAckStatus(const TA_Base_Bus::EScadaAlarmAckState& alarmAckState);
            void			updateAlarmProperties(const TA_Base_Bus::AlarmPropertyVect& alarmProperties);
            void			updateInhibitedStatus(const TA_Base_Bus::InhibitState& inhibitedState);
			void			updateControlStatus(const TA_Base_Bus::DataPointControl& controlState);
			void			loadConfiguration();
			void			updateLastOperatorInfo(const TA_Base_Bus::LastOperatorInfo& operatorInfo);
	
			//
			// Utility functions
			//
			EDataPointType	dpTypeToScadaDpType(EDataPointType dpType);
			EDataPointType	stringToScadaDpType(std::string typeStr);
            EDataPointAlarms stringToDpAlarmType(std::string& typeStr);

            EDataPointDataType getDpDataTypeFromString(std::string dataTypeStr);
            EDataPointType getDpTypeFromString(std::string typeStr);
            
			void			localSetInputInhibitStatus(EInputInhibitState inputInhibit, EInputInhibitState originalInhibitState, std::string sessionId = "");

            // weimin: to initialise and access datapoint access object
            IDataPointAccess*   getObjAccess();

        private: 
		
			TA_Base_Core::DataPointEntityDataPtr   m_entityData;
            
            // weimin: for sending control or accessing unsynchronised info
            IDataPointAccess*                                    m_objAccess;

			// initialisation state - will be set true once initial
			//                        synchronisation data is received.
			volatile bool									m_dataPointInitialised;

			// Exclusive update lock
			mutable TA_Base_Core::ReEntrantThreadLockable	m_updateLock;

			// Provides access to data point objects used to perform 
			// write operations
			DataPointSyncManager&							m_syncManager;

             struct DataPointProxyData
             {
                bool isConfigValid;
                bool isSynchronized;

                DpCompleteDynamicSyncState      dynamicData;
                DatapointCompleteStaticConfig   configData;
             } m_proxyData;

	};
}

#endif // #ifndef DATAPOINTPROXY_H
