/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/PlanDataAccess/src/PlanDataWriter.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2015/01/23 11:17:33 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */
#include <iomanip>

#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "core/exceptions/src/PlanStepDetailException.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataWriter.h"


namespace TA_Base_Bus
{
    const unsigned long PLANEDITLOCK_TIMEOUT_SECONDS = 60;
    const char* const PLAN_EDIT_LOCK_SEPARATOR = ":";
    const char* const EMPTY_STRING = "";

    std::string PlanDataWriterImp::CopyNameSuffix = " - Copy";
    std::string PlanDataWriterImp::NewPlanName = "New Plan";
    std::string PlanDataWriterImp::NewCategoryName = "New Category";

    PlanDataWriterImp::PlanDataWriterImp()
    {
        UpdatePlansDBSequence();
    }


    void PlanDataWriterImp::UpdatePlansDBSequence()
    {
		//std::string sql = "call update_plans_seq('plans_seq')";
		TA_Base_Core::IDatabase* dataBase = getDatabase();
//		std::string sql = dataBase->prepareSQLStatement(STEP_DETAILES_UPDATE_63509);
		TA_Base_Core::SQLStatement sql;
		dataBase->prepareSQLStatement(sql, STEP_DETAILES_UPDATE_63509);
		dataBase->executeProcedure(sql);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", "The Database sequence[plans_seq] has been updated to the max of p_key[node_tree, step_details, step_parameters].");
    }

    std::string PlanDataWriterImp::getEditLockOwner(TA_Base_Core::NodeId plan)
    {
		TA_Base_Core::IDatabase* database = getDatabase();

        /*std::ostringstream sql;

        sql << "select edit_lock_owner from node_tree where p_key = "
            << plan; */
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33503, plan);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33503, plan);

		std::vector<std::string> columns;
		columns.push_back("edit_lock_owner");

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
            return EMPTY_STRING;
        }

        return data->getStringData(0, columns[0] );
    }

	bool PlanDataWriterImp::isPlanBeingEdited(TA_Base_Core::NodeId plan)
	{
		TA_Base_Core::IDatabase* database = getDatabase();
       /* std::ostringstream sql;

		sql << "select is_plan_edit_locked("
            << plan << ","
            << PLANEDITLOCK_TIMEOUT_SECONDS << ") isPlanEditLocked from dual";*/
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33551, 
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33551,
			plan, PLANEDITLOCK_TIMEOUT_SECONDS);

		std::vector<std::string> columns;
		columns.push_back("isPlanEditLocked");

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database);

		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
            TA_THROW(TA_Base_Core::DataException("The plan does not exist.", TA_Base_Core::DataException::NO_VALUE,""));
        }

        int nEditing = data->getUnsignedLongData(0, columns[0] );

        return ( 0 != nEditing );
	}


    std::string PlanDataWriterImp::setEditLockInDataBase (const std::string& session, TA_Base_Core::NodeId plan )
    {
		TA_Base_Core::IDatabase* database = getDatabase();

       /* std::ostringstream sql;
		sql << "call Set_Plan_Edit_Lock("
            << plan << ",'"
            << session.c_str() << "',"
            << PLANEDITLOCK_TIMEOUT_SECONDS << ")";*/

		//std::string strSQL = sql.str();
//		std::string strSql = database->prepareSQLStatement(NODE_TREE_SELECT_33504, 
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33504,
			plan, session, PLANEDITLOCK_TIMEOUT_SECONDS);
		database->executeProcedure(strSql);

        //sql.str("");

       /* sql << "select edit_lock_owner from node_tree where p_key = "
            << plan; */
//		strSql = database->prepareSQLStatement(NODE_TREE_SELECT_33503, plan);

		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33503, plan);

		std::vector<std::string> columns;

		columns.push_back("edit_lock_owner");

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
            TA_THROW(TA_Base_Core::DataException("The plan does not exist.", TA_Base_Core::DataException::NO_VALUE,""));
        }

        return data->getStringData(0, columns[0] );
    }


    void PlanDataWriterImp::releaseEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan )
    {
		TA_Base_Core::IDatabase* database = getDatabase();
        /*std::ostringstream sql1;
        sql1 << "select nvl(edit_lock_owner, ' ') from node_tree where p_key = "
            << plan; */
//		std::string strSelectSql = database->prepareSQLStatement(NODE_TREE_SELECT_33552, plan);
		TA_Base_Core::SQLStatement strSelectSql;
		database->prepareSQLStatement(strSelectSql, NODE_TREE_SELECT_33552, plan);
          
		std::vector<std::string> columns;
		columns.push_back("edit_lock_owner");

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSelectSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
            TA_THROW(TA_Base_Core::DataException("The plan does not exist.", TA_Base_Core::DataException::NO_VALUE,""));
        }

        std::string lockOwner = data->getStringData(0, columns[0] );

        if (lockOwner.find(session) != 0)
        {
            TA_THROW(TA_Base_Core::PlanAgentException(lockOwner, TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR));
        }

       /* std::ostringstream sql2;
        sql2 << "update node_tree set edit_lock_owner = NULL, edit_time_stamp = NULL where p_key = "
            << plan;*/
