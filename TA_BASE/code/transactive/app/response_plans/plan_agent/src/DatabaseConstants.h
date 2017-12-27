#ifndef DATABASE_CONSTANTS_HEADER_INCLUDED
#define DATABASE_CONSTANTS_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DatabaseConstants.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2012/06/12 13:35:44 $
  *  Last modified by:  $Author: builder $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

// Database Table names
const std::string APPROVAL_STATE_TABLE = "approval_state";
const std::string NODE_TYPE_TABLE = "node_type";
const std::string NODE_TREE_TABLE = "node_tree";
const std::string STEP_TYPE_TABLE = "step_type";
const std::string STEP_DETAILS_TABLE = "step_details";
const std::string PARAMETER_TYPE_TABLE = "parameter_type_table";
const std::string STEPTYPE_PARAMTYPE_TABLE = "step_type_parameter_type";
const std::string STEP_PARAMETER_TABLE = "step_parameters";

//Views
const std::string NODE_SUMMARY_VIEW = "node_summary";
const std::string PLAN_DETAILS_VIEW = "plan_details";
const std::string CATEGORY_DETAILS_VIEW = "category_details";
const std::string PARAMETER_NAMES_VIEW = "parameter_names";

//Column Names for APPROVAL_STATE_TABLE (ASTAB)
const std::string PKEY_ASTAB = "p_key";
const std::string STATE_VALUE_ASTAB = "state_value";
const std::string STATE_NAME_ASTAB = "state_name";

//Column Names for NODE_TYPE_TABLE (NTYTAB)
const std::string PKEY_NTYTAB = "p_key";
const std::string TYPE_VALUE_NTYTAB = "type_value";
const std::string TYPE_NAME_NTYTAB = "type_name";

//Column Names for NODE_TREE_TABLE (NTRTAB)
const std::string PKEY_NTRTAB = "p_key";
const std::string NODE_TYPE_NTRTAB = "fk_node_type";
const std::string LEFT_ORDER_NTRTAB = "left_order";
const std::string RIGHT_ORDER_NTRTAB = "right_order";
const std::string PARENT_NODE_NTRTAB = "parent_node";
const std::string NAME_NTRTAB = "name";
const std::string DESCRIPTION_NTRTAB = "description";
const std::string APPROVAL_STATE_NTRTAB = "fk_approval_state";

//Column Names for STEP_TYPE_TABLE (STTAB)
const std::string PKEY_STTAB = "p_key";
const std::string TYPE_VALUE_STTAB = "type_value";
const std::string TYPE_NAME_STTAB = "type_name";

//Column Names for STEP_DETAILS_TABLE (SDTAB)
const std::string PKEY_SDTAB = "p_key";
const std::string STEP_TYPE_SDTAB = "fk_step_type";
const std::string NODE_TREE_SDTAB = "fk_node_tree";
const std::string POSITION_SDTAB = "position";
const std::string DESCRIPTION_SDTAB = "description";
const std::string SKIP_SDTAB = "skip";
const std::string SKIPPABLE_SDTAB = "skippable";
const std::string DELAY_SDTAB = "delay";

//Column Names for PARAMETER_TYPE_TABLE (PTTAB)
const std::string PKEY_PTTAB = "p_key";
const std::string TYPE_VALUE_PTTAB = "type_value";
const std::string TYPE_NAME_PTTAB = "type_name";

//Column Names for STEPTYPE_PARAMTYPE_TABLE (SPTTAB)
const std::string PKEY_SPTTAB = "p_key";
const std::string STEP_TYPE_SPTTAB = "fk_step_type";
const std::string PARAMETER_TYPE_SPTTAB = "fk_parameter_type";
const std::string POSITION_SPTTAB = "position";

//Column Names for STEP_PARAMETERS_TABLE (SPTAB)
const std::string PKEY_SPTAB = "p_key";
const std::string STEP_DETAILS_SPTAB = "fk_step_details";
const std::string POSITION_SPTAB = "position";
const std::string VALUE_SPTAB = "value";


//Column Names for Node_Summary_View (NSView)
const std::string PKEY_NSVIEW = "node_id";



#endif //DATABASE_CONSTANTS_HEADER_INCLUDED
