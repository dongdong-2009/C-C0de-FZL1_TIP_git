/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/EnumDataPoint.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	EnumDataPoint is a multi-state data point.
  *
  */

#ifndef ENUM_DATAPOINT_H
#define ENUM_DATAPOINT_H

#include <map>

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DerivedState.h"

namespace TA_Base_Core
{
	class DerivedDataPointEntityData;
};

namespace TA_Base_Bus
{

	class EnumDataPoint : virtual public DataPoint
	{
	public:


		EnumDataPoint ( TA_Base_Core::DataPointEntityDataPtr				dataPointEntityAccess,
						TA_Base_Bus::EDataPointType		                    dpType,
						TA_Base_Bus::EDataPointDataType	                    dpDataType,
                        ScadaPersistencePtr                                 persistence,
                        TA_Base_Core::ScadaRootEntityData&                  scadaRootEntityData,
                        MmsScadaUtility&                                    mmsScadaUtility,
                        CommandProcessor&                                   commandProcessor );


		virtual ~EnumDataPoint();


		//
		// overloaded functions specific to DataPoint implementation
		//

		virtual void startABETimeout(const std::string& sessionID);
		virtual void setToControlMode();
		virtual void setToMonitorMode();
		virtual const std::string & getStateLabel(unsigned long state) const;
		virtual void getEngineeringUnits ( TA_Base_Bus::DataPointEngineeringUnits & eng );
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
		//
		// overloaded functions specific to DataPoint implementation
		//

		virtual void updateSpecificConfiguration ( const std::vector< std::string > * configChanges );
		virtual bool isValidUpdate( DataPointState & updateState );
		virtual bool isValidWrite( DpValue & writeValue );

	private:


		void createDerivedState();
		void initialise();
		void cleanup();
		bool isValidStateValue ( const DpValue & dpValue );
		const std::string & getStateReturnCondition ( const DpValue & dpValue );

		DerivedStatesMap						m_listOfDerivedStates;

		std::string								m_dummyReturnCondition;
		bool									m_initialised;
		TA_Base_Core::ReEntrantThreadLockable		m_ownLock;
		TA_Base_Core::DerivedDataPointEntityData *	m_derivedDataPointEntityData;
	};

}

#endif

