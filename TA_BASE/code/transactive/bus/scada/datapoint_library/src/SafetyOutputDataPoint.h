/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/SafetyOutputDataPoint.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	A Safety Output Datapoint maps a single value to several target
  * datapoints.
  *
  */

#ifndef SAFTETY_OUTPUT_DATAPOINT_H
#define SAFTETY_OUTPUT_DATAPOINT_H

#include <vector>

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DerivedState.h"

namespace TA_Base_Core
{
    // forward declaration
	class DerivedDataPointEntityData;
};

namespace TA_Base_Bus
{
	class SafetyOutputDataPoint : public DataPoint
	{
	public:
		SafetyOutputDataPoint ( TA_Base_Core::DataPointEntityDataPtr	            dataPointEntityAccess,
					   			TA_Base_Bus::EDataPointType                 		dpType,
						   		TA_Base_Bus::EDataPointDataType                 	dpDataType,
                                ScadaPersistencePtr                                 persistence,
                                TA_Base_Core::ScadaRootEntityData&                  scadaRootEntityData,
                                MmsScadaUtility&                                    mmsScadaUtility,
                                CommandProcessor&                                   commandProcessor );


		virtual ~SafetyOutputDataPoint();


		//
		// overloaded functions specific to DataPoint implementation
		//
        virtual void getEngineeringUnits ( TA_Base_Bus::DataPointEngineeringUnits & eng );
        virtual std::string getDataPointValueAsString( const DpValue & dpValue );
        virtual void writeDataPoint( const std::string& sessionId,
							 const TA_Base_Bus::DataPointValue & value,
							 bool auditEventLoggingEnabled = true,
							 unsigned long triggeringDpKey = 0,
							 const std::string triggeringDpValue = "");

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
		 * Update configuration data specific to this SafetyOutputDataPoint
		 *
		 * @param configChanges		The list of configuration changes
		 *
		 */
		virtual void updateSpecificConfiguration ( const std::vector< std::string > * configChanges );


	private:
		/**
		 * createDerivedState
		 *
		 * Create the derived state associated with this DerivedDataPoint based on data
		 * extracted from database
		 *
		 */
		void createDerivedState();

		void initialise();
		void cleanup();

    private:
		bool									    m_initialised;
		TA_Base_Core::ReEntrantThreadLockable		m_ownLock;
		TA_Base_Core::DerivedDataPointEntityData *	m_derivedDataPointEntityData;
		DerivedState *							    m_selectedDerivedState;
		DerivedStatesMap							m_listOfDerivedStates;
	};

}

#endif

