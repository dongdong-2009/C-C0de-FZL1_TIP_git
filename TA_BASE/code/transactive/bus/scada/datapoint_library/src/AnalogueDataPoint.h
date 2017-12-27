/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/AnalogueDataPoint.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	AnalogueDataPoint represents a single numeric
  *	value. It inherits from the DataPoint base class,
  *	and also includes details representing raw and
  * engineering ranges, as well as a possible update
  * threshold (the % of the raw range that must change
  * before the DataPoint is updated).
  *
  */

#ifndef ANALOGUEDATAPOINT_H
#define ANALOGUEDATAPOINT_H

#include <string>

#include "bus/scada/datapoint_library/src/DataPoint.h"

namespace TA_Base_Bus
{

	class AnalogueDataPoint : public DataPoint
	{

	public:

		/**
  		 * AnalogueDataPoint::AnalogueDataPoint()
  		 *
  		 * AnalogueDataPoint, initialising all basic
		 * configuration items.
		 *
		 * @param	dataPointEntityAccess	Reference to db entity access
		 * @param	dpType					The type of the data point
		 * @param	dpDataType				The type of value being represented
		 * 									by the DataPoint
         * @param   persistence             Interface for reading/writing persisted
         *                                  state values
		 *
  		 */
		AnalogueDataPoint( TA_Base_Core::DataPointEntityDataPtr				    dataPointEntityAccess,
						   TA_Base_Bus::EDataPointType		                    dpType,
						   TA_Base_Bus::EDataPointDataType	                    dpDataType,
                           ScadaPersistencePtr                                  persistence,
                           TA_Base_Core::ScadaRootEntityData&                   scadaRootEntityData,
                           MmsScadaUtility&                                     mmsScadaUtility,
                           CommandProcessor&                                    commandProcessor );

		virtual ~AnalogueDataPoint();

		/**
  		 * getAlarmThresholdConstant
  		 *
		 * Returns the constant, the % of eng limit range, that the value must
		 * be below the alarm limit before the alarm is considered normalised
  		 *
  		 */
		virtual double getAlarmThresholdConstant() const;

		/**
  		 * AnalogueDataPoint::setEngineeringUnits()
  		 *
  		 * Specifies the DataPoint's Engineering Units conversion
		 *
  		 * @param engUnits	the units that the DataPoint's value is
		 * 					expressed in
		 *
		 * @param rawHi		the highest possible raw value for the DataPoint
		 *
		 * @param rawLo		the lowest possible raw value for the DataPoint
		 *
		 * @param engHi		the highest possible value for the DataPoint,
		 * 					after a linear engineering conversion has been done
		 *
		 * @param engLo		the lowest possible raw value for the DataPoint,
		 * 					after a linear engineering conversion has been done
		 *
  		 */
		virtual void setEngineeringUnits( const std::string & engUnits,
										  double		rawHi = 0,
										  double		rawLo = 0,
										  double		engHi = 0,
										  double		engLo = 0 );

		/**
		 * AnalogueDataPoint::updateEngineeringUnits()
		 *
		 * Modifies one or more elements that make up an Analogue
		 * DataPoint's engineering units configuration.
		 *
		 * @param enabled	whether or not eng units are enabled
		 *
		 * @param engUnits	pointer to an engineering units string.
		 *					If this is not NULL, then the engineering units string
		 *					will be updated to this new value.
		 *
		 * @param rawHi		pointer to a Raw High Limit value.
		 *					If this is not NULL, then the Raw High Limit
		 *					will be updated to this new value.
		 *
		 * @param rawLo		pointer to a Raw Low Limit value.
		 *					If this is not NULL, then the Raw Low Limit
		 *					will be updated to this new value.
		 *
		 * @param engHi		pointer to an Engineering High Limit value.
		 *					If this is not NULL, then the Engineering High Limit
		 *					will be updated to this new value.
		 *
		 * @param engLo		pointer to an Engineering Low Limit value.
		 *					If this is not NULL, then the Engineering Low Limit
		 *					will be updated to this new value.
		 *
		 */
		void updateEngineeringUnits( bool enabled,
                        			 std::string * engUnits,
                        			 double * rawHi,
                        			 double * rawLo,
                        			 double * engHi,
                        			 double * engLo );


		/**
  		 * AnalogueDataPoint::setUpdateThreshold()
  		 *
  		 * Specifies a percentage of the DataPoint's raw range. The
		 * DataPoint's value must change by this amount before it
		 * will be update. Optionally, a time limit can be specified
		 * after which the DataPoint will be updated regardless.
		 * It is very important that the raw limits be defined before
		 * calling this function.
		 *
  		 * @param threshold		the update threshold ( 0-100 )
		 *
		 * @param timeLimit		the time limit in seconds
  		 *
  		 */
		void setUpdateThreshold( double threshold, int timeLimit = 0 );


		/**
		 * AnalogueDataPoint::setZeroValueThreshold()
		 *
		 * Specifies a percentage of the DataPoint's raw range as
		 * the zero value thershold. If the difference between a new
		 * value and zero is less than this threshold, the value will
		 * be modified to zero. It is very important that the raw limits
		 * be defineds before calling this function.
		 *
		 * @param	threshold	the zero value threshold ( 0-100 )
		 *
		 */
		void setZeroValueThreshold( double threshold );

		//
		// overloaded methods of the base class DataPoint
		//

		virtual void setToControlMode();
		virtual void setToMonitorMode();
		virtual void updateFieldState( DataPointState & newState );