//		std::string strUpdSql = database->prepareSQLStatement(NODE_TREE_UPDATE_33651, plan);
		TA_Base_Core::SQLStatement strUpdSql;
		database->prepareSQLStatement(strUpdSql, NODE_TREE_UPDATE_33651, plan);

		database->executeProcedure(strUpdSql);

        //Check whether the lock has really been released
		std::auto_ptr<TA_Base_Core::IData> data1( database->executeQuery( strSelectSql, columns ) );

		if( (NULL == data1.get() ) || (1 != data1->getNumRows()) ) 
        {
            TA_THROW(TA_Base_Core::DataException("The plan does not exist.", TA_Base_Core::DataException::NO_VALUE,""));
        }

        lockOwner = data1->getStringData(0, columns[0] );

        if (lockOwner.compare(session) == 0)
        {
            TA_THROW(TA_Base_Core::PlanAgentException("Error: Could not release the edit lock. Check database connection", 
                TA_Base_Core::PAE_CANNOT_RELEASE_EDIT_LOCK));
        }
    }

    std::string PlanDataWriterImp::getPlanEditSession(TA_Base_Core::NodeId plan, const std::string& session)
	{
		TA_Base_Core::IDatabase* database = getDatabase();
       /* std::ostringstream sql;
        sql << "select nvl(edit_lock_owner, ' ') from node_tree where p_key = "
            << plan; */
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33552, plan);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33552, plan);

		std::vector<std::string> columns;
		columns.push_back("edit_lock_owner");

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
            TA_THROW(TA_Base_Core::DataException("The plan does not exist.", TA_Base_Core::DataException::NO_VALUE,""));
        }

        return data->getStringData(0, columns[0] );
	}

	void PlanDataWriterImp::updatePlanDetail(const std::string& session, const TA_Base_Core::PlanDetail& detail)
	{
		TA_Base_Core::IDatabase* dataBase = getDatabase();

	/*	std::ostringstream sql;

		sql << "update node_tree set name = '"
			<< parseForApostrophe(detail.name.in()).c_str()
			<< "',description = '"
			<< parseForApostrophe(detail.description.in()).c_str()
			<< "',fk_approval_state = "
			<< " ( select p_key from approval_state where state_value = "
			<< detail.approvalState << "), "
			<< "node_modified_by = '"
            << getOperatorName(session).c_str() << "', "
            << "date_modified = sysdate "
			<< " where p_key = " << detail.plan;*/
//		std::string strSql  = dataBase->prepareSQLStatement(NODE_TREE_UPDATE_33652, 
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql, NODE_TREE_UPDATE_33652,
			parseForApostrophe(detail.name.in()), parseForApostrophe(detail.description.in()), detail.approvalState,
			getOperatorName(session), detail.plan);

		dataBase->executeModification(strSql);
	}


	void PlanDataWriterImp::updatePlan(const std::string& session, const TA_Base_Core::Plan& plan)
	{
		// Delete all the steps of the plans
		// Update the details of the plan
		TA_Base_Core::IDatabase* dataBase = getDatabase();

	/*	std::ostringstream sql;
		sql << "update node_tree set name = '"
			<< parseForApostrophe(plan.detail.name.in()).c_str() 
			<< "',description = '"
			<< parseForApostrophe(plan.detail.description.in()).c_str() 
			<< "',fk_approval_state = "
			<< " ( select p_key from approval_state where state_value = "
			<< plan.detail.approvalState << "), "
			<< "node_modified_by = '"
            << getOperatorName(session).c_str() << "', "
            << "date_modified = sysdate "
			<< " where p_key = " << plan.detail.plan;*/
//		std::string strSql = dataBase->prepareSQLStatement(NODE_TREE_UPDATE_33652, 
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql, NODE_TREE_UPDATE_33652,
			parseForApostrophe(plan.detail.name.in()), parseForApostrophe(plan.detail.description.in()), 
			plan.detail.approvalState, getOperatorName(session), plan.detail.plan);

		dataBase->executeModification(strSql);
	}

    void PlanDataWriterImp::copyPlan( const std::string& session, const TA_Base_Core::NodeId planToCopy, const TA_Base_Core::NodeId parent, const std::string planNewName )
    {
        TA_Base_Core::IDatabase* pDb = getDatabase();			
        //std::ostringstream sql;
        TA_Base_Core::SQLStatement strSql;

        pDb->prepareSQLStatement(strSql, STEP_DETAILES_UPDATE_63502, planToCopy, parent,
            parseForApostrophe(planNewName), getOperatorName(session));

        pDb->executeProcedure(strSql);
    }


    TA_Base_Core::AuxPlanDetail* PlanDataWriterImp::exportPlan(const std::string& session, TA_Base_Core::NodeId& planId)
    {
        std::string nvlValue = EMPTY_STRING;
        if ( TA_Base_Bus::PlanDataReader::instance()->getNodeType(planId) != TA_Base_Core::PLAN_NODE)
        {
            std::ostringstream message;
            message << "The node [" << planId << "] being exported is not of type PLAN.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Core::AuxPlanDetail_var auxPlanDetail = new TA_Base_Core::AuxPlanDetail();

        //get plan details from DB
        TA_Base_Core::IDatabase* database = getDatabase();

        //std::ostringstream sql;
        std::vector<std::string> columns;
        columns.push_back( "name" );
        columns.push_back( "description" );
        columns.push_back( "StepNum" );

		/* sql << "select name, nvl(description, '" << nvlValue.c_str() << "')"
          << " from plan_details where node_id = " << planId;*/
//		 std::string strSql = database->prepareSQLStatement(NODE_TREE_SELECT_33553, nvlValue, planId);
        TA_Base_Core::SQLStatement strSql;
        database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33553, nvlValue, planId);

        std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
        TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

        if(( NULL == data.get() ) || ( 0u == data->getNumRows() )) 
        {
            std::ostringstream message;
            message << "Cannot find plan from node_tree of which p_key is %lu" << planId;
            TA_THROW(TA_Base_Core::DataException(message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "node_tree.p_key"));
        }

        {
            int nDetailIndex = 0;
            auxPlanDetail->name = data->getStringData( 0, columns[nDetailIndex++]).c_str();
            auxPlanDetail->description = data->getStringData(0, columns[nDetailIndex++]).c_str();
            auxPlanDetail->steps.length( data->getUnsignedLongData(0, columns[nDetailIndex++]) );
        }

        // Step details query
	/*	stepSql << "select step_details.position, step_details.name,"
              << " step_type.type_value, nvl(step_details.description, '" << nvlValue.c_str() << "'),"
			     << " step_details.skip, step_details.skippable, step_details.delay,"
              << " nvl(step_details.ignore_failure, 0), nvl(step_details.no_wait, 0) "
			     << " from step_details, node_tree, step_type"
			     << " where step_details.fk_node_tree = node_tree.p_key"
			     << " and step_details.fk_step_type = step_type.p_key"
			     << " and step_details.fk_node_tree = " << planId;

		 stepSql << " order by step_details.position";*/
//		 strSql = database->prepareSQLStatement(STEP_DETAILES_SELECT_63551, nvlValue, planId);

        database->prepareSQLStatement(strSql, STEP_DETAILES_SELECT_63551, nvlValue, planId);

        columns.clear();
        columns.push_back( "position" );
        columns.push_back( "name" );
        columns.push_back( "type" );
        columns.push_back( "description" );
        columns.push_back( "skip" );
        columns.push_back( "skippable" );
        columns.push_back( "delay" ); 
        columns.push_back( "ignore_failure" ); 
        columns.push_back( "no_wait" );

		TA_Base_Core::IData* stepData = database->executeQuery( strSql, columns ) ;

        do
        {
            if ( NULL == stepData ) 
            {
                break;
            }

            std::auto_ptr<TA_Base_Core::IData> dataPtr(stepData);

            if ( 0u == dataPtr->getNumRows() )
            {
                break;
            }

            for( unsigned long dataIdx = 0; dataIdx < dataPtr->getNumRows(); dataIdx++ )
            {
                int nIndex = 0;
                unsigned long ulPos = dataPtr->getUnsignedLongData( dataIdx, columns[nIndex++] );
                if ( ulPos > auxPlanDetail->steps.length() )
                {
                    continue;
                }

                TA_Base_Core::AuxStepDetail& refPlanStep = auxPlanDetail->steps[ulPos - 1u];

                refPlanStep.position = ulPos;
                refPlanStep.name = dataPtr->getStringData( dataIdx, columns[nIndex++] ).c_str();
                refPlanStep.type = static_cast<TA_Base_Core::EStep>( dataPtr->getUnsignedLongData( dataIdx, columns[nIndex++]));
                refPlanStep.description = dataPtr->getStringData(dataIdx, columns[nIndex++] ).c_str();
                refPlanStep.skip = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[nIndex++] ) );
                refPlanStep.skippable = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[nIndex++] ) );
                refPlanStep.delay = static_cast<TA_Base_Core::Seconds>( dataPtr->getUnsignedLongData( dataIdx, columns[nIndex++] ) );
                refPlanStep.ignoreFailure = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[nIndex++] ) );
                refPlanStep.nowait = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[nIndex++] ) ); 
            }

            dataPtr.reset();		// ensure memory is released
            stepData = NULL;
        }
        while ( database->moreData( stepData ) );

        std::vector<std::string> paramCol;
        paramCol.push_back( "P_KEY" );
        paramCol.push_back( "POSITION" );
        paramCol.push_back( "TYPE_VALUE" );
        paramCol.push_back( "PARAMETERPOS" );
        paramCol.push_back( "PARAMETERVALUE" );

		database->prepareSQLStatement(strSql, STEP_PARAMETERS_SELECT_63051, nvlValue, planId);

        TA_Base_Core::IData* paramData = database->executeQuery(strSql, paramCol);

        do
        {
            if ( NULL == paramData )
            {
                break;
            }

            std::auto_ptr<TA_Base_Core::IData> dataPtr(paramData);

            if ( 0u == dataPtr->getNumRows() )
            {
                break;
            }

            for( unsigned int unParamIndex = 0u; unParamIndex < dataPtr->getNumRows(); ++unParamIndex )
            {
                // Step position base on 1
                unsigned long ulStepPos = dataPtr->getUnsignedLongData( unParamIndex, "POSITION" ) - 1u;
                if ( ulStepPos >= auxPlanDetail->steps.length() )
                {
                    continue;
                }

                TA_Base_Core::AuxStepDetail& refStep = auxPlanDetail->steps[ulStepPos];
                unsigned long ulParamPos = dataPtr->getUnsignedLongData( unParamIndex, "PARAMETERPOS" );
                if ( ulParamPos > refStep.stepParams.length() )
                {
                    refStep.stepParams.length( ulParamPos );
                }

                TA_Base_Core::AuxStepParam& refStepParam = refStep.stepParams[ulParamPos - 1u];
                refStepParam.position = ulParamPos;
                refStepParam.value = dataPtr->getStringData( unParamIndex, "PARAMETERVALUE" ).c_str();
            }

            dataPtr.reset();
            paramData = NULL;
        }
        while ( database->moreData( paramData ));

        return auxPlanDetail._retn();
    }

	TA_Base_Core::NodeId PlanDataWriterImp::importPlan (const std::string& session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId)
	{
		//first insert imported plan into selected category
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planInsertLock);

		std::string name = plan.name.in ();
		std::string description = plan.description.in ();
		//every plan imported is to have UNAPPROVED state to start with
		TA_Base_Core::EApprovalState approvalState = TA_Base_Core::UNAPPROVED_APPROVAL_STATE;
		unsigned long importStep = 0;//when importing don't add an extra end step in

	/*	std::ostringstream sql;

		sql << "call insert_plan("
			 << TA_Base_Core::PLAN_NODE << ","
			 << parentId << ",'"
			 << name.c_str() << "','"
			 << description.c_str() << "',"
			 << approvalState << ",'"
          << getOperatorName(session).c_str() << "',"
			 << importStep 
          << ")";

		std::string strSQL = sql.str();
		std::cout << strSQL.c_str() << std::endl;*/

		TA_Base_Core::IDatabase* dataBase = getDatabase();

