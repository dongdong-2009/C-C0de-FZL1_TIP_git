/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/PlanDataAccess/src/PlanDataReader.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2015/12/09 17:40:41 $
  *  Last modified by:  $Author: hongzhi.zhang $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "boost/tokenizer.hpp"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"


namespace TA_Base_Bus
{
    static std::vector<std::string> tokenizeString( std::string theString, const std::string& separatorList )
    {
        std::vector<std::string> parts;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( separatorList.c_str() );
        tokenizer tokens( theString, sep );

        for ( tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter )
        {
            parts.push_back( *tok_iter );
        }

        // if parts is empty, then this should return the entire string
        if ( parts.size() == 0 )
        {
            parts.push_back( theString );
        }

        return parts;
    }

	PlanDataReadingClass::PlanDataReadingClass() : m_rootNodeId(TA_Base_Core::ROOT_NODE_ID), m_nvlValue(" ")
	{
        
	}

    std::map<unsigned long, int> PlanDataReadingClass::getPlanTypeMap()
    {
        TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

        //std::string strSql  = database->prepareSQLStatement(PLAN_TYPE_MAP_SELECT_80001);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_TYPE_MAP_SELECT_80001);

        std::vector<std::string> planTypeColumns;
        planTypeColumns.push_back( "FK_NODE_ID" );
        planTypeColumns.push_back( "FK_PLAN_TYPE" );

        TA_Base_Core::IData* planTyepData = database->executeQuery( strSql, planTypeColumns );
        PlanDataBaseHelper dataBaseHelper(database);

        std::map<unsigned long, int> typeMap;

        if( (NULL == planTyepData ) || (0 == planTyepData->getNumRows()) )
        {
            return typeMap;
        }

        do
        {
            std::auto_ptr<TA_Base_Core::IData> planTyepDataPtr(planTyepData);
            for( unsigned long dataIdx = 0; dataIdx < planTyepData->getNumRows(); dataIdx++ )
            {
                typeMap[planTyepDataPtr->getUnsignedLongData(dataIdx, planTypeColumns[0])] = static_cast<TA_Base_Core::EPlanType>(planTyepDataPtr->getUnsignedLongData(dataIdx, planTypeColumns[1]));
            }
            planTyepDataPtr.reset();
        }
        while ( database->moreData( planTyepData ) );

        return typeMap;
    }

 	TA_Base_Core::SiblingNodes* PlanDataReadingClass::getRootNodeChildren( void ) const
    {
        return getChildren(m_rootNodeId);
    }

	TA_Base_Core::SiblingNodes* PlanDataReadingClass::getChildren( TA_Base_Core::NodeId parent ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Node summary query
		/*sql << "select node_summary.node_id, node_summary.parent, node_summary.name"
			<< ", node_summary.node_t, node_summary.approval_s"
			<< ", node_summary.has_children, node_summary.has_approved_descendant"
			<< ", node_summary.has_unapproved_descendant"
			<< " from node_summary, node_tree"
			<< " where node_summary.node_id = node_tree.p_key"
			<< " and node_tree.parent_node = " << parent;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_SUMMARY_STD_SELECT_64001, parent);
//		std::string strSql  = database->prepareSQLStatement(NODE_SUMMARY_SELECT_64001, parent);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_SUMMARY_SELECT_64001, parent);
		
        std::vector<std::string> summaryColumns;
		summaryColumns.push_back( "node_id" );
		summaryColumns.push_back( "parent" );
		summaryColumns.push_back( "name" );
        summaryColumns.push_back( "node_t" );
		summaryColumns.push_back( "approval_s" );
		summaryColumns.push_back( "has_children" ); 
		summaryColumns.push_back( "has_approved_descendant" );
		summaryColumns.push_back( "has_unapproved_descendant" );

		TA_Base_Core::IData* summaryData = database->executeQuery( strSql, summaryColumns );
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::SiblingNodes_var sn = new TA_Base_Core::SiblingNodes;
				
		if( (NULL == summaryData ) || (0 == summaryData->getNumRows()) )
        {
			sn->length( 0 );
			return( sn._retn() );
        }

		// TD9308 - keep fetching data until there is no more
		unsigned long rowsFetched = 0;
		do
		{

			std::auto_ptr<TA_Base_Core::IData> summaryDataPtr(summaryData);
			sn->length( summaryData->getNumRows() + rowsFetched );

			for( unsigned long dataIdx = 0; dataIdx < summaryData->getNumRows(); dataIdx++ )
			{

				sn[rowsFetched + dataIdx].node                    = summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[0] );
				sn[rowsFetched + dataIdx].parent                  = summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[1] );
				sn[rowsFetched + dataIdx].name                    = CORBA::string_dup( summaryDataPtr->getStringData( dataIdx, summaryColumns[2] ).c_str() );
				sn[rowsFetched + dataIdx].type                    = static_cast<TA_Base_Core::ENode>( summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[3] ) );
				sn[rowsFetched + dataIdx].approvalState           = static_cast<TA_Base_Core::EApprovalState>( summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[4] ) );
				sn[rowsFetched + dataIdx].hasChildren             = static_cast<CORBA::Boolean>( 0 != summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[5] ) );
				sn[rowsFetched + dataIdx].hasApprovedDescendant   = static_cast<CORBA::Boolean>( 0 != summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[6] ) );
				sn[rowsFetched + dataIdx].hasUnapprovedDescendant = static_cast<CORBA::Boolean>( 0 != summaryDataPtr->getUnsignedLongData( dataIdx, summaryColumns[7] ) );
			}
			rowsFetched += summaryDataPtr->getNumRows();
            summaryDataPtr.reset();
		}
		while ( database->moreData( summaryData ) );

		return sn._retn();
	}

	TA_Base_Core::NodeId	PlanDataReadingClass::getParent( TA_Base_Core::NodeId node ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		
		sql << "select parent_node from node_tree where p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33508, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33508, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33508, node);

		std::vector<std::string> columns;

		columns.push_back("parent_node");
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
			std::ostringstream message;
			message << "The node " << node << "does not exist.";
			//TA_THROW( TA_Base_Core::DataException( message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, strSql ));
			

			TA_THROW( TA_Base_Core::DataException( message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "p_key" ));

        }

		TA_Base_Core::NodeId parent = data->getUnsignedLongData( 0, columns[0] ); 
		return parent;
	}

	std::string PlanDataReadingClass::getNodeName(TA_Base_Core::NodeId node) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);
		/*std::ostringstream sql;

		sql << "select name from node_tree where p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33509, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33509, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33509, node);
		std::vector<std::string> columns;
		columns.push_back("name");
		std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		if ( ( NULL == data.get() ) || ( 1 != data->getNumRows() ) )
		{
			std::ostringstream message;
			message << "The node " << node << "does not exist.";
			//TA_THROW( TA_Base_Core::DataException (message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, strSql ));
			TA_THROW( TA_Base_Core::DataException (message.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "p_key" ));
		}

		std::string nodeName = data->getStringData(0, columns[0]);

		return nodeName;
	}

    TA_Base_Core::NodeId PlanDataReadingClass::getNodeId(const std::string& path) const
    {
        //
        TA_Base_Core::NodeId node = 0;
        bool foundNode = false;

        TA_Base_Core::NodeId parent = m_rootNodeId;
        std::string nodeName = "";

		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		const std::string delimiter = "/"; 
			
		std::string::size_type beginPos = 0;

        //Remove the terminating "/" if any

        beginPos = path.find_last_of(delimiter);

        std::string newPath;

        if ( beginPos == (path.size() -1)  )
        {
            newPath = path.substr(0, beginPos );
        }
        else
        {
            newPath = path;
        }

        beginPos = 0;
		while(beginPos < newPath.size())
		{
			beginPos = newPath.find_first_of(delimiter, beginPos);
			if (beginPos == std::string::npos)
			{
                break;
			}
			
			std::string::size_type endPos = newPath.find_first_of(delimiter, beginPos+1);

			if (endPos == std::string::npos) 
			{
				nodeName = newPath.substr(beginPos+1);
			} 
			else
            {
				nodeName = newPath.substr(beginPos+1, endPos - 1 - beginPos);
            }

		    /*std::ostringstream sql;

		    sql << "select p_key from node_tree where parent_node = "
			    << parent << " and name = '"
			    << database->escapeQueryString(nodeName) << "'";*/
//			std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33505, parent, 
//			std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33505, parent, 
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33505, parent,
				database->escapeQueryString(nodeName));

            std::vector<std::string> columns;
		    columns.push_back( "p_key" );
		    
