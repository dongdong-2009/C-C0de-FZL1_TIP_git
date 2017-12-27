/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPolicyFactory.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/29 17:51:30 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  Implements DutyPolicyFactoryImpl
  */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"
#include "bus/security/duty_agent/duty_agent_state/src/SubsystemState.h"

namespace TA_Base_Bus
{
	/******************* RAIL DUTY POLICY *******************/
	// Allocate Region Duty
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::m_allocRegionPolicy[2][3][3] = {
		/*Prf Type,       Region,     Prf Grp, Result
		----------------------------------------------*/
		/*PT_CENTRAL*/ {/*EQ_EQ */ {/*EQ_EQ */ IDutyPolicy::PR_DENY,
		/*PT_CENTRAL*/  /*EQ_EQ */  /*EQ_NEQ*/ IDutyPolicy::PR_ADD ,
		/*PT_CENTRAL*/  /*EQ_EQ */  /*EQ_NA */ IDutyPolicy::PR_ADD },
		/*PT_CENTRAL*/  /*EQ_NEQ*/ {/*EQ_EQ */ IDutyPolicy::PR_DENY,
		/*PT_CENTRAL*/  /*EQ_NEQ*/  /*EQ_NEQ*/ IDutyPolicy::PR_DENY,
		/*PT_CENTRAL*/  /*EQ_NEQ*/  /*EQ_NA */ IDutyPolicy::PR_DENY},
		/*PT_CENTRAL*/  /*EQ_NA */ {/*EQ_EQ */ IDutyPolicy::PR_DENY,
		/*PT_CENTRAL*/  /*EQ_NA */  /*EQ_NEQ*/ IDutyPolicy::PR_DENY,
		/*PT_CENTRAL*/  /*EQ_NA */  /*EQ_NA */ IDutyPolicy::PR_DENY}},
		/*PT_LOCAL  */ {/*EQ_EQ */ {/*EQ_EQ */ IDutyPolicy::PR_ADD ,
		/*PT_LOCAL  */  /*EQ_EQ */  /*EQ_NEQ*/ IDutyPolicy::PR_ADD ,
		/*PT_LOCAL  */  /*EQ_EQ */  /*EQ_NA */ IDutyPolicy::PR_ADD },
		/*PT_LOCAL  */  /*EQ_NEQ*/ {/*EQ_EQ */ IDutyPolicy::PR_DENY,
		/*PT_LOCAL  */  /*EQ_NEQ*/  /*EQ_NEQ*/ IDutyPolicy::PR_DENY,
		/*PT_LOCAL  */  /*EQ_NEQ*/  /*EQ_NA */ IDutyPolicy::PR_DENY},
		/*PT_LOCAL  */  /*EQ_NA */ {/*EQ_EQ */ IDutyPolicy::PR_DENY,
		/*PT_LOCAL  */  /*EQ_NA */  /*EQ_NEQ*/ IDutyPolicy::PR_DENY,
		/*PT_LOCAL  */  /*EQ_NA */  /*EQ_NA */ IDutyPolicy::PR_DENY}}
	};