//		std::string strSql = dataBase->prepareSQLStatement(STEP_DETAILES_UPDATE_63503, TA_Base_Core::PLAN_NODE,
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql, STEP_DETAILES_UPDATE_63503, TA_Base_Core::PLAN_NODE,
			parentId, name, description, approvalState, getOperatorName(session), importStep);
		dataBase->executeProcedure(strSql);

		//We have successfully inserted the new plan, let us retrieve its NodeId

	/*	sql.str("");
		sql << "select p_key from node_tree where parent_node = "
			 << parentId << " and name = '"
			 << name.c_str() << "'";*/
//		strSql = dataBase->prepareSQLStatement(NODE_TREE_SELECT_33505, parentId, name);

		dataBase->prepareSQLStatement(strSql, NODE_TREE_SELECT_33505, parentId, name);

        std::vector<std::string> columns;
        columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( strSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
            TA_THROW(TA_Base_Core::PlanAgentException("Failed to import a new Plan", TA_Base_Core::PAE_CANNOT_CREATE_PLAN));
        }

		//get the new plan id
		TA_Base_Core::NodeId planId = data->getUnsignedLongData( 0, columns[0] );
		
		//now update plan step details
		for ( unsigned int i = 0;  i < plan.steps.length () ; i++ )
		{	
			//std::ostringstream sqlstep;
			TA_Base_Core::StepId stepId = TA_Base_Bus::PlanDataReader::instance()->getNextPlanSeqNumber();
			
		/*	sqlstep << "insert into step_details ( p_key, fk_step_type, fk_node_tree,"
				    << "position, description, skip, skippable, delay, name, ignore_failure, no_wait ) "
					<< " values "
					<< "( " << stepId 
					<< ",(select p_key from step_type where type_value =" << plan.steps[i].type << ")"
					<< ","  << planId
					<< ","  << plan.steps[i].position
					<< ",'" << parseForApostrophe(plan.steps[i].description.in()).c_str()
					<< "'," << plan.steps[i].skip
					<< ","  << plan.steps[i].skippable
					<< ","  << plan.steps[i].delay
					<< ",'" << parseForApostrophe(plan.steps[i].name.in()).c_str()
               << "'"
					<< ","  << plan.steps[i].ignoreFailure
					<< ","  << plan.steps[i].nowait
					<< ")";*/
//			strSql = dataBase->prepareSQLStatement(STEP_DETAILES_INSERT_63801, stepId, plan.steps[i].type, planId,

		        dataBase->prepareSQLStatement(strSql, STEP_DETAILES_INSERT_63801, stepId, plan.steps[i].type, planId,
				plan.steps[i].position, parseForApostrophe(plan.steps[i].description.in()), plan.steps[i].skip, 
				plan.steps[i].skippable, plan.steps[i].delay, parseForApostrophe(plan.steps[i].name.in()), 
				plan.steps[i].ignoreFailure, plan.steps[i].nowait);

			dataBase->executeModification(strSql);

			//and step params
			for (unsigned int pIdx = 0; pIdx < plan.steps[i].stepParams.length (); pIdx++)
			{
				/*std::ostringstream sqlParams;
				sqlParams << "insert into step_parameters (p_key, fk_step_details, position, value) "
					       << " values "
							 << "( plans_seq.nextval, " << stepId  
							 << "," 
							 << plan.steps[i].stepParams[pIdx].position
							 << ",'" << parseForApostrophe (plan.steps[i].stepParams[pIdx].value.in ()) << "')";*/
//				strSql = dataBase->prepareSQLStatement( STEP_PARAMETERS_INSERT_63351, stepId, 

		dataBase->prepareSQLStatement(strSql,  STEP_PARAMETERS_INSERT_63351, stepId,
					plan.steps[i].stepParams[pIdx].position, parseForApostrophe (plan.steps[i].stepParams[pIdx].value.in ()));

				dataBase->executeModification(strSql);
			}
		}

        return planId;
	}


	TA_Base_Core::NodeId PlanDataWriterImp::insertPlan(const std::string& session, const TA_Base_Core::NodeId parent)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planInsertLock);
		std::string name = getNewNodeName(parent, TA_Base_Core::PLAN_NODE);
		std::string description = name;
		description +=  " Description";
		TA_Base_Core::EApprovalState approvalState = TA_Base_Core::UNAPPROVED_APPROVAL_STATE;

	/*	std::ostringstream sql;

		sql << "call insert_plan("
			<< TA_Base_Core::PLAN_NODE << ","
			<< parent << ",'"
			<< name.c_str() << "','"
			<< description.c_str() << "',"
			<< approvalState << ",'"
            << getOperatorName(session).c_str()
            << "')";

		std::string strSQL = sql.str();
		std::cout << strSQL.c_str() << std::endl;*/

		TA_Base_Core::IDatabase* dataBase = getDatabase();

