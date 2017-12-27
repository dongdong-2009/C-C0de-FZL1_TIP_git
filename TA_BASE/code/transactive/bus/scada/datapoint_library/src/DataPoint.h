/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPoint.h $
  * @author  J. Welton
  * @version $Revision: #7 $
  * Last modification : $DateTime: 2017/09/08 09:54:53 $
  * Last modified by : $Author: wang.wang $
  *
  *
  *	DataPoint is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * DataPoint also realises the DataPointCorbaDef interface,
  * exposing the DataPoint's configuration to remote
  * clients.
  *
  *
  * Modified: Daniel Hoey : 11/11/02 : Added isVirtual() method
  */

#ifndef DATAPOINT_H
#define DATAPOINT_H

#include <sys/timeb.h>
#include <iostream>
#include <string>
#include <vector>

#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/datapoint_library/src/DataPointState.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"
#include "bus/scada/datapoint_library/src/PersistQueue.h"
#include "bus/scada/datapoint_library/src/SingletonAnswerBackErrorUtil.h"
#include "bus/scada/datapoint_library/src/IAnswerBackErrorNotification.h"
#include "bus/scada/datapoint_library/src/IExpressionNodeNotification.h"
#include "bus/scada/datapoint_library/src/CommandProcessor.h"
#include "bus/scada/common_library/src/MmsScadaUtility.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/src/NameValuePair.h"

#include "core/threads/src/IWorkItem.h"

#include "bus/scada/datapoint_library/src/LFThreadPoolSingleton.h"

namespace TA_Base_Core
{
    // forward declaration
    class ScadaRootEntityData;
	class AuditMessageSender;
	class MessageType;
	class DataPointParameterValuePair;
    class AlarmHelper;              
    class AlarmModificationHelper;      
	class ThreadPoolSingletonManager;
}

namespace TA_Base_Bus
{
    // forward declaration
	class ScadaUtilities;
	class DataPointFactory;
	class DataPointState;
	class DataPointAlarmDetails;
	class CalculatedExpression;
	class EventDetailsFactory;
	class LaunchingConditionCheck;
	class IDataPointObserverNotification;

	const std::string NotLoaded ( "StringNotLoaded" );