	// Allocate Subsystem Duty
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::m_allocSubPolicy[3][2][3] = {
		/*Dst Rights,       Sub Type,         Src Duty,      Result
		------------------------------------------------------------*/
		/*RT_EXCLUSIVE*/ {/*ST_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_EXCLUSIVE*/  /*ST_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_EXCLUSIVE*/  /*ST_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADD },
		/*RT_EXCLUSIVE*/  /*ST_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_EXCLUSIVE*/  /*ST_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_EXCLUSIVE*/  /*ST_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADD }},
		/*RT_ADMISSIVE*/ {/*ST_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_ADMISSIVE*/  /*ST_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_ADMISSIVE*/  /*ST_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADD },
		/*RT_ADMISSIVE*/  /*ST_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_ADMISSIVE*/  /*ST_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_ADMISSIVE*/  /*ST_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADD }},
		/*RT_NONE     */ {/*ST_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_NONE     */  /*ST_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_NONE     */  /*ST_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY},
		/*RT_NONE     */  /*ST_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_NONE     */  /*ST_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_NONE     */  /*ST_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY}}
	};

	// Transfer Region Duty
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::m_xferRegionPolicy[3] = {
		/*Prf Grp, Result
		---------------------*/
		/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*EQ_NA */ IDutyPolicy::PR_ADD
	};

	// Transfer Subsystem Duty via a Region Duty Transfer
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::m_xferSubViaRegionPolicy[3][2][3][3] = {
		/*Dst Rights,        Sub Type,           Src Duty,         Prf Grp, Result
		------------------------------------------------------------------------------*/
		/*RT_EXCLUSIVE*/  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_DENY   ,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NA */ IDutyPolicy::PR_DENY   },
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NA */ IDutyPolicy::PR_ADD    },
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*EQ_NA */ IDutyPolicy::PR_ADD    }},
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NA */ IDutyPolicy::PR_ADD    },
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NA */ IDutyPolicy::PR_ADD    },
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*EQ_NA */ IDutyPolicy::PR_ADD    }}},
		/*RT_ADMISSIVE*/  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NA */ IDutyPolicy::PR_ADD    },
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NA */ IDutyPolicy::PR_ADD    },
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*EQ_NA */ IDutyPolicy::PR_ADD    }},
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NA */ IDutyPolicy::PR_ADD    },
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NA */ IDutyPolicy::PR_ADD    },
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */ {/*EQ_EQ */ IDutyPolicy::PR_REPLACE,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*EQ_NEQ*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*EQ_NA */ IDutyPolicy::PR_ADD    }}},
		/*RT_NONE     */  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REMOVE ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NA */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REMOVE ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NA */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */ {/*EQ_EQ */ IDutyPolicy::PR_REMOVE ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*EQ_NEQ*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*EQ_NA */ IDutyPolicy::PR_DENY   }},
		/*RT_NONE     */   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REMOVE ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*EQ_NA */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/ {/*EQ_EQ */ IDutyPolicy::PR_REMOVE ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NEQ*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*EQ_NA */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */ {/*EQ_EQ */ IDutyPolicy::PR_REMOVE ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*EQ_NEQ*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*EQ_NA */ IDutyPolicy::PR_DENY   }}}
	};

	// Directly transfer Subsystem Duty
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::m_xferSubPolicy[3][2][3][3] = {
		/*Dst Rights    ,    Sub Type,           Src Duty,         Src Rights,    Result
		------------------------------------------------------------------------------------*/
		/*RT_EXCLUSIVE*/  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_REPONLY,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_REPLACE,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_REPONLY},
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADDONLY,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADDONLY,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY}},
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY}}},
		/*RT_ADMISSIVE*/  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY}},
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY}}},
		/*RT_NONE     */  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_DENY   }},
		/*RT_NONE     */   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_DENY   }}}
	};

	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::evaluateAllocRegionPolicy(const DutyContext& context) const
	{
		if( (context.m_dstPrfType >= PT_UNDEFINED) || (context.m_dstPrfType < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_regions >= IDutyPolicy::EQ_UNDEFINED) || (context.m_regions < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_prfGrps >= IDutyPolicy::EQ_UNDEFINED) || (context.m_prfGrps < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "AllocRegion PRF:%lu,REG:%lu,GRP:%lu", context.m_dstPrfType,
			context.m_regions, context.m_prfGrps );
		return( m_allocRegionPolicy[context.m_dstPrfType][context.m_regions][context.m_prfGrps] );
	}

	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::evaluateAllocSubPolicy(const DutyContext& context) const
	{
		if( (context.m_dstRights >= IDutyPolicy::RT_UNDEFINED) || (context.m_dstRights < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_subType >= IDutyPolicy::ST_UNDEFINED) || (context.m_subType < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_srcDuty >= IDutyPolicy::RT_UNDEFINED) || (context.m_srcDuty < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "AllocSub DST:%lu,SUB:%lu,SRC:%lu", context.m_dstRights,
			context.m_subType, context.m_srcDuty );

		return( m_allocSubPolicy[context.m_dstRights][context.m_subType][context.m_srcDuty] );
	}

	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::evaluateXferRegionPolicy(const DutyContext& context) const
	{
		if( (context.m_prfGrps >= IDutyPolicy::EQ_UNDEFINED) || (context.m_prfGrps < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "XferRegion GRP:%lu", context.m_prfGrps );

		return( m_xferRegionPolicy[context.m_prfGrps] );
	}

	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::evaluateXferSubViaRegionPolicy(const DutyContext& context) const
	{
		if( (context.m_dstRights >= IDutyPolicy::RT_UNDEFINED) || (context.m_dstRights < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_subType >= IDutyPolicy::ST_UNDEFINED) || (context.m_subType < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_srcDuty >= IDutyPolicy::RT_UNDEFINED) || (context.m_srcDuty < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_prfGrps >= IDutyPolicy::EQ_UNDEFINED) || (context.m_prfGrps < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "XferSubViaRegion DST:%lu,SUB:%lu,SRC:%lu,PRF:%lu", context.m_dstRights,
			context.m_subType, context.m_srcDuty, context.m_prfGrps );

		return( m_xferSubViaRegionPolicy[context.m_dstRights][context.m_subType][context.m_srcDuty][context.m_prfGrps] );
	}
	
	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RailDutyPolicy::evaluateXferSubPolicy(const DutyContext& context) const
	{
		if( (context.m_dstRights >= IDutyPolicy::RT_UNDEFINED) || (context.m_dstRights < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_subType >= IDutyPolicy::ST_UNDEFINED) || (context.m_subType < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_srcDuty >= IDutyPolicy::RT_UNDEFINED) || (context.m_srcDuty < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_srcRights >= IDutyPolicy::RT_UNDEFINED) || (context.m_srcRights < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "XferSub DST:%lu,SUB:%lu,SRCD:%lu,SRCR:%lu", context.m_dstRights,
			context.m_subType, context.m_srcDuty, context.m_srcRights );

        // TD15075
        // if exclusive subsystem state change from normal to delegated mode and source duty is exlusive duty 
        // with admissive rights means subsystem state change cause central op lost their exclusive duty, hence
        // need to get response from central op for accept or deny the request
	// Cancel at:2008-9-24, this causing Exclusive Subsystem have two Control Duty at same time when Local Profile
	// ask for Control Duty from Centrel Profile and make Subsystem State change to Delegation Mode.
	// Using default PR_REPONLY
        //if ((context.m_subType == IDutyPolicy::ST_EXCLUSIVE) && (context.m_srcDuty == IDutyPolicy::RT_EXCLUSIVE) &&
        //    (context.getPreState() == SubsystemState::SS_NORMAL) && (context.getPostState() == SubsystemState::SS_DELEGATED))
        //{
        //    return IDutyPolicy::PR_REPLACE;
        //}

		return( m_xferSubPolicy[context.m_dstRights][context.m_subType][context.m_srcDuty][context.m_srcRights] );
	}

	/******************* ROAD DUTY POLICY *******************/
	// Allocate Region Duty
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::m_allocRegionPolicy[2][3] = {
		/*Prf Type,       Region,   Result
		----------------------------------------------*/
		/*PT_CENTRAL*/ {/*EQ_EQ */  IDutyPolicy::PR_ADD ,
		/*PT_CENTRAL*/  /*EQ_NEQ*/  IDutyPolicy::PR_ADD ,
		/*PT_CENTRAL*/  /*EQ_NA */  IDutyPolicy::PR_ADD },
		/*PT_LOCAL  */ {/*EQ_EQ */  IDutyPolicy::PR_ADD ,
		/*PT_LOCAL  */  /*EQ_NEQ*/  IDutyPolicy::PR_DENY,
		/*PT_LOCAL  */  /*EQ_NA */  IDutyPolicy::PR_DENY}
	};

	// Allocate Subsystem Duty
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::m_allocSubPolicy[3][2][3] = {
		/*Dst Rights,       Sub Type,         Src Duty,      Result
		------------------------------------------------------------*/
		/*RT_EXCLUSIVE*/ {/*ST_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_EXCLUSIVE*/  /*ST_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_EXCLUSIVE*/  /*ST_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADD },
		/*RT_EXCLUSIVE*/  /*ST_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_EXCLUSIVE*/  /*ST_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_EXCLUSIVE*/  /*ST_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADD }},
		/*RT_ADMISSIVE*/ {/*ST_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_ADMISSIVE*/  /*ST_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_ADMISSIVE*/  /*ST_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADD },
		/*RT_ADMISSIVE*/  /*ST_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_ADMISSIVE*/  /*ST_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD ,
		/*RT_ADMISSIVE*/  /*ST_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADD }},
		/*RT_NONE     */ {/*ST_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_NONE     */  /*ST_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_NONE     */  /*ST_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY},
		/*RT_NONE     */  /*ST_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_NONE     */  /*ST_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY,
		/*RT_NONE     */  /*ST_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY}}
	};

	// Transfer Region Duty
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::m_xferRegionPolicy[1] = {
		/*Result
		---------------------*/
		IDutyPolicy::PR_ADD
	};

	// Transfer Subsystem Duty via a Region Duty Transfer
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::m_xferSubViaRegionPolicy[3][2][3] = {
		/*Dst Rights,        Sub Type,           Src Duty,       Result
		------------------------------------------------------------------------------*/
		/*RT_EXCLUSIVE*/  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/  IDutyPolicy::PR_DENY   ,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  IDutyPolicy::PR_ADD    },
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/  IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  IDutyPolicy::PR_ADD    }},
		/*RT_ADMISSIVE*/  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/  IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  IDutyPolicy::PR_ADD    },
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/  IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  IDutyPolicy::PR_ADD    }},
		/*RT_NONE     */  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/  IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */  IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/  IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */  IDutyPolicy::PR_DENY   }}
	};

	// Directly transfer Subsystem Duty
	const IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::m_xferSubPolicy[3][2][3][3] = {
		/*Dst Rights    ,    Sub Type,           Src Duty,         Src Rights,    Result
		------------------------------------------------------------------------------------*/
		/*RT_EXCLUSIVE*/  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_REPONLY,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_REPLACE,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_REPONLY},
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADDONLY,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADDONLY,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY}},
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_EXCLUSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY}}},
		/*RT_ADMISSIVE*/  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY}},
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY},
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_ADD    ,
		/*RT_ADMISSIVE*/   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_ADDONLY}}},
		/*RT_NONE     */  {/*ST_EXCLUSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_EXCLUSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_DENY   }},
		/*RT_NONE     */   /*ST_ADMISSIVE*/   {/*RT_EXCLUSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_EXCLUSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_ADMISSIVE*/  /*RT_NONE     */ IDutyPolicy::PR_DENY   },
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */ {/*RT_EXCLUSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_ADMISSIVE*/ IDutyPolicy::PR_DENY   ,
		/*RT_NONE     */   /*ST_ADMISSIVE*/    /*RT_NONE     */  /*RT_NONE     */ IDutyPolicy::PR_DENY   }}}
	};

	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::evaluateAllocRegionPolicy(const DutyContext& context) const
	{
		if( (context.m_dstPrfType >= PT_UNDEFINED) || (context.m_dstPrfType < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_regions >= IDutyPolicy::EQ_UNDEFINED) || (context.m_regions < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "AllocRegion PRF:%lu,REG:%lu", context.m_dstPrfType,
			context.m_regions );
		return( m_allocRegionPolicy[context.m_dstPrfType][context.m_regions] );
	}

	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::evaluateAllocSubPolicy(const DutyContext& context) const
	{
		if( (context.m_dstRights >= IDutyPolicy::RT_UNDEFINED) || (context.m_dstRights < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_subType >= IDutyPolicy::ST_UNDEFINED) || (context.m_subType < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_srcDuty >= IDutyPolicy::RT_UNDEFINED) || (context.m_srcDuty < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "AllocSub DST:%lu,SUB:%lu,SRC:%lu", context.m_dstRights,
			context.m_subType, context.m_srcDuty );

		return( m_allocSubPolicy[context.m_dstRights][context.m_subType][context.m_srcDuty] );
	}

	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::evaluateXferRegionPolicy(const DutyContext& context) const
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "XferRegion" );

		return( m_xferRegionPolicy[0] );
	}

	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::evaluateXferSubViaRegionPolicy(const DutyContext& context) const
	{
		if( (context.m_dstRights >= IDutyPolicy::RT_UNDEFINED) || (context.m_dstRights < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_subType >= IDutyPolicy::ST_UNDEFINED) || (context.m_subType < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_srcDuty >= IDutyPolicy::RT_UNDEFINED) || (context.m_srcDuty < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "XferSubViaRegion DST:%lu,SUB:%lu,SRC:%lu", context.m_dstRights,
			context.m_subType, context.m_srcDuty );

		return( m_xferSubViaRegionPolicy[context.m_dstRights][context.m_subType][context.m_srcDuty] );
	}
	
	IDutyPolicy::POLICY_RESULT DutyPolicyFactoryImpl::RoadDutyPolicy::evaluateXferSubPolicy(const DutyContext& context) const
	{
		if( (context.m_dstRights >= IDutyPolicy::RT_UNDEFINED) || (context.m_dstRights < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_subType >= IDutyPolicy::ST_UNDEFINED) || (context.m_subType < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_srcDuty >= IDutyPolicy::RT_UNDEFINED) || (context.m_srcDuty < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		if( (context.m_srcRights >= IDutyPolicy::RT_UNDEFINED) || (context.m_srcRights < 0) )
		{
			return IDutyPolicy::PR_DENY;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "XferSub DST:%lu,SUB:%lu,SRCD:%lu,SRCR:%lu", context.m_dstRights,
			context.m_subType, context.m_srcDuty, context.m_srcRights );

		return( m_xferSubPolicy[context.m_dstRights][context.m_subType][context.m_srcDuty][context.m_srcRights] );
	}

	/******************* FACTORY *******************/
	DutyPolicyFactoryImpl::DutyPolicyFactoryImpl()
	{
		try
		{
			m_dutyPolicy.push_back( new RailDutyPolicy() );
			m_dutyPolicy.push_back( new RoadDutyPolicy() );
		}
		catch(...)
		{
			std::vector<IDutyPolicy*>::iterator it;

			for( it=m_dutyPolicy.begin(); it!=m_dutyPolicy.end(); it++ )
			{
				delete *it;
				*it = NULL;
			}

			throw;
		}
	}

	DutyPolicyFactoryImpl::~DutyPolicyFactoryImpl()
	{
		delete m_dutyPolicy[DASConst::POLICY_TYPE_RAIL];
		m_dutyPolicy[DASConst::POLICY_TYPE_RAIL] = NULL;
		
		delete m_dutyPolicy[DASConst::POLICY_TYPE_ROAD];
		m_dutyPolicy[DASConst::POLICY_TYPE_ROAD] = NULL;
	}

	const IDutyPolicy& DutyPolicyFactoryImpl::getDutyPolicy() const
	{
		return( *(m_dutyPolicy[TransactiveAgentInfo::m_policyType]) );
	}
}



