#ifndef PLAN_DATA_DB_WRITER_HEADER_INCLUDED
#define PLAN_DATA_DB_WRITER_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/PlanDataAccess/src/PlanDataWriter.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2015/01/23 11:17:33 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */
//

#include <string>
#include <vector>

#include "ace/Singleton.h"
#include "ace/RW_Mutex.h"

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
    class IDatabase;
}

namespace TA_Base_Bus
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "PlanDataWriter" and that "PlanDataDBWriter" can
	// not be used.

    
	class PlanDataWriterImp
	{
    public:
        // Support dynamic language switch
        static std::string CopyNameSuffix;
        static std::string NewPlanName;
        static std::string NewCategoryName;

	// Public methods
	public:

		friend class ACE_Singleton<PlanDataWriterImp, ACE_Recursive_Thread_Mutex>;

        //void setPlanApprovalState(const std::string& session, TA_Base_Core::NodeId nodeId, TA_Base_Core::EApprovalState approvalState);

		std::string getOperatorName(const std::string& session);

        void UpdatePlansDBSequence();

	//Plan Edit methods

        //void getEditPlanLock( const std::string& session, const std::string& planManagerId, TA_Base_Core::NodeId plan, bool noDBAccess = false );
		//void releaseEditPlanLock(const std::string& session,TA_Base_Core::NodeId plan, bool noDBAccess = false );
		bool isPlanBeingEdited(TA_Base_Core::NodeId plan);
        std::string setEditLockInDataBase (const std::string& session, TA_Base_Core::NodeId plan );
        void releaseEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan );

        std::string getPlanEditSession(TA_Base_Core::NodeId plan, const std::string& session);

		//void validatePlan(const TA_Base_Core::Plan& plan);

		//void deleteCategory(const std::string& session, TA_Base_Core::NodeId category);
		TA_Base_Core::NodeId insertCategory(const std::string& session, TA_Base_Core::NodeId parent);
		//void updateCategory(const std::string& session, const TA_Base_Core::CategoryDetail& detail);

		void updatePlanDetail(const std::string& session, const TA_Base_Core::PlanDetail& detail);
		void updatePlan(const std::string& session, const TA_Base_Core::Plan& plan);

		TA_Base_Core::NodeId insertPlan(const std::string& session, const TA_Base_Core::NodeId parent);
		//void deletePlans(const std::string& session, const TA_Base_Core::NodeIds& plans);
		//void movePlan(const std::string& session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce);
		//void copyPlan(const std::string& session, TA_Base_Core::NodeId plan, TA_Base_Core::NodeId parent, bool useForce );
		/////////////////////////////////////////////////////////////
		//Please note that the name for exportPlan may be a bit misleading 
		//as this method only loads relevant plan details
		//into a structure suitable for export to a .pln file
		//the actual export (file writing happens on manager's side
		/////////////////////////////////////////////////////////////
		TA_Base_Core::AuxPlanDetail* exportPlan (const std::string& session, TA_Base_Core::NodeId& planId);
		TA_Base_Core::NodeId importPlan (const std::string& session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId);

        bool insertPlanType( const std::string& session, const TA_Base_Core::NodeId plan, TA_Base_Core::EPlanType planType );
        void updatePlanType(TA_Base_Core::NodeId nodeId, TA_Base_Core::EPlanType visibleState);
        void deletePlanType(TA_Base_Core::NodeId nodeId);

        void deleteAllStepParameters(const std::string& session, const TA_Base_Core::Plan& plan);
        void deleteAllSteps(const std::string& session, const TA_Base_Core::Plan& plan);
        void insertStepDetail(const TA_Base_Core::Plan& plan, const TA_Base_Core::StepDetail& detail);
        void updateStepParameters(const TA_Base_Core::StepDetail& detail, const std::vector<std::string>& parameters);
        void replacePlan(const std::string& session, const TA_Base_Core::NodeId existingPlan, const TA_Base_Core::NodeId planToOverwrite);
        void copyPlan(const std::string& session, const TA_Base_Core::NodeId planToCopy, const TA_Base_Core::NodeId parent, const std::string planNewName);
        void updateNodeParent(const std::string& session, const TA_Base_Core::NodeId parent, const TA_Base_Core::NodeId plan);
        void deletePlan(const std::string& session, const TA_Base_Core::NodeId plan);
        void updateCategory(const std::string& session, const std::string& name, const std::string& description, const TA_Base_Core::NodeId category);
        void deleteCategory(const TA_Base_Core::NodeId category);
        void setPlanApprovalState(const std::string& session, const TA_Base_Core::NodeId plan, TA_Base_Core::EApprovalState approvalState);

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

	// Private methods
	private:
		PlanDataWriterImp();
		~PlanDataWriterImp(){}

		PlanDataWriterImp( const PlanDataWriterImp& );
		PlanDataWriterImp& operator=( const PlanDataWriterImp& );

		std::string parseForApostrophe(const std::string& inString);

        std::string makeNumericDataPointValueString(const TA_Base_Core::NumericDataPoint& ndp);
		TA_Base_Core::IDatabase* getDatabase() const;

        //TA_Base_Core::NodeId getPlanToReplace(const std::string& planName, TA_Base_Core::NodeId parent, bool useForce);

        TA_Base_Core::StepNumber getStepNumber(const TA_Base_Core::Plan& plan, TA_Base_Core::StepId step) const;

    // Private data
    private:
		mutable ACE_RW_Mutex m_planInsertLock;
	}; // PlanDataDBWriter

	typedef ACE_Singleton<PlanDataWriterImp, ACE_Recursive_Thread_Mutex> PlanDataWriter;

} // TA_Base_Bus

#endif //PLAN_DATA_DB_WRITER_HEADER_INCLUDED
