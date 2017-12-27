#ifndef IEC_102_POLLING_CONFIG_DATA
#define IEC_102_POLLING_CONFIG_DATA

#include <string>

namespace TA_Base_Core
{
	class IEC102PollingConfig
	{
	public:
		unsigned long m_PEnergyUnitKey;
		time_t m_startTime;
		unsigned long m_pollingTypeKey;
        std::string m_typeName;
        
		IEC102PollingConfig()
		{
			m_PEnergyUnitKey = 0;
            m_startTime = 0;
            m_pollingTypeKey = 0;
			m_typeName = "";
		}
	};
}

#endif //define IEC_102_POLLING_CONFIG_DATA