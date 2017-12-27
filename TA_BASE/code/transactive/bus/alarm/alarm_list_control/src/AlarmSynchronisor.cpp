// AlarmSynchronisor.cpp: implementation of the AlarmSynchronisor class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmSynchronisor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/alarm/src/Shmem.h"

using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// map shared memory when program is loaded and save the reference counter of shared memory.
//int AlarmSynchronisor::SharedMemSync::m_mapShmemRef = TA_Base_Core::Shmem::mapSharedMem();

AlarmSynchronisor::AlarmSynchronisor(ACE_Thread_Manager *thr_mgr, int syn_timeout):
ACE_Task_Base(thr_mgr), m_waitTime(syn_timeout),m_target(NULL),m_canNotify(false), m_sharedMemsyn(NULL)
{
	LOGBEFORECOTS("ACE::ACE_Thread_Manager");
	//m_sa.init();
	LOGAFTERCOTS("ACE::ACE_Thread_Manager");
	std::string entity_name = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
	m_servername = entity_name + "s";
	m_clientname = entity_name + "c";
	m_server = CreateEvent(NULL, FALSE, FALSE, m_servername.c_str());
	m_client = CreateEvent(NULL, FALSE, FALSE, m_clientname.c_str());

	//zyr++: for map/unmap shared memory
	m_remapEvName = entity_name + "_remapEvName";
	m_unmapEvName = entity_name + "_unmapEvName";
	m_remapMemEv = CreateEvent(NULL, FALSE, FALSE, m_remapEvName.c_str());
	m_unmapMemEv = CreateEvent(NULL, FALSE, FALSE, m_unmapEvName.c_str());

	
	//++zyr

	
	/*if(ERROR_ALREADY_EXISTS == GetLastError())
	{
		mayExist = true;
	}*/
	m_shutDownEv = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_rebuildEvent = CreateEvent(NULL,FALSE,FALSE,REBUILDEVENTNAME);
	//Move to thread function; 
	//need add error check
	//if(m_sa.addClient(m_servername.c_str(), m_clientname.c_str(), m_csPos, mayExist))
// 	try
// 	{
// 		if(TA_Base_Bus::SharedAlarmAccessFactory::getInstance().addClient(m_servername.c_str(), m_clientname.c_str(), m_remapEvName.c_str(), m_unmapEvName.c_str(), m_csPos, mayExist))
// 		{
// 			m_canNotify = true;
// 		}
// 		else
// 		{
// 			m_canNotify = false;
// 		}
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SharedAlarmAccessFactory::getInstance().addClient() finish");
// 	}
// 	catch(...)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught an exception when sa.addClient() in AlarmSynchronisor::AlarmSynchronisor(...)");
// 	}

	//activate();
}

AlarmSynchronisor::~AlarmSynchronisor()
{
	shutdown();
	thr_mgr_->wait();
	TA_Base_Bus::SharedAlarmAccessFactory::getInstance().removeClient(m_servername.c_str(), m_clientname.c_str(), m_remapEvName.c_str(), m_unmapEvName.c_str());
	unselectAll();
	CloseHandle(m_server);
	CloseHandle(m_client);

	//zyr++: for map/unmap shared memory
	if(m_sharedMemsyn != NULL)
	{
		delete m_sharedMemsyn;
		m_sharedMemsyn = NULL;
	}

	CloseHandle(m_remapMemEv);
	CloseHandle(m_unmapMemEv);
	//++zyr
}

//zyr++: for map/unmap shared memory
void AlarmSynchronisor::notifyAlarmStoreResart()
{
	PostMessage(m_target, AlARMSTORE_RESTART,NULL, NULL);
}
//++zyr

