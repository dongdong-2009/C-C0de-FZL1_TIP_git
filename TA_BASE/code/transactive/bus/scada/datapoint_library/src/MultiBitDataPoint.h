#ifndef _MULTIBIT_DATAPOINT_H__
#define _MULTIBIT_DATAPOINT_H__

#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/ConfiguredDerivedState.h"
namespace TA_Base_Core
{
	class DerivedDataPointEntityData;
};
namespace TA_Base_Bus
{
	class MultiBitDataPoint : public DataPoint
	{
	public:
		MultiBitDataPoint ( TA_Base_Core::DataPointEntityDataPtr		            dataPointEntityAccess,
						   TA_Base_Bus::EDataPointType		                    dpType,
						   TA_Base_Bus::EDataPointDataType	                    dpDataType,
                           ScadaPersistencePtr                                  persistence,
                           TA_Base_Core::ScadaRootEntityData&                   scadaRootEntityData,
                           MmsScadaUtility&                                     mmsScadaUtility,
                           CommandProcessor&                                    commandProcessor );


		virtual ~MultiBitDataPoint();
		
		bool getIsBitAddressMSB();
		
		void initialise();

		virtual void setAlarmProperties( const std::string& sessionId, const TA_Base_Bus::AlarmProperty & newAlarmProperties);

		virtual void getEngineeringUnits(TA_Base_Bus::DataPointEngineeringUnits & eng );

		virtual TA_Base_Bus::AlarmPropertyVect getAlarmPropertiesVect();

		virtual std::string getDataPointValueAsString( const DpValue & dpValue );

		virtual void updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties);
		
		virtual bool getIsDigitalDataPoint() const;
	private:
		void initialiseEnumMap();
	private:
		bool			m_isBigEndian;
		TA_Base_Core::DerivedDataPointEntityData *	m_MultiBitDataPointEntityData;
		std::vector<TA_Base_Core::ConfiguredDerivedState*>		m_listOfConfiguredDerivedStates;
	};
}

#endif