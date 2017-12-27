/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/CODE_TA_BASE_DDB/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentSynHandler.h $
  * @author  Liang Weimin
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2006/11/15 15:11:01 $
  * Last modified by : $Author: speteam $
  *
  *	DataNodeAgentCorbaDef_Impl is an abstract base class that implements
  * the DataNodeAgentCorbaDef interface, providing DataNode configurations
  * and updates to remote clients.
  *
  */

#ifndef DATANODE_AGENT_SYN_HANDLER_H
#define DATANODE_AGENT_SYN_HANDLER_H

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "core/naming/src/NamedObject.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"

namespace TA_Base_Bus
{
    class DataNodeAgentSynHandler :public virtual TA_Base_Core::Thread
	{
	public:


        DataNodeAgentSynHandler (std::string& agentname, TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent );


		virtual ~DataNodeAgentSynHandler();

        virtual void run();
        virtual void terminate();
        virtual void stopSynchronize();
        virtual void startSynchronize();


    private:
        volatile bool m_toBeTerminated;
        volatile bool m_isSynchronizing;
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::IDataNodeAgentCorbaDef,
                                           TA_Base_Bus::IDataNodeAgentCorbaDef_ptr,
                                           TA_Base_Bus::IDataNodeAgentCorbaDef_var >   DataNodeAgentInterface;	
        
        DataNodeAgentInterface * m_agentInterface;
		std::map < unsigned long, DataNode * > m_listOfLocalDataNodes;
        unsigned long m_observerId;        
        // int m_corbaErrorPollSkipCount;
        TA_Base_Core::Condition					m_timerCondition;
        static const unsigned short BULK_POLL_AGENT_SYN_PERIOD_MSECS;
        static const unsigned short BULK_POLL_COMMS_ERROR_RETRY_PERIOD_MSECS;
	};
}

#endif

