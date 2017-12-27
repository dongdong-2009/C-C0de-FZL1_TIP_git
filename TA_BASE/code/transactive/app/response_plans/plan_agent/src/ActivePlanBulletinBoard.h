#ifndef ACTIVEPLANBULLETINBOARD_H_INCLUDED
#define ACTIVEPLANBULLETINBOARD_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

#include <map>
#include <list>

#include "ace/RW_Mutex.h"
#include "ace/Mutex.h"
#include "ace/Future.h"
#include "ace/Singleton.h"

namespace TA_Base_Core
{
    class StateUpdateMessageSender;
    class CommsMessageSender;
    class ActivePlanDetails;
    class PASUpdateDataSeq;
    class ActiveStepCompletionStatusSeq;
    struct ActiveStepDetail;
    struct ActivePlanDetail;
    struct ActivePlanId;
    struct ExecutionHistory;
    struct CustomisationHistory;
    struct ExecutionHistoryUpdate;
    struct CustomisationHistoryUpdate;
    struct PlanCustomisationUpdate;
    struct StepCustomisableDetail;
    struct PASUpdateData;
    struct PlanConfigUpdateData;
    struct CurrentPlanStepDetail;
}

namespace TA_Base_Bus
{
    class _objref_IPlanManagerCorbaDef;
    class RightsLibrary;
    class AuthenticationLibrary;
    class SortableActiveStepId;
}