int AlarmSynchronisor::svc()
{
	const DWORD evcount = 3;
	SYN_EVENT evs[evcount] = {m_server,m_rebuildEvent, m_shutDownEv};
	DWORD result;
    bool mayExist = true;
	m_sharedMemsyn = new AlarmSynchronisor::SharedMemSync(m_remapMemEv, m_unmapMemEv, this);

    TA_Base_Bus::SharedAlarmAccessFactory::getInstance().addClient(m_servername.c_str(), m_clientname.c_str(), m_remapEvName.c_str(), m_unmapEvName.c_str(), m_csPos, mayExist);
    m_canNotify=true;

	TA_Base_Bus::SharedAlarmAccessFactory::getInstance().updateIndexInfo(m_info,m_csPos);
	PostMessage(m_target, ALARMSTORE_REFRESH,reinterpret_cast<WPARAM>(&m_info), NULL);	
	while(thr_mgr_->testcancel (ACE_OS::thr_self ()) == 0)
	{
		result = WaitForMultipleObjects(evcount, evs, FALSE, INFINITE);
		if( WAIT_OBJECT_0 == result )
		{
			TA_Base_Bus::SharedAlarmAccessFactory::getInstance().updateIndexInfo(m_info, m_csPos);
			PostMessage(m_target, ALARMSTORE_REFRESH,reinterpret_cast<WPARAM>(&m_info), NULL);	
			
		}
		else if (WAIT_OBJECT_0 + 1 == result) 
		{
			//zyr++: for map/unmap shared memory
			//PostMessage(m_target, AlARMSTORE_RESTART,NULL, NULL);
			Sleep(100);
			notifyAlarmStoreResart();
			//++zyr
		}
	}
	return 0;
}

//zyr++: for map/unmap shared memory
AlarmSynchronisor::SharedMemSync::SharedMemSync(SYN_EVENT& remapEv, SYN_EVENT& unmapEv, AlarmSynchronisor* parent)
: m_parent(parent)
, m_terminated(false)
, m_remapEv(remapEv)
, m_unmapEv(unmapEv)
, m_mapShmemRef(1)
{
	TA_ASSERT(m_parent != NULL, "m_parent should not be NULL");

	m_termiateEv = CreateEvent(NULL, TRUE, FALSE, NULL);

	start();
}

AlarmSynchronisor::SharedMemSync::~SharedMemSync()
{
	terminateAndWait();

	CloseHandle(m_termiateEv);
}


void AlarmSynchronisor::SharedMemSync::terminate()
{
	m_terminated = true;
	SetEvent(m_termiateEv);
}

void AlarmSynchronisor::SharedMemSync::run()
{
	m_terminated = false;

	const DWORD evcount = 3;
	SYN_EVENT evs[evcount] = {m_remapEv, m_unmapEv, m_termiateEv};
	DWORD result = 0;
	while(!m_terminated)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] Waiting for map/unmap Event.");
		result = WaitForMultipleObjects(evcount, evs, FALSE, INFINITE);
		if(result == WAIT_OBJECT_0)
		{
			// remap shared memory
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] %s remap shared memory.", 
				m_mapShmemRef == 0 ? "need to" : "ignore to");
			if(m_mapShmemRef == 0)
			{
				//m_mapShmemRef = TA_Base_Core::Shmem::mapSharedMem();
				m_mapShmemRef = TA_Base_Bus::SharedAlarmAccessFactory::getInstance().remapSharedMem();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] remap shared memory, m_mapShmemRef=%d.",
					m_mapShmemRef);

			}

			//notify alarm_list_control that alarm store restarted
			if(m_parent != NULL)
			{
				m_parent->notifyAlarmStoreResart();
			}
		}
		else if(result == WAIT_OBJECT_0 + 1)
		{
			// unmap shared memory
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] %s unmap shared memory.",
				m_mapShmemRef != 0 ? "need to" : "ignore to");
			if(m_mapShmemRef != 0)
			{
				//TA_Base_Core::Shmem::unmapSharedMem();
				m_mapShmemRef = TA_Base_Bus::SharedAlarmAccessFactory::getInstance().unmapSharedMem();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] unmap shared memory, ref_counter=%d.",
				m_mapShmemRef);
				m_mapShmemRef = 0;
			}
		}
		else
		{
			// here will be terminated.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] receive m_terminateEv signal.");
		}
	}
}	
//++zyr