	class DataPoint : public TA_Base_Bus::IEntity,
					  public IAnswerBackErrorNotification,
					  public IExpressionNodeNotification,
					  public TA_Base_Core::IThreadPoolExecuteItem,
					  public TA_Base_Core::IWorkItem
	{
		//
		// These classes modify the DataPoint's configuration
		// and need access to it's protected functions
		//
		friend class DataPointFactory;
        friend class CommandProcessor;
		friend class DataPointAlarmDetails;

	public:


		/**
  		 * DataPoint::DataPoint()
  		 *
  		 * DataPoint, initialising all basic configuration items, activate its
		 * corba servant and register itself to naming service.
		 *
		 * @param	dataPointEntityAccess	Reference to db entity access
		 * @param	dpType					The type of the data point
		 * @param	dpDataType				The type of value being represented
		 * 									by the DataPoint
         * @param   persistence             Interface for reading/writing persisted
         *                                  state values
		 *
  		 */
		DataPoint( TA_Base_Core::DataPointEntityDataPtr						dataPointEntityAccess,
				   TA_Base_Bus::EDataPointType			                    dpType,
				   TA_Base_Bus::EDataPointDataType		                    dpDataType,
                   ScadaPersistencePtr                                      persistence,
                   TA_Base_Core::ScadaRootEntityData&                       scadaRootEntityData,    // Raymond Pau++ TD 13367
                   MmsScadaUtility & mmsScadaUtility,
                   CommandProcessor& commandProcessor);  


		virtual ~DataPoint();


		//
		// Operations specific to IWorkItemCallback
		//


		virtual void executeWorkItem();

		
		//
		// operations required by IEntity
		//

		/**
		  * isValid
		  *
		  * This is a pure vitual method that must be implemented by
		  * all enitity classes. It will return true if the entity is
		  * valid. Otherwise it will return false.
		  *
		  * @return TRUE if the entity is valid, FALSE otherwise
		  */
		virtual bool isValid();

		/**
		  * start
		  *
		  * This is a pure vitual method that must be implemented by
		  * all enitity classes. It will start any object specific tasks,
		  * such as creating its own thread.
		  */
		virtual void start();

		/**
		  * stop
		  *
		  * This is a pure vitual method that must be implemented by all
		  * enitity classes. It will stop any object specific tasks, such
		  * as stopping its own thread.
		  */
		virtual void stop();

		/**
		  * update
		  *
		  * This method is called by the agent controlling the entity to ask
		  * the entity to update its configuration. Called after an update
		  * configuration event is received.
		  */
		virtual void update ( const TA_Base_Core::ConfigUpdateDetails& updateEvent );

		/**
		  * remove
		  *
		  * When a configuration update event is received to delete an entity
		  * from an agent, the agent can call this method to tell the entity
		  * to commence any clean up opperations.
		  *
		  * The agent then still has to remove the object from its vector of entities.
		  */
		virtual void remove();

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
		  * setToStandbyMode
		  *
		  * This method can be called by the agent responsible for the entity in
		  * the event of its mode of operation changing to standby. This enables the
		  * entity to perform tasks such as disconnecting from any field devices it was
		  * communicating with.
		  */
		virtual void setToStandbyMode();


		//
		// Operations required by IAnswerBackErrorNotification interface
		//

		/**
		 * processPostEvaluationStep
		 *
		 * Call back for processing the answer back error checking result
		 *
		 * @param 	answerBackErrorCheckingResult	The result of the AnswerBackError checking mechanism
		 *
		 */
		virtual void processPostEvaluationStep ( bool answerBackErrorCheckingResult );


		/**
		* SetAbeExpressionValue
		*
		* Set abe expression value according to the input
		*
		* @return TRUE if the entity is valid, FALSE otherwise
		*/
		virtual void SetAbeExpressionValue(bool bValue);

		//
		// operations specific to IExpressionNodeNotification
		//

		/**
		 * processExpressionChanges
		 *
		 * Call back when the calculated expression has changed.
		 *
		 */
		virtual void processExpressionChanges();

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
		void syncOutputInhibitState(const char* sessionId, TA_Base_Bus::EOutputInhibitState inhibit);
        //******************************************
        // interface access method for the datapoint
        //******************************************
         /**
	      * getCompleteDynamicUpdate
	      * 
	      * return the complete dynamic update
	      * 
	      * @return virtual boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> 
	      * 
	      */
        virtual boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> getCompleteDynamicUpdate();

        /**
	      * setAlarmProperties
	      * 
	      * set alarm properties
	      * 
	      * @return virtual void 
	      * @param :  const std::string& sessionId
	      * @param : const TA_Base_Bus::AlarmProperty & newAlarmProperties
	      * 
	      * @exception <exceptions> Optional
	      */
         virtual void setAlarmProperties( const std::string& sessionId, const TA_Base_Bus::AlarmProperty & newAlarmProperties) = 0;

	    /**
	      * getFieldValue
	      * 
	      * get field value. the caller must delete the return allocated resource after usage
	      * 
	      * @return virtual DpValue& 
	      * 
	      */
         virtual boost::shared_ptr<TA_Base_Bus::DataPointValue> getFieldValue();
        
	    /**
	      *  getRawValue
	      * 
	      *  get raw value. the caller must delete the return allocated resource after usage
	      * 
	      * @return virtual DataPointValue& 
	      * 
	      */
        virtual boost::shared_ptr<TA_Base_Bus::DataPointValue>  getRawValue();

		/**
	      *  getRawDpValue
	      * 
	      *  get raw value. the caller must delete the return allocated resource after usage
	      * 
	      * @return virtual DpValue& 
	      * 
	      */
        virtual DpValue  getRawDpValue();

	    /**
	      *  WriteDataPointValue
	      * 
	      *  write the datapoint value. 
	      * 
	      * @return virtual void& 
	      * 
	      */
        virtual void WriteDataPointValue( const std::string& sessionId,
								  const TA_Base_Bus::DataPointValue& value,
								  unsigned long triggeringDpKey, const std::string& triggeringDpValue );

	    /**
	      *  WriteDataPointValueWithoutLoggingEvent
	      * 
	      *  write the datapoint value without logging event
	      * 
	      * @return virtual void& 
	      * 
	      */
        virtual void WriteDataPointValueWithoutLoggingEvent( const std::string& sessionId,
								  const TA_Base_Bus::DataPointValue& value,
								  unsigned long triggeringDpKey, const std::string& triggeringDpValue  );

		/**
		 * setForcedStateWithValue(std::string& sessionId, TA_Base_Bus::DataPointValue & value)
		 *
		 * Sets a new DataPointState. Engineering conversion is also
		 * done if necessary. It is the joint responsibility
		 * of the caller and the DataPoint subclass (through the
		 * isValidUpdate function) to ensure that the type of the
		 * value is valid for this DataPoint.
		 *
		 * @param	newState	the new DataPointState
		 *
		 * @return	True if the forced value is set successfully,
		 *			False otherwise
		 *
		 */
		virtual void setForcedStateWithValue(const std::string& sessionId, const TA_Base_Bus::DataPointValue & value);

        /**SetIsForcedState
	      * 
	      * 
	      * set force state without any force value
	      * 
	      * @return void SetIsForcedState 
	      * @param :  std::string& sessionID
	      * @param : bool indication
	      * 
	      */
        void SetIsForcedState ( const std::string& sessionID, bool indication );

	    /**
	      * setInputInhibitState
	      * 
	      * set the input inhibit state
	      * 
	      * @return virtual void 
	      * @param : const std::string& sessionId
	      * @param : TA_Base_Bus::EInputInhibitState service
	      * 
	      */
        virtual void setInputInhibitState(const std::string& sessionId, TA_Base_Bus::EInputInhibitState inhibitstate);

		/**
  		 * setOutputInhibitState
  		 *
  		 * Update the current control inhibited status of the data point with the
		 * specified indication.  Returns true to indicate if the operation is
		 * successfull, false otherwise
		 *
		 * @param indication [IN]	An indication if the control is inhibited, true
		 *							to inhibit the data point's control ability
		 *
  		 */
		virtual void setOutputInhibitState ( const std::string& sessionId, TA_Base_Bus::EOutputInhibitState inhibitstate  );

	    /**
	      * getLastOperatorSession
	      * 
	      * return the last operator string
	      * 
	      * @return 
	      * 
	      */
        virtual const std::string &	getLastOperatorSession();

	    /**
	      * timeb& getLastOperatorTimeStamp
	      * 
	      * return the last operator timestamp
	      * 
	      * @return virtual const 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual const timeb& getLastOperatorTimeStamp();
        
	    /**
	      * setMeteredCorrectionValue 
	      * 
	      *  set the metered correction value
	      * 
	      * @return virtual void 
	      * @param :  const std::string& sessionId
	      * @param : double newValue
	      * 
          **/
        virtual void setMeteredCorrectionValue( const std::string& sessionId, double newValue );

	    /**
	      * * DataPoint::getDisplayState
	      * 
	      * get display state
	      * 
	      * @return virtual TA_Base_Bus::DataPointDisplayState 
	      * 
	      */
        virtual boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> DataPoint::getDisplayState();

		/**
		 * isInAlarm()
		 *
		 * Determines whether a DataPoint is in alarm, for a given alarm type.
		 *
		 * @param dpAlmType		alarm type, as specified by DataPointCorbaDef.idl
		 *
		 * @return true if the alarm type is configured for this DataPoint and
		 * 		   it is currently in alarm state. Otherwise false is returned.
		 *
		 */
	    bool isInAlarm( const TA_Base_Bus::EDataPointAlarms & dpAlmType ) const;
		// Operations specific to DataPoint
		//

        //******************************************
        //"gets" method for the datapoint
        //******************************************

	    /**
	      * getCompleteStaticConfig
	      * 
	      * return the complete static config for the datapoint. it is caller's reposibility to delete the return struct pointer
	      * 
	      * @return DatapointCompleteStaticConfig 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual boost::shared_ptr<DatapointCompleteStaticConfig> getCompleteStaticConfig();

		/**
		 *
		 * getEntityKey
		 *
		 * Get the entity key of the DataPoint (not 'set' method is provided for the dp's entitykey)
		 *
		 */
		unsigned long getEntityKey() const;

		/**
		 * DataPoint::getDataPointName()
		 *
		 * Returns the DataPoint's name. Preferable to using
		 * the DataPointCorbaDef GetName() function which returns
		 * char *.
		 *
		 * @return	the DataPoint's name
		 *
		 */
		virtual const std::string &	getDataPointName() const;

		/**
		 * DataPoint::getDataPointAddress()
		 *
		 * Returns the DataPoint's address. 
		 *
		 * @return	the DataPoint's address configuration
		 *
		 */
		virtual const std::string &	getDataPointAddress() const;

		/**
		 * DataPoint::getDataPointRegisterAddress()
		 *
		 * Returns the DataPoint's Modbus register address.
		 *
		 * @return	the DataPoint's register address
		 *
		 */
		virtual unsigned long getDataPointRegisterAddress() const;


		/**
		 * DataPoint::getDataPointBitAddress()
		 *
		 * Returns the DataPoint's Modbus bit address.
		 *
		 * @return	the DataPoint's bit address
		 *
		 */
		virtual int getDataPointBitAddress() const;


	    /**
	      * getDescription
	      * 
	      * return the description of the datapoint
	      * 
	      * @return std::string 
	      * 
	      */
        virtual const std::string &	getDescription() const;

	    /**
	      * getViewAssociation
	      * 
	      * return the association of the datapoint
	      * 
	      * @return 
	      * 
	      */
        virtual const std::string &	getViewAssociation() const;


	    /**
	      * getEngineeringNote
	      * 
	      * return the association of the datapoint
	      * 
	      * @return 
	      * 
	      */
        virtual const std::string &	getEngineeringNote() const;
        
	    /**
	      * getHelpURL
	      * 
	      * return the help URL of the datapoint
	      * 
	      * @return 
	      * 
	      */
        virtual const std::string &	getHelpURL() const;
        
	    /**
	      * getLaunchingConditionResultString
	      * 
	      * return the lcc result string
	      * 
	      * @return 
	      * 
	      */
        virtual const std::string &	getLaunchingConditionResultString();

		/**
		 * getOMFlag()
		 *
		 * Returns the string indicating if the data point is operational, maintenance
		 * or both or none.
		 *
		 */
		virtual const std::string & getOMFlag() const;

		/**
		 * getAssetName()
		 *
		 * Returns the asset name of this data point
		 *
		 */
		virtual const std::string & getAssetName();

		/**
		 * DataPoint::getAlarmState()
		 *
		 * Determines whether a DataPoint is in alarm state and in which alarm type.
		 *
		 * @return TA_Base_Bus::DataPointAlarmState.
		 *
		 */
		virtual TA_Base_Bus::EDataPointAlarms getAlarmState() const;

	    /**
	      * std::string & getAlarmStateString
	      * 
	      * return the alarm state string
	      * 
	      * @return virtual const 
	      * 
	      */
		virtual const std::string & getAlarmStateString() const;
        
	    /**
	      * getScanFactor
	      * 
	      * return the scan factor
	      * 
	      * @return virtual int 
	      * 
	      */
        virtual int getScanFactor() const;

	    /**
	      * getDisplayOrder
	      * 
	      * return the displayorder
	      * 
	      * @return virtual long 
	      * 
	      */
        virtual long getDisplayOrder() const;

	    /**
	      * getPrecision
	      * 
	      * return the precision
	      * 
	      * @return virtual long 
	      * 
	      */
        virtual long getPrecision() const;

	    /**
	      * getUpdatePriorityLevel
	      * 
	      * return the update priority level
	      * 
	      * @return virtual long 
	      * 
	      */
		virtual long getUpdatePriorityLevel() const;

		/**
  		 * DataPoint::getTypeKey()
  		 *
  		 * Get the type key of the Data Point
  		 *
		 * @return the type key of the DataPoint
  		 */
		virtual unsigned long getTypeKey() const;

		/**
  		 * DataPoint::getSubsystemKey()
  		 *
  		 * Get the Sub System Key of the Data Point
  		 *
		 * @return the sub system key of the DataPoint
  		 */
		virtual unsigned long getSubsystemKey() const;

		/**
  		 * DataPoint::getPhysicalSubsystemKey()
  		 *
  		 * Get the physical subsystem Key of the Data Point
  		 *
		 * @return the physical subsystem key of the DataPoint
  		 */
		virtual unsigned long getPhysicalSubsystemKey() const;

		/**
  		 * DataPoint::getLocationKey()
  		 *
  		 * Get the Region Key of the Data Point
  		 *
		 * @return the region key of the DataPoint
  		 */
		virtual unsigned long getLocationKey() const;

		/**
		* DataPoint::getLocationName()
		*
		* Get the Region Key of the Data Point
		*
		* @return the region key of the DataPoint
		*/
		virtual const std::string & getLocationName() const;

        /**
		 *
		 * getParentEntityKey
		 *
		 * Get the entity key of the parent of the DataPoint
		 *
		 */
		unsigned long getParentEntityKey() const;

	    /**
	      * isWriteable
	      * 
	      * return whether the datapoint is writable
	      * 
	      * @return virtual bool 
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual bool isWriteable() const;

		/**
		 * DataPoint::isInForcedState()
		 *
		 * @return 	whether the DataPoint's value has been forced to a
		 * manually entered value or not
		 *
		 */
		virtual bool isInForcedState() const;        

	    /**
	      * getControlInhibitedState
	      * 
	      * return control inhibit state
	      * 
	      * @return virtual bool 
	      * 
	      */
        virtual bool getControlInhibitedState() const;

	    /**
	      * getAlarmInhibitedState
	      * 
	      * return alarm inhibit state
	      * 
	      * @return virtual bool 
	      * 
	      */
        virtual bool getAlarmInhibitedState() const;

	    /**
	      * getMMSAlarmInhibitedState
	      * 
	      * return mms alarm inhibit state
	      * 
	      * @return virtual bool 
	      * 
	      */
        virtual bool getMMSAlarmInhibitedState() const;

	    /**
	      * getTrending
	      * 
	      * get the trending flag
	      * 
	      * @return virtual bool 
	      * 
	      */
        virtual bool getTrending() const;

		/**
  		 * DataPoint::isEngUnitsEnabled()
  		 *
  		 * Returns whether or not engineering units are configured
		 * for the DataPoint.
		 *
		 * @return	whether eng units are configured or not
  		 *
  		 */
		virtual bool isEngUnitsEnabled() const;

	   /**
		 * DataPoint::isVirtual()
		 *
		 * Determines whether this DataPoint is virtual. Virtual DataPoints are
		 * not associated with a register on a PLC, their value is stored in the
		 * agent's memory space.
		 *
		 * @return true if the DataPoint is virtual, false otherwise.
		 *
		 */
		virtual bool isVirtual() const;

	    /**
	      * getLogChanges
	      * 
	      * return log changes
	      * 
	      * @return virtual bool 
	      * 
	      */
		virtual bool getLogChanges() const;

	    /**
	      * getAnswerBackErrorResult
	      * 
	      * return answer back error result
	      * 
	      * @return virtual bool 
	      * 
	      */
		virtual bool getAnswerBackErrorResult() const;

	    /**
	      * getWordSwapIndication
	      * 
	      * return word swap indication
	      * 
	      * @return virtual bool 
	      * 
	      */
		virtual bool getWordSwapIndication() const; // fixes for TD# 3603


	    /**
	      * getByteSwapIndication
	      * 
	      * return byte swap indication
	      * 
	      * @return virtual bool 
	      * 
	      */
		virtual bool getByteSwapIndication() const;

		/**
		* getByteSwapIndication
		* 
		* return bit width indication
		* 
		* @return virtual long 
		* 
		*/
		virtual long DataPoint::getBitWidth() const;


		/**
	      * getIsBigEndian 
	      * 
	      *  return the bitsequence of the datapoint
	      * 
	     */
		virtual bool getIsBigEndian() const;

	    /**
	      * DpvEnumLabelsMap& getEnumLabelsMap
	      * 
	      * return the Enum labels map
	      * 
	      * @return virtual const 
	      * 
	      */
		virtual const DpvEnumLabelsMap& getEnumLabelsMap() const;

	    /**
	      * DpvBooleanLabels& getBooleanLabels
	      * 
	      * return the boolean label map
	      * 
	      * @return virtual const 
	      * 
	      */
		virtual const DpvBooleanLabels& getBooleanLabels() const;

		/**
		 * DataPoint::getCurrentValue()
		 *
		 * Returns the DataPoint's current value as a DataPointValue
		 * object. It is the responsibility of the caller to
		 * interpret the type of the DataPointValue (using the get*
		 * functions) correctly.
		 *
		 * @return	the DataPoint's current value
		 *
		 */
		virtual DpValue & getCurrentValue();


		/**
		*DataPoint::getCurrentFieldValue()
		* Returns the DataPoint's current Field value as a DataPointValue
		* object. It is the responsibility of the caller to
		* interpret the type of the DataPointValue (using the get*
		* functions) correctly.
		*@return	the DataPoint's current Field Value
		*/
		virtual DpValue & getCurrentFieldValue();


		/**
		 * DataPoint::getCurrentEngValue()
		 *
		 * Returns the current value, converted into engineering
		 * units. If engineering units are not enabled, the raw value
		 * is returned.
		 *
		 * @return the engineered value as a DataPointValue object
		 *
		 */
		virtual DpValue & getCurrentEngValue();


		/**
		 * DataPoint::getTimestamp()
		 *
		 * Returns the time (in secs since the start of UTC epoch) of the last detected state change.
		 *
		 * @return	time of the last state change, as an unsigned long number
		 *
		 */
		virtual unsigned long getTimestamp() const;


		//Using for IEC104 Protocol
		/**
		* DataPoint::getPScadaFlag()
		*
		* Returns the PScada Flag of this Data Point
		*
		* @return	true indicate this is a data point used in PSCADA.
		*
		*/
		virtual bool getPScadaFlag() const;

		/**
		 * DataPoint::getTimebStructure()
		 *
		 * Returns the time (in secs, including miliseconds, since the start of UTC epoch) of the last detected state change.
		 *
		 * @return	time of the last state change, as timeb structure.
		 *
		 */
		virtual const timeb & getTimebStructure() const;

		/**
		 * DataPoint::getQualityStatus()
		 *
		 * Returns the current quality status of the data point value
		 *
		 *
		 */
		virtual TA_Base_Bus::EQualityStatus getQualityStatus() const;

		/**
		 * DataPoint::getStatus()
		 *
		 * Returns the DataPoint's status, which is a summary of
		 * it's current GoodValue and ServiceState.
		 *
		 * @return 	the DataPoint's status as a DataPointStatus enum
		 *
		 */
		virtual TA_Base_Bus::EQualityStatus	getStatus() const;

		/*
		* DataPoint::getFieldValueQualityStatus()
		* return the datapoint's fieldValue status.
		*/
		virtual TA_Base_Bus::EQualityStatus getFieldValueQualityStatus() const;

		/**
  		 * DataPoint::getDataPointType()
  		 *
  		 * Returns the DataPoint's type. Preferable to the
		 * DataPointCorbaDef implemented GetType() function.
		 *
		 * @return	the DataPoint's type, as an enum
  		 *
  		 */
		virtual TA_Base_Bus::EDataPointType getDataPointType() const;


		/**
  		 * DataPoint::getDataPointDataType()
  		 *
  		 * Returns the DataPoint's data type. Preferable to the
		 * DataPointCorbaDef implemented GetDataType() function.
		 *
		 * @return	the DataPoint's type, as an enum
  		 *
  		 */
		virtual TA_Base_Bus::EDataPointDataType getDataPointDataType() const;

        /**
		 * DataPoint::getServiceState()
		 *
		 * Returns the current service state of the DataPoint
		 *
		 * @return	the DataPoint's service state, as a
		 *			DataPointServiceState enum
		 *
		 */
        virtual TA_Base_Bus::EDataPointServiceState getServiceState() const;

	    /**
	      * isDataPointInService
	      * 
	      * return whether the datapoint is in service
	      * 
	      * @return virtual bool 
	      * 
	      */
	    virtual bool isDataPointInService() const;

		/** 
  		 * getControlState
  		 *
  		 * for DerivedState to get the control state from its parent datapoint.
		 *
  		 */
        virtual TA_Base_Bus::DataPointControl& getControlState();

		/**
		 * DataPoint::getIsDigitalDataPoint()
		 *
		 * Returns true if the data point is a digital data point.
		 *
		 * @return	True if this data point is a digital data point
		 *
		 */
		virtual bool getIsDigitalDataPoint() const;


		/**
		 * DataPoint::getIsTextDataPoint()
		 *
		 * Returns true if the data point is a text data point.
		 *
		 * @return	True if this data point is a text data point
		 *
		 */
		virtual bool getIsTextDataPoint() const;

		/**
		 * DataPoint::getIsAnalogueDataPoint()
		 *
		 * Returns true if the data point is an analogue data point.
		 *
		 * @return	True if this data point is an analogue data point
		 *
		 */
		virtual bool getIsAnalogueDataPoint() const;

         /**
  		 * DataPoint::getEngineeringUnits()
  		 *
		 * Modifies a DataPointCorbaDef structure to reflect the DataPoint's
		 * engineering units configuration. To be implemented by subclass.
  		 *
  		 * @param eng	the DataPointCorbaDef structure to be modified
		 *
  		 */
		virtual void getEngineeringUnits(
						TA_Base_Bus::DataPointEngineeringUnits & eng ) = 0;


        //******************************************
        // register/de-register for the notifying datapoint details
        //******************************************
	    /**
	      * registerForUpdateDetails 
	      * 
	      * register for the notifying datapoint details
	      * 
	      * @return virtual bool 
	      * @param : TA_Base_Bus::IDataPointObserverNotification * updateDetailsObserver
	      * 
	      */
		virtual bool registerForUpdateDetails ( TA_Base_Bus::IDataPointObserverNotification * updateDetailsObserver );

	    /**
	      * deregisterForUpdateDetails 
	      * 
	      * de-register for the notifying datapoint details
	      * 
	      * @return virtual bool 
	      * @param : TA_Base_Bus::IDataPointObserverNotification * updateDetailsObserver
	      * 
	      */
		virtual bool deregisterForUpdateDetails ( TA_Base_Bus::IDataPointObserverNotification * updateDetailsObserver );

	    /**
	      * notifyRegisteredObservers 
	      * 
	      * notifyRegistered Observers
	      * 
	      * @return virtual void 
	      * @param : const TA_Base_Bus::DpDynamicSyncUpdate & updateDetails
	      * 
	      */
        virtual void notifyRegisteredObservers ( boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails );
		

        //******************************************
        //"sets" method for the datapoint
        //******************************************

	    /**
	      * setPersistedValueFromDB
	      * 
	      * read the persisted value from database then sets it as the current field value
	      * 
	      * @return virtual void 
	      * @param : const std::string & description
	      * 
	      */
        virtual void setPersistedValueFromDB();


	    /**
	      * setDescription
	      * 
	      * set the description
	      * 
	      * @return virtual void 
	      * @param : const std::string & description
	      * 
	      */
		virtual void setDescription( const std::string & description );
        
	    /**
	      * setViewAssociation
	      * 
	      * set the view association
	      * 
	      * @return virtual void 
	      * @param : const std::string & viewAssociation
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual void setViewAssociation( const std::string & viewAssociation );

	    /**
	      * setEngNote
	      * 
	      * set the engineering note 
	      * 
	      * @return virtual void 
	      * @param : const std::string & engNote
	      * 
	      */
		virtual void setEngNote( const std::string & engNote );

	    /**
	      * setHelpURL
	      * 
	      * set the help URL
	      * 
	      * @return virtual void 
	      * @param : const std::string & helpURL
	      * 
	      */
		virtual void setHelpURL( const std::string & helpURL );

	    /**
	      * setScanFactor
	      * 
	      * set the scan factor
	      * 
	      * @return virtual void 
	      * @param : int scanFactor
	      * 
	      */
		virtual void setScanFactor( int scanFactor );

	    /**
	      * setPrecision
	      * 
	      * set the precision
	      * 
	      * @return void 
	      * @param : long precision
	      * 
	      */
		void setPrecision( long precision );

	    /**
	      * setWritable
	      * 
	      * set writable flag
	      * 
	      * @return virtual void 
	      * @param : bool writeable
	      * 
	      */
		virtual void setWritable( bool writeable );
        
		/** TD15649
  		 * setLaunchingConditionResultString
  		 *
  		 * for DerivedState to set the launching condition Result String to its parent datapoint.
		 *
  		 */
		virtual void setLaunchingConditionResultString(const std::string & newResultString);

	    /**
	      * setTrending
	      * 
	      * set the trending flag
	      * 
	      * @return virtual void 
	      * @param : bool trendingEnabled
	      * 
	      */
		virtual void setTrending( bool trendingEnabled );

		/**
  		 * DataPoint::setEngUnitsEnabled()
  		 *
  		 * Sets the m_engUnitsEnabled member variable to indicate
		 * whether engineering units have been configured or not.
		 *
  		 * @param enabled 		the new setting
		 *
  		 */
		virtual void setEngUnitsEnabled( bool enabled );

		/**
		 * DataPoint::setFieldValue()
		 *
		 * Sets a new DataPointState. Engineering conversion is also
		 * done if necessary. It is the joint responsibility
		 * of the caller and the DataPoint subclass (through the
		 * isValidUpdate function) to ensure that the type of the
		 * value is valid for this DataPoint.
		 *
		 * @param	newState	the new DataPointState
		 *
		 * @return	True if the field value is set successfully,
		 *			False otherwise
		 *
		 */
		virtual bool setFieldValue(const TA_Base_Bus::DataPointState & newState );

		/**
  		 * DataPoint::setControlStatus
  		 *
  		 * Sets the current control status of the data point with the
		 * specified control status.
		 *
		 * @param controlStatus [IN]	The new control status structure of the data point
		 *
  		 */
		virtual void setControlState (const TA_Base_Bus::DataPointControl& controlStatus, bool isRequestedBySystem = false ); //limin++, SystemSession should not access db

	    /**setControlState
	      * 
	      * 
  		 * Sets the current control status of the data point with the
		 * specified control status.
         *
	      * @return void setControlState 
	      * @param : TA_Base_Bus::EDataPointControlState controlStatus
	      * 
	      */
	    virtual void setControlState ( TA_Base_Bus::EDataPointControlState controlStatus, bool isRequestedBySystem = false ); //limin++, SystemSession should not access db

	    /**updateControlState
	      * 
	      * 
	      * update the control state
	      * 
	      * @return void updateControlState 
	      * @param : TA_Base_Bus::DataPointControlState controlStatus
	      * 
	      */
        virtual void updateControlState (const  TA_Base_Bus::EDataPointControlState controlStatus, bool isRequestedBySystem = false ); //limin++, SystemSession should not access db

        /**
  		 * DataPoint::addAssociation()
  		 *
  		 * Associates the DataPoint with another TransActive
		 * entity.
		 *
  		 * @param entName	the name of the entity to associate with the
		 * 					DataPoint
		 *
		 * @param preset	a preset, typically used with CCTV cameras
  		 *
  		 */
		virtual void addAssociation( std::string & entName, std::string & preset );


		/**
		 * DataPoint::clearAssociations()
		 *
		 * Removes all associations with other entities from the DataPoint.
		 *
		 */
		virtual void clearAssociations();

        /**
  		 * startABETimeout
  		 *
  		 * Start ABE timeout clock
		 *
  		 */
		virtual void startABETimeout(const std::string& sessionID);

		/**
		 * updateForcedState()
		 *
		 * Determines whether a DataPointState represents a change for
		 * the DataPoint or not. Also performs engineering unit
		 * conversion and alarm condition testing.  The default operation will
		 * also generate and submit an event for the new state
		 *
		 * @param	newState			The new DataPointState
		 *
		 * @return	True if the forced state is persisted into databse,
		 *			False otherwise
		 *
		 */
		virtual bool updateForcedState( DataPointState & newState );

		/**
		 * updateFieldState()
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
	      * getInhibitState
	      * 
	      * get the inhibitstate
	      * 
	      * @return virtual TA_Base_Bus::InhibitState 
	      * 
	      */
         virtual TA_Base_Bus::InhibitState getInhibitState();

		/**
		 * DataPoint::getMaxTextLength()
		 *
		 * Returns the maximum number of chars that a text data point
		 * can handle.  To be implemented by specific derived class
		 *
		 */
		virtual int getMaxTextLength() const { throw; };

	    /**
	      * DataPointStateUpdate 
	      * 
	      *  this is called when in monitor mode
	      * 
	      * @return virtual void 
	      * @param :  
	      * @param : 
	      * 
	      */
        virtual void syncControlState(DataPointStateUpdate&);

	    /**
	      * isRCCConfigured 
	      * 
	      *  returns if this datapoint is configured with return condition check
          *  should only ve valid for writable datapoints
	      * 
	      * @return virtual bool 
	      * @param :  
	      * @param : 
	      * 
	      */
        virtual bool isRCCConfigured();

		void setSession(std::string strSession);

		std::string getSession();

		/**
		* getEventMessage
		*
		* Returns the Event Message of the DataPoint entity
		*
		* @return the Event Message of the DataPoint entity as a std::string
		*/
		std::string getEventMessage();

		/**
	      * getAlarmMessage 
	      * 
	      *  returns alarm message description of alarmType
	      * 
	      * @return alarm message description
	      * @param alarmType: alarm type
	      * 
	      */
		virtual std::string getAlarmDescription(TA_Base_Bus::EDataPointAlarms alarmType, const DpValue& dpValue);

		/**
	      * getEventDescription 
	      * 
	      *  returns event description of this DataPoint
	      * 
	      * @return event description
	      * 
	      */
		virtual std::string getEventDescription();
		virtual void initAfterProxyFactoryReady();
        
        /**
	      * setControlCommandState 
	      * 
	      *  set control command state of this DataPoint
	      * 
          * @param state: pass EControlCommandState type
	      * @return void
	      * 
          * This method was designed for WF using.
	      */
        virtual void setControlCommandState(EControlCommandState state);
    protected:

		/**
		 * DataPoint::lock()
		 *
		 * Prevents the DataPoint accepting any write requests or changes
		 * to it's service state, until it is unlocked
		 *
		 */
		virtual void lock();


		/**
		 * DataPoint::unlock()
		 *
		 * Allows the DataPoint to accept write requests or changes
		 * to it's service state.
		 *
		 */
		virtual void unlock();

	    /**
	      * setCurrentDataPointValue
	      * 
	      * set current Datapoint value
	      * 
	      * @return void 
	      * @param : virtual boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpvalue
	      * 
	      */
        void setCurrentDataPointValue( TA_Base_Bus::DpCompleteDynamicSyncState& dpvalue);

	    /**
	      * configureControlStatus
	      * 
	      * config the control status
	      * 
	      * @return virtual void 
	      * 
	      */
		virtual void configureControlStatus();

        /**
  		 * isActionPermitted
		 *
  		 * Returns an indication if the specified action is permitted to be executed
		 * by the specified session ID.  True is returned if the specified action is
		 * allowed otherwise false is returned.
		 *
		 * @param sessionID [IN]			the specified session ID
		 * @param actionKey [IN]			the specified action
		 *
  		 */
		virtual bool isActionPermitted ( const std::string & sessionID, unsigned long actionKey );

        /**
	      * setServiceState
	      * 
	      * set the service state. scan inhibit related
	      * 
	      * @return virtual void 
	      * @param :  std::string& sessionId
	      * @param : bool service
	      * 
	      */
		virtual void setServiceState( const std::string& sessionId, bool service );
      
        /**
  		 * DataPoint::updateAlarmInhibitedStatus
  		 *
  		 * Update the current alarm inhibited status of the data point with the
		 * specified indication.  Returns true to indicate if the operation is
		 * successfull, false otherwise
		 *
		 * @param indication [IN]	An indication if the alarm is inhibited, true
		 *							to inhibit the data point's alarm generation
		 *
  		 */
		virtual void updateAlarmInhibitedStatus ( const std::string& sessionId, bool indication );


		/**
  		 * DataPoint::updateMMSAlarmInhibitedStatus
  		 *
  		 * Update the current MMS alarm inhibited status of the data point with the
		 * specified indication.  Returns true to indicate if the operation is
		 * successfull, false otherwise
		 *
		 * @param indication [IN]	An indication if the MMS alarm is inhibited, true
		 *							to inhibit the data point's alarm generation
		 *
  		 */
		virtual void updateMMSAlarmInhibitedStatus ( const std::string& sessionId, bool indication );

        /**
	      * setForcedState
	      * 
	      * set the force sate flag
	      * 
	      * @return void 
	      * @param : bool newState
	      * 
	      */
	    void setForcedState( bool newState );

        /**
	      * setForcedValue
	      * 
	      * set forced value
	      * 
	      * @return bool 
	      * @param : TA_Base_Bus::DataPointState & newState
	      * 
	      * @exception <exceptions> Optional
	      */
	    bool setForcedValue(const DataPointState & newState );

		/**
		 * DataPoint::setDataPointServiceState()
		 *
		 * Modifies the DataPoint's service state. This can affect
		 * the scanning of new PLC values for the DataPoint, as well
		 * as alarm processing.
		 *
		 * @param newState		the new service state
		 *
		 */
        virtual void setDataPointServiceState( TA_Base_Bus::EDataPointServiceState newState );

		/**
  		 * evaluateLaunchCondition
  		 *
  		 * Evaluate the launch condition that is used as interlocking mechanism before
		 * a command is executed.  Evaluation is done through Mathematical_Expression library.
		 *
  		 */
		virtual bool evaluateLaunchCondition();

		/**
		 * DataPoint::isGoodValue()
		 *
		 * Returns an indication of whether the current DataPointValue
		 * can be trusted or not.
		 *
		 * @return	the DataPoint's GoodValue indicator, as a boolean
		 *
		 */
		virtual bool isGoodValue() const;

		/**
  		 * DataPoint::isValidWrite()
  		 *
  		 * Determines whether the requested value is valid for the DataPoint.
		 * This function is called for every write request, before the request
		 * is passed to the PLCWorker class for writing to the PLC.
		 *
		 * @return	whether or not the value is valid for writing
  		 *
  		 * @param 	writeValue	the value that has been requested for writing
		 *
  		 */
		virtual bool isValidWrite( DpValue & writeValue );


		/**
  		 * DataPoint::isValidUpdate()
  		 *
  		 * Determines whether the requested state update is valid or not.
		 * This function is called for every state update.
		 *
		 * @return  whether or not the state is valid for further processing
		 * 			by the DataPoint::UpdateState() function
  		 *
  		 * @param 	updateState		the new state for processing
		 *
  		 */
         virtual bool isValidUpdate( DataPointState & updateState );


	    /**
	      * DataPoint::getAlarmPropertiesVect
	      * 
	      *  get the alarm properties vector
	      * 
	      * @return virtual TA_Base_Bus::AlarmPropertyVect 
	      * 
	      */
         virtual TA_Base_Bus::AlarmPropertyVect getAlarmPropertiesVect() = 0;

         /**
		 * submitAuditMessage()
		 *
		 * Generates a log-only event.
		 *
		 * @param	eventMsg	the message for the event
		 *
		 */
		virtual void submitAuditMessage( const TA_Base_Bus::DataPointControl& controlState,
								 const std::string & sessionID,
								 TA_Base_Core::DescriptionParameters & description,
								 const TA_Base_Core::MessageType & messageType);

		virtual void submitAuditMessage( const DpValue& dpValue,
								 const std::string & sessionID,
								 TA_Base_Core::DescriptionParameters & description,
								 const TA_Base_Core::MessageType & messageType);

		virtual void submitAuditMessage( const std::string & sessionID,
								 TA_Base_Core::DescriptionParameters & description,
								 const TA_Base_Core::MessageType & messageType,
								 unsigned long entityKey = 0);

        /**
  		 * submitUpdate()
  		 *
  		 * Submits a new item to the EventQueue, requesting the
		 * UpdateEventProcessor to generate an event describing
		 * the DataPoint's current state.
		 *
		 * @param type [IN]				The type of changes in data point internal state
		 * @param controlStatus [IN]	The control status of the data point, defaulted
		 *								to TA_Base_Bus::noControl
		 * @param configChanges [IN]	The list of configuration changes applied to
		 *								the data point
		 *
  		 */
		virtual void submitUpdate(  boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> );   // dpValueStatus

		/**
  		 * DataPoint::convertToEngUnits()
  		 *
  		 * Converts a raw value into an engineering value, according to the
		 * DataPoint's configuration. By default, this function copies the
		 * original raw value.
		 *
  		 * @param rawVal	the raw value to be converted
		 *
		 * @param engVal	the resulting engineering value
		 *
  		 */
		virtual void  convertToEngUnits( const DataPointState & rawState,
										 DataPointState & engState );


		/**
  		 * DataPoint::convertToRaw()
  		 *
  		 * Converts an engineering value to a raw value, according to the
		 * DataPoint's configuration. By default, this function copies the
		 * original engineering value.
		 *
  		 * @param engVal	the engineering value to be converted
		 *
		 * @param rawVal	the resultant raw value
  		 *
  		 */
		virtual void  convertToRaw( const DpValue & engVal,
									DpValue & rawVal );

        /**
  		 * DataPoint::convertDataPointAddress()
  		 * Convert the address of the data point to register and bit address.
		 *
		 * @param originDpAddress [IN]			the original address of the data point
		 * @param registerAddress [INOUT]		the register address of the data point
		 * @param bitAddress [INOUT]			the bit address of the data point
		 *
  		 */
		virtual void convertDataPointAddress ( const std::string & originDpAddress,
									  unsigned long & registerAddress,
									  int & bitAddress );

		/**
  		 * DataPoint::updateSpecificConfiguration
		 *
  		 * To be implemented by the specific derived data points
		 *
		 * @param configChanges [IN]		The list of configuration changes
		 *
  		 */
		virtual void updateSpecificConfiguration ( const std::vector< std::string > * configChanges );


		/**
  		 * DataPoint::updateDataPointState
		 *
  		 * Update the data point state, alarm and generat updating event
		 * based on the specified data.  Returns TRUE if successful, FALSE otherwise
		 *
  		 */
		virtual bool updateDataPointState ( DataPointState & newState, bool fieldValue );
        
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
	      * updateEngUnitForAlarms 
	      * 
	      * update EngUnit for alarms, used by the analog alarm datapoint
	      * 
	      * @return virtual void 
	      * @param : const std::string & engUnit
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual void updateEngUnitForAlarms ( const std::string & engUnit );

        /**
	      * saveLastOperatorInfo 
	      * 
	      * save last operator Info
	      * 
	      * @return virtual void 
	      * @param :  const std::string & sessionID
	      * @param : const timeb & timestamp
	      * 
	      */
		virtual void saveLastOperatorInfo ( const std::string & sessionID, const timeb & timestamp );

		/**
  		 * getDataPointValueAsString()
  		 *
  		 * @param dpValue   the DataPointValue to be stringified
         *
		 * Return DataPoint's value as a string. To be implemented by subclass.
  		 *
  		 */
        virtual std::string getDataPointValueAsString( const DpValue & dpValue ) = 0;

		/**
  		 * writeDataPoint()
  		 *
         * update the datapoint value
  		 *
  		 */
        virtual void writeDataPoint( const std::string& sessionId,
							 const TA_Base_Bus::DataPointValue & value,
							 bool auditEventLoggingEnabled = true,
							 unsigned long triggeringDpKey = 0,
							 const std::string triggeringDpValue = "");

        //******************************************
        // alarm related functions
        //******************************************
        /**
  		 * updateAlarms
  		 *
  		 * Passes a new DataPointState to each configured alarm rule
		 * to process for possible alarm conditions. It is the
		 * responsibility of the DataPointAlarmDetails object to
		 * act upon any new alarm condition.
		 *
  		 * @param newState			The new state for the DataPoint
		 *
  		 */
		virtual void updateAlarms( DataPointState & newState );

        /**
	      * reEvaluateAlarmCondition 
	      * 
	      * reevaluate the alarm condition
	      * 
	      * @return virtual void 
	      * 
	      */
		virtual void reEvaluateAlarmCondition ( const TA_Base_Bus::EDataPointAlarms & alarmType,
												bool inAlarm );
        /**
		 * DataPoint::setAlarmDetails()
		 *
		 * Adds or modifies alarm details for the DataPoint. If the alarm type
		 * is not already configured, and all of the other parameters ( with the
		 * exception of value for Boolean TRUE/FALSE alarm type ) are defined,
		 * a new alarm is added to the DataPoint, Otherwise, the non-NULL parameters
		 * are used to modify the existing alarm details.
		 *
		 * @param almType		the alarm type for these details, as defined by
		 *						commondef
		 *
		 * @param almEnabled	pointer to a boolean defining whether the alarm is
		 *						enbled or not. If the pointer is not NULL, the value
		 *						will be used.
		 *
		 * @param almDelay		pointer to an alarm delay value. If the pointer is
		 *						not NULL, the value will be used.
		 *
		 * @param almSeverity	pointer to an alarm severity value. If the pointer
		 *						is not NULL, the value will be used.
		 *
		 * @param almMessage	pointer to an alarm message string. If the pointer
		 *						is not NULL, the string will be used.
		 *
		 * @param almValue		pointer to an alarm value. If the pointer is not
		 *						NULL, the value will be used.
		 *
		 * @param engUnit		the eng label associated with the alarm value
		 *
		 * @param MMSConfiguration		the MMS Configuration associated with the alarm
		 *
		 * @param alarmThresholdConstant	the threshold that the dp value must be below
		 *									in order to in order for the alarm to be normalised.
		 *									this parameter is mainly for Analogue data points.
		 *
		 */
	    void setAlarmDetails( const TA_Base_Bus::EDataPointAlarms & almType,
									     bool almEnabled,
									     int almDelay,
									     int almSeverity,
									     const std::string & almMessage,
									     const DpValue & almValue,
									     const std::string & engUnit,
									     const std::string & MMSConfiguration,
									     double alarmThresholdConstant = 0
									     );

        /**
	      * saveAlarmState 
	      * 
	      * save the alarm condition
	      * 
	      * @return  void 
	      * 
	      */
	    void DataPoint::saveAlarmState ( const TA_Base_Bus::EDataPointAlarms & alarmType,
									 bool inAlarm );

	    /**
	      * ackCloseAlarm
	      * 
	      * ack and close the associated alarm
	      * 
	      * @return void 
	      * @param : const std::string & sessionID
	      * 
	      */
        void ackCloseAlarm( const std::string & sessionID );

	    /**
	      * raiseNonUniqueAlarm 
	      * 
	      * raised non unique alarm
	      * 
	      * @return virtual std::string 
	      * @param :  const TA_Base_Core::MessageType & alarmType
	      * @param : const std::string & alarmMessage
	      * @param : const std::string & dpValue
	      * 
	      */
		virtual std::string raiseNonUniqueAlarm ( const TA_Base_Core::MessageType & alarmType,
                                                  const std::string & alarmMessage,
                                                  const std::string & dpValue );

	    /**
	      * closeNonUniqueAlarm 
	      * 
	      *  close non unique alarm
	      * 
	      * @return virtual void 
	      * @param :  const std::string & alarmID
	      * @param : const TA_Base_Core::MessageType & alarmType
	      * 
	      */
		virtual void closeNonUniqueAlarm ( const std::string & alarmID, const TA_Base_Core::MessageType & alarmType );

        
	    /**
	      * updateAlarmAckState 
	      * 
	      *  receive this call when alarm ack state changed
	      * 
	      * @return virtual void 
	      * @param :  
	      * @param : 
	      * 
	      */
        virtual void updateAlarmAckState();


        //
		// The EntityAssociation class internally represents a
		// (entity, preset) pair associated with this DataPoint
		//
		class EntityAssociation
		{
		public:
			EntityAssociation( std::string & eName, std::string & ePreset )
				: m_entityName( eName ), m_entityPreset( ePreset ) {}

			~EntityAssociation() {}

			const std::string & getEntityName() { return m_entityName; }
			const std::string & getPreset() { return m_entityPreset; }

		private:
			std::string	m_entityName;
			std::string	m_entityPreset;
		};

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
        virtual void updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties) = 0;

