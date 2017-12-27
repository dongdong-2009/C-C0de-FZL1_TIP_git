/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/BooleanDataPoint.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	BooleanDataPoint represents a single boolean value. It
  * inherits from the DataPoint base class and also
  * includes functionality for alarm rules and conversion
  * to/from engineering units.
  *
  *
  */

#ifndef BOOLEANDATAPOINT_H
#define BOOLEANDATAPOINT_H

#include <string>

#include "bus/scada/datapoint_library/src/DataPoint.h"

namespace TA_Base_Bus
{

	class BooleanDataPoint : public DataPoint
	{

	public:

		/**
  		 * BooleanDataPoint::BooleanDataPoint()
  		 *
  		 * BooleanDataPoint, initialising all basic
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
		BooleanDataPoint( TA_Base_Core::DataPointEntityDataPtr				dataPointEntityAccess,
						  TA_Base_Bus::EDataPointType		                dpType,
						  TA_Base_Bus::EDataPointDataType	                dpDataType,
                          ScadaPersistencePtr                               persistence,
                          TA_Base_Core::ScadaRootEntityData&                scadaRootEntityData,
                          MmsScadaUtility&                                  mmsScadaUtility,
                          CommandProcessor&                                 commandProcessor );

		virtual ~BooleanDataPoint();

		//
		// overloaded methods of the DataPoint
		//

		virtual const std::string & getTrueLabel() const;
		virtual const std::string & getFalseLabel() const;
		virtual void setToControlMode();
		virtual void setToMonitorMode();
        
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
	      * @param : const TA_Base_Bus::AlarmProperty & newAlarmProperties
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

		virtual void updateSpecificConfiguration ( const std::vector< std::string > * configChanges );

		virtual void configureAlarms();

		virtual void configureTrueAlarm();

		virtual void configureFalseAlarm();


		/**
  		 * BooleanDataPoint::setTrueLabel()
  		 *
  		 * Specifies the DataPoint's True Engineering Units conversion
		 *
  		 * @param trueEngUnits	the string to use to represent a
		 * 						true value
		 *
  		 */
		void setTrueLabel( const std::string & trueEngUnits );


		/**
  		 * BooleanDataPoint::setFalseLabel()
  		 *
  		 * Specifies the DataPoint's False Engineering Units conversion
		 *
  		 * @param falseEngUnits	the string to use to represent a
		 * 						false value
		 *
  		 */
		void setFalseLabel( const std::string & falseEngUnits );


		/**
  		 * BooleanDataPoint::getEngineeringUnits()
  		 *
		 * Modifies a DataPointCorbaDef structure to reflect the DataPoint's
		 * engineering units configuration
  		 *
  		 * @param eng	the DataPointCorbaDef structure to be modified
		 *
  		 */
		virtual void getEngineeringUnits(
						TA_Base_Bus::DataPointEngineeringUnits & eng );

        virtual std::string getDataPointValueAsString( const DpValue & dpValue );

	};

} // close namespace TA_Base_Bus

#endif

