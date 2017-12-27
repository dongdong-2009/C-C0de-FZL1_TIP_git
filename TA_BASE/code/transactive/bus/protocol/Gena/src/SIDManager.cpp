
#include "SIDManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_String.h"

namespace TA_Base_Bus
{
	// I think we need to create a more detailed SID that should contain 
	// the server information if possible. For now a simple SID will do.
	std::string SIDManager::getNextSID()
	{
		FUNCTION_ENTRY("SIDMgr::getNextSID");
		TA_Base_Core::ThreadGuard guard(m_sidLock);
		m_sidCounter++; 
		//char szTemp[25] = {0};
		//sprintf(szTemp, "%d", m_sidCounter);
		std::string sid=TA_Base_Core::toString(m_sidCounter);
		FUNCTION_EXIT;
		return sid;
	}

	void SIDManager::resetSID()
	{
		FUNCTION_ENTRY("SIDMgr::resetSID");
		TA_Base_Core::ThreadGuard guard(m_sidLock);

		m_sidCounter = 0; 
		FUNCTION_EXIT;
	}

} //TA_Base_Bus