		virtual bool pscadaOperationCheck(TA_Base_Bus::EControlOperation operation);

	private:
	    /**
	      * updateSynInhibitState
	      * 
	      * update the internal inhibit state
	      * 
	      * @return void 
	      * @param : TA_Base_Bus::InhibitState updateinhibit
	      * 
	      * @exception <exceptions> Optional
	      */
        void updateSynInhibitState(const TA_Base_Bus::InhibitState& updateinhibit, bool inputOnly = false);

	    /**
	      * updateSynDataPointValue
	      * 
	      * update the internal datapoint value
	      * 
	      * @return void 
	      * @param : TA_Base_Bus::DataPointValue updatevalue
	      * 
	      * @exception <exceptions> Optional
	      */
        void updateSynDataPointValue(const TA_Base_Bus::DataPointValue& updatevalue);


        //
		// Declare the copy constructor as private
		// to prevent pass-by-value of this monolithic object
		//
		DataPoint ( const DataPoint & theInstance );
		DataPoint & operator= ( const DataPoint & rhs );

        void initialiseData();

	private:

		TA_Base_Core::ReEntrantThreadLockable m_lock;
		TA_Base_Core::ReEntrantThreadLockable m_updateDetailsObserverlock;
        TA_Base_Core::ReEntrantThreadLockable m_processExpressionChangeslock;