//		std::string strSql = dataBase->prepareSQLStatement( STEP_DETAILES_UPDATE_63503, 
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql,  STEP_DETAILES_UPDATE_63503,
			TA_Base_Core::PLAN_NODE, parent, name, description, approvalState, getOperatorName(session), 1);
		dataBase->executeProcedure(strSql);

		//We have successfully inserted the new plan, let us retrieve its NodeId
		//and send the update

	/*	sql.str("");
		sql << "select p_key from node_tree where parent_node = "
			<< parent << " and name = '"
			<< name.c_str() << "'";*/
//		strSql = dataBase->prepareSQLStatement( NODE_TREE_SELECT_33505, parent, name);

		dataBase->prepareSQLStatement(strSql,  NODE_TREE_SELECT_33505, parent, name);

        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( strSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
            std::ostringstream message;
			message << "Failed to create a new Plan. Please check the plans_seq value, the current"
                    << " value might be conflicting with the existing entries in node_tree or step_details tables.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_CREATE_PLAN));
        }

        return data->getUnsignedLongData( 0, columns[0] );
	}

    bool PlanDataWriterImp::insertPlanType( const std::string& session, const TA_Base_Core::NodeId plan, TA_Base_Core::EPlanType planType)
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();
		TA_Base_Core::SQLStatement strSql;

        /*std::string strSql = dataBase->prepareSQLStatement( PLAN_TYPE_MAP_INSERT_80004, 
            plan, planType);*/

		dataBase->prepareSQLStatement(strSql, PLAN_TYPE_MAP_INSERT_80004, plan, planType );

        dataBase->executeModification(strSql);

        dataBase->prepareSQLStatement(strSql, PLAN_TYPE_MAP_SELECT_80002, plan);

        std::vector<std::string> columns;
        columns.push_back( "fk_plan_type" );

        std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( strSql, columns ) );
        TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase);

        if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Insert plan type fail for node %lu", plan);
            return false;
        }

        return true;
    }

    void PlanDataWriterImp::updateCategory( const std::string& session, const std::string& name, const std::string& description, const TA_Base_Core::NodeId category )
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();

        //		std::string strSql  = dataBase->prepareSQLStatement(NODE_TREE_UPDATE_33654, name, description, category);
        TA_Base_Core::SQLStatement strSql;
        dataBase->prepareSQLStatement(strSql, NODE_TREE_UPDATE_33654, name, description, category);

        dataBase->executeModification(strSql);
    }


    TA_Base_Core::NodeId PlanDataWriterImp::insertCategory(const std::string& session, TA_Base_Core::NodeId parent)
	{
		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
        {
            std::ostringstream message;
			message << "Error: The parent [" << parent << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_PARENT_NOT_CATEGORY));
		}

		std::string name = getNewNodeName(parent, TA_Base_Core::CATEGORY_NODE);
		std::string description = name;
		description +=  " Description";
		/*std::ostringstream sql;

		sql << "insert into node_tree "
			<< " (p_key, fk_node_type, parent_node, name, description, fk_approval_state, node_created_by) "
			<< " values "
			<< " ( plans_seq.nextval, (select node_type.p_key from node_type where node_type.TYPE_VALUE = "
			<< TA_Base_Core::CATEGORY_NODE
			<< ") , " << parent << " ,'" 
			<< name.c_str()
			<< "','" << description.c_str() << "',"
			<< " (select approval_state.p_key from approval_state where approval_state.state_value = "
			<< TA_Base_Core::UNDEFINED_APPROVAL_STATE << "),'"
            << getOperatorName(session).c_str() 
            << "')";*/

		TA_Base_Core::IDatabase* dataBase = getDatabase();