        virtual std::string getDataPointValueAsString( const DpValue & dpValue );

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
         virtual void setAlarmProperties( const std::string& sessionId, const TA_Base_Bus::AlarmProperty & newAlarmProperties);

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

		virtual void updateSpecificConfiguration ( const std::vector< std::string > * configChanges );

		virtual void updateAnalogueEngineeringUnits( const std::string & paramName );

		virtual void updateAnalogueUpdateThreshold( const std::string & paramName );

		virtual void configureAlarms();

		virtual void configureHiHiAlarm();

		virtual void configureHiAlarm();

		virtual void configureLoAlarm();

		virtual void configureLoLoAlarm();

		virtual void configureOverScaleHiAlarm();

		virtual void configureOverScaleLoAlarm();

	    /**
	      * DataPoint::getAlarmPropertiesVect
	      * 
	      *  get the alarm properties vector
	      * 
	      * @return virtual TA_Base_Bus::AlarmPropertyVect 
	      * 
	      */
         virtual TA_Base_Bus::AlarmPropertyVect getAlarmPropertiesVect();

		//To set the other limit of alarms. It should be called after the alarms are created or configured.
		virtual void setAlarmsOtherLimit(); 


		/**
  		 * AnalogueDataPoint::isValidWrite()
  		 *
  		 * Determines whether the requested value is valid for the DataPoint.
		 * This function is called for every write request, before the request
		 * is passed to the PLCWorker class for writing to the PLC. For an
		 * Analogue DataPoint, raw/engineering range checking is done in this
		 * function.
		 *
		 * @return	whether or not the value is valid for writing
  		 *
  		 * @param 	writeValue	the value that has been requested for writing
		 *
  		 */
		 virtual bool isValidWrite( DpValue & writeValue );


		/**
  		 * AnalogueDataPoint::isValidUpdate()
  		 *
  		 * Determines whether the requested state update is valid or not.
		 * This function is called for every state update. For an
		 * AnalogueDataPoint, the update threshold/time limit functionality
		 * is implemented here.
		 *
		 * @return  whether or not the state is valid for further processing
		 * 			by the DataPoint::UpdateState() function
  		 *
  		 * @param 	updateState		the new state for processing
		 *
  		 */
		virtual bool isValidUpdate( DataPointState & updateState );


		/**
  		 * AnalogueDataPoint::convertToEngUnits()
  		 *
  		 * Converts a raw value into an engineering value, according to the
		 * DataPoint's configuration. For an AnalogueDataPoint, this involves
		 * a linear conversion from the raw data range to the engineering
		 * data range.
		 *
  		 * @param rawVal	the raw value to be converted
		 *
  		 * @param engVal	the resultant eng value
		 *
  		 */
		virtual void convertToEngUnits( const DataPointState & rawState,
									    DataPointState & engState );


		/**
  		 * AnalogueDataPoint::convertToRaw()
  		 *
  		 * Converts an engineering value to a raw value, according to the
		 * DataPoint's configuration. For an AnalogueDataPoint, this involves
		 * a linear conversion from the engineering data range to the raw
		 * data range.
		 *
  		 * @param engVal	the engineering value to be converted
  		 *
  		 * @param rawVal	the resultant raw value
  		 *
  		 */
		virtual void  convertToRaw( const DpValue & engVal,
									DpValue & rawVal );


		/**
  		 * AnalogueDataPoint::getEngineeringUnits()
  		 *
		 * Modifies a DataPointCorbaDef structure to reflect the DataPoint's
		 * engineering units configuration
  		 *
  		 * @param eng	the DataPointCorbaDef structure to be modified
		 *
  		 */
		virtual void getEngineeringUnits(
					   TA_Base_Bus::DataPointEngineeringUnits & eng );

        /**
  		 * AnalogueDataPoint::updateThresholdExceeded()
  		 *
  		 * Determines whether or not the difference between a new value and
		 * the current one exceeds the configured update threshold
		 *
		 * @return	whether or not the threshold has been exceeded
  		 *
  		 * @param state		the new state
  		 *
  		 */
		bool isUpdateThresholdExceeded( DataPointState & state ); //TD16169


		/**
  		 * AnalogueDataPoint::updateTimeLimitExceeded()
  		 *
  		 * Determines whether or not the time limit for a value
		 * change to exceed the update threshold has been met, and
		 * the DataPoint should be updated regardless of whether it
		 * exceeds the threshold
		 *
		 * @return	whether or not the time limit has been exceeded
  		 *
  		 * @param state		the new state
  		 *
  		 */
		bool isUpdateTimeLimitExceeded( DataPointState & state ); //TD16169


		/**
  		 * DataPoint::updateAlarms
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
  		 * Helper functions for checking engineering limits and raw value limits, 
         *            and datatype limits as well
  		 */
        bool isValidEngValue( const DpValue & dpValue );
        bool isValidRawValue( const DpValue & dpValue );
        bool isValidDataTypeValue( const DpValue & dpValue );
        double getDataTypeLimitValue(bool isMaxLimit);

	protected:

		double			 		 m_updateThreshold;
		int						 m_updateTimeLimit;
		time_t					 m_updateTimestamp;
		double					 m_alarmThresholdConstant;
		double					 m_engConstant;
		std::string				 m_engineeringUnits;
		double					 m_rawHighLimit;
		double					 m_rawLowLimit;
		double					 m_rawRange;
		double					 m_engHighLimit;
		double					 m_engLowLimit;
		double					 m_zeroValueConstant;
	};

} // close namespace TA_Base_Bus


#endif