    protected:
        mutable TA_Base_Core::ReEntrantThreadLockable m_alarmslock;
		typedef std::vector< EntityAssociation * > DpAssociationList;
		typedef std::vector< DataPointAlarmDetails * > DpAlarmList;

        //static member variable
		unsigned long				m_entityKey;            
		std::string 				m_name;   
		unsigned long				m_entityTypeKey;
		unsigned long				m_subsystemKey;
		unsigned long				m_locationKey;
		unsigned long				m_parentKey;
		std::string					m_description;
		std::string					m_locationName;
		std::string					m_viewAssociation;
		std::string					m_engNote;
		std::string					m_helpURL;
		std::string					m_typeShortName;//no set/get method provided, since not necessary
        std::string					m_OMFlag;
		std::string					m_assetName;
		long						m_displayOrder;
		long						m_precision;
		long						m_updatePriorityLevel;
		bool						m_writable;
		int							m_scanTimeFactor;
		bool						m_wordSwap;
        bool                        m_byteSwap;
		long                        m_bitWidth;
		bool						m_bigEndian;
		TA_Base_Bus::EDataPointType                 m_type;
		TA_Base_Bus::EDataPointDataType             m_dataType;

		//Using for IEC104 Protocol 
		bool						m_pscadaFlag;

        //datapoint address related member variable
		std::string					m_address;      
		int							m_bitAddress;
		unsigned long				m_registerAddress;
        //abe expression-related member variable
        std::string					m_abeExpression;//no set/get method provided, since not necessary
		int							m_abeTimeoutPeriodSeconds;//no set/get method provided, since not necessary

