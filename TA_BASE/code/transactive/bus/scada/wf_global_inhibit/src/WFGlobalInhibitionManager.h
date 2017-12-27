#pragma once
#include <string>
#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/scada/wf_global_inhibit/IDL/src/IWFGlobalInhibitCorbaDef.h"

namespace TA_Base_Bus
{
	class WFGlobalInhibitionManager
	{
	public:
		~WFGlobalInhibitionManager(void);
		// Singleton Methods
		static WFGlobalInhibitionManager& getInstance();
		static void freeInstance();

		bool getWFGlobalInhibitState(bool& globalInhibitState);
		bool setWFGlobalInhibitState(bool bInhibit, std::string& strSessionID);
		bool verifyPassoword(std::string& strSessionID,std::string& strPassword);

	private:
		// private constructor because this class is a singleton
		WFGlobalInhibitionManager(void);

		typedef TA_Base_Core::NamedObject<TA_Base_Bus::IWFGlobalInhibitCorbaDef,
			TA_Base_Bus::IWFGlobalInhibitCorbaDef_ptr, 
			TA_Base_Bus::IWFGlobalInhibitCorbaDef_var> WFInhibitAgentNamedObject;

		static WFGlobalInhibitionManager* m_pInstance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		
		WFInhibitAgentNamedObject m_objInhibitAgent;
		bool	m_initialized;
		TA_Base_Core::NonReEntrantThreadLockable m_threadLock;
	};
}
