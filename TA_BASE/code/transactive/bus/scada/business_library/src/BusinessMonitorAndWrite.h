#ifndef MONITOR_AND_WRITE_H
#define MONITOR_AND_WRITE_H

#include "bus/scada/business_library/src/Business.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/ModbusHelper/src/ModbusHelperFactory.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "core/data_access_interface/src/IScadaBusinessItem.h"
#include "core/threads/src/QueueProcessor.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	struct CalcStateRequest
	{
		bool isInAlarm;
	};

	class BusinessMonitorAndWrite : public Business,
									public IEntityUpdateEventProcessor,
									public TA_Base_Core::QueueProcessor<CalcStateRequest>
	{
	public:
		BusinessMonitorAndWrite(const TA_Base_Core::ScadaBusinessEntitiesVector& entityKey, 
							    const TA_Base_Core::ScadaBusinessEntitiesVector& patchOrder,
								unsigned long location,
								TA_Base_Core::EScadaBusinessSubsystem subSystem);
		virtual ~BusinessMonitorAndWrite();
		void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType);
		void processEvent( boost::shared_ptr<CalcStateRequest> newEvent );
		void setToControl();
		void setToMonitor();
		void hostStart();
		void hostStop();
	protected:
		void createDataPointProxies();
		void clearDataPointProxies();

		void init(const TA_Base_Core::ScadaBusinessEntitiesVector &entityKey,
				  const TA_Base_Core::ScadaBusinessEntitiesVector& patchOrder);
		void checkAllDpConfigValid();
		bool isAnyDpInAlarm();
		void processFireSystemData(bool inAlarm);
		void makeFireSystemDataBlock(DataBlock<WORD> &dataBlock);
		bool convertEScadaBusSubToEModbusFactorySub( const TA_Base_Core::EScadaBusinessSubsystem &subSystem,
			ModbusHelperFactory::EnumSubSystem &retSubSystem);	
	private:
		bool							m_isMonitorMode;
		bool							m_haveAllDpConfigValid;
		bool							m_preInAlarm;
		ScadaProxyFactory* m_scadaProxyFactory;
		std::vector<DataPointProxySmartPtr*> m_dataPoints;	

		std::map<unsigned long, unsigned long> m_patchOrder;

		unsigned long					m_startAddress;
		unsigned long					m_patchBlockWordNumber;
		unsigned long					m_location;
		TA_Base_Core::EScadaBusinessSubsystem	m_subSystem;
		DataBlock<WORD>					m_lastSavedDataBlock;
		boost::shared_ptr<IModbusHelper> m_mobusHelper;
	};
}

#endif