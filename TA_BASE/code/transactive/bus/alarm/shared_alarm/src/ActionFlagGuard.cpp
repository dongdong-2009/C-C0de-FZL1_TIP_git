// ActionFlagGuard.cpp: implementation of the ActionFlagGuard class.
//
//////////////////////////////////////////////////////////////////////

#include "ActionFlagGuard.h"
#include "core/utilities/src/DebugUtil.h"
 

using namespace TA_Base_Bus;

using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	ActionFlagGuard::ActionFlagGuard(bool& actionFlag):m_actionFlag(actionFlag)
	{
		m_actionFlag=true;
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "ActionFlagGuard::ActionFlagGuard");
	}

	ActionFlagGuard::~ActionFlagGuard()
	{
	   m_actionFlag=false;
       //LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "ActionFlagGuard::~ActionFlagGuard");
	}
}
