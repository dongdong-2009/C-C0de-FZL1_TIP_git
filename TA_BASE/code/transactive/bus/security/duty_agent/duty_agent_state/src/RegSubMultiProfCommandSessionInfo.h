/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4677_ALL_TIP/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/RegSubMultiProfCommandSessionInfo.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 10:00:53 $
 * Last modified by:  $Author: builder $
 *
 * Region or Subsystem commands that operatate on all profiles associated with a session.
 *
 */

#ifndef REGSUBMULTIPROFCOMMANDSESSIONINFO_H_INCLUDED
#define REGSUBMULTIPROFCOMMANDSESSIONINFO_H_INCLUDED

namespace TA_Base_Bus
{
	template< class RegSub >
    class RegSubMultiProfCommandSessionInfo : public IRegSubCommand<RegSub>
    {
    public:
		RegSubMultiProfCommandSessionInfo( void (RegSub::*stateCmd)(DutyState&, const DutyPrimitive&),
			DutyState& state, const SessionInfo& session );

		virtual void execute(RegSub& regSub, bool doApply);

	private:
		void (RegSub::*m_stateCmd)(DutyState&, const DutyPrimitive&);
		DutyState& m_state;
		const SessionInfo& m_session;
    };

	template < class RegSub >
	RegSubMultiProfCommandSessionInfo<RegSub>::RegSubMultiProfCommandSessionInfo( void (RegSub::*stateCmd)(DutyState&, const DutyPrimitive&),
		DutyState& state, const SessionInfo& session ) :
		m_stateCmd(stateCmd),
		m_state(state),
		m_session(session)
	{
	}

	template < class RegSub >
	void RegSubMultiProfCommandSessionInfo<RegSub>::execute(RegSub& regSub, bool doApply)
	{
		std::vector<DASType::DbKey> profiles;
		std::vector<DASType::DbKey>::const_iterator prfIt;

		try
		{
			profiles = m_session.ProfileId;

			for( prfIt = profiles.begin(); prfIt != profiles.end(); prfIt++ )
			{
				DutyPrimitive dp(m_session.SessionId,*prfIt);

				(regSub.*m_stateCmd)( m_state, dp );
			}

// 			if( doApply )
// 			{
// 				regSub.apply(m_state);
// 			}
		}
		catch( const TA_Base_Core::DutyAgentException& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "RegSubMultiProfCommandSessionInfo aborted." );
		}

		
	}
}

#endif // RegSubMultiProfCommandSessionInfo_H_INCLUDED
