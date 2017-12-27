/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/EvaluateDutyPolicy.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/29 17:51:30 $
 * Last modified by:  $Author: liwei.gong $
 *
 * Applies the Duty Policy for each of the DutyPrimitives in the range itBegin to itEnd.
 * 
 */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	IDutyPolicy::POLICY_RESULT EvaluateDutyPolicy::evaluate( const IDutyPolicy& dutyPolicy,
			IDutyPolicy::POLICY_RESULT (IDutyPolicy::*policy)(const DutyContext&) const, DutyContext& context,
			const DASType::DutySet& srcDuty, DutyPrimitive& replace, DASType::DutySet& noAdd )
	{
		DutyPolicyCombAlg pca;
		DASType::DutySet::const_iterator it = srcDuty.begin();

		// for the case where there are no source sessions
		if( srcDuty.size() == 0 )
		{
			DutyContextBuilderFactory::instance()->getDutyContextBuilder().setNullSrcSession(context);
			pca.setResult( (dutyPolicy.*policy)(context) );

			// can not remove a null session
			TA_ASSERT( pca.getCombinedResult() != IDutyPolicy::PR_REPLACE, "Result must not be PR_REPLACE" );
			TA_ASSERT( pca.getCombinedResult() != IDutyPolicy::PR_REPONLY, "Result must not be PR_REPONLY" );
			TA_ASSERT( pca.getCombinedResult() != IDutyPolicy::PR_REMOVE , "Result must not be PR_REMOVE"  );

			return( pca.getCombinedResult() );
		}

		int replaceCount = 0;

		while( !pca.isResultKnown() && it != srcDuty.end() )
		{
			DutyContextBuilderFactory::instance()->getDutyContextBuilder().setSrcSession(context, *it);
			IDutyPolicy::POLICY_RESULT result = (dutyPolicy.*policy)(context);
			pca.setResult( result );
			
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "session:%s, profile: %lu policy result: %l", (*it).m_session.c_str(), (*it).m_profile ,result );
			// remember which session to replace
			if( pca.getLastResult() == IDutyPolicy::PR_REPLACE ||
				pca.getLastResult() == IDutyPolicy::PR_REPONLY ||
				pca.getLastResult() == IDutyPolicy::PR_REMOVE  )
			{
				replace = *it;

				replaceCount++;
			}

			// remember which session(s) not to process for adding if a subsystem
			// state change occurs
			if( pca.getLastResult() == IDutyPolicy::PR_ADDONLY ||
				pca.getLastResult() == IDutyPolicy::PR_REPONLY )
			{
				noAdd.insert(*it);
			}

			it++;
		}

		if( replaceCount > 1 )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Found %d sessions to replace, only the last one to be processed will be replaced.", replaceCount );
		}

		return( pca.getCombinedResult() );
	};
}