		    std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);

		    if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
            {
			    break;
            }

		    node = data->getUnsignedLongData( 0, columns[0] );

            if ( endPos == std::string::npos )
            {
                foundNode = true;
                break;
            }

            parent = node;

			// set up for next loop
			beginPos = endPos;
	    }

        if ( foundNode )
        { 
            return node;
        }

        std::string message = "The node '";
        message += path;
        message += "' does not exist!";
        
        throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(TA_Base_Core::PNFE_NODE_NOT_EXIST, 0, 0, message.c_str()));
    }

	TA_Base_Core::NodeId PlanDataReadingClass::getNodeId( TA_Base_Core::NodeId parent, const std::string& planName )
	{
		TA_Base_Core::NodeId node = 0;

		TA_Base_Core::IDatabase* dataBase = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		sql << "select p_key from node_tree where parent_node = "
			<< parent << " and name = '"
			<< planName.c_str() << "'";*/
//		std::string strSql = defPrepareSQLStatement(dataBase->GetDbServerType(), NODE_TREE_STD_SELECT_33505, parent, planName);
//		std::string strSql  = dataBase->prepareSQLStatement(NODE_TREE_SELECT_33505, parent, planName);
		TA_Base_Core::SQLStatement strSql;
		dataBase->prepareSQLStatement(strSql, NODE_TREE_SELECT_33505, parent, planName);

        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(dataBase);

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
		{
			std::ostringstream message;
			message << "The plan " << planName << " with parent " << parent << " does not exist.";
			throw(TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError(
				TA_Base_Core::PNFE_NODE_NOT_EXIST, 0, 0, message.str().c_str()));
		}

		node = data->getUnsignedLongData( 0, columns[0] );

		return( node );
	}

    TA_Base_Core::EPlanType PlanDataReadingClass::getPlanType ( TA_Base_Core::NodeId plan ) const
    {
        TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

        //std::string strSql  = database->prepareSQLStatement(PLAN_TYPE_MAP_SELECT_80002, plan);

		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, PLAN_TYPE_MAP_SELECT_80002, plan);

        std::vector<std::string> planTypeColumns;
        planTypeColumns.push_back( "fk_plan_type" );

        std::auto_ptr<TA_Base_Core::IData> planTypeData( database->executeQuery( strSql, planTypeColumns ) );
        PlanDataBaseHelper dataBaseHelper(database);

        if( (NULL == planTypeData.get() ) || (1 != planTypeData->getNumRows()) ) 
        {
            return TA_Base_Core::UNDEFINED_TYPE;
        }

        TA_Base_Core::EPlanType eType = static_cast<TA_Base_Core::EPlanType>(planTypeData->getUnsignedLongData(0, planTypeColumns[0]));

        return eType;
    }

	TA_Base_Core::NodeSummary*	PlanDataReadingClass::getNodeSummary ( TA_Base_Core::NodeId node ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Node summary query
		/*sql << "select node_summary.node_id, node_summary.parent, node_summary.name"
			<< ", node_summary.node_t, node_summary.approval_s"
			<< ", node_summary.has_children, node_summary.has_approved_descendant"
			<< ", node_summary.has_unapproved_descendant"
			<< " from node_summary, node_tree"
			<< " where node_summary.node_id = " << node
			<< " and node_tree.p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_SUMMARY_STD_SELECT_64002, node, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_SUMMARY_SELECT_64002, node, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_SUMMARY_SELECT_64002, node, node);
		
        std::vector<std::string> summaryColumns;
		summaryColumns.push_back( "node_id" );
		summaryColumns.push_back( "parent" );
		summaryColumns.push_back( "name" );
        summaryColumns.push_back( "node_t" );
		summaryColumns.push_back( "approval_s" );
		summaryColumns.push_back( "has_children" ); 
		summaryColumns.push_back( "has_approved_descendant" );
		summaryColumns.push_back( "has_unapproved_descendant" );

		std::auto_ptr<TA_Base_Core::IData> summaryData( database->executeQuery( strSql, summaryColumns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::NodeSummary_var ns = new TA_Base_Core::NodeSummary;
				
		if( (NULL == summaryData.get() ) || (1 != summaryData->getNumRows()) ) 
        {
			return( ns._retn() );
        }
		
		ns->node                    = summaryData->getUnsignedLongData( 0, summaryColumns[0] );
		ns->parent                  = summaryData->getUnsignedLongData( 0, summaryColumns[1] );
		ns->name                    = CORBA::string_dup( summaryData->getStringData( 0, summaryColumns[2] ).c_str() );
		ns->type                    = static_cast<TA_Base_Core::ENode>( summaryData->getUnsignedLongData( 0, summaryColumns[3] ) );
		ns->approvalState           = static_cast<TA_Base_Core::EApprovalState>( summaryData->getUnsignedLongData( 0, summaryColumns[4] ) );
		ns->hasChildren             = static_cast<CORBA::Boolean>( 0 != summaryData->getUnsignedLongData( 0, summaryColumns[5] ) );
		ns->hasApprovedDescendant   = static_cast<CORBA::Boolean>( 0 != summaryData->getUnsignedLongData( 0, summaryColumns[6] ) );
		ns->hasUnapprovedDescendant = static_cast<CORBA::Boolean>( 0 != summaryData->getUnsignedLongData( 0, summaryColumns[7] ) );

		return( ns._retn() );
	}

	bool PlanDataReadingClass::existsNode(TA_Base_Core::NodeId node)
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

	/*	std::ostringstream sql;
		
		sql << "select p_key from node_tree "
			<< " where p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33510, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33510, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33510, node);

        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
				
		if( (NULL == data.get() || 0 == data->getNumRows() ) ) 
        {
			return false;
        }

		return true;
	}

	bool PlanDataReadingClass::canUseName(TA_Base_Core::NodeId node, TA_Base_Core::NodeId parent, const std::string& name)
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;
		// Node summary query
		/*sql << "select p_key from node_tree "
			<< " where name = '" << name.c_str() << "' "
			<< " and parent_node = " << parent;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33511, name, parent);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33511, name, parent);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33511, name, parent);
		
        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
				
		if( (NULL == data.get() || 0 == data->getNumRows() ) ) 
        {
			return true;
        }

		if ( data->getNumRows() > 1 )
		{
			return false;
		}
		
		TA_Base_Core::NodeId curNode = data->getUnsignedLongData( 0, columns[0] );

		if ( curNode == node )
		{
			return true;
		}

		return false;
	}

	TA_Base_Core::NodeDetail* PlanDataReadingClass::getNodeDetail( TA_Base_Core::NodeId node ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;
		TA_Base_Core::SQLStatement strSql;
		std::vector<std::string> columns;
		
		TA_Base_Core::ENode nodeType = getNodeType( node );

		TA_Base_Core::NodeDetail_var nd = new TA_Base_Core::NodeDetail;

		switch( nodeType )
		{
			case TA_Base_Core::PLAN_NODE:
			{

				/*sql << "select parent, node_id, name, nvl(description, '" << m_nvlValue.c_str() << "'), approval_state,"
                    << " modified_by, to_char(modified_time, 'DD-Mon-YY HH24:MI:SS'), approval_changed_by,"
                    << " to_char(approval_changed_time, 'DD-Mon-YY HH24:MI:SS') "
					<< " from plan_details where node_id = " << node;*/
//				strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_Oracle_SELECT_33554, m_nvlValue, node);
//				strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33554, m_nvlValue, node);

		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33554, m_nvlValue, node);

				columns.push_back( "parent" );
				columns.push_back( "node_id" );
				columns.push_back( "name" );
				columns.push_back( "description" );
				columns.push_back( "approval_state" );
                columns.push_back( "modified_by" );
                columns.push_back( "modified_time" );
                columns.push_back( "approval_changed_by" );
                columns.push_back( "approval_changed_time" );
			}
			break;

			case TA_Base_Core::CATEGORY_NODE:
			{
			/*	sql << "select parent, node_id, name, nvl(description, '" << m_nvlValue.c_str() << "')"
                    << " from category_details"
					<< " where node_id = " << node;*/
//				strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_Oracle_SELECT_33555, m_nvlValue, node);
//				strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33555, m_nvlValue, node);

		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33555, m_nvlValue, node);
				
				columns.push_back( "parent" );
				columns.push_back( "node_id" );
				columns.push_back( "name" );
				columns.push_back( "description" );
			}
			break;

			default:
			{
				TA_ASSERT( false, "Invalid node type" );
			}
			break;
		}
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
					
		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
        {
			//TA_THROW( TA_Base_Core::DataException( "Failed to find node details", TA_Base_Core::DataException::NO_VALUE, strSql ) );
			TA_THROW( TA_Base_Core::DataException( "Failed to find node details", TA_Base_Core::DataException::NO_VALUE, "node_id" ) );

        }

		switch( nodeType )
		{
			case TA_Base_Core::PLAN_NODE:
			{
				TA_Base_Core::PlanDetail planDetail;

				planDetail.parent		 = static_cast<TA_Base_Core::NodeId>( data->getUnsignedLongData( 0, columns[0] ) );
				planDetail.plan			 = static_cast<TA_Base_Core::NodeId>( data->getUnsignedLongData( 0, columns[1] ) );
				planDetail.name          = CORBA::string_dup( data->getStringData( 0, columns[2] ).c_str() );
				planDetail.description   = CORBA::string_dup( getNvlString( data->getStringData( 0, columns[3] )).c_str());
				planDetail.approvalState = static_cast<TA_Base_Core::EApprovalState>( data->getUnsignedLongData( 0, columns[4] ) );
                planDetail.modifiedBy    = CORBA::string_dup( data->getStringData( 0, columns[5] ).c_str() );
                planDetail.modifiedTime  = CORBA::string_dup( data->getStringData( 0, columns[6] ).c_str() );
                planDetail.approvalChangedBy    = CORBA::string_dup( data->getStringData( 0, columns[7] ).c_str() );
                planDetail.approvalChangedTime  = CORBA::string_dup( data->getStringData( 0, columns[8] ).c_str() );
				nd->plan( planDetail );
			}
			break;

			case TA_Base_Core::CATEGORY_NODE:
			{
				TA_Base_Core::CategoryDetail categoryDetail;

				categoryDetail.parent		 = static_cast<TA_Base_Core::NodeId>( data->getUnsignedLongData( 0, columns[0] ) );
				categoryDetail.category			 = static_cast<TA_Base_Core::NodeId>( data->getUnsignedLongData( 0, columns[1] ) );
				categoryDetail.name          = CORBA::string_dup( data->getStringData( 0, columns[2] ).c_str() );
				categoryDetail.description   = CORBA::string_dup( getNvlString(data->getStringData( 0, columns[3] )).c_str() );

				nd->category( categoryDetail );
			}
			break;

			default:
			{
				TA_ASSERT( false, "Invalid node type" );
			}
			break;
		}

		return nd._retn();
	}

	TA_Base_Core::StepNames* PlanDataReadingClass::getStepNames( void ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::string sql;

		// Step name query
		/*sql  = "select type_name from step_type order by type_value";*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_TYPE_STD_SELECT_64501);
//		std::string strSql  = database->prepareSQLStatement(STEP_TYPE_SELECT_64501);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, STEP_TYPE_SELECT_64501);
		
        std::vector<std::string> columns;
		columns.push_back( "type_name" );

		TA_Base_Core::IData* data = database->executeQuery( strSql, columns ) ;
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::StepNames_var sn = new TA_Base_Core::StepNames;
		
        if( (NULL == data) || (0 == data->getNumRows()) )
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "failed to find any step names." );
			sn->length( 0 );
			return( sn._retn() );
        }

		// TD9308 - keep fetching data until there is no more
		unsigned long rowsFetched = 0;
		do 
		{
			std::auto_ptr<TA_Base_Core::IData> dataPtr(data);
			sn->length( data->getNumRows() + rowsFetched );

			for( unsigned long dataIdx = 0; dataIdx < data->getNumRows(); dataIdx++ )
			{
				sn[rowsFetched + dataIdx] = CORBA::string_dup( dataPtr->getStringData( dataIdx, columns[0] ).c_str() );
			}
			rowsFetched += dataPtr->getNumRows();
            dataPtr.reset();
		}
		while ( database->moreData( data ) );

		return sn._retn();
	}

	TA_Base_Core::StepDetails* PlanDataReadingClass::getAllStepDetails( TA_Base_Core::NodeId plan ) const
	{
		return( getStepDetails( plan, "" ) );
	}

	TA_Base_Core::StepDetails* PlanDataReadingClass::getSpecifiedStepDetails( TA_Base_Core::NodeId plan, const TA_Base_Core::StepIds& steps ) const
	{
		std::ostringstream stepConstraint;
		
		for( CORBA::ULong i = 0; i < steps.length(); i++ )
		{
			stepConstraint << steps[i];

			// if it isn't the last one, add a comma
			if( (steps.length()-1) != i )
			{
				stepConstraint << ",";
			}
		}

		return( getStepDetails( plan, stepConstraint.str() ) );
	}

	TA_Base_Core::ParameterNames* PlanDataReadingClass::getParameterNames( TA_Base_Core::EStep stepType ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Step name query
	/*	sql << "select name from parameter_names where step_type = " << stepType;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_TYPE_STD_SELECT_64502, stepType);
//		std::string strSql  = database->prepareSQLStatement(STEP_TYPE_SELECT_64502, stepType);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, STEP_TYPE_SELECT_64502, stepType);

        std::vector<std::string> columns;
		columns.push_back( "name" );

		TA_Base_Core::IData* data = database->executeQuery( strSql, columns );
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::ParameterNames_var pn = new TA_Base_Core::ParameterNames;

        if( (NULL == data) || (0 == data->getNumRows()) )
        {
			pn->length( 0 );
			return pn._retn();
        }

		// TD9308 - keep fetching data until there is no more
		unsigned long rowsFetched = 0;
		do
		{
			std::auto_ptr<TA_Base_Core::IData> dataPtr(data);
			pn->length( data->getNumRows() + rowsFetched );

			for( unsigned long dataIdx = 0; dataIdx < data->getNumRows(); dataIdx++ )
			{
				pn[rowsFetched + dataIdx] = CORBA::string_dup( dataPtr->getStringData( dataIdx, columns[0] ).c_str() );
			}
			rowsFetched += dataPtr->getNumRows();
			dataPtr.reset();
		}
		while ( database->moreData( data ) );

		return pn._retn();
	}

    TA_Base_Core::Steps* PlanDataReadingClass::getAllSteps( TA_Base_Core::NodeId plan ) const
    {
        TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

//        std::ostringstream stmSql;
        std::vector<std::string> vecColumns;

//         stmSql << "select nvl(step_parameters.position, 1) as ParameterPos,"
//             << " nvl(step_parameters.value, '') as ParameterValue,"
//             << " step_details.p_key, step_details.position, nvl(step_details.name, ''), step_type.type_value,"
//             << " nvl(step_details.description, ''), step_details.skip, step_details.skippable,"
//             << " step_details.delay, nvl(step_details.ignore_failure, 0), nvl(step_details.no_wait, 0)" 
//             << " from step_type, step_details left join step_parameters on"
//             << " step_parameters.fk_step_details = step_details.p_key"
//             << " where step_details.fk_step_type = step_type.p_key"
//             << " and step_details.fk_node_tree = " << plan;
        // Improve database performance, sort by std::map.
//         stmSql << " order by step_details.position, step_parameters.position";

//		std::string strSql  = database->prepareSQLStatement(STEP_PARAMETERS_SELECT_63053, plan);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, STEP_PARAMETERS_SELECT_63053, plan);


        vecColumns.push_back( "ParameterPos" );
        vecColumns.push_back( "ParameterValue" );
        vecColumns.push_back( "p_key");
        vecColumns.push_back( "position" );
        vecColumns.push_back( "name" );
        vecColumns.push_back( "type_value" );
        vecColumns.push_back( "description" );
        vecColumns.push_back( "skip" );
        vecColumns.push_back( "skippable" );
        vecColumns.push_back( "delay" );
        vecColumns.push_back( "ignore_failure" );
		vecColumns.push_back( "no_wait" );

        TA_Base_Core::IData* pData = database->executeQuery( strSql, vecColumns );

		TA_Base_Core::Steps_var varResult = new TA_Base_Core::Steps();

        if( NULL == pData ) 
        {
            return varResult._retn();
		}

        typedef std::map<TA_Base_Core::StepNumber, MapParameters> StepNumToParamatersMap;
        StepNumToParamatersMap mapStepParameters;
        StepNumToDetailMap mapSteps;
        StepNumToStepIdMap mapStepNumToIds;

        TA_Base_Core::StepNumber ulStepPos = static_cast<TA_Base_Core::StepNumber>(0);
        TA_Base_Core::ParameterNumber ulParamaterPos = static_cast<TA_Base_Core::ParameterNumber>(0);
        unsigned long ulStepPkey = 0;
        unsigned long ulType = 0;
        unsigned long ulSkip = 0;
        unsigned long ulSkipAble = 0;
        unsigned long ulDelay = 0;
        unsigned long ulIgnoreFailure = 0;
        unsigned long ulNoWait = 0;
        std::string strParamaterValue = "";
        std::string strStepName = "";
        std::string strDescription = "";
        bool bNewStep = false;

        do
        {
            for ( unsigned long nLoop = 0; nLoop < pData->getNumRows(); ++nLoop )
            {
                try
                {
                    ulStepPos = static_cast<TA_Base_Core::StepNumber>(
                        pData->getUnsignedLongData( nLoop, "position" ));
                    if ( mapSteps.end() == mapSteps.find( ulStepPos ))
                    {
                        ulStepPkey = pData->getUnsignedLongData( nLoop, "p_key" );
                        ulType = pData->getUnsignedLongData( nLoop, "type_value" );
                        ulSkip = pData->getUnsignedLongData( nLoop, "skip" );
                        ulSkipAble = pData->getUnsignedLongData( nLoop, "skippable" );
                        ulDelay = pData->getUnsignedLongData( nLoop, "delay" );
                        ulIgnoreFailure = pData->getUnsignedLongData( nLoop, "ignore_failure" );
                        ulNoWait = pData->getUnsignedLongData( nLoop, "no_wait" );
                        strStepName = pData->getStringData( nLoop, "name" );
                        strDescription = pData->getStringData( nLoop, "description" );
                        bNewStep = true;
                    }

                    ulParamaterPos = static_cast<TA_Base_Core::ParameterNumber>(
                        pData->getUnsignedLongData( nLoop, "ParameterPos" ));
                    strParamaterValue = pData->getStringData( nLoop, "ParameterValue" );
                }
                catch ( std::exception& expWhat )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.what() );
                    continue;
                }
                catch (...)
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception" );
                    continue;
                }

                if ( bNewStep )
                {
                    TA_Base_Core::StepDetail stNewStepDetail;

                    std::pair<StepNumToDetailMap::iterator, bool> pirRes = mapSteps.insert( 
                        StepNumToDetailMap::value_type( ulStepPos, stNewStepDetail ));

                    if ( pirRes.second )
                    {
                        pirRes.first->second.delay = static_cast<TA_Base_Core::Seconds>(ulDelay);
                        pirRes.first->second.description = CORBA::string_dup(strDescription.c_str());
                        pirRes.first->second.ignoreFailure = static_cast<CORBA::Boolean>(0l != ulIgnoreFailure);
                        pirRes.first->second.nowait = static_cast<CORBA::Boolean>(0l != ulNoWait);
                        pirRes.first->second.name = CORBA::string_dup(strStepName.c_str());
                        pirRes.first->second.position = ulStepPos;
                        pirRes.first->second.skip = static_cast<CORBA::Boolean>(0l != ulSkip);
                        pirRes.first->second.skippable = static_cast<CORBA::Boolean>(0l != ulSkipAble);
                        pirRes.first->second.step = static_cast<TA_Base_Core::StepId>(ulStepPkey);
                        pirRes.first->second.type = static_cast<TA_Base_Core::EStep>(ulType);
                    }

                    mapStepNumToIds.insert( StepNumToStepIdMap::value_type( ulStepPos, ulStepPkey ));
                }

                StepNumToParamatersMap::iterator itParameter = mapStepParameters.find( ulStepPos );
                if ( mapStepParameters.end() == itParameter )
                {
                    MapParameters mapNewParameters;
                    std::pair<StepNumToParamatersMap::iterator, bool> pirInsertResult;
                    pirInsertResult = mapStepParameters.insert( StepNumToParamatersMap::value_type( ulStepPos, mapNewParameters ));
                    if ( !pirInsertResult.second )
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Can not insert item to map");
                        continue;
                    }
                    else
                    {
                        itParameter = pirInsertResult.first;
                    }
                }

                if ( mapStepParameters.end() != itParameter )
                {
                    (itParameter->second)[ulParamaterPos] = strParamaterValue;
                }
            }

            delete pData;
            pData = NULL;

        } while ( database->moreData( pData ));

        if ( 0u == mapSteps.size() )
        {
            return varResult._retn();
        }

        varResult->length( mapSteps.size() );

        int nResultLoop = 0;
        for ( StepNumToDetailMap::iterator itLoop = mapSteps.begin(); mapSteps.end() != itLoop; ++itLoop )
        {
            varResult[nResultLoop].detail.delay = itLoop->second.delay;
            varResult[nResultLoop].detail.description = CORBA::string_dup( itLoop->second.description );
            varResult[nResultLoop].detail.ignoreFailure = itLoop->second.ignoreFailure;
            varResult[nResultLoop].detail.nowait = itLoop->second.nowait;
            varResult[nResultLoop].detail.name = CORBA::string_dup( itLoop->second.name );
            varResult[nResultLoop].detail.position = itLoop->second.position;
            varResult[nResultLoop].detail.skip = itLoop->second.skip;
            varResult[nResultLoop].detail.skippable = itLoop->second.skippable;
            varResult[nResultLoop].detail.step = itLoop->second.step;
            varResult[nResultLoop].detail.type = itLoop->second.type;

            TA_Base_Core::StepNumber ulTempStepNum = itLoop->second.position;
            StepNumToParamatersMap::iterator itFound = mapStepParameters.find( ulTempStepNum );
            if ( mapStepParameters.end() != itFound )
            {
                try
                {
                    convertStepParameters( varResult[nResultLoop].parameters, mapStepNumToIds, 
                        ulTempStepNum, itLoop->second.type, itFound->second );
                }
                catch ( std::exception& expWhat )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
                }
                catch ( ... )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unkonwn", "Caught unkonwn exception when convert step parameters" );
                }
            }
            ++nResultLoop;
        }

        return varResult._retn();
    }

	TA_Base_Core::StepParameters* PlanDataReadingClass::getStepParameters( TA_Base_Core::NodeId plan, TA_Base_Core::StepId step, bool isStepNumber ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;
		TA_Base_Core::SQLStatement strSql;
		std::vector<std::string> columns;

		if (isStepNumber)
		{
		    /*sql << "select step_details.p_key, step_details.position, step_type.type_value,"
                << " nvl(step_parameters.position, 1) as ParameterPos,"
                << " nvl(step_parameters.value, '" << m_nvlValue.c_str() << "') as ParameterValue"
				<< " from step_type, step_details"
                << " left join step_parameters on step_parameters.fk_step_details = step_details.p_key"
                << " where step_details.fk_step_type = step_type.p_key"
				<< " and step_details.fk_node_tree = " << plan;*/
//			strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_PARAMETERS_Oracle_SELECT_63051, m_nvlValue, plan, step);
//			strSql  = database->prepareSQLStatement(STEP_PARAMETERS_SELECT_63051, m_nvlValue, plan, step);

		database->prepareSQLStatement(strSql, STEP_PARAMETERS_SELECT_63051, m_nvlValue, plan, step);

            columns.push_back( "p_key" );
            columns.push_back( "position" );
            columns.push_back( "type_value" );
            columns.push_back( "ParameterPos" );
            columns.push_back( "ParameterValue" );
		}
		else
		{
			/*sql << "select sdref.p_key, sdref.position, step_type.type_value,"
                << " nvl(step_parameters.position, 1) as ParameterPos,"
                << " nvl(step_parameters.value, '" << m_nvlValue.c_str() << "') as ParameterValue"
				<< " from step_details sdcmp, step_type, step_details sdref"
				<< " left join step_parameters on step_parameters.fk_step_details = sdref.p_key"
                << " where sdref.fk_step_type = step_type.p_key"
                << " and sdref.fk_node_tree = sdcmp.fk_node_tree" 
                << " and sdcmp.p_key = " << step;*/
//			strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_PARAMETERS_Oracle_SELECT_63052, m_nvlValue, step);
//			strSql  = database->prepareSQLStatement(STEP_PARAMETERS_SELECT_63052, m_nvlValue, step);

		database->prepareSQLStatement(strSql, STEP_PARAMETERS_SELECT_63052, m_nvlValue, step);

            columns.push_back( "p_key" );
            columns.push_back( "position" );
            columns.push_back( "type_value" );
            columns.push_back( "ParameterPos" );
            columns.push_back( "ParameterValue" );
		}

		TA_Base_Core::IData* pData = database->executeQuery( strSql, columns );
        PlanDataBaseHelper dataBaseHelper(database);

        TA_Base_Core::StepParameters* pStepParams = new TA_Base_Core::StepParameters();

        // don't check for zero rows becase some steps don't have parameters!
        if( NULL == pData ) 
        {
            return pStepParams;
        }

        StepNumToStepIdMap mapStepNumToIds;
        MapParameters mapParameters;

        TA_Base_Core::StepNumber ulStepPos = static_cast<TA_Base_Core::StepNumber>(0);
        TA_Base_Core::ParameterNumber ulParamaterPos = static_cast<TA_Base_Core::ParameterNumber>(0);
        unsigned long ulStepPkey = 0;
        TA_Base_Core::EStep eType = TA_Base_Core::UNDEFINED_STEP;
        TA_Base_Core::StepId ulStepId = 0;
        bool bGetStep = false;

        do
        {
            for ( unsigned long nLoop = 0; nLoop < pData->getNumRows(); ++nLoop )
            {
                try
                {
                    ulStepPos = static_cast<TA_Base_Core::StepNumber>(pData->getUnsignedLongData( nLoop, "position" ));
                    ulStepPkey = pData->getUnsignedLongData( nLoop, "p_key" );
                    if ( mapStepNumToIds.end() == mapStepNumToIds.find( ulStepPos ))
                    {
                        mapStepNumToIds.insert( StepNumToStepIdMap::value_type( ulStepPos, ulStepPkey ));
                    }

                    bGetStep = ( isStepNumber && ( ulStepPos == step )) || ( !isStepNumber && ( ulStepPkey == step ));
                    if ( bGetStep )
                    {
                        ulStepId = static_cast<TA_Base_Core::StepId>(ulStepPkey);
                        eType = static_cast<TA_Base_Core::EStep>(pData->getUnsignedLongData( nLoop, "type_value" ));
                        ulParamaterPos = static_cast<TA_Base_Core::ParameterNumber>(pData->getUnsignedLongData( nLoop, "ParameterPos" ));

                        mapParameters[ulParamaterPos] = pData->getStringData( nLoop, "ParameterValue" );;
                    }
                }
                catch ( std::exception& expWhat )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.what() );
                    continue;
                }
                catch (...)
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception" );
                    continue;
                }
            }

            delete pData;
            pData = NULL;

        } while ( database->moreData( pData ));

        convertStepParameters( *pStepParams, mapStepNumToIds, ulStepId, eType, mapParameters );

        return pStepParams;
	}

	TA_Base_Core::StepNumber PlanDataReadingClass::getStepCount( TA_Base_Core::NodeId plan ) const
	{
		TA_Base_Core::StepNumber rval = 0;

		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Step count query
	/*	sql << "select count(position) as steps from step_details where fk_node_tree =  " << plan;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_STD_SELECT_63506, plan);
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63506, plan);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, STEP_DETAILES_SELECT_63506, plan);

		std::vector<std::string> columns;
		columns.push_back( "steps" );

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		
		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
		{
			return rval;
		}
		
		rval = data->getUnsignedLongData( 0, columns[0] );

		return( rval );
	}

	TA_Base_Core::EApprovalState PlanDataReadingClass::getPlanApprovalState(TA_Base_Core::NodeId plan) const
	{
		TA_Base_Core::EApprovalState rval = TA_Base_Core::UNDEFINED_APPROVAL_STATE;

		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		
		sql << "select state_value from approval_state where p_key = "
			<< "(select node_tree.fk_approval_state from node_tree where p_key =  "
			<< plan << ")";*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33512, plan);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33512, plan);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33512, plan);

		std::vector<std::string> columns;
		columns.push_back("state_value");

		std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		if ( (NULL == data.get()) || ( 0 == data->getNumRows())  )
		{
			return rval;
		}

		rval = (TA_Base_Core::EApprovalState) data->getUnsignedLongData (0, columns[0] );

		return (rval);
	}


	int PlanDataReadingClass::getCategoryLevel(TA_Base_Core::NodeId category)
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);
		
		//std::ostringstream sql;
		
		// Node summary query
		/*sql << "select parent_node" 
			<< " from node_tree"
			<< " where p_key = "
			<< category;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33508, category);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33508, category);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33508, category);
		
		std::vector<std::string> columns;
		columns.push_back( "parent_node" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);
		
		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
		{
			std::ostringstream msg;
			msg << "The node " << category << " does not exist.";
			TA_THROW( TA_Base_Core::DataException( msg.str().c_str(), TA_Base_Core::DataException::NO_VALUE, columns[0] ) );
		}
		
		int parent = data->getUnsignedLongData( 0, columns[0] );
		
		if ( parent == 1 )
		{
			return 1;
		}
		else if (getParent (parent) == 1)
		{
			return 2;
		}
		else
		{
			return 3;
		}
		
	}


	TA_Base_Core::StepNumber PlanDataReadingClass::getStepNumber( TA_Base_Core::StepId step) const
	{
		TA_Base_Core::StepNumber rval = 0;
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		
		sql << "select position from step_details where p_key =  " << step;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_STD_SELECT_63507, step);
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63507, step);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, STEP_DETAILES_SELECT_63507, step);

		std::vector<std::string> columns;
		columns.push_back("position");

        try
        {
		    std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);
		    if ( (NULL == data.get()) || ( 0 == data->getNumRows())  )
		    {
			    return rval;
		    }

		    rval = data->getUnsignedLongData (0, columns[0] );
        }
        catch ( ... )
        {
            rval = 0;
        }

		return (rval);
	}


    TA_Base_Core::StepId PlanDataReadingClass::getStepId( TA_Base_Core::NodeId plan, TA_Base_Core::StepNumber step) const
    {
		TA_Base_Core::StepId rval = 0;
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

	/*	std::ostringstream sql;
		
		sql << "select p_key from step_details "
            << "where fk_node_tree =  " << plan << " and position = " << step;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_STD_SELECT_63508, plan, step);
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63508, plan, step);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, STEP_DETAILES_SELECT_63508, plan, step);

		std::vector<std::string> columns;
		columns.push_back("stepId");

        try
        {
		    std::auto_ptr<TA_Base_Core::IData> data ( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);
		    if ( (NULL == data.get()) || ( 0 == data->getNumRows())  )
		    {
			    return rval;
		    }

		    rval = data->getUnsignedLongData (0, columns[0] );
        }
        catch ( ... )
        {
            rval = 0;
        }

		return (rval);        
    }


	std::string PlanDataReadingClass::getNodePath( TA_Base_Core::NodeId plan ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);
		//int nDbServerType = database->GetDbServerType();

		//std::string nodePath, strSql;
		std::string nodePath;
		TA_Base_Core::SQLStatement strSql;

		TA_Base_Core::NodeId currentNode(plan);

		while( 0 != currentNode )
		{
			//std::ostringstream sql;

			// Step details query
			/*sql << "select name, nvl(parent_node,0) as parent_node from node_tree"
				<< " where p_key = " << currentNode;*/
//			strSql = database->prepareSQLStatement(NODE_TREE_SELECT_33556, currentNode);

		    database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33556, currentNode);

			std::vector<std::string> columns;
			columns.push_back( "name" );
			columns.push_back( "parent_node" );

			std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);
						
			if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
			{
                std::ostringstream reasonMessage;
                reasonMessage << "Node path for plan ID " << plan << " broken at node ID " << currentNode; 

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed at node %lu to find path for plan %lu", currentNode, plan );
                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
			}

			nodePath.insert( 0, data->getStringData( 0, columns[0] ) );
			nodePath.insert( 0, "/" );

			currentNode = data->getUnsignedLongData( 0, columns[1] );
		}
		
		// the substr removes the unwanted leading /'s
        //MP: The nodePath is returned if its length is less than 3.
        //    Fix for PropWeb Issue #1736
        if ( nodePath.length() < 3 )
        {
            return nodePath;
        }
        else
        {
			return( nodePath.substr(2) );
        }
	}

    // Returns the list of node IDs of the nodes making up the specified plan's path. 
    // The first element in the list is the root node's ID and the last element is 
    // the actual plan node's ID.
	TA_Base_Core::NodeIds* PlanDataReadingClass::getNodeIdsInNodePath( TA_Base_Core::NodeId plan ) const
	{

        std::vector<TA_Base_Core::NodeId> nodeIds;

		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);
		TA_Base_Core::NodeId currentNode(plan);
		//int nDbServerType = database->GetDbServerType();
		TA_Base_Core::SQLStatement strSql;

		while( 0 != currentNode )
		{
			//std::ostringstream sql;

			// Node ID + parent ID query
		/*	sql << "select p_key, nvl(parent_node,0) as parent_node from node_tree"
				<< " where p_key = " << currentNode;*/
//			strSql = database->prepareSQLStatement(NODE_TREE_SELECT_33557, currentNode);

		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33557, currentNode);

			std::vector<std::string> columns;
			columns.push_back( "p_key" );
			columns.push_back( "parent_node" );

			std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
			PlanDataBaseHelper dataBaseHelper(database);
						
			if ( (NULL == data.get()) || (0 == data->getNumRows()) ) 
			{
                std::ostringstream reasonMessage;
                reasonMessage << "Node path for plan ID " << plan << " broken at node ID " << currentNode; 

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed at node %lu to find path for plan %lu", currentNode, plan );
                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
			}

            nodeIds.insert( nodeIds.begin(), data->getUnsignedLongData( 0, columns[0] ) );

			currentNode = data->getUnsignedLongData( 0, columns[1] );
		}

		TA_Base_Core::NodeIds_var ni = new TA_Base_Core::NodeIds;

		ni->length(nodeIds.size());

		std::vector<TA_Base_Core::NodeId>::iterator it;

		int i = 0;
		for (it = nodeIds.begin(); it != nodeIds.end(); it++)
		{
			ni.operator [](i++) = *it;
		}

		return ni._retn();
	}

	TA_Base_Core::ENode PlanDataReadingClass::getNodeType( TA_Base_Core::NodeId node ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;

		// Node summary query
		/*sql << "select node_type.type_value" 
			<< " from node_tree, node_type"
			<< " where node_tree.fk_node_type = node_type.p_key"
			<< " and   node_tree.p_key = " << node;*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33513, node);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33513, node);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33513, node);
		
        std::vector<std::string> columns;
		columns.push_back( "type_value" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
            std::ostringstream message;
            message << "Category or Plan Node with NodeId[" << node << "] was not found.";
            std::string strMessage = message.str();
			TA_THROW( TA_Base_Core::DataException( strMessage.c_str(), TA_Base_Core::DataException::NO_VALUE, columns[0] ) );
        }


		switch( data->getUnsignedLongData( 0, columns[0] ) )
		{
			case TA_Base_Core::PLAN_NODE:
			{
				return( TA_Base_Core::PLAN_NODE );
			}
			break;

			case TA_Base_Core::CATEGORY_NODE:
			{
				return( TA_Base_Core::CATEGORY_NODE );
			}
			break;

			default:
			{
				TA_THROW( TA_Base_Core::DataException( "Invalid node type", TA_Base_Core::DataException::WRONG_TYPE, data->getStringData( 0, columns[0] ) ) );
			}
			break;
		}
	}

	TA_Base_Core::EStep PlanDataReadingClass::getStepType( TA_Base_Core::NodeId plan, TA_Base_Core::StepId step, bool byStepNumber ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		//std::ostringstream sql;
		TA_Base_Core::SQLStatement strSql;

		if (byStepNumber)
		{
		/*	sql << "select step_type.type_value"
				<< " from step_type, step_details"
				<< " where step_details.fk_step_type = step_type.p_key"
				<< " and step_details.fk_node_tree = " << plan
				<< " and step_details.position = " << step;*/
//			strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_TYPE_STD_SELECT_64503, plan, step);
//			strSql  = database->prepareSQLStatement(STEP_TYPE_SELECT_64503, plan, step);

		database->prepareSQLStatement(strSql, STEP_TYPE_SELECT_64503, plan, step);
		}
		else
		{
			/*sql << "select step_type.type_value"
				<< " from step_type, step_details"
				<< " where step_details.fk_step_type = step_type.p_key"
				<< " and step_details.p_key = " << step;*/
//			strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_TYPE_STD_SELECT_64504, step);
//			strSql  = database->prepareSQLStatement(STEP_TYPE_SELECT_64504, step);

		database->prepareSQLStatement(strSql, STEP_TYPE_SELECT_64504, step);
		}
		
        std::vector<std::string> columns;
		columns.push_back( "type_value" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			return( TA_Base_Core::UNDEFINED_STEP );
        }

		return(  static_cast<TA_Base_Core::EStep>( data->getUnsignedLongData( 0, columns[0] ) ) );
	}

	TA_Base_Core::StepDetails* PlanDataReadingClass::getStepDetails( TA_Base_Core::NodeId plan, const std::string& stepConstraint ) const
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		std::ostringstream sql;

		// Step details query
	/*	sql << "select step_details.p_key, step_details.position, step_details.name,"
            << " step_type.type_value, nvl(step_details.description, '" << m_nvlValue.c_str() << "'),"
			<< " step_details.skip, step_details.skippable, step_details.delay,"
            << " nvl(step_details.ignore_failure, 0), nvl(step_details.no_wait, 0) "
			<< " from step_details, node_tree, step_type"
			<< " where step_details.fk_node_tree = node_tree.p_key"
			<< " and step_details.fk_step_type = step_type.p_key"
			<< " and step_details.fk_node_tree = " << plan;*/

		if( stepConstraint.length() > 0 )
		{
			sql << " and step_details.position in (" << stepConstraint << ")";
		}
		
		//sql << " order by step_details.position";
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_Oracle_SELECT_63552,
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63552,
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, STEP_DETAILES_SELECT_63552,
			m_nvlValue, plan, sql.str());

        std::vector<std::string> columns;
		columns.push_back( "p_key");
		columns.push_back( "position" );
		columns.push_back( "name" );
        columns.push_back( "type" );
        columns.push_back( "description" );
		columns.push_back( "skip" );
		columns.push_back( "skippable" );
		columns.push_back( "delay" ); 
		columns.push_back( "ignore_failure" ); 
		columns.push_back( "no_wait" ); 

		TA_Base_Core::IData* data = database->executeQuery( strSql, columns ) ;
		PlanDataBaseHelper dataBaseHelper(database);
		TA_Base_Core::StepDetails_var sd = new TA_Base_Core::StepDetails;

        if( (NULL == data) || (0 == data->getNumRows()) )
        {
			sd->length( 0 );
			return sd._retn();
        }

        // TD9308 - keep fetching data until there is no more
		unsigned long rowsFetched = 0;
		do
		{
			std::auto_ptr<TA_Base_Core::IData> dataPtr(data);
			sd->length( data->getNumRows() + rowsFetched );

			for( unsigned long dataIdx = 0; dataIdx < dataPtr->getNumRows(); dataIdx++ )
			{
				sd[rowsFetched + dataIdx].step        = static_cast<TA_Base_Core::StepId> (dataPtr->getUnsignedLongData( dataIdx, columns[0] ) );
				sd[rowsFetched + dataIdx].position    = dataPtr->getUnsignedLongData( dataIdx, columns[1] );
				sd[rowsFetched + dataIdx].name        = CORBA::string_dup( dataPtr->getStringData( dataIdx, columns[2] ).c_str() );
				sd[rowsFetched + dataIdx].type        = static_cast<TA_Base_Core::EStep>( dataPtr->getUnsignedLongData( dataIdx, columns[3] ) );
				sd[rowsFetched + dataIdx].description = CORBA::string_dup( getNvlString(dataPtr->getStringData( dataIdx, columns[4] )).c_str() );
				sd[rowsFetched + dataIdx].skip        = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[5] ) );
				sd[rowsFetched + dataIdx].skippable   = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[6] ) );
				sd[rowsFetched + dataIdx].delay       = static_cast<TA_Base_Core::Seconds>( dataPtr->getUnsignedLongData( dataIdx, columns[7] ) );
				sd[rowsFetched + dataIdx].ignoreFailure= static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[8] ) );
				sd[rowsFetched + dataIdx].nowait= static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[9] ) );
			}
			rowsFetched += dataPtr->getNumRows();
			dataPtr.reset();
		}
		while ( database->moreData( data ) );

		return sd._retn();
	}


	TA_Base_Core::Plan* PlanDataReadingClass::getPlan( TA_Base_Core::NodeId plan )
	{
		TA_Base_Core::Plan_var thePlan = new TA_Base_Core::Plan;
		TA_Base_Core::NodeDetail_var nodeDetail = getNodeDetail(plan);

		thePlan->detail = nodeDetail->plan();

        TA_Base_Core::Steps_var varSteps = getAllSteps( plan );

        thePlan->stepseq.length( varSteps->length() );

		for( unsigned long i=0; i < varSteps->length(); i++ )
		{
			thePlan->stepseq[i].detail = varSteps[i].detail;
			thePlan->stepseq[i].parameters = varSteps[i].parameters;
		}

		return( thePlan._retn() );
	}

	TA_Base_Core::DataPointValue PlanDataReadingClass::buildDataPointValue( const std::string& dbString ) const
	{
		TA_Base_Core::DataPointValue rval;

		std::istringstream st(dbString);

		
		CORBA::ULong dataType;
		st >> dataType;
		st.ignore();
		
		switch( dataType )
		{
			case TA_Base_Core::BOOLEAN_DATA_POINT:
				CORBA::Boolean tmpBool;
				st >> tmpBool;
				rval.booleanValue( tmpBool );
				break;

			case TA_Base_Core::NUMERIC_DATA_POINT:
				TA_Base_Core::NumericDataPoint tmpNd;
				st >> tmpNd.value;
				st.ignore();
				st >> tmpNd.precision;
				rval.numericValue(tmpNd);
				break;

			case TA_Base_Core::TEXT_DATA_POINT:
				{

					std::string tmpStr = st.str().substr(2, st.str().length());
					rval.textValue( tmpStr.c_str() );
				}
				break;
		}
		return rval;
	}

	TA_Base_Core::WindowPosition PlanDataReadingClass::getWindowPos( const std::string& dbString ) const
	{
		TA_Base_Core::WindowPosition rval;

		std::istringstream st(dbString);
		st >> rval.top;
		st.ignore();
        st >> rval.left;
        st.ignore();
        st >> rval.width;
        st.ignore();
        st >> rval.height;
		return rval;
	}

	unsigned long PlanDataReadingClass::getNextPlanSeqNumber(void)
	{
		TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;

		sql << "select plans_seq.nextval Next_Step_Id from dual";*/
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), STEP_DETAILS_Oracle_SELECT_63553);
//		std::string strSql  = database->prepareSQLStatement(STEP_DETAILES_SELECT_63553);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, STEP_DETAILES_SELECT_63553);
		
        std::vector<std::string> columns;
		columns.push_back( "Next_Step_Id" );
		
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
		PlanDataBaseHelper dataBaseHelper(database);

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			std::cout << "No next step id returned." << std::endl;
        }

		return(  data->getUnsignedLongData( 0, columns[0] ) );
	}

    std::string PlanDataReadingClass::getNvlString( const std::string& strSrc ) const
    {
        if ( strSrc.compare(m_nvlValue) == 0 )
        {
            return "";
        }

        return strSrc;
    }

    TA_Base_Core::NodeId PlanDataReadingClass::getPlanId(const std::string& planReference) const
    {
        // This method is called when extracting from the database plan step parameters that reference a plan
        // (such as the child plan parameter of a Run Plan step). The specified plan reference can contain a 
        // numeric plan ID or it can be a fully qualified plan path.
        // If the plan reference contains only numeric characters it must be a plan ID, otherwise it is 
        // interpreted to be a plan path.

        if (!planReference.empty())
        {
            if (planReference.find_first_not_of(" 0123456789") == std::string::npos)
            {
                return ::strtoul(planReference.c_str(), NULL, 10);
            }
            else
            {
                try
                {
                    return getNodeId(planReference);
                }
                catch (...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to retrieve plan ID for plan %s", planReference.c_str());
                }
            }
        }

        return 0;
    }

    TA_Base_Core::StepId PlanDataReadingClass::getStepId(TA_Base_Core::NodeId plan, const std::string& stepReference) const
    {
        // This method is called when extracting from the database plan step parameters that reference a plan
        // step (such as the target step parameter of a Jump step). The specified step reference can contain a 
        // numeric step ID or it can be a relative step position.
        // If the plan reference contains only numeric characters it must be a step ID, otherwise it is 
        // interpreted to be a step number.

        if (!stepReference.empty())
        {
            if (stepReference.find_first_not_of(" 0123456789") == std::string::npos)
            {
                return ::strtoul(stepReference.c_str(), NULL, 10);
            }
            else
            {
                try
                {
                    // When the step is referenced by its relative position, the format used is "#<step number".
                    std::string stepNumber(stepReference.substr(stepReference.find_first_of("0123456789")));

                    return getStepId(plan, ::strtoul(stepNumber.c_str(), NULL, 10));
                }
                catch (...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                        "Failed to retrieve step ID for plan %lu, step %s", plan, stepReference.c_str());
                }
            }
        }

        return 0;
    }

    TA_Base_Core::StepId PlanDataReadingClass::getStepId( const StepNumToStepIdMap& refStepNumToIds, const std::string& stepReference ) const
    {
        if ( !stepReference.empty() )
        {
            if ( stepReference.find_first_not_of(" 0123456789") == std::string::npos )
            {
                return stringToUlong( stepReference );
            }
            else
            {
                // When the step is referenced by its relative position, the format used is "#<step number".
                std::string stepNumber(stepReference.substr(stepReference.find_first_of("0123456789")));
                TA_Base_Core::StepNumber ulStepNum = static_cast<TA_Base_Core::StepNumber>(stringToUlong( stepNumber ));
                StepNumToStepIdMap::const_iterator itFound = refStepNumToIds.find( ulStepNum );
                if ( refStepNumToIds.end() != itFound )
                {
                    return itFound->second;
                }
                else
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Step number [%lu] could not be found", ulStepNum );
                }
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Step ID is empty string" );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not convert Step ID from [%s]", stepReference.c_str() );
        return 0u;
    }

    void PlanDataReadingClass::convertStepParameters(  TA_Base_Core::StepParameters& refStepParameters,
        const StepNumToStepIdMap& refStepNumToIds, const TA_Base_Core::StepId step, 
        const TA_Base_Core::EStep eType, MapParameters& mapParameters ) const
    {
        TA_Base_Core::StepNumber nPos = static_cast<TA_Base_Core::StepNumber>(1);

		switch( eType )
		{
			case TA_Base_Core::END_STEP:
			{
				TA_Base_Core::EndParameters ep = 0;
				refStepParameters.end( ep );
			}
			break;

			case TA_Base_Core::ABORT_STEP:
			{
				TA_Base_Core::AbortParameters ap = 0;
				refStepParameters.abort( ap );
			}
			break;

			case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
			{
				if( 3 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for Active Plan Check Step with step number " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::ActivePlanCheckParameters apc;
                apc.plan    = getPlanId(mapParameters[nPos++]);
				apc.yesStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
				apc.noStep  = getStepId(refStepNumToIds, mapParameters[nPos++]);                
				refStepParameters.activePlanCheck(apc);
			}
			break;

			case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
			{
                if( 2 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Insufficient parameters found for Assign Camera to Monitor Step with step number " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::AssignCameraToMonitorParameters cameraToMonitor;
				cameraToMonitor.camera = stringToUlong( mapParameters[nPos++] );
				cameraToMonitor.monitor = stringToUlong( mapParameters[nPos++] );
				refStepParameters.cameraMonitor(cameraToMonitor);
			}
			break;

			case TA_Base_Core::DATA_POINT_CHECK_STEP:
			{
				if( 5 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Found " << mapParameters.size() << " parameters, expecting 5 for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                TA_Base_Core::DataPointCheckParameters dpc;
				dpc.dataPoint   = CORBA::string_dup( mapParameters[nPos++].c_str() );
				dpc.value = buildDataPointValue( mapParameters[nPos++] );
				dpc.testType = static_cast<TA_Base_Core::EDataPointTest>( stringToUlong( mapParameters[nPos++] ));
				dpc.successStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
				dpc.failureStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
				refStepParameters.dataPointCheck(dpc);
			}
			break;

			case TA_Base_Core::DATA_POINT_SET_STEP:
			{
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::DataPointSetParameters dps;
				dps.dataPoint = CORBA::string_dup( mapParameters[nPos++].c_str() );
				dps.newValue  = buildDataPointValue( mapParameters[nPos++] );
				refStepParameters.dataPointSet(dps);
			}
			break;

			case TA_Base_Core::DECISION_STEP:
			{
				int numRows = mapParameters.size(); 
				if(( 4 != numRows) &&  (9 != numRows))
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::DecisionParameters dec;
				dec.decisionMessage = CORBA::string_dup( mapParameters[nPos++].c_str() );
				dec.responseTimeout = static_cast<TA_Base_Core::Seconds>(stringToUlong( mapParameters[nPos++] ));
				dec.yesStep         = getStepId(refStepNumToIds, mapParameters[nPos++]);
				dec.noStep          = getStepId(refStepNumToIds, mapParameters[nPos++]);

				if ( 4 == numRows )
				{
					// set default values when the new decision step paramaters are not available					
					dec.yesButtonLabel     = CORBA::string_dup("Yes");
					dec.noButtonLabel      = CORBA::string_dup("No");
					dec.pauseButtonLabel   = CORBA::string_dup("Pause");
					dec.noButtonEnabled    = CORBA::Boolean(true);
					dec.pauseButtonEnabled = CORBA::Boolean(false);
				}
				else
				{	
					dec.yesButtonLabel  = CORBA::string_dup( mapParameters[nPos++].c_str() );
					dec.noButtonLabel   = CORBA::string_dup( mapParameters[nPos++].c_str() );
					dec.pauseButtonLabel  = CORBA::string_dup( mapParameters[nPos++].c_str() );
					
					unsigned long buttonEnabled; 
					buttonEnabled = stringToUlong( mapParameters[nPos++] );
					dec.noButtonEnabled = buttonEnabled ? CORBA::Boolean ( true ) : CORBA::Boolean ( false );
					
					buttonEnabled = stringToUlong( mapParameters[nPos++] );
					dec.pauseButtonEnabled  = buttonEnabled ? CORBA::Boolean ( true ) : CORBA::Boolean ( false );
				}
				refStepParameters.decision(dec);
			}
			break;

			case TA_Base_Core::JUMP_STEP:
			{
				if( 1 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::JumpParameters jump;
				jump.step = getStepId(refStepNumToIds, mapParameters[nPos++]);
				refStepParameters.jump(jump);
			}
			break;

			case TA_Base_Core::LOG_STEP:
			{
				if( 3 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::LogParameters log;
				log.message = CORBA::string_dup( mapParameters[nPos++].c_str() );

                unsigned long required = stringToUlong( mapParameters[nPos++] );
                if ( required )
                    log.logRequired = CORBA::Boolean ( true );
                else
                    log.logRequired = CORBA::Boolean ( false );

				log.responseTimeout = static_cast<TA_Base_Core::Seconds>( stringToUlong( mapParameters[nPos++] ));
				refStepParameters.log(log);
			}
			break;

            case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
            {
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::ActivateCameraPresetParameters cameraPreset;
				cameraPreset.camera        = stringToUlong( mapParameters[nPos++] );
				cameraPreset.preset       = stringToUlong( mapParameters[nPos++] );
				refStepParameters.cameraPreset(cameraPreset);
            }
            break;


			case TA_Base_Core::PROMPT_STEP:
			{
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::PromptParameters prompt;
				prompt.message        = CORBA::string_dup( mapParameters[nPos++].c_str() );
				prompt.dismissTimeout = static_cast<TA_Base_Core::Seconds>(stringToUlong( mapParameters[nPos++] ));
				refStepParameters.prompt(prompt);
			}
			break;

			case TA_Base_Core::RUN_PLAN_STEP:
			{
				if( 3 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::RunPlanParameters rp;
                rp.plan              = getPlanId(mapParameters[nPos++]);
				rp.manualLaunch      = static_cast<CORBA::Boolean>(0 != stringToUlong( mapParameters[nPos++] ));
				rp.waitForCompletion = static_cast<CORBA::Boolean>(0 != stringToUlong( mapParameters[nPos++] )); 
				refStepParameters.runPlan(rp);
			}
			break;

			case TA_Base_Core::TERMINATE_PLAN_STEP:
			{
				if( 1 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::TerminatePlanParameters tp;
                tp.plan = getPlanId(mapParameters[nPos++]);
				refStepParameters.terminatePlan(tp);
			}
			break;

			case TA_Base_Core::VMS_SET_SCENE_STEP:
			{
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VmsSetSceneParameters vms;
				vms.vmsSign    = stringToUlong( mapParameters[nPos++] );
				vms.scene   = stringToUlong( mapParameters[nPos++] );
				refStepParameters.vmsSetScene(vms);
			}
			break;

            case TA_Base_Core::LAUNCH_GUI_APP_STEP:
            {
                if ( 3 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::LaunchGuiAppParameters guiApp;
				guiApp.app    = stringToUlong( mapParameters[nPos++] );
				guiApp.args   = CORBA::string_dup( mapParameters[nPos++].c_str() );
                std::string strPos = mapParameters[nPos++];

                guiApp.pos = getWindowPos( strPos );

				refStepParameters.guiAppParameters(guiApp);
            }
            break;

            case TA_Base_Core::VMS_BLANKOUT_STEP:
            {
				if( 1 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VmsBlankoutParameters vms;
				vms.vmsSign    = stringToUlong( mapParameters[nPos++] );
				refStepParameters.vmsBlankout(vms);

            }
            break;

            case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
            {
				if( 1 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VmsRestoreScheduleParameters vms;
				vms.vmsSign    = stringToUlong( mapParameters[nPos++] );
				refStepParameters.vmsRestoreSchedule(vms);

            }
                break;

            case TA_Base_Core::VIDEO_WALL_STEP:
            {
				if( 2 != mapParameters.size() ) 
				{
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

				TA_Base_Core::VideoWallParameters videoWall;
				videoWall.wallBoard = stringToUlong( mapParameters[nPos++] );
				videoWall.layout    = stringToUlong( mapParameters[nPos++] );
				refStepParameters.videoWallParams(videoWall);

            }
            break;

            case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
            {
	            
	            // Must be at least  8 rows, may be more depending on number of specific zones targetted.
	            if ( 8 > mapParameters.size() )
	            {
		            std::ostringstream reasonMessage;
		            reasonMessage  << "Insufficient parameters found for step " << step; 
		            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
	            }
	            
	            const std::string SEPARATOR(",");
	            TA_Base_Core::StationPublicAddressParameters stationPAParams;
	            std::string destination(mapParameters[nPos++]);
	            

	            // The station destination parameter is formatted like so:
	            // "[station],[LocationKey],all_stations"
	            //
	            // Station may be blank (in which case all_stations should be true).
	            std::string station(destination.substr(0, destination.find(SEPARATOR)));
	            destination.erase(0, station.length() + 1);
	            stationPAParams.station.station = CORBA::string_dup(station.c_str());
                std::istringstream istrLocationkey(destination.substr(0, destination.find(SEPARATOR)));
                istrLocationkey >> stationPAParams.station.locationKey;
                destination.erase(0, istrLocationkey.str().length() + 1);
	            stationPAParams.station.allStations = (destination.substr(0, destination.find(SEPARATOR)) != "0");
	            
	            // The station PA message is made up of 3 numerics formatted like so:
	            // "library_version,library_section,message_id"
	            //
	            // The separator is ignored, so it need not be a comma.
	            std::istringstream message(mapParameters[nPos++]);
	            //message >> stationPAParams.message.libVersion;
	            //message.ignore(1);
	            //message >> stationPAParams.message.libSection;
	            //message.ignore(1);
	            message >> stationPAParams.message.messageId;
	            
	            //chime
	            unsigned long chime = stringToUlong( mapParameters[nPos++] );
	            if ( chime )
                    stationPAParams.chime = CORBA::Boolean ( true );
                else
                    stationPAParams.chime = CORBA::Boolean ( false );


                //cyclic
                unsigned long cyclic = stringToUlong( mapParameters[nPos++] );
                if ( cyclic )
                    stationPAParams.cyclic = CORBA::Boolean ( true );
                else
                    stationPAParams.cyclic = CORBA::Boolean ( false );

                //TIS Synch
                unsigned long synch = stringToUlong( mapParameters[nPos++] );
                if ( synch )
                    stationPAParams.TISSynch = CORBA::Boolean ( true );
                else
                    stationPAParams.TISSynch = CORBA::Boolean ( false );

                //duration
                 stationPAParams.duration = stringToUlong( mapParameters[nPos++] );

                //interval
                stationPAParams.interval = stringToUlong( mapParameters[nPos++] );

                // cyclic times
                stationPAParams.cyclicTimes = stringToUlong( mapParameters[nPos++] );


                // Finally, the zones. These are stored in batches of 15 per row.
                // The row format is: "zoneid1,zoneid2, ... ,zoneidN"

                try
                {
	                stationPAParams.zones.length(0); // Assume there are no zones configured

                    while ( nPos <= mapParameters.size() )
                    {
                        std::string zoneList( mapParameters[nPos++] );

                        while (!zoneList.empty())
                        {
	                        std::string zone(zoneList.substr(0, zoneList.find(SEPARATOR)));
	                        zoneList.erase(0, zone.length() + 1);

	                        unsigned long i(stationPAParams.zones.length());
	                        
	                        // Expand the sequence to receive the new string.
	                        stationPAParams.zones.length(i + 1);
	                        stationPAParams.zones[i] = atol (zone.c_str());
                        }
                    }
                }
                catch (...)
                {
	                std::ostringstream reasonMessage;
	                reasonMessage  << "Failed to retrieve zone ID parameters for step " << step; 
	                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));                       
                }

                refStepParameters.stationPublicAddress(stationPAParams);						
            }
            break;

            case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
            {
                // Must be 4 rows
                if ( 4 > mapParameters.size() )
                {
	                std::ostringstream reasonMessage;
	                reasonMessage  << "Insufficient parameters found for step " << step; 
	                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                const std::string SEPARATOR(",");
                TA_Base_Core::TrainPublicAddressParameters trainPAParams;

                // The train TIS message destination parameter is made up of 2 numerics formatted like so:
                // "train_id,train_group"
                //
                // The separator is ignored, so it need not be a comma.
                std::istringstream destination( mapParameters[nPos++] );
                destination >> trainPAParams.destination.trainId;
                destination.ignore(1);
                destination >> reinterpret_cast<int&>(trainPAParams.destination.trainGroup);

                // The station PA message is made up of 3 numerics formatted like so:
                // "library_version,library_section,message_id"
                //
                // The separator is ignored, so it need not be a comma.
                std::istringstream message( mapParameters[nPos++] );
                message >> trainPAParams.message.libVersion;
                message.ignore(1);
                message >> trainPAParams.message.libSection;
                message.ignore(1);
                message >> trainPAParams.message.messageId;

                //cyclic
                unsigned long cyclic = stringToUlong( mapParameters[nPos++] );
                if ( cyclic )
	                trainPAParams.cyclic = CORBA::Boolean ( true );
                else
	                trainPAParams.cyclic = CORBA::Boolean ( false );

                //duration
                trainPAParams.duration = stringToUlong( mapParameters[nPos++] );

                refStepParameters.trainPublicAddress(trainPAParams);
            }
            break;

            case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
            {
                if ( 2 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
				}

                TA_Base_Core::SendSmsMessageParameters sendSmsMessageParams;
                sendSmsMessageParams.recipientIdentifier = CORBA::string_dup( mapParameters[nPos++].c_str() );
                sendSmsMessageParams.textMessage = CORBA::string_dup( mapParameters[nPos++].c_str() );
                refStepParameters.sendSmsMessage( sendSmsMessageParams );
            }
            break;

            case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
            {
                if ( 3 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::LaunchSchematicParameters launchSchematicParams;
				launchSchematicParams.schematic = CORBA::string_dup( mapParameters[nPos++].c_str() );
                launchSchematicParams.screen = stringToUlong( mapParameters[nPos++] );
				launchSchematicParams.commands = CORBA::string_dup( mapParameters[nPos++].c_str() );

				refStepParameters.launchSchematic(launchSchematicParams);
            }
            break;

            case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
            {
                if ( 3 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::EvaluateExpressionParameters evalExp;
                evalExp.expression  = CORBA::string_dup( mapParameters[nPos++].c_str() );
                evalExp.successStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                evalExp.failureStep = getStepId(refStepNumToIds, mapParameters[nPos++]);

                refStepParameters.evaluateExpression(evalExp);
            }
            break;
				 
            case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
            {
                // Must be at least 5 rows, will be more if specific PIDs have been targetted.
                if ( 5 > mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::StationTravellerInfoSystemParameters stationTisParams;
                const std::string SEPARATOR( "," );
                const std::string SEPARATOR2( ";" );

                //pidlist
                stationTisParams.pidList.length( 0 );
                std::string strpidsList( mapParameters[nPos++] );
                std::vector<std::string> pidsVector = tokenizeString( strpidsList, SEPARATOR );
                for ( std::vector<std::string>::iterator ite = pidsVector.begin();
                        ite != pidsVector.end();
                        ite++ )
                {
                    if ( !( *ite ).empty() )
                    {
                        stationTisParams.pidList.length( stationTisParams.pidList.length() + 1 );
                        stationTisParams.pidList[stationTisParams.pidList.length() - 1]	= CORBA::string_dup( ( *ite ).c_str() );
                    }
                }

                //PidsDestinationList
                stationTisParams.destinationList.length( 0 ); //set destination list length to 0
                std::string destinationList( mapParameters[nPos++] );
                std::vector<std::string> listVector = tokenizeString( destinationList, SEPARATOR2 );
                for ( std::vector<std::string>::iterator ite = listVector.begin();
                        ite != listVector.end();
                        ite++ )
                {
                    if ( !( *ite ).empty() )
                    {
                        TA_Base_Core::PidsDestination tmpDest;
                        std::vector<std::string> destinationVector = tokenizeString( *ite, SEPARATOR );

                        tmpDest.station = stringToUlong( destinationVector[0] );
						tmpDest.gateway = stringToUlong( destinationVector[1] );
                        tmpDest.downTrack = stringToUlong( destinationVector[2] );
                        tmpDest.upTrack = stringToUlong( destinationVector[3] );
                        tmpDest.concourse = stringToUlong( destinationVector[4] );

                        stationTisParams.destinationList.length( stationTisParams.destinationList.length() +  1 );
                        stationTisParams.destinationList[stationTisParams.destinationList.length() - 1] = tmpDest;
                    }
                }

                //PidsMessage
                std::string tmpMsgContent( mapParameters[nPos++] );
                stationTisParams.messageContent.length( tmpMsgContent.length() );
                int i = 0;
                std::string::iterator ite = tmpMsgContent.begin();
                for ( ; ite != tmpMsgContent.end(); ite++, i++ )
                {
                    stationTisParams.messageContent[i] = *ite;
                }

				stationTisParams.msgType = stringToUlong( mapParameters[nPos++] );
				stationTisParams.showType = stringToUlong( mapParameters[nPos++] );
				stationTisParams.isTrain = stringToUlong( mapParameters[nPos++] );

                //startTime
                std::vector<std::string> startTimeVector = tokenizeString( mapParameters[nPos++], SEPARATOR );

                if ( !startTimeVector.empty() )
                {
                    stationTisParams.startTime.year = stringToUlong( startTimeVector[0] );
                    stationTisParams.startTime.month = stringToUlong( startTimeVector[1] );
                    stationTisParams.startTime.day = stringToUlong( startTimeVector[2] );
                    stationTisParams.startTime.hour = stringToUlong( startTimeVector[3] );
                    stationTisParams.startTime.minute = stringToUlong( startTimeVector[4] );
                    stationTisParams.startTime.second = stringToUlong( startTimeVector[5] );
                }
				
                //endTime
                std::vector<std::string> endTimeVector = tokenizeString( mapParameters[nPos++], SEPARATOR );
                if ( !endTimeVector.empty() )
                {
                    stationTisParams.startTime.year = stringToUlong( endTimeVector[0] );
                    stationTisParams.startTime.month = stringToUlong( endTimeVector[1] );
                    stationTisParams.startTime.day = stringToUlong( endTimeVector[2] );
                    stationTisParams.startTime.hour = stringToUlong( endTimeVector[3] );
                    stationTisParams.startTime.minute = stringToUlong( endTimeVector[4] );
                    stationTisParams.startTime.second = stringToUlong( endTimeVector[5] );
                }
				
				stationTisParams.interval = stringToUlong( mapParameters[nPos++] );
				stationTisParams.isCyclic = stringToUlong( mapParameters[nPos++] );

                refStepParameters.stationTravellerInfoSystem(stationTisParams);
            }
            break;

            case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
            {
                if ( 4 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }
                
				TA_Base_Core::TrainTravellerInfoSystemParameters trainTisParams;
                std::istringstream destination(mapParameters[nPos++]);
                std::istringstream message(mapParameters[nPos++]);
                trainTisParams.priority = static_cast<unsigned short>(stringToUlong(mapParameters[nPos++]));
                trainTisParams.duration = stringToUlong(mapParameters[nPos++]);


                // The train TIS message destination parameter is made up of 2 numerics formatted like so:
                // "train_id,train_group"
                //
                // The separator is ignored, so it need not be a comma.
 
                destination >> trainTisParams.destination.trainId;
                destination.ignore(1);
                destination >> reinterpret_cast<int&>(trainTisParams.destination.trainGroup);
                
                // The train TIS message is made up of 3 numerics formatted like so:
                // "library_version,library_section,message_id"
                //
                // The separator is ignored, so it need not be a comma.

                message >> trainTisParams.message.libraryVersion;
                message.ignore(1);
                message >> trainTisParams.message.librarySection;
                message.ignore(1);
                message >> trainTisParams.message.messageId;

                refStepParameters.trainTravellerInfoSystem(trainTisParams);
            }
            break;

			case TA_Base_Core::FAILURE_SUMMARY_STEP:
			{
				if ( 1 != mapParameters.size() )
                {
					std::ostringstream reasonMessage;
					reasonMessage  << "Insufficient parameters found for step " << step; 
					TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

				TA_Base_Core::FailureSummaryParameters summary;
				summary.failureCategory = static_cast<TA_Base_Core::EStepFailureCategory>(stringToUlong(mapParameters[nPos++]));

				refStepParameters.failureSummary(summary);
			}
			break;

            case TA_Base_Core::PLAN_CHECK_TIMER_STEP:
            {
                if( 3 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Found " << mapParameters.size() << " parameters, expecting 3 for step " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::PlanCheckTimerParameters pstc;
                pstc.timeSpan = stringToUlong(mapParameters[nPos++]);
                pstc.successStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                pstc.failureStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                refStepParameters.planCheckTimer(pstc);
            }
            break;

            case TA_Base_Core::PLAN_SET_TIMER_STEP:
            {
                if( 1u != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Found " << mapParameters.size() << " parameters, expecting 1 for step " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::PlanSetTimerParameters stParamSetTimer;
                stParamSetTimer.time = stringToUlong(mapParameters[nPos++]);
                refStepParameters.planSetTimer(stParamSetTimer);
            }
            break;

            case TA_Base_Core::EXECUTE_SCRIPT_STEP:
            {
                if( 1 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Found " << mapParameters.size() << " parameters, expecting 1 for step " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::ExecuteScriptParameters rsp;
                rsp.commands   = CORBA::string_dup( mapParameters[nPos++].c_str() );

                refStepParameters.executeScript(rsp);
            }
            break;

            case TA_Base_Core::DATA_POINT_TIME_CHECK_STEP:
            {
                if( 4 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Found " << mapParameters.size() << " parameters, expecting 4 for step " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::DataPointTimeCheckParameters dptc;
                dptc.dataPoint   = CORBA::string_dup( mapParameters[nPos++].c_str() );
                dptc.timeSpan    = stringToUlong(mapParameters[nPos++]);
                dptc.successStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                dptc.failureStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                refStepParameters.dataPointTimeCheck(dptc);
            }
            break;

            case TA_Base_Core::POPUP_MFT_MESSAGE_STEP:
            {
                if( 12 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Found " << mapParameters.size() << " parameters, expecting 4 for step " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::PopupMFTMessageParameters pmm;
                pmm.decisionMessage = CORBA::string_dup( mapParameters[nPos++].c_str() );
                pmm.yesStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                pmm.noStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                pmm.pendingStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                pmm.operatorProfileNumber = stringToUlong(mapParameters[nPos++]);
                pmm.messageBoxType = stringToUlong(mapParameters[nPos++]);
                pmm.abortStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                pmm.ignoreStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                pmm.retryStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                pmm.okStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                pmm.cancelStep = getStepId(refStepNumToIds, mapParameters[nPos++]);
                pmm.timeSpan = stringToUlong(mapParameters[nPos++]);

                refStepParameters.popupMFTMessage(pmm);
            }
            break;

            case TA_Base_Core::SHUTDOWN_SYSTEM_STEP:
            {
                if( 1 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Found " << mapParameters.size() << " parameters, expecting 1 for step " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::ShutdownSystemParameters ssp;

                unsigned long notifyPeer = stringToUlong( mapParameters[nPos++] );                    
                ssp.notifyPeer = ( 0u != notifyPeer ) ? CORBA::Boolean ( true ) : CORBA::Boolean ( false );

                refStepParameters.shutdownSystem(ssp);
            }
            break;

			case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP_BY_ALARM:
			{
				TA_Base_Core::StationPublicAddressByAlarmParameters stationPAByAlarmParameters;

				if (1 == mapParameters.size())
				{					
					stationPAByAlarmParameters.message = CORBA::string_dup( mapParameters[nPos++].c_str() );
				}
				else
				{
					stationPAByAlarmParameters.message = CORBA::string_dup( "Station Public Address By Alarm." );
				}

				refStepParameters.stationPublicAddrssByAlarm(stationPAByAlarmParameters);
			}
			break;

            case TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP:
            {
                if( 6 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Insufficient parameters found for background launch schematic Step with step number " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::BackgroundLaunchSchematicParameters bglsp;
                bglsp.schematic = CORBA::string_dup( mapParameters[nPos++].c_str() );
                bglsp.screen = stringToUlong( mapParameters[nPos++] );
                bglsp.commands = CORBA::string_dup( mapParameters[nPos++].c_str() );
                bglsp.actionTypeKey = stringToUlong(mapParameters[nPos++]);
                bglsp.dataPoint = stringToUlong(mapParameters[nPos++]);
                bglsp.profile = stringToUlong(mapParameters[nPos++]);
                refStepParameters.bgLaunchSchematic(bglsp);
            }
            break;

            case TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP:
            {
                if( 8 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Insufficient parameters found for background launch gui Step with step number " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::BackgroundLaunchGuiAppParameters bglgap;

                bglgap.app    = stringToUlong( mapParameters[nPos++] );
                bglgap.args   = CORBA::string_dup( mapParameters[nPos++].c_str() );
                std::string strPos = mapParameters[nPos++];

                bglgap.pos = getWindowPos( strPos );
                bglgap.bAddTriggerEntity = stringToUlong( mapParameters[nPos++] );
                bglgap.actionTypeKey = stringToUlong(mapParameters[nPos++]);
                bglgap.dataPoint = stringToUlong(mapParameters[nPos++]);
                bglgap.bTriggerEntityCheckAction = stringToUlong( mapParameters[nPos++] );
                bglgap.profile = stringToUlong( mapParameters[nPos++] );

                refStepParameters.bgLaunchGuiApp(bglgap);
            }
            break;

            case TA_Base_Core::BACKGROUND_DECISION_STEP:
            {
                int numRows = mapParameters.size();
                if(( 5 != numRows) &&  (10 != numRows))
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Insufficient parameters found for step " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::BackgroundDecisionParameters bgdec;
                bgdec.decisionMessage = CORBA::string_dup( mapParameters[nPos++].c_str() );
                bgdec.responseTimeout = static_cast<TA_Base_Core::Seconds>(stringToUlong( mapParameters[nPos++] ));
                bgdec.yesStep         = getStepId(refStepNumToIds, mapParameters[nPos++]);
                bgdec.noStep          = getStepId(refStepNumToIds, mapParameters[nPos++]);

                if ( 5 == numRows )
                {
                    // set default values when the new decision step paramaters are not available					
                    bgdec.yesButtonLabel     = CORBA::string_dup("Yes");
                    bgdec.noButtonLabel      = CORBA::string_dup("No");
                    bgdec.pauseButtonLabel   = CORBA::string_dup("Pause");
                    bgdec.noButtonEnabled    = CORBA::Boolean(true);
                    bgdec.pauseButtonEnabled = CORBA::Boolean(false);
                }
                else
                {	
                    bgdec.yesButtonLabel  = CORBA::string_dup( mapParameters[nPos++].c_str() );
                    bgdec.noButtonLabel   = CORBA::string_dup( mapParameters[nPos++].c_str() );
                    bgdec.pauseButtonLabel  = CORBA::string_dup( mapParameters[nPos++].c_str() );

                    unsigned long buttonEnabled; 
                    buttonEnabled = stringToUlong( mapParameters[nPos++] );
                    bgdec.noButtonEnabled = buttonEnabled ? CORBA::Boolean ( true ) : CORBA::Boolean ( false );

                    buttonEnabled = stringToUlong( mapParameters[nPos++] );
                    bgdec.pauseButtonEnabled  = buttonEnabled ? CORBA::Boolean ( true ) : CORBA::Boolean ( false );
                }
                bgdec.profile = stringToUlong( mapParameters[nPos++] );
                refStepParameters.bgDecision(bgdec);
            }
            break;

            case TA_Base_Core::BACKGROUND_LOG_STEP:
            {
                if( 4 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Insufficient parameters found for step " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::BackgroundLogParameters bglog;
                bglog.message = CORBA::string_dup( mapParameters[nPos++].c_str() );

                unsigned long required = stringToUlong( mapParameters[nPos++] );
                if ( required )
                    bglog.logRequired = CORBA::Boolean ( true );
                else
                    bglog.logRequired = CORBA::Boolean ( false );

                bglog.responseTimeout = static_cast<TA_Base_Core::Seconds>( stringToUlong( mapParameters[nPos++] ));
                bglog.profile = stringToUlong( mapParameters[nPos++] );
                refStepParameters.bgLog(bglog);
            }
            break;

            case TA_Base_Core::BACKGROUND_PROMPT_STEP:
            {
                if( 3 != mapParameters.size() ) 
                {
                    std::ostringstream reasonMessage;
                    reasonMessage  << "Insufficient parameters found for step " << step; 
                    TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,""));
                }

                TA_Base_Core::BackgroundPromptParameters bgprompt;
                bgprompt.message        = CORBA::string_dup( mapParameters[nPos++].c_str() );
                bgprompt.dismissTimeout = static_cast<TA_Base_Core::Seconds>(stringToUlong( mapParameters[nPos++] ));
                bgprompt.profile = stringToUlong( mapParameters[nPos++] );
                refStepParameters.bgPrompt(bgprompt);
            }
            break;

			default:
				// throw an exception
				break;
		}
    }

    unsigned long PlanDataReadingClass::stringToUlong( const std::string& strSrc ) const
    {
        std::stringstream stmConverter;
        stmConverter << strSrc.c_str();

        unsigned long ulResult = 0;
        stmConverter >> ulResult;

        return ulResult;
    }

    std::map<unsigned long, int> PlanDataReadingClass::getStepUseMap()
    {
        TA_Base_Core::IDatabase* database= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement strSql;
        database->prepareSQLStatement(strSql, STEP_TYPE_SELECT_64505);

        std::vector<std::string> stepTypeColumns;
        stepTypeColumns.push_back( "TYPE_VALUE" );
        stepTypeColumns.push_back( "STEP_USE" );

        TA_Base_Core::IData* stepUseData = database->executeQuery( strSql, stepTypeColumns );
        PlanDataBaseHelper dataBaseHelper(database);

        std::map<unsigned long, int> stepUseMap;

        if( (NULL == stepUseData ) || (0 == stepUseData->getNumRows()) )
        {
            return stepUseMap;
        }

        do
        {
            std::auto_ptr<TA_Base_Core::IData> stepUseDataPtr(stepUseData);
            for( unsigned long dataIdx = 0; dataIdx < stepUseData->getNumRows(); dataIdx++ )
            {
                stepUseMap[stepUseDataPtr->getUnsignedLongData(dataIdx, stepTypeColumns[0])] = stepUseDataPtr->getUnsignedLongData(dataIdx, stepTypeColumns[1]);
            }
            stepUseDataPtr.reset();
        }
        while ( database->moreData( stepUseData ) );

        return stepUseMap;
    }

} // TA_Base_Bus
