#pragma once
#include "MySink.h"
#include "FilterCriteria.h"
#include <vector>
#include "core/data_access_interface/src/ICombinedEventData.h"
#include "ActionCommand.h"
/*#include "atltime.h"*/


//#import "../../../../../../bin/win32_nd/ATSBridgeUtil.dll" named_guids

#ifdef _DEBUG
#import "../../../../../../sdk/win32_nd/ATSBridgeUtil.tlb" named_guids
#else
#import "../../../../../../sdk/win32_nr/ATSBridgeUtil.tlb" named_guids
#endif


using namespace ATSBridgeUtilLib;

using namespace TA_Base_Core;


namespace TA_Base_App
{
	class ComSimulator
	{	
	public:
		ComSimulator(void);
		~ComSimulator(void);

		ActionCommand startATSSubscription(
			FilterCriteria filterCriteria,
			const unsigned int backloglength,				
			std::vector<TA_Base_Core::ICombinedEventData*>& events);

		ActionCommand  doStartTimeEndTimeQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
			FilterCriteria filterCriteria, unsigned long atsMaxLength);

		ActionCommand doPreviousPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
			FilterCriteria filterCriteria, unsigned long atsMaxLength);

		ActionCommand doNextPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
			FilterCriteria filterCriteria, unsigned long atsMaxLength);

		void initATSSubscriber();

		void transformToATSFilter(IATSFilterCriteria * pATSFilter, FilterCriteria & filterCriteria );
		void setSystemNameToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria);
		void setSubsystemNameToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria);
		void setOperatorToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria);
		void setLocationToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria);
		void setAssetToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria);
		void setAlarmSeverityToATSFilter(IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria);


		void  ComSimulator::setStartEndTimeToATSFilter ( ATSBridgeUtilLib::IATSFilterCriteria * atsfilter, 
			FilterCriteria & filterCriteria);

		//void adjustATSFilterTime(ATSBridgeUtilLib::IATSFilterCriteria *  atsfilter);

		void setATSFilterStartTimeByCTime(ATSBridgeUtilLib::IATSFilterCriteria *  atsfilter, CTime time);
		void setATSFilterEndTimeByCTime(ATSBridgeUtilLib::IATSFilterCriteria *  atsfilter, CTime time);

		void setTMTypeToATSFilter(ATSBridgeUtilLib::IATSFilterCriteria * atsfilter, FilterCriteria & filterCriteria);
		void transformToISCSEvents (IATSEventList * pATSEvents, 
			std::vector<TA_Base_Core::ICombinedEventData*>& events , unsigned long lastKey = 0);

		ICombinedEventData * createCombinedEvent ( IATSEvent* pATSEvent, unsigned long key);

		ActionCommand stopATSSubscription(void);

		void truncateATSEvents (IATSEventList * pATSEvents, unsigned long atsMaxLength, bool cutSmallTime);

		void restartATSSubscriptionForRefresh();

		static ComSimulator* getInstance();
		static void freeInstance();


	private:

		static ComSimulator * m_instance;

		bool m_ATS_func_debug_level;
		CComPtr<IATSEventSubscription> m_pSubscriber;
		CComPtr<IConnectionPoint> m_pCP;

		DWORD m_dwAdvise;


	};

};
