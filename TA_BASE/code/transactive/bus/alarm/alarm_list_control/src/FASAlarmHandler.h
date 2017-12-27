#pragma once
#ifndef FASAlarmHandler_13_05_2010_H
#define FASAlarmHandler_13_05_2010_H

#include <vector>
#include <string>
#include "core/threads/src/Thread.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedSchematic.h"

 

namespace TA_Base_Bus
{

	class FASAlarmHandler : public  TA_Base_Core::Thread
	{
	public:
		FASAlarmHandler(DatabaseCache& cache);	 
		virtual ~FASAlarmHandler(void);

		virtual void run();	 
		virtual void terminate();
	private:
		DatabaseCache& m_databaseCache;
		bool m_run;
		ActionRelatedSchematic* m_ActionRelatedSchematic;
	};


}

#endif // FILTER_TUE_10_02_2004_H