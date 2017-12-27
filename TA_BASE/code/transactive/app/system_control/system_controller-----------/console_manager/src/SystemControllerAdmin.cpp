#include "SystemControllerAdmin.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include <ctime>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace TA_Base_Core;


struct Agent : Thread
{
    Agent(ProcessData& data,
          const std::string& ProcessId,
          const std::string& host,
          EProcessStatus state,
          EProcessStatus requestedState,
          EOperationMode operationMode,
          time_t started,
          size_t restarts,
          const std::string& debugLevel)
        : m_data(data),
          m_semaphore(0),
          m_running(true)
    {
        data.ProcessId = ProcessId.c_str();
        data.host = host.c_str();
        data.state = state;
        data.requestedState = requestedState;
        data.operationMode = operationMode;
        data.started = started;
        data.restarts = restarts;
        data.debugLevel = debugLevel.c_str();

        m_defaultOperationMode = Control;
        start();
    }

    void startProcess()
    {
        m_data.requestedState = Startup;
        m_semaphore.post();
    }

    void stopProcess()
    {
        m_data.requestedState = Stopped;
        m_semaphore.post();
    }

    void changeOperationMode(EOperationMode mode)
    {
        m_data.operationMode = mode;
        m_semaphore.post();
    }

    void setDebugLevel(const std::string& debugLevel)
    {
        m_data.debugLevel = debugLevel.c_str();
    }

    virtual void run()
    {
        while (m_running)
        {
            m_semaphore.wait();

            switch (m_data.requestedState)
            {
            case Startup:
                if (m_data.state == Stopped)
                {
                    m_data.started = time(NULL);
                    m_data.restarts++;
                    m_data.state = Startup;
                    sleep();
                    m_data.operationMode = m_defaultOperationMode;
                    m_data.requestedState = (m_defaultOperationMode == Control ? RunningControl : RunningMonitor);
                    sleep();
                    m_data.state = (m_defaultOperationMode == Control ? GoingToControl : GoingToMonitor);
                    sleep();
                    m_data.state = (m_defaultOperationMode == Control ? RunningControl : RunningMonitor);
                }

                break;

            case Stopped:
                if (m_data.state != Stopped)
                {
                    m_data.state = Terminating;
                    sleep();
                    sleep();
                    m_data.state = Stopped;
                    m_data.operationMode = NotApplicable;
                }

                break;

            default:
                NULL;
            }

            switch (m_data.operationMode)
            {
            case Control:
                m_defaultOperationMode = Control;

                if (m_data.state != RunningControl)
                {
                    m_data.requestedState = RunningControl;
                    sleep();
                    m_data.state = GoingToControl;
                    sleep();
                    m_data.state = RunningControl;
                }

                break;

            case Monitor:
                m_defaultOperationMode = Monitor;

                if (m_data.state != RunningMonitor)
                {
                    m_data.requestedState = RunningMonitor;
                    sleep();
                    m_data.state = GoingToMonitor;
                    sleep();
                    m_data.state = RunningMonitor;
                }

                break;

            default:
                NULL;
            }
        }
    }

    void sleep()
    {
        static boost::random::mt19937 gen;
        static boost::random::uniform_int_distribution<> rand(0, 1000);
        Thread::sleep(rand(gen));
    }

    virtual void terminate() { m_running = false; m_semaphore.post(); }
    bool m_running;
    ProcessData& m_data;
    EOperationMode m_defaultOperationMode;
    Semaphore m_semaphore;
};


SystemControllerAdmin::SystemControllerAdmin()
{
    initAgents();
    TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "SystemControllerAdmin");
    activateServant();
}


TA_Base_Core::ProcessDataSeq* SystemControllerAdmin::getProcessData()
{
    return new ProcessDataSeq(m_data);
}


void SystemControllerAdmin::initAgents()
{
    size_t i = 0;
    m_data.length(6);
    time_t current = time(NULL);

    m_agents["OccAlarmMonitoredProcess"]                = new Agent(m_data[i++], "OccAlarmMonitoredProcess", "occa", RunningControl, RunningControl, Control, current, 0, "INFO");
    m_agents["OccAuthenticationMonitoredProcess"]       = new Agent(m_data[i++], "OccAuthenticationMonitoredProcess", "occa", RunningControl, RunningControl, Control, current, 0, "INFO");
    m_agents["OccDutyMonitoredProcess"]                 = new Agent(m_data[i++], "OccDutyMonitoredProcess", "occa", RunningControl, RunningControl, Control, current, 0, "INFO");
    m_agents["OccNotificationServiceMonitoredProcess"]  = new Agent(m_data[i++], "OccNotificationServiceMonitoredProcess", "occa", RunningControl, RunningControl, Control, current, 0, "INFO");
    m_agents["OccRightsMonitoredProcess"]               = new Agent(m_data[i++], "OccRightsMonitoredProcess", "occa", RunningControl, RunningControl, Control, current, 0, "INFO");
    m_agents["OccSystemStatusMonitoredProcess"]         = new Agent(m_data[i++], "OccSystemStatusMonitoredProcess", "occa", RunningControl, RunningControl, Control, current, 0, "INFO");
}


void SystemControllerAdmin::startProcess(const char* entityName)
{
    Agent* agent = getAgent(entityName);

    if (agent)
    {
        agent->startProcess();
    }
}


void SystemControllerAdmin::stopProcess(const char* entityName)
{
    Agent* agent = getAgent(entityName);

    if (agent)
    {
        agent->stopProcess();
    }
}


void SystemControllerAdmin::changeAgentOperationMode(const char* entityName, ::TA_Base_Core::EOperationMode operationMode)
{
    Agent* agent = getAgent(entityName);

    if (agent)
    {
        agent->changeOperationMode(operationMode);
    }
}


void SystemControllerAdmin::setRunParam(const char* entityName, const ::TA_Base_Core::RunParam& param)
{
    CORBA::String_var str = param.value;
    Agent* agent = getAgent(entityName);

    if (agent)
    {
        agent->setDebugLevel(param.value.in());
    }
}


Agent* SystemControllerAdmin::getAgent(const std::string& name)
{
    std::map<std::string, Agent*>::iterator it = m_agents.find(name);

    if (it != m_agents.end())
    {
        return it->second;
    }

    return NULL;
}