namespace TA_Base_App
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "ActivePlanBulletinBoard" and that "ActivePlanBulletinBoardImpl" can
	// not be used.
    class SortablePlanId;
    class PlanDetail;

    class ActivePlanBulletinBoardImpl
	{
	// Public methods
	public:

		friend class ACE_Singleton<ActivePlanBulletinBoardImpl, ACE_Recursive_Thread_Mutex>;

		bool isRegisteredManager (const std::string& session, const std::string& pmId);
        void registerPlanManager(const std::string& session, const std::string& pmId, TA_Base_Bus::_objref_IPlanManagerCorbaDef* planManagerRef);
		void deregisterPlanManager(const std::string& session, const std::string& pmId);

		bool isOwner(const std::string& session, const TA_Base_Core::ActivePlanId& plan);

		void setControlMode( bool controlMode );

        bool isControlMode(void);

        bool existsPlan(const TA_Base_Core::ActivePlanId& plan);
		////////////////////////////////////////////////////////////////////
		//returns true for plans that come from GRC category
		//according to the Plan Management System Software Requirements spec
		//to differentiate between GRC and DSS plans is to check which category
		//a plan belongs to and if it contains any BRC steps
		//Hence, the method is implemented to reflect this requirement
		////////////////////////////////////////////////////////////////////////
		bool isGRCPlan (const TA_Base_Core::ActivePlanId& plan);
		bool isManualPlan (const TA_Base_Core::ActivePlanId& plan);

		bool isActionPermitted(const std::string& session, unsigned long actionKey, std::string& reason);
		bool isActionPermitted(const std::string& session, unsigned long resouceKey, unsigned long actionKey, std::string& reason);

		void getPlanManagerObjRef( const TA_Base_Core::ActivePlanId& plan, TA_Base_Bus::_objref_IPlanManagerCorbaDef* &pmCorbaRef);

		std::string getSession( const TA_Base_Core::ActivePlanId& plan ) const;
		std::string getPlanManagerId( const TA_Base_Core::ActivePlanId& plan ) const;
		std::string getOwner( const std::string& idString );
		unsigned long getOwnerId( const std::string& idString );
        std::string getOperatorName( const TA_Base_Core::ActivePlanId& plan );
        bool getUserKeyFromSession( const std::string& session, unsigned long& userKey );
	    unsigned long getUserKey( const TA_Base_Core::ActivePlanId& plan );

        bool isAllPlanStopped() const;
        bool isActive( const unsigned long plan,  const TA_Base_Core::ActivePlanId& ignoreInstance ) const;
        bool isActive( const TA_Base_Core::ActivePlanId& plan ) const;
        TA_Base_Core::ExecutionHistory* getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate = true);
        TA_Base_Core::CustomisationHistory* getPlanCustomisationHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate = true);
        void getActiveInstances( const unsigned long plan, std::list<TA_Base_Core::ActivePlanId>& lstActiveInstances ) const;
        TA_Base_Core::ActivePlanDetails* getActivePlanDetail( bool stateUpdate = false ) const;
        TA_Base_Core::ActivePlanDetails* getActivePlanDetail( unsigned long plan ) const;
        const TA_Base_Core::ActivePlanDetail& getActivePlanDetail( const TA_Base_Core::ActivePlanId& plan ) const;
        const TA_Base_Core::ActivePlanDetail& getAssocAlarmActivePlanDetail (const std::string& alarmId) const;	
        void setAssociatedAlarmId (const TA_Base_Core::ActivePlanId& plan, const std::string& alarmId);
        int getState( const TA_Base_Core::ActivePlanId& plan ) const;
        ACE_Future<int> getFuture( const TA_Base_Core::ActivePlanId& plan ) const;
        int waitForOperatorResponse ( const TA_Base_Core::ActivePlanId& plan, ACE_Time_Value& tv ) const;
        int waitForCompletion ( const TA_Base_Core::ActivePlanId& plan, ACE_Time_Value& tv ) const;
        int waitForTermination( unsigned long plan, ACE_Time_Value& tv ) const;

        bool reserveInstance( const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, 
            const int initialState, const ACE_Future<int>& planResult, bool manExecuted);
		bool reserveInstance( const std::string& session, const std::string& pmId, const TA_Base_Core::ActivePlanId& parentPlan, 
            TA_Base_Core::ActivePlanId& childPlan, const int initialState, const ACE_Future<int>& planResult );
        void undoReserveInstance( TA_Base_Core::ActivePlanId plan );
		void releaseInstance( const TA_Base_Core::ActivePlanId& plan );

		void initStep( const TA_Base_Core::ActivePlanId& plan, const unsigned long step, const std::string& stepName, const std::string& remark );
		void updatePlanState( const TA_Base_Core::ActivePlanId& plan,const int state, const std::string& remark );
		void updateOwner( const TA_Base_Core::ActivePlanId& plan, const std::string& session, const std::string& pmId );
		void updateStepState( const TA_Base_Core::ActivePlanId& plan, const unsigned long step, const int state, const std::string& remark );
        void updateCustom( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail );
        void updateActivePlanTimer( const TA_Base_Core::ActivePlanId& plan, const unsigned long ulTimer );
        void updateActivePlanDetail( const TA_Base_Core::ActivePlanDetail& refDetail );
		void sendStateUpdateRequestToControlAgent( void );

        void processExecutionHistoryUpdate( const TA_Base_Core::ExecutionHistoryUpdate& historyUpdate );
        void processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate );

        void clearAllPlanStates();
		void cullStoppedPlans();
        void cullAllPlans();
		void deleteMessageSenders();
		//as per TD 11324
		void deleteRightsLib (void);
        
		void setSession( const TA_Base_Core::ActivePlanId& plan, const std::string& session );
        void setPlanManagerId( const TA_Base_Core::ActivePlanId& plan, const std::string& pmId );

		bool wasHangupMessageReceived(void);
		void setHangupMessageReceived(void);

		void sendConfigUpdateMessage( const TA_Base_Core::PlanConfigUpdateData& configData );

        void sendUpdateMessage(const TA_Base_Core::ActivePlanId& plan);

		void sendStateUpdateMessage( TA_Base_Core::PASUpdateData& stateUpdateData );
		void sendStateUpdateMessage( const TA_Base_Core::ActivePlanDetail& planDetail );
        void sendStateUpdateMessage( const unsigned long updateId );
        void sendAllStatesUpdateMessage( TA_Base_Core::PASUpdateDataSeq& refStateUpdateDataSeq );
        void sendOperatorPausedStateUpdate( const TA_Base_Core::ActivePlanId& plan );
		void sendAllStateUpdatesToMonitorAgent(void);

		unsigned long getNextUpdateId(void);
        unsigned long getNextPasUpdateId(void);
        void setPasUpdateId(unsigned long updateId);
        void setUpdateId(unsigned long updateId);

        void cullStoppedPlan(TA_Base_Core::ActivePlanId plan);

        void setStepCompletionStatus(const TA_Base_Bus::SortableActiveStepId& step, const int status);
        void removeStepCompletionStatus(const TA_Base_Bus::SortableActiveStepId& step);
        int getStepCompletionStatus(const TA_Base_Bus::SortableActiveStepId& step);
        TA_Base_Core::ActiveStepCompletionStatusSeq* getStepCompletionStatusSeq();
        void updateStepResponseStatusMap(const TA_Base_Core::ActiveStepCompletionStatusSeq& statusSeq);

    private:
        ActivePlanBulletinBoardImpl();
        ~ActivePlanBulletinBoardImpl();
        ActivePlanBulletinBoardImpl( const ActivePlanBulletinBoardImpl& );
        ActivePlanBulletinBoardImpl& operator=( const ActivePlanBulletinBoardImpl& );

		typedef std::map<SortablePlanId,PlanDetail> PlanMap;
		typedef std::map<SortablePlanId,PlanDetail>::iterator PlanMapIterator;
		typedef std::map<SortablePlanId,PlanDetail>::const_iterator ConstPlanMapIterator;

		void sendUpdateMessage( const PlanDetail& detail );
		void sendCustomUpdateMessage( const TA_Base_Core::PlanCustomisationUpdate& updateData );
		void getInstances( const unsigned long plan, ConstPlanMapIterator& itLower, ConstPlanMapIterator& itUpper ) const;
		unsigned long getFirstAvailableInstanceNumber(const unsigned long plan);

        std::map<SortablePlanId,PlanDetail> m_planMap;

		// add for the error session info in monitor mode
		// @parameter [in] strCompositePmId format is: 51a7f5cd0153704b3b095a01ec524f9408ec297bc45f-fdd8-46bb-8c31-f5f88226192f
		// @parameter [out] strSessionID should be:    51a7f5cd0153704b3b095a01ec524f9408ec
		// @parameter [out] strPmID should be:                                             297bc45f-fdd8-46bb-8c31-f5f88226192f
		bool getSessionIDAndPmID(const std::string& strCompositePmId, std::string& strSessionID, std::string& strPmID);
		static const int INDICATION_SESSION_AND_PMID;

		//marvin++
		//TD13527
		//<entity key,planid> map
		typedef std::multimap<unsigned long, unsigned long> EntityPlanRestrictionMultiMap;
		typedef EntityPlanRestrictionMultiMap::iterator EntityPlanRestrictionMultiMapIterator;
		typedef EntityPlanRestrictionMultiMap::value_type EntityPlanRestrictionMultiMapValue;
		EntityPlanRestrictionMultiMap m_entityPlanRestrictionMultiMap;
		mutable ACE_RW_Mutex m_entityPlanRestrictionMultiMapGuard;

		void deleteEntityPlanMapRestrictionIfApplicable(const TA_Base_Core::ActivePlanId& plan, const int state);
        bool insertEntityPlanMapRestrictionIfNotExst(const TA_Base_Core::ActivePlanId& plan);
		//marvin++
		//TD13527

		typedef std::map<std::string, TA_Base_Bus::_objref_IPlanManagerCorbaDef*> PMOBJREFMap;
		typedef PMOBJREFMap::iterator PMOBJREFMapIterator;
		PMOBJREFMap	m_pmObjRef;
		mutable ACE_RW_Mutex m_pmObjMapLock;

		mutable ACE_RW_Mutex m_planMapLock;
		mutable ACE_Mutex m_cullLock;

		TA_Base_Core::CommsMessageSender* m_commsSender;
		TA_Base_Core::StateUpdateMessageSender* m_stateSender;

		unsigned long m_updateId;
        unsigned long m_pasUpdateId;

		mutable ACE_Mutex m_updateIdLock;
		mutable ACE_Mutex m_pasUpdateIdLock;

		bool m_hangupMessageReceived;
		bool m_isControlMode;

		TA_Base_Bus::RightsLibrary* m_rightsLibrary;
        unsigned long m_resourceId;

        TA_Base_Bus::AuthenticationLibrary* m_authenticationLib;

        typedef std::map<TA_Base_Bus::SortableActiveStepId, int> StepResponseStatusMap;
        typedef StepResponseStatusMap::iterator StepResponseStatusMapIt;
        StepResponseStatusMap m_mapStepReponseStatus;
        mutable ACE_RW_Mutex m_mtxStepResponseStatusLock;
	
	}; // ActivePlanBulletinBoardImpl
	typedef ACE_Singleton<ActivePlanBulletinBoardImpl, ACE_Recursive_Thread_Mutex> ActivePlanBulletinBoard;
} // TA_Base_App

#endif //ACTIVEPLANBULLETINBOARD_H_INCLUDED
