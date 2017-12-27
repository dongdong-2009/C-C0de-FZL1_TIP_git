/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSRealTimeWorker.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ISCSRealTimeWorker is ...
  *
  */


#include "ISCSRealTimeWorker.h"
#include "app/event/event_viewer/src/ConstantItems.h"
#include "EventHandlerManager.h"
#include "ISCSWorkerHelper.h"



#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_App{


	ISCSRealTimeWorker::ISCSRealTimeWorker(IEventWorkObserver* observer): IEventWorker(observer)
	{
		m_DBWorker = NULL;
		m_agentWorker = NULL;
		init2Workers();
		m_refreshRateAgent = EventHandlerManager::getRefreshRateAgent();
	}

	ISCSRealTimeWorker::~ISCSRealTimeWorker()
	{
		clear2Workers();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "~ISCSRealTimeWorker() finished.");
	}

	void ISCSRealTimeWorker::doCurrentTask()
	{

	}
	void ISCSRealTimeWorker::run()
	{

		while( !m_needToTerminate ){
			try
			{

				int workStatus = getNextTaskToCurrent ( );

				if( workStatus ==1) //new task comes in for this worker from GUI
				{
					//pass the new task to 2 workers (task_id, filter)
					m_agentWorker->addNewTask(m_curTaskSeqID, m_filter);  //agent worker init rights, start loading
					m_DBWorker->addNewTask(m_curTaskSeqID, m_filter);  //DB worker start loading backlog
					sleep(m_refreshRateAgent);
				}
				else if(workStatus==0) //user didn't start new task, continue the current task
				{
					//Will check Agent status : how it changed or not changed

					//when agent get error in refreshing preriod or get success in checkStatus: It will set isStatusSwitched to true.
					//(i.e. when agentworker fails in getLatestEvents() or succeed in checkStatus())
					if ( m_agentWorker->isStatusSwictched() ) 
					{
						m_workerObserver->reload(); //m_workerObserver is a EventHandler, this will start a new task
						sleep(EventViewerConstants::WORKER_REFRESH_TASK_PERIOD);
					}
					else if(  m_agentWorker->getStatus()==AgentStatus::UnHealthy ) //!isAgentStatusSwitched() &&!m_realTimeAgentWorker->isHealthy() 
					{
						if( m_DBWorker->isAutoRefresh() == false) //to avoid duplicating setting
						{
							m_DBWorker ->setAutoRefresh();
						}
						sleep(EventViewerConstants::AGENT_STATUS_CHECK_INTERVAL);
					}
					else
					{
						sleep(m_refreshRateAgent);
					}

				}
				else   //should stop current task: (e.g. change historical task in main)
				{
					pause2workers(); //set m_paused flag to true for each worker, and for agentWorker, set m_status to Unknown.
					sleep(EventViewerConstants::WORKER_REFRESH_TASK_PERIOD);
				}
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught in ISCSRealTimeWorker::run()");
			}
		}

	}


	void ISCSRealTimeWorker::pause2workers()
	{
		if(m_agentWorker!=NULL && !m_agentWorker->isPaused())
		{
			m_agentWorker->pause();
		}
		if(m_DBWorker!=NULL && !m_DBWorker->isPaused())
		{
			m_DBWorker->pause();
		}
	}


	void ISCSRealTimeWorker::init2Workers()
	{
		if(m_DBWorker==NULL)
		{
			m_DBWorker = new ISCSRealTimeDBWorker(m_workerObserver);
			m_DBWorker->start();
		}
		if(m_agentWorker == NULL)
		{
			m_agentWorker=new ISCSRealTimeAgentWorker(m_workerObserver);
			m_agentWorker->start();
		}
	}
	void ISCSRealTimeWorker::clear2Workers()
	{
		if(m_DBWorker!=NULL)
		{
			m_DBWorker->terminateAndWait();
			delete m_DBWorker;
			m_DBWorker = NULL;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RealTime DB worker terminated and deleted.");
		}
		if(m_agentWorker!=NULL)
		{
			m_agentWorker->terminateAndWait();
			delete m_agentWorker;
			m_agentWorker = NULL;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RealTime Agent worker terminated and deleted.");
		}
	}





};