		std::string					m_trueRccExpression;//no set/get method provided, since not necessary
		std::string					m_falseRccExpression;//no set/get method provided, since not necessary

        //last operator-related member variable
        std::string					m_lastOperatorSessionID; 
		timeb						m_lastOperatorTimestamp;
        //alarm state related member variable
		TA_Base_Bus::EDataPointAlarms          m_alarmState;
		TA_Base_Bus::EDataPointAlarms          m_previousAlarmState;
        TA_Base_Bus::EScadaAlarmAckState       m_ackState;
		std::string					m_alarmStateString;
		std::string					m_unknownHiHiLimitAlarmID;//no set/get method provided, since not necessary
		std::string					m_unknownHiLimitAlarmID;//no set/get method provided, since not necessary
		std::string					m_unknownLoLoLimitAlarmID;//no set/get method provided, since not necessary
		std::string					m_unknownLoLimitAlarmID;//no set/get method provided, since not necessary
        //dynamic member variables
		std::string					m_commandLaunchingCondition; //no set/get method provided, since not necessary
		std::string					m_calculatedExpression;	//no set/get method provided, since not necessary	
		bool						m_forcedState;
		bool						m_ControlInhibitedState;
		bool						m_AlarmInhibitedState;
		bool						m_MMSAlarmInhibitedState;
		bool						m_trendingEnabled;
		bool						m_engUnitsEnabled;
		bool						m_virtual;
		bool						m_logChanges;	
		volatile bool				m_clockHasStarted; // declare this switch variable as volatile, no set/get method provided, since not necessary
		bool						m_auditEventLoggingEnabled;//no set/get method provided, since not necessary
		bool						m_answerBackErrorCheckingResult;
        bool                        m_ackCloseAlarmOnInhibit;   //no set/get method provided, since not necessary
        bool                        m_ackAnalogAlarmOnTransition;   // no set/get method provided, since not necessary
        TA_Base_Bus::EInputInhibitState m_previousInputInhibitState;
        //datapoint force value
		DpValue						m_forcedValue;
		DpValue						m_forcedEngValue;
		TA_Base_Bus::EQualityStatus	m_forcedValueQualityStatus;
		timeb						m_forcedValueTimestamp;
        //datapoint field value
		DpValue						m_fieldValue;
		DpValue						m_fieldEngValue;
		TA_Base_Bus::EQualityStatus	m_fieldValueQualityStatus;
		TA_Base_Bus::EQualityStatus	m_previousFieldValueQualityStatus;
		timeb						m_fieldValueTimestamp;
        //Label maps with the datapoint
		DpvEnumLabelsMap			m_enumLabelsMap;
		DpvBooleanLabels			m_booleanLabels;
         //last known field state, no set/get method provided, since not necessary
		TA_Base_Bus::DataPointState				m_lastKnownFieldState; //Wenching, store the latest state changes (to be used when unscan inhibit)

