#pragma once
#include "core/process_management/idl/src/ISystemControllerAdminCorbaDef.h"
#include "core/corba/src/ServantBase.h"

using namespace TA_Base_Core;
struct Agent;

struct SystemControllerAdmin : public virtual POA_TA_Base_Core::ISystemControllerAdminCorbaDef,
    public virtual TA_Base_Core::ServantBase
{
public:

    SystemControllerAdmin();
    virtual TA_Base_Core::ProcessDataSeq* getProcessData();
    virtual void shutdown(const char* hostname) {}
    virtual void startProcess(const char* entityName);
    virtual void changeAgentOperationMode(const char* entityName, ::TA_Base_Core::EOperationMode operationMode);
    virtual void stopProcess(const char* entityName);
    virtual void setRunParam(const char* entityName, const ::TA_Base_Core::RunParam& param);
    virtual TA_Base_Core::AgentStatusEnum getAgentsStatus() { return TA_Base_Core::Normal; }
    virtual void notifyStandbyAgent(const char* entityName, const char* hostName) {}

private:

    void initAgents();
    Agent* getAgent(const std::string& name);

private:

    ProcessDataSeq m_data;
    std::map<std::string, Agent*> m_agents;
};
