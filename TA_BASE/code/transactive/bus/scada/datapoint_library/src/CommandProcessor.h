/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source:  $
 * @author:  
 * @version: $Revision:  $
 *
 * Last modification: $Date: $
 * Last modified by:  $Author: $
 * 
 * The m_writeQ and m_dpStateUpdateQ must be separately thread locked.
 * In Control Mode:
 * This class queues control commands into m_writeQ, which is processed by the
 * driver part of the agent.
 * In Monitor mode:
 * In run method,  this thread processes the m_dpStateUpdateQ. If it finds a state
 * update with a state "Completed with success" or "Completed with error", the
 * entry is removed from the map, in other cases the state of the command is
 * replaced with the received one. While processing the m_dpStateUpdateQ, the
 * variable should be assigned the commandId of the command received in the update
 * IF received commanId > m_commandId. This will ensure that when the agent makes
 * transion to control, no duplicated commandId will be issued to new scheduled
 * commands. Since the m_commandId will roll over after the max value, this
 * condition should be checked.
 * Transition from Monitor to Control Mode:
 * The run method continue the processing of m_dpStateUpdateQ till it is empty( no
 * new update will come as the agent mode has changed.). When m_dpStateUpdateQ is
 * empty.
 * It should start processing the m_mapDpStateUpdates, and schedule any unfinished
 * writes. 
 */

#if !defined CommandProcessor__INCLUDED_
#define CommandProcessor__INCLUDED_

#include <queue>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"
#include "bus/scada/common_library/IDL/src/IDataPointStateUpdateCorbaDef.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/datapoint_library/src/DataPointStateUpdateSender.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"

namespace TA_Base_Bus
{
    class CommandProcessor : public TA_Base_Core::Thread
    {

    public:
	    virtual ~CommandProcessor();

    public:

        /**
	     * virtual functions need to be implemented
	     * @param
	     */
        virtual void run();
        virtual void terminate();

	    /**
	     * 
	     * @param dpStateUpdateSender
	     */
        CommandProcessor(TA_Base_Bus::DataPointStateUpdateSender* dpStateUpdateSender, TA_Base_Bus::IDataPointAgent* dpAgent);

        /**
	     * 
	     * @param dpWriteStateUpdate
	     */
	    void processDataPointStateUpdate(DataPointStateUpdate& dpWriteStateUpdate);
	    
        /**
	     * This is used in Control Mode only to register the monitor agent. The received
	     * uuid should be compared with m_uuid. If these are different, complet update is
	     * required to sync.
	     * 
	     * @param uuid
	     */
	    void registerMonitorAgent(const std::string& uuid, TA_Base_Bus::IDataPointStateUpdateCorbaDef_ptr monitorAgentStateUpdateRef);

        void deregisterMonitorAgent();
            
	    /**
	     * 
	     * @param dpWriteRequest
	     * @param commandId
	     */
	    void scheduleCommand(DataPointWriteRequest* dpWriteRequest);

	    /**
	     * The return value is the commandId assigned. This will be new if the second
	     * argument is true.
	     * 
	     * @param dpStateUpdate
	     * @param newCommandIdRequired
	     * @param commandId
	     */
	    unsigned long sendDataPointStateUpdate(DataPointStateUpdate& dpStateUpdate, bool newCommandIdRequired);
	    
        /**
	     * 
	     * @param stateUpdates
	     */
	    void sendDataPointStateUpdates(const std::vector<DataPointStateUpdate>& stateUpdates);

	    /**
	     * sets m_isCompleteSyncRequired to true.
	     */
	    void setCompleteSyncRequired();

        void setToControl();
        void setToMonitor();
		void setOperationMode(TA_Base_Core::EOperationMode mode);
    private:
        //CommandProcessor(){};
        //CommandProcessor(CommandProcessor&){};

        unsigned long getNextCommandId();
	    void processDataPointStateUpdateQueue();
        void updateDataPointStateUpdateMap(DataPointStateUpdate& dpStateUpdate);

    private:
	    unsigned long m_commandId;
        std::queue<DataPointStateUpdate*> m_dpStateUpdateQ;
	    
        /**
	     * Set to true by setCompletSyncRequired()
	     * and set to false when complete sync is done by sendDataPointStateUpdates.
	     * 
	     * This flag should be checked in the run method so that restarts of monitor agent
	     * can be taken care of.
	     */
	    bool m_isCompleteSyncRequired;
	    std::map<unsigned long, DataPointStateUpdate> m_mapDpStateUpdates;
        TA_Base_Core::ReEntrantThreadLockable m_mapDpStateUpdatesLock;
	    TA_Base_Core::ReEntrantThreadLockable m_stateUpdateQLock;
	    
        /**
	     * In control mode, this stores the uuid sent by the monitor agent during
	     * registration.
	     */
	    std::string                         m_uuid;
	    TA_Base_Bus::DataPointWriteQueue*   m_writeQ;

	    DataPointStateUpdateSender*         m_dpStateUpdateSender;
        TA_Base_Bus::IDataPointAgent*       m_dpAgent;
        bool                                m_isTerminating;
        bool                                m_isControl;
        bool                                m_isJustChangedToControl;
        TA_Base_Core::Condition             m_conditon;
    };
}
#endif