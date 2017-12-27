#pragma once
#include <string>
#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsGlobalInhibitCorbaDef.h"

namespace TA_Base_Bus
{
	class MMSGlobalInhibitionManager
	{
	public:
		~MMSGlobalInhibitionManager(void);
		// Singleton Methods
		static MMSGlobalInhibitionManager& getInstance();
		static void freeInstance();

		bool getGlobalMmsInhibit();
		bool setGlobalMmsInhibit(bool bInhibit, std::string& strSessionID);

	private:
		// private constructor because this class is a singleton
		MMSGlobalInhibitionManager(void);

		typedef TA_Base_Core::NamedObject<TA_Base_Bus::IMmsGlobalInhibitCorbaDef,
			TA_Base_Bus::IMmsGlobalInhibitCorbaDef_ptr, 
			TA_Base_Bus::IMmsGlobalInhibitCorbaDef_var> MmsInhibitAgentNamedObject;

		static MMSGlobalInhibitionManager* m_pInstance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		MmsInhibitAgentNamedObject m_objInhibitAgent;
		TA_Base_Core::NonReEntrantThreadLockable m_threadLock;
	};
}
