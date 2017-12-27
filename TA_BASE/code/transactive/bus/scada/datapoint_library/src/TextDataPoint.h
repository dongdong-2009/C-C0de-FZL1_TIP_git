/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/TextDataPoint.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  * A DataPoint that represents a series of ASCII 
  * characters. No alarms can be configured for this
  * DataPoints. No engineering conversion is 
  * available, though the DataPoint may be associated 
  * with some engineering units.
  *
  *
  */

#ifndef TEXTDATAPOINT_H
#define TEXTDATAPOINT_H

#include <string>

#include "bus/scada/datapoint_library/src/DataPoint.h"


namespace TA_Base_Bus
{

	class TextDataPoint : public DataPoint
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
		TextDataPoint( TA_Base_Core::DataPointEntityDataPtr				    dataPointEntityAccess,
					   TA_Base_Bus::EDataPointType		                    dpType,
					   TA_Base_Bus::EDataPointDataType	                    dpDataType,
                       ScadaPersistencePtr                                  persistence,
                       TA_Base_Core::ScadaRootEntityData&                   scadaRootEntityData,
                       MmsScadaUtility&                                     mmsScadaUtility,
                       CommandProcessor&                                    commandProcessor);

		virtual ~TextDataPoint();


		void setEngineeringUnits( const std::string & engUnits );


		/**
		 * DataPoint::getMaxTextLength()
		 *
		 * Returns the maximum number of chars that a text data point
		 * can handle.  To be implemented by specific derived class
		 *
		 */
		virtual int getMaxTextLength() const;


		//
		// overloaded methods from the base class DataPoint
		//

		virtual void setToControlMode();
		virtual void setToMonitorMode();
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

	private:

		virtual void getEngineeringUnits(
   					 TA_Base_Bus::DataPointEngineeringUnits & eng );

		/**
		 * TextDataPoint::getAlarmProperties()
		 *
		 * Modifies a DataPointCorbaDef structure to reflect the alarm's
		 * configuration, if the almType matches. 
		 *
		 * @param almType   the type of alarm being queried
		 *
		 * @param almProps  the DataPointCorbaDef structure to be modified
		 *
		 */
//		virtual void getAlarmProperties(
//						TA_Base_Bus::DataPointCorbaDef::DataPointAlarmTypes & almType,
//						TA_Base_Bus::DataPointCorbaDef::AlarmProperties  & almProps );
//

		/**
		 * setMaxTextLength
		 *
		 * Set the maximum number of chars that a Text DataPoint can handle
		 *
		 * @param newMax  The new maximum number of chars of the text data point
		 *
		 */
		void setMaxTextLength ( int newMax );


		std::string		m_engineeringUnits;
		int				m_maxTextLength;
	};

}

#endif