		DpAssociationList			                m_associations;//no set/get method provided, since not necessary
        TA_Base_Bus::EDataPointServiceState 		m_serviceState;
		DpAlarmList					m_alarms;//no set/get method provided, since not necessary

		TA_Base_Bus::DataPointControl m_controlState;
		TA_Base_Core::EOperationMode m_operationMode;
		TA_Base_Core::EOperationMode m_lastSavedOperationMode;

		TA_Base_Bus::LaunchingConditionCheck *		m_launchingConditionCheck;
		//TA_Base_Bus::DataPointWriteQueue *			m_writeQ; // write is moved into CommandProcessor.
        TA_Base_Bus::PersistQueue*                  m_persistQ;
		TA_Base_Core::DataPointEntityDataPtr    	m_dataPointEntityAccess;
		TA_Base_Bus::SingletonAnswerBackErrorUtil 	m_singletonAnswerBackErrorUtil; //TD15103
		TA_Base_Bus::CalculatedExpression *			m_calculatedExpressionHandle;
		TA_Base_Core::AuditMessageSender *			m_auditMessageSender;
		TA_Base_Bus::ScadaUtilities *				m_scadaUtilities;
		EventDetailsFactory *						m_eventDetailsFactory;
		std::vector < TA_Base_Bus::IDataPointObserverNotification * > m_listOfUpdateDetailsObservers;
        ScadaPersistencePtr                         m_persistence;
		TA_Base_Core::IWorkItemPtr                  m_workItem;
        TA_Base_Core::AlarmHelper&                  m_alarmHelper;              // Raymond Pau++ TD13622
        TA_Base_Core::AlarmModificationHelper&      m_alarmModificationHelper;  // Raymond Pau++ TD13622
        MmsScadaUtility&                            m_mmsScadaUtility;

        unsigned long                               m_commandId;
        EControlCommandState                        m_controlCommandState;
        std::string                                 m_controlSessionId;
        CommandProcessor&                           m_commandProcessor;

		std::string									m_strSession;

		LFThreadPoolSingleton&						m_threadPool;
	};

} // close namespace TA_Base_Bus

#endif
