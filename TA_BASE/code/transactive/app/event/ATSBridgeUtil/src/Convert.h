#pragma once

#include "IATSEventsAPI.h"
#include <list>
#include "ATSBridgeUtil_i.h"

#ifdef ENABLE_LOG4CXX
	#include "log4cxx/logger.h" 
	#include "log4cxx/helpers/exception.h"
	using namespace log4cxx;  
	using namespace log4cxx::helpers;  
#else
	#include "logger.h"  
	using namespace log4cxx;  
#endif

class CConvert
{
public:
	CConvert(void);
	~CConvert(void);
	HRESULT ATSType2IATSType(IATSEventsAPI::ATSDateTime atsDateTime, IATSDateTime** pIatsSDateTime);
	HRESULT ATSType2IATSType(IATSEventsAPI::ATSEventSystem atsEventSystem, IATSEventSystem** pIatsEventSystem);
	HRESULT ATSType2IATSType(IATSEventsAPI::ATSEvent atsEvent, IATSEvent** pIatsEvent);
	HRESULT ATSType2IATSType(IATSEventsAPI::ATSFilterCriteria atsFilterCriteria, IATSFilterCriteria** pIatsFilterCriteria);
	HRESULT ATSType2IATSType(std::list<IATSEventsAPI::ATSEvent*> atsEvents, IATSEventList** pIatsEvents);
	HRESULT ATSType2IATSType(std::list<IATSEventsAPI::ATSEventSystem> atsEventSystems, IATSEventSystemList** pIatsEventSystems);
	HRESULT ATSType2IATSType(std::list<std::string> strList, IATSStringList** pIatsStringList);
	HRESULT ATSType2IATSType(std::list<short int> intList, IATSIntList** pIatsIntList);

	HRESULT IATSType2ATSType(IATSEventList* pIatsEventList, std::list<IATSEventsAPI::ATSEvent*>& atsEvents);
	HRESULT IATSType2ATSType(IATSDateTime* pIatsDateTime, IATSEventsAPI::ATSDateTime& atsDateTime);
	HRESULT IATSType2ATSType(IATSEventSystem* pIatsEventSystem, IATSEventsAPI::ATSEventSystem& atsEventSystem);
	HRESULT IATSType2ATSType(IATSEvent* pIatsEvent, IATSEventsAPI::ATSEvent& atsEvent);
	HRESULT IATSType2ATSType(IATSFilterCriteria* pIatsFilterCriteria, IATSEventsAPI::ATSFilterCriteria& atsFilterCriteria);
	HRESULT IATSType2ATSType(IATSEventSystemList* pIatsEventSystems, std::list<IATSEventsAPI::ATSEventSystem>& atsEventSystems); 
	HRESULT IATSType2ATSType(IATSStringList* pIatsStringList, std::list<std::string>& strList);
	HRESULT IATSType2ATSType(IATSIntList* pIatsIntList, std::list<short int>& intList);

	// Interface Objects
	std::string ObjectToString(IATSEventList* pAtsEventList);
	std::string ObjectToString(IATSDateTime* pIatsDateTime);
	std::string ObjectToString(IATSEventSystem* pIatsEventSystem);
	std::string ObjectToString(IATSEvent* pIatsEvent);
	std::string ObjectToString(IATSFilterCriteria* pAtsfilterCriteria);
	std::string ObjectToString(IATSEventSystemList* pIatsEventSystems);
	std::string ObjectToString(IATSStringList* pIatsStringList);
	std::string ObjectToString(IATSIntList* pIatsIntList);

	inline std::string ObjectToString(LONG lNum)
	{
		char szData[50];
		sprintf(szData, "%ld", lNum);
		
		return std::string(szData);
	}
	// IATSEventsAPI Objects
	std::string ObjectToString(IATSEventsAPI::ATSDateTime& atsDateTime);
	std::string ObjectToString(IATSEventsAPI::ATSEventSystem& atsEventSystem);
	std::string ObjectToString(IATSEventsAPI::ATSEvent& atsEvent);
	std::string ObjectToString(IATSEventsAPI::ATSFilterCriteria& atsFilterCriteria);
	std::string ObjectToString(std::list<IATSEventsAPI::ATSEvent*>& atsEvents);
	std::string ObjectToString(std::list<IATSEventsAPI::ATSEventSystem>& atsEventSystems);
	std::string ObjectToString(std::list<std::string>& strList);
	std::string ObjectToString(std::list<short int>& intList);

private:
	static LoggerPtr m_pLogger;

};
