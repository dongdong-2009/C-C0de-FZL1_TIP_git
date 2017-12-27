#ifndef PLAN_DATA_WRITER_HEADER_INCLUDED
#define PLAN_DATA_WRITER_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Mahaveer Pareek
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */
//

#include <string>

#include "ace/Singleton.h"
#include "ace/RW_Mutex.h"

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_App
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "PlanDataWriter" and that "PlanDataWritingClass" can
	// not be used.

    
	class PlanDataWritingClass
	{
	// Public methods
	public:

		friend class ACE_Singleton<PlanDataWritingClass, ACE_Recursive_Thread_Mutex>;

        void setPlanApprovalState(const std::string& session, TA_Base_Core::NodeId nodeId, TA_Base_Core::EApprovalState approvalState);

        void UpdatePlansDBSequence();

	//Plan Edit methods
		
        void getEditPlanLock( const std::string& session, const std::string& planManagerId, TA_Base_Core::NodeId plan, bool noDBAccess = false );
		void releaseEditPlanLock(const std::string& session,TA_Base_Core::NodeId plan, bool noDBAccess = false );
		bool isPlanBeingEdited(TA_Base_Core::NodeId plan);
        std::string setEditLockInDataBase (const std::string& session, TA_Base_Core::NodeId plan );
        bool refreshEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan);
        void releaseEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan );

		void CheckPlanEditSession(TA_Base_Core::NodeId plan, const std::string& session);

		void validatePlan(const TA_Base_Core::Plan& plan);

		void deleteCategory(const std::string& session, TA_Base_Core::NodeId category);
		void insertCategory(const std::string& session, TA_Base_Core::NodeId parent);
		void updateCategory(const std::string& session, const TA_Base_Core::CategoryDetail& detail);

		void updatePlanDetail(const std::string& session, const TA_Base_Core::PlanDetail& detail);
		void updatePlan(const std::string& session, const TA_Base_Core::Plan& plan);

		void insertPlan(const std::string& session, const TA_Base_Core::NodeId parent);
		void deletePlans(const std::string& session, const TA_Base_Core::NodeIds& plans);
		void movePlan(const std::string& session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce);
		void copyPlan(const std::string& session, TA_Base_Core::NodeId plan, TA_Base_Core::NodeId parent, bool useForce );
        /////////////////////////////////////////////////////////////
		//Please note that the name for exportPlan may be a bit misleading 
		//as this method only loads relevant plan details
		//into a structure suitable for export to a .pln file
		//the actual export (file writing happens on manager's side
		/////////////////////////////////////////////////////////////
		TA_Base_Core::AuxPlanDetail* exportPlan (const std::string& session, TA_Base_Core::NodeId& planId);
		void importPlan (const std::string& session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId);
        void setPlanType( const std::string& session, TA_Base_Core::NodeId nodeId, TA_Base_Core::EPlanType visibleState);

	// Private methods
	private:
		PlanDataWritingClass();
		~PlanDataWritingClass(){}

		PlanDataWritingClass( const PlanDataWritingClass& );
		PlanDataWritingClass& operator=( const PlanDataWritingClass& );

        /** 
          * getEditLockOwner
          *
          * Returns the session that has owns the edit lock on the plan
          *
          * @param plan The node Id of the plan
          *
          * @return The session Id
          */
        std::string getEditLockOwner(TA_Base_Core::NodeId plan);

		std::string getNewNodeName(TA_Base_Core::NodeId parent, TA_Base_Core::ENode nodeType);
		std::string getNodeNameToCopy(TA_Base_Core::NodeId node, TA_Base_Core::NodeId parent);

		std::string parseForApostrophe(const std::string& inString);

        std::string makeNumericDataPointValueString(const TA_Base_Core::NumericDataPoint& ndp);

        TA_Base_Core::NodeId getPlanToReplace(const std::string& planName, TA_Base_Core::NodeId parent, bool useForce);

        TA_Base_Core::StepNumber getStepNumber(const TA_Base_Core::Plan& plan, TA_Base_Core::StepId step) const;

    // Private data
    private:
		mutable ACE_RW_Mutex m_planInsertLock;
	}; // PlanDataWritingClass

	typedef ACE_Singleton<PlanDataWritingClass, ACE_Recursive_Thread_Mutex> PlanDataWriter;

} // TA_Base_App

#endif //PLAN_DATA_WRITER_HEADER_INCLUDED
