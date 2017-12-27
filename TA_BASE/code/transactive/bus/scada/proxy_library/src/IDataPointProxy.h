#ifndef IDataPointProxy_H
#define IDataPointProxy_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/IDataPointProxy.h $
  * @author:  Darren Sampson
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/12/30 18:22:55 $
  *
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

#include "core/naming/src/INamedObject.h"    // CorbaName definition
#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Bus
{
	class DpValue;

	class IDataPointProxy
	{
		public:


			/**
			  * getEntityKey
			  *
			  * This method gets the entity key of the data point.
			  * It is populated during construction so will always
			  * be available.
			  *
			  * @return The entity key of the data point
			  */
			virtual unsigned long	getEntityKey() const = 0;
            virtual unsigned long	getSubSystemKey() const = 0;
            virtual unsigned long	getPhysicalSubSystemKey() const = 0;
			virtual unsigned long	getParentKey() const = 0;
			virtual unsigned long	getLocationKey() const = 0;
			const virtual std::string&	getDescription() const = 0;
			virtual std::string 	getAgentName() const = 0;
            virtual std::string 	getEntityName() const = 0;
            


			// 
			// Accessors for Data Point Value State
			//

			virtual bool			isAlarmPropertiesValid() const = 0; //wenching++


			virtual bool			isConfigValid() const = 0;
			/**
			  * getStatus
			  *
			  * This method gets the quality status of the Data point.
			  *
			  * @return The quality status of the data point
			  */
			virtual unsigned long	getStatus() =0;

			/**
			  * getQualityAsString
			  *
			  * This method gets the quality status of the Data point as a string.
			  *
			  * @return The quality status of the data point as a string
			  *
			  */
			virtual const char* const getQualityAsString() const = 0;

			/**
			  * isBoolean
			  *
			  * This method returns true if the data point type is boolean.
			  *
			  * @return Returns true if the datapoint type is boolean, false otherwise
			  */
			virtual bool			isBoolean()= 0;

			/**
			  * isNumber
			  *
			  * This method returns true if the data point type is numeric.
			  *
			  * @return Returns true if the datapoint type is numeric, false otherwise
			  */
			virtual bool			isNumber() = 0;

			/**
			  * isText
			  *
			  * This method returns true if the data point type is text.
			  *
			  * @return Returns true if the datapoint type is text, false otherwise
			  */
			virtual bool			isText()= 0;

			/**
			  * isDerived
			  *
			  * This method returns true if the data point type is derived.
			  *
			  * @return Returns true if the datapoint type is derived, false otherwise
			  */
			virtual bool			isDerived()= 0;

			/**
			  * getValue
			  *
			  * This method gets the current value of the Data point.
			  * If the data point is overriden, this will be the override value,
			  * otherwise, this will be the field value.
			  *
			  * @return The value of the data point
			  */
			virtual DpValue	getValue() = 0;

			/**
			  * getValueAsString
			  *
			  * This method gets the current value of the Data point.
			  * If the data point is overriden, this will be the override value,
			  * otherwise, this will be the field value.
			  *
			  * @return The value of the data point
			  */
			virtual std::string		getValueAsString() = 0;

			/**
			  * getValueAsBool
			  *
			  * This method gets the current value of the Data point.
			  * If the data point is overriden, this will be the override value,
			  * otherwise, this will be the field value.
			  *
			  * @return The value of the data point
			  */
			virtual bool			getValueAsBool() = 0;

			/**
			  * getValueAsDouble
			  *
			  * This method gets the current value of the Data point.
			  * If the data point is overriden, this will be the override value,
			  * otherwise, this will be the field value.
			  *
			  * @return The value of the data point
			  */
			virtual double			getValueAsDouble() = 0;

			/**
			  * getTimestamp
			  *
			  * This method gets the current timestamp (in secs since the start of UTC epoch)
			  * of the Data point.
			  *
			  * @return The timestamp of the data point as a long integer
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  *
			  */
			virtual long			getTimestamp() = 0;

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
			virtual const timeb & getTimestampStructure() = 0;

			/**
			  * getLastStatusChangeSessionId
			  *
			  * This method returns the full name of the operator who
			  * last performed any status change on a datapoint
			  *
			  * @return Full name of the operator
			  */
			virtual std::string getLastStatusChangeSessionId() = 0;

			/**
			  * getLastStatusChangeTimestamp
			  *
			  * This method returns the timestamp when a datapoint's status
			  * has been last modified.
			  *
			  * @return timestamp 
			  */
			virtual unsigned long getLastStatusChangeTimestamp() = 0;

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
			virtual std::string getLaunchingConditionResultString() = 0;

			/**
			  * getFieldValue
			  *
			  * This method gets the current field value of the Data point.
			  * If the data point is not overriden, this will return an empty string
			  *
			  * @return The field value of the data point if it is overriden, empty string otherwise
			  */
			virtual std::string		getFieldValue() = 0;

			/**
			  * setValue
			  *
			  * This method sets the value of the Data point.
			  * This method is valid for control data points and for overriding information data points.
			  *
			  * @param The value to set
			  *
			  * @exception ScadaProxyException raised if setting the data point fails.
			  */
			virtual void			setValue(std::string value, std::string sessionId = "", unsigned long triggeringEntityKey = 0, char * triggeringEntityValue = NULL, bool auditEventLoggingEnabled = true) = 0; //TD15649

			/**
			* setControlValue
			*
			* This method sets the value of the Data point.
			* This method is valid for control data points.
			*
			* @param The value to set
			*
			* @exception ScadaProxyException raised if setting the data point fails.
			*/
			virtual void			setControlValue(std::string value, std::string sessionId = "",EControlOperation controlOperation = CONTROL_NO_OPERATION,  unsigned long triggeringEntityKey = 0, char * triggeringEntityValue = NULL, bool auditEventLoggingEnabled = true) = 0; 

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
			virtual void			setBoolValue(bool value, std::string sessionId) = 0;

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
			virtual void			setAnalogueValue(double value, std::string sessionId) = 0;

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
			virtual void			setMeteredCorrectionValue(double adjustment, std::string sessionId) = 0;

			/**
			  * isOverrideEnabled
			  *
			  * This method returns true if the data point override is enabled.
			  *
			  * @return Returns true if the data point override is enabled, false otherwise
			  */
			virtual bool			isOverrideEnabled() = 0;

			/**
			  * setOverrideEnableStatus
			  *
			  * This method sets the override enable status of the data point.
			  * This method is valid for information data points.
			  *
			  * @param The status to set
			  *
			  * @exception ScadaProxyException raised if setting fails.
			  */
			virtual void			setOverrideEnableStatus(bool enable, std::string sessionId) = 0;

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
			virtual void			setManuallyOverrideValue ( const std::string & value, const std::string & sessionId ) = 0;

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
			virtual void setAlarmProperties ( const AlarmProperty & newAlarmProperty, const std::string & sessionId ) = 0;

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
			virtual bool			isControlInhibited() = 0;

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
			virtual void			setOutputInhibitStatus(EOutputInhibitState inhibit, std::string sessionId) = 0;

			/**
			  * isScanInhibited
			  *
			  * This method returns true if data point scanning is inhibited.
			  *
			  * @return Returns true if data point scanning is inhibited, false otherwise
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			virtual bool			isScanInhibited() = 0;

			/**
			  * isAlarmInhibited
			  *
			  * This method returns true if data point alarms are inhibited.
			  *
			  * @return Returns true if data point alarms are inhibited, false otherwise
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			virtual bool			isAlarmInhibited() = 0;

			/**
			  * isMmsAlarmInhibited
			  *
			  * This method returns true if data point mms alarms are inhibited.
			  *
			  * @return Returns true if data point mms alarms are inhibited, false otherwise
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			virtual bool			isMmsAlarmInhibited() = 0;

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
			virtual void			setInputInhibitStatus(EInputInhibitState inhibit, std::string sessionId) = 0;

			//
			// Accessors for Data point Control State
			//

			/**
			  * getControlState
			  *
			  * This method gets the current control state of the Data point.
			  *
			  * @return The control state of the data point
			  */
			virtual EDataPointControlState getControlState() = 0;

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
			virtual unsigned long getControlTriggeringEntity() = 0;

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
			virtual std::string getControlTriggeringValue() = 0;

			//
			// Accessors for Data point configuration
			//

			/**
			  * isInActiveAlarmState
			  *
			  * This method returns true if data point is writeable.
			  *
			  * @return Returns true if data point is writeable, false otherwise
			  */
			virtual bool			isWriteable() = 0;

			/**
			  * getType
			  *
			  * This method gets the type of the data point
			  *
			  * @return The type of the data point
			  */
			virtual EDataPointType	getType() = 0;

			/**
			  * getTypeAsString
			  *
			  * This method gets the type of the data point as a string.
			  *
			  * @return The type of the data point as a string
			  */
			virtual std::string		getTypeAsString() = 0;

			/**
			 *
			 * getShortNameOfType
			 *
			 * This method gets the short name of the type of datapoint (eg. DI, AI, DDI)
			 *
			 * @return The short name of the type of datapoint
			 */
			virtual std::string getShortNameOfType() = 0;

			/**
			  * getDisplayOrder
			  *
			  * This method gets the display order number of the data point.
			  * A negative number indicates that the data point should not be displayed
			  *
			  * @return The display order number of the data point
			  */
			virtual int		getDisplayOrder() = 0;


			/**
			  * getUnit
			  *
			  * This method gets the units of the data point
			  *
			  * @return The units of the data point
			  */
			virtual std::string		getUnit() = 0;

			/**
			  * getLimit
			  *
			  * This method gets a limit for the data point
			  *
			  * @param The limit to retrieve - true retrieves the low limit, false the high limit
			  *
			  * @return A limit for the data point
			  */
			virtual double			getLimit(bool lowLimit) = 0;
			
			/**
			  * getBooleanLabel
			  *
			  * This method gets a boolean for the data point
			  *
			  * @param The label to retrieve - true retrieves the true label, false the false label
			  *
			  * @return A boolean label for the data point
			  */
			virtual std::string		getBooleanLabel(bool bTrue) = 0;

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
			virtual DpvEnumLabelsMap getDerivedStatesAsMap() = 0;

			/**
			  * getDerivedStateAsString
			  *
			  * This method gets a derived state of the data point as a string
			  *
			  * @param The state to retrieve
			  *
			  * @return A derived state as specified by the parameter as a string
			  */
			virtual std::string		getDerivedStateAsString(unsigned long stateValue) = 0;

			/**
			  * getDerivedStateAsUnsignedLong
			  *
			  * This method gets a derived state of the data point as an unsigned long
			  *
			  * @param The state to retrieve
			  *
			  * @return A derived state as specified by the parameter as a double
			  */
			virtual unsigned long	getDerivedStateAsUnsignedLong(std::string stateValue) = 0;
			
			/**
			  * getAlarmProperties
			  *
			  * This method returns the alarm properties for the datapoint 
			  *
			  * @return a vector of alarm properties
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			virtual AlarmPropertyVect getAlarmProperties() = 0;

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
			virtual bool isAlarmEnabled(std::string alarmType) = 0;

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
			virtual bool isGeneralAlarmEnabled(unsigned long value) = 0;

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
			virtual bool			isInAlarmState() = 0;

			/**
			  * getAlarmStateShortString
			  *
			  * This method returns the short string representing the alarm state.
			  *
			  * @return Returns the short string representing the alarm state
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			virtual std::string		getAlarmStateShortString() = 0;

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
			virtual double getAnalogueAlarmLimit(std::string alarmType) = 0;

			/**
			  * getAlarmAckState
			  *
			  * This method returns the alarm ack state.
			  *
			  * @return Returns the alarm ack state
			  *
			  * @exception ScadaProxyException raised if the proxy is not initialised.
			  */
			virtual EScadaAlarmAckState getAlarmAckState() = 0;

			//Using for IEC104 Protocol
			/**
			* DataPoint::getPScadaFlag()
			*
			* Returns the PScada Flag of this Data Point
			*
			* @return	true indicate this is a data point used in PSCADA.
			*
			*/
			virtual bool getPScadaFlag()	= 0;			
			
			//Using for IEC104 Protocol
			/**
			* DataPoint::getRelatedInputDataPoint()
			*
			* Returns the Related input data point key for this Data Point
			*
			*/
			virtual unsigned long getRelatedInputDataPoint() = 0;
            
            //Using for WuFang IEC104
			/**
			* DataPoint::getRelatedWfDataPoint()
			*
			* Returns the Related WuFang data point key for this Data Point
			*
			*/
			virtual unsigned long getRelatedWfDataPoint() = 0;

			/**
			* getEventMessage
			*
			* Returns the Event Message of the DataPoint entity
			*
			* @return the Event Message of the DataPoint entity as a std::string
			*/
			virtual std::string getEventMessage() = 0;
		private:

			// Private default constructor
			//IDataPointProxy() {};

			// Protected Destructor - must be destroyed using the factory
			//virtual ~IDataPointProxy() {};

	};
}

#endif // #ifndef IDataPointProxyWithoutAlarmAckStatusWITHOUTALARMACKSTATUS_H