//		std::string strSql = dataBase->prepareSQLStatement( NODE_TREE_INSERT_33851, TA_Base_Core::CATEGORY_NODE,
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql,  NODE_TREE_INSERT_33851, TA_Base_Core::CATEGORY_NODE,
			parent, name, description, TA_Base_Core::UNDEFINED_APPROVAL_STATE, getOperatorName(session));
		dataBase->executeModification(strSql);

		//We have successfully inserted the new category, let us retrieve its NodeId
		/*sql.str("");
		sql << "select p_key from node_tree where parent_node = "
			<< parent << " and name = '"
			<< name.c_str() << "'";*/
//		strSql = dataBase->prepareSQLStatement( NODE_TREE_SELECT_33505, parent, name);

		dataBase->prepareSQLStatement(strSql,  NODE_TREE_SELECT_33505, parent, name);

        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( strSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			TA_THROW( TA_Base_Core::DataException( "Failed to insertCategory", TA_Base_Core::DataException::NO_VALUE, columns[0] ) );
        }

		return data->getUnsignedLongData( 0, columns[0] );
	}


    void PlanDataWriterImp::deleteCategory( const TA_Base_Core::NodeId category )
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();

        //std::string strSQL = sql.str();
        //		std::string strSql  = dataBase->prepareSQLStatement(NODE_TREE_DELETE_33901, category);
        TA_Base_Core::SQLStatement strSql;
        dataBase->prepareSQLStatement(strSql, NODE_TREE_DELETE_33901, category);

        dataBase->executeModification(strSql);
    }


    void PlanDataWriterImp::setPlanApprovalState( const std::string& session, const TA_Base_Core::NodeId plan, TA_Base_Core::EApprovalState approvalState )
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();
        TA_Base_Core::SQLStatement strSql;

        dataBase->prepareSQLStatement(strSql, NODE_TREE_UPDATE_33701, ( approvalState + 1 ),
            getOperatorName(session), plan);

        dataBase->executeModification(strSql);
    }


	std::string PlanDataWriterImp::getNodeNameToCopy(TA_Base_Core::NodeId node, TA_Base_Core::NodeId parent)
	{
		TA_Base_Core::IDatabase* dataBase = getDatabase();

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(node);
		std::string originalName = ns->name.in();
        std::string newName = originalName + CopyNameSuffix;

	/*	std::ostringstream sql;
		sql << "select name from node_tree where parent_node = "
			<< parent
			<< " and ( name = '" << parseForApostrophe(originalName) << "' "
			<< " or name like '" << parseForApostrophe(copyNamePrefix.c_str()) << "%')";*/
//		std::string strSql  = dataBase->prepareSQLStatement(NODE_TREE_SELECT_33506, parent,
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql, NODE_TREE_SELECT_33506, parent,
			parseForApostrophe(originalName), parseForApostrophe(newName.c_str()));

		std::vector <std::string> columns;
		columns.push_back("name");

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( strSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase);

		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
        {
			return originalName;
        }

		unsigned int nRows =  data->getNumRows();
		std::string name;
		std::ostringstream newNameStrm;
		unsigned long dataIdx = 0;
		newNameStrm << newName.c_str();

		bool foundMatch = false;
		bool foundNewName = false;

		unsigned long Index = 2;

		while (!foundNewName)
		{
			for( dataIdx = 0; dataIdx < nRows; dataIdx++ )
			{
				name = data->getStringData( dataIdx, columns[0] );
				int result = name.compare(newName);

				if (result == 0)
				{
					foundMatch = true;
					break;
				}
			}

			if (foundMatch)
			{
				foundMatch = false;
				newNameStrm.str(EMPTY_STRING);
                newNameStrm << originalName << CopyNameSuffix << "(" << Index++ << ") ";
			}
			else
			{
				foundNewName = true;
			}

			newName = newNameStrm.str().c_str();
		}

		return newName;
	}

	std::string PlanDataWriterImp::getNewNodeName(TA_Base_Core::NodeId parent, TA_Base_Core::ENode nodeType)
	{
		TA_Base_Core::IDatabase* dataBase = getDatabase();

		std::string nodeName = "";

		if (nodeType == TA_Base_Core::PLAN_NODE)
        {
			nodeName = NewPlanName;
        }
		else if (nodeType == TA_Base_Core::CATEGORY_NODE)
        {
			nodeName = NewCategoryName;
        }

	/*	std::ostringstream sql;
		sql << "select name from node_tree where parent_node = "
			<< parent
			<< " and "
			<< " fk_node_type = (select p_key from node_type where type_value = "
			<< nodeType
			<< ") "
			<< "and name like '" << nodeName.c_str() << "%'";*/
//		std::string strSql  = dataBase->prepareSQLStatement(NODE_TREE_SELECT_33507, parent, nodeType, nodeName);
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql, NODE_TREE_SELECT_33507, parent, nodeType, nodeName);

		std::vector <std::string> columns;

		columns.push_back("name");

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( strSql, columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
        {
			return nodeName;
        }

		unsigned int nRows =  data->getNumRows();

		std::string name;
		std::ostringstream newNameStrm;
		std::string newName;

		int iPos = -1;
		unsigned long dataIdx = 0;
		newNameStrm << nodeName.c_str();
		newName = nodeName;

		bool foundMatch = false;
		bool foundNewName = false;
		unsigned long Index = 2;

		while (!foundNewName)
		{
			for( dataIdx = 0; dataIdx < nRows; dataIdx++ )
			{
				name = data->getStringData( dataIdx, columns[0] );
				int result = name.compare(newName);

				if (result == 0)
				{
					foundMatch = true;
					break;
				}
			}

			if (foundMatch)
			{
				foundMatch = false;
				newNameStrm.str(EMPTY_STRING);
				newNameStrm << nodeName.c_str();
				newNameStrm << " (" << Index++ << ")";				
			}
			else
			{
				foundNewName = true;
			}
			newName = newNameStrm.str();
		}

		return newName;
	}


	TA_Base_Core::IDatabase* PlanDataWriterImp::getDatabase() const
	{   
		TA_Base_Core::IDatabase* database = NULL;
		try
		{
            database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Write);
		}
		catch ( ... )
		{
			std::string message = "Failed to connect to the database.";
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", message.c_str() );
			TA_THROW(  TA_Base_Core::PlanAgentException (message.c_str() ,TA_Base_Core::PAE_DATABASE_ERROR) );
		}

		return database;
	}


	std::string PlanDataWriterImp::parseForApostrophe(const std::string& inString)
	{
		std::string retValue = EMPTY_STRING;
		int iPos = 0;
		int nextPos = 0;
		std::string temp = EMPTY_STRING;

		while ( (nextPos = inString.find_first_of("'",iPos)) != std::string::npos ) 
		{
			temp = inString.substr(iPos, nextPos-iPos);
			retValue += temp;
			retValue += "''";
			iPos = nextPos+1;
		}

		if (retValue.empty())
        {
			retValue = inString;
        }
		else
        {
			retValue += inString.substr(iPos, inString.length());
        }

		return retValue;
	}


    std::string PlanDataWriterImp::getOperatorName(const std::string& session)
    {
        try
        {
            // The format is "<operator name> (<base profile>[ / <override profile>])"
            std::auto_ptr<TA_Base_Core::ISession> ptrSession( TA_Base_Core::SessionAccessFactory::getInstance().getSession( session ));
            std::string strOperator = ptrSession->getOperatorName();
            std::string strProfile = EMPTY_STRING;

            try
            {
                std::auto_ptr<TA_Base_Core::IProfile> iProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(ptrSession->getProfileKey(), false));
                strProfile = iProfile->getName();

                unsigned long ulOverrideProfile = ptrSession->getOverrideProfileKey();
                if ( TA_Base_Core::DatabaseKey::isValidKey( ulOverrideProfile ))
                {
                    std::auto_ptr<TA_Base_Core::IProfile> iOvrProfile( TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( ulOverrideProfile, false ));
                    strProfile += " / ";
                    strProfile = iOvrProfile->getName();
                }
            }
            catch ( ... )
            {
            }

            std::ostringstream opNameAndProfile;
            opNameAndProfile << strOperator << " (" << strProfile << ")";

		    return opNameAndProfile.str();
        }
		catch (const TA_Base_Core::DataException& ex )
		{
            std::string message = "PlanDataWriterImp::getOperatorName. Caught TA_Base_Core::DataException: ";
            message += ex.what();
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", message.c_str());
		}
        catch ( ... )
        {
            std::string message = "PlanDataWriterImp::getOperatorName. Caught unknown exception.";
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", message.c_str());
        }

	    std::string defaultName = "session: " + session;
        return defaultName;
    }


    std::string PlanDataWriterImp::makeNumericDataPointValueString(const TA_Base_Core::NumericDataPoint& ndp)
    {
        std::stringstream strmValue;
        strmValue << ndp.value;
        std::string strValue = strmValue.str();
        unsigned long precision = 0;

		std::string::size_type beginPos = 0;
		beginPos = strValue.find_first_of(".", beginPos);

		if (beginPos == std::string::npos)
        {
            precision = strValue.size();
        }
        else
        {
            precision = beginPos;
        }

	    std::ostringstream ostrstrm;
		ostrstrm << TA_Base_Core::NUMERIC_DATA_POINT << ",";
		ostrstrm << std::setprecision(ndp.precision + precision) << ndp.value << ",";
		ostrstrm << ndp.precision;

        return ostrstrm.str();
    }


    TA_Base_Core::StepNumber PlanDataWriterImp::getStepNumber(const TA_Base_Core::Plan& plan, TA_Base_Core::StepId step) const
    {
        unsigned long stepCount(plan.stepseq.length());

        for (unsigned long i(0); i < stepCount; i++)
		{
            const TA_Base_Core::StepDetail& stepDetail = plan.stepseq[i].detail;

			if (stepDetail.step == step)
			{
				return stepDetail.position;
			}
		}

        return 0;
    }

    void PlanDataWriterImp::updatePlanType( TA_Base_Core::NodeId nodeId, TA_Base_Core::EPlanType visibleState )
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();

        TA_Base_Core::SQLStatement strSql;

        /*std::string strSql  = dataBase->prepareSQLStatement(PLAN_TYPE_MAP_UPDATE_80005, visibleState, nodeId);*/
        dataBase->prepareSQLStatement(strSql, PLAN_TYPE_MAP_UPDATE_80005, visibleState, nodeId);

        dataBase->executeModification(strSql);
    }

    void PlanDataWriterImp::deletePlanType( TA_Base_Core::NodeId nodeId )
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();

        TA_Base_Core::SQLStatement strSql;
        dataBase->prepareSQLStatement(strSql, PLAN_TYPE_MAP_DELETE_80003, nodeId);
        dataBase->executeModification(strSql);
    }

    void PlanDataWriterImp::deleteAllStepParameters( const std::string& session, const TA_Base_Core::Plan& plan )
    {
		TA_Base_Core::IDatabase* dataBase = getDatabase();

		//Delete all the existing step parameters and steps of this plan
		/*std::ostringstream sql1;
		sql1 << " delete from step_parameters where fk_step_details in "
			<< " (select p_key from step_details where fk_node_tree = " << plan.detail.plan << ")";*/
//		std::string strDelSql1 = dataBase->prepareSQLStatement(STEP_PARAMETERS_DELETE_63401, plan.detail.plan); 
		TA_Base_Core::SQLStatement strDelSql1;
		dataBase->prepareSQLStatement(strDelSql1, STEP_PARAMETERS_DELETE_63401, plan.detail.plan);

		dataBase->executeModification(strDelSql1); //delete all step parameters
    }

    void PlanDataWriterImp::deleteAllSteps( const std::string& session, const TA_Base_Core::Plan& plan )
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();

		/*std::ostringstream sql2;
		sql2 << " delete from step_details where fk_node_tree = " << plan.detail.plan;*/
        TA_Base_Core::SQLStatement strDelSql2;
        dataBase->prepareSQLStatement(strDelSql2, STEP_DETAILES_DELETE_63901, plan.detail.plan);

        dataBase->executeModification(strDelSql2); //delete all steps
    }

    void PlanDataWriterImp::insertStepDetail( const TA_Base_Core::Plan& plan, const TA_Base_Core::StepDetail& detail )
    {
        // Update the details of the plan
        TA_Base_Core::IDatabase* dataBase = getDatabase();

        TA_Base_Core::SQLStatement strSql;

        dataBase->prepareSQLStatement(strSql, STEP_DETAILES_INSERT_63801, detail.step, detail.type,
            plan.detail.plan, detail.position, parseForApostrophe(detail.description.in()), detail.skip,
            detail.skippable, detail.delay, parseForApostrophe(detail.name.in()), detail.ignoreFailure,
            detail.nowait);

        dataBase->executeModification(strSql);
    }

    void PlanDataWriterImp::updateStepParameters( const TA_Base_Core::StepDetail& detail, const std::vector<std::string>& parameters)
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();
        TA_Base_Core::SQLStatement strSql;

		int position = 1;

		for ( std::vector<std::string>::const_iterator it = parameters.begin(); it != parameters.end(); it++)
		{
			/*std::ostringstream sql;
			sql << "insert into step_parameters (p_key, fk_step_details, position, value) "
				<< " values "
				<< "( plans_seq.nextval, " << stepDetail->step  
				<< "," 
				<< position++
				<< ",'" << parseForApostrophe(*it) << "')";*/
//				strSql = dataBase->prepareSQLStatement(STEP_PARAMETERS_INSERT_63351, stepDetail->step, 

	        dataBase->prepareSQLStatement(strSql, STEP_PARAMETERS_INSERT_63351, detail.step, position++,  parseForApostrophe(*it));

			dataBase->executeModification(strSql);
		}

    }

    void PlanDataWriterImp::replacePlan( const std::string& session, const TA_Base_Core::NodeId existingPlan, const TA_Base_Core::NodeId planToOverwrite )
    {
        TA_Base_Core::IDatabase* pDb = getDatabase();			
        TA_Base_Core::SQLStatement strSql;

        pDb->prepareSQLStatement(strSql, STEP_DETAILES_UPDATE_63501, existingPlan, planToOverwrite,
            getOperatorName(session));

        pDb->executeProcedure(strSql);
    }

    void PlanDataWriterImp::updateNodeParent( const std::string& session, const TA_Base_Core::NodeId parent, const TA_Base_Core::NodeId plan )
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();

		/*std::ostringstream sql;
		sql << "update node_tree set parent_node = "
			<< parent
			<< " where p_key = " << plan;

		    std::cout << sql.str() << std::endl;*/
//			std::string strSql  = dataBase->prepareSQLStatement(NODE_TREE_UPDATE_33653, parent, plan);
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql, NODE_TREE_UPDATE_33653, parent, plan);

	    dataBase->executeModification(strSql);

    }

    void PlanDataWriterImp::deletePlan( const std::string& session, const TA_Base_Core::NodeId plan )
    {
        TA_Base_Core::IDatabase* dataBase = getDatabase();
        TA_Base_Core::SQLStatement strSql;

        dataBase->prepareSQLStatement(strSql, STEP_DETAILES_UPDATE_63505, plan);
        dataBase->executeProcedure(strSql);
    }

}
