#include "bus/alarm/alarm_list_control/src/stdafx.h"

#include "FASAlarmHandler.h"
#include "bus/alarm/alarm_store_library/src/SharedAlarmAccessFactory.h"

#include "bus/alarm/alarm_list_control/src/resource.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/exceptions/src/AlarmActionException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
 
using TA_Base_Core::AlarmActionException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	FASAlarmHandler::FASAlarmHandler(DatabaseCache& cache):m_databaseCache(cache)
	{
		m_run=true;
		m_ActionRelatedSchematic=new ActionRelatedSchematic(false);
		 
	}

	FASAlarmHandler::~FASAlarmHandler(void)
	{
	}

	void FASAlarmHandler::run()
	{
       while (m_run)
       {
		  unsigned long  entitykey=SharedAlarmAccessFactory::getInstance().getLatestFASAlarm();
		  if (entitykey!=0)
		  {
			  try
			  {
				  LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "launchGraphWorxDisplay entitykey=%d",entitykey);	
				  m_ActionRelatedSchematic->launchGraphWorxDisplay(m_databaseCache, entitykey);
			  }
			  catch( const TA_Base_Core::AlarmActionException& ex)
			  {
				  TA_Base_Bus::TransActiveMessage userMsg;
				  CString actionName;
				  unsigned int errorId;

				  errorId = IDS_UE_010002;
				  actionName.LoadString(IDS_SCHEMATIC);

				  if (!actionName.IsEmpty())
				  {
					  userMsg << actionName;
				  }

				  userMsg.showMsgBox(errorId);
			  }
			  catch( const TA_Base_Core::TransactiveException& te )
			  {
				  LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransActiveException", te.what() );
			  }
			  catch(...)
			  {
				  std::ostringstream desc;
				  desc << "[FASAlarmHandler::run] Caught unknown exception";
				  LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", desc.str().c_str() );
			  }
		  }	
		  int i=0;
		  for (i=0;i<5 && m_run;i++)
		  {
			  Sleep(200);
		  }
       }


	}
	void FASAlarmHandler::terminate()
	{
		m_run=false;     
	}
}
