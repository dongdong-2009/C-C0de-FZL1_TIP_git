#ifndef POLICYDECISIONPOINT_H_INCLUDED
#define POLICYDECISIONPOINT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/PolicyDecisionPoint.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

#if defined( WIN32 ) || defined( __WIN32__ )
#pragma warning( disable : 4786 4018 4146 4503 )
#include <crtdbg.h>
#endif

#include <assert.h>

#include <string>
#include <set>
#include <map>
#include <list>
#include <iostream>
#include <sstream>

#include "core/utilities/src/Logger.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{
	namespace PdpConst
	{
		// all possible results for comparing name/value paris
		enum EMatchResult
		{
			MATCH_MR        , // all names & values match
			NO_MATCH_MR     , // all names match, at least one value does not
			INDETERMINATE_MR, // at least one name does not match
			UNDEFINED_MR
		};

		enum EConditionResult
		{
			TRUE_CR         ,
			FALSE_CR        ,
			INDETERMINATE_CR,
			UNDEFINED_CR
		};

		enum EPolicyResult
		{
			PERMIT_PR        ,
			DENY_PR          ,
			NOT_APPLICABLE_PR,
			INDETERMINATE_PR ,
			UNDEFINED_PR
		};

		enum EComparisonType
		{
			IS_EQUAL_CT            ,
			IS_LESSTHAN_OR_EQUAL_CT,
			IS_NOT_EQUAL_CT        ,
			UNDEFINED_CT
		};

		const std::string ATTRIBUTE_ID      ("ATTRIBUTE_ID"      );
		const std::string ATTRIBUTE_CATEGORY("ATTRIBUTE_CATEGORY");
	}
}

namespace TA_Base_Bus
{
	class Context;
}

// Logging defines
#define SOURCE_INFO      SourceInfo

#define GENERIC_LOG_TYPE          TA_Base_Core::DebugUtil::GenericLog
#define EXCEPTION_CATCH_LOG_TYPE  TA_Base_Core::DebugUtil::ExceptionCatch
#define EXCEPTION_DETAIL_LOG_TYPE TA_Base_Core::DebugUtil::ExceptionDetails
#define FUNCTION_ENTRY_LOG_TYPE   TA_Base_Core::DebugUtil::FunctionEntry
#define FUNCTION_EXIT_LOG_TYPE   TA_Base_Core::DebugUtil::FunctionExit

#define FATAL_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugFatal
#define ERROR_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugError
#define WARN_LOG_LEVEL   TA_Base_Core::DebugUtil::DebugWarn
#define INFO_LOG_LEVEL   TA_Base_Core::DebugUtil::DebugInfo
#define DEBUG_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugDebug

#include "core/exceptions/src/PolicyDecisionPointException.h"

#include "bus/security/rights_agent/policy_decision_point/src/ValueSet.h"

#include "bus/security/rights_agent/policy_decision_point/src/Function.h"
#include "bus/security/rights_agent/policy_decision_point/src/CompareFunction.h"
#include "bus/security/rights_agent/policy_decision_point/src/SubjectAttributeSelector.h"
#include "bus/security/rights_agent/policy_decision_point/src/ActionAttributeSelector.h"
#include "bus/security/rights_agent/policy_decision_point/src/ResourceAttributeSelector.h"
#include "bus/security/rights_agent/policy_decision_point/src/ConstAttributeSelector.h"

#include "bus/security/rights_agent/policy_decision_point/src/AttributeSet.h"
#include "bus/security/rights_agent/policy_decision_point/src/Context.h"
#include "bus/security/rights_agent/policy_decision_point/src/UnmatchedTarget.h"
#include "bus/security/rights_agent/policy_decision_point/src/PolicyCombiningAlg.h"
#include "bus/security/rights_agent/policy_decision_point/src/RuleCombiningAlg.h"

#include "bus/security/rights_agent/policy_decision_point/src/Target.h"
#include "bus/security/rights_agent/policy_decision_point/src/Condition.h"
#include "bus/security/rights_agent/policy_decision_point/src/Rule.h"
#include "bus/security/rights_agent/policy_decision_point/src/Policy.h"
#include "bus/security/rights_agent/policy_decision_point/src/PolicySet.h"

#endif // POLICYDECISIONPOINT_H_INCLUDED
