/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/TransferRegionsMO.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  The ACE Method Object that executes the transfer (regions) method.
  */

#ifndef TRANSFERREGIONSMO_H_INCLUDED
#define TRANSFERREGIONSMO_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyPeerTask;
}

namespace TA_Base_Bus
{
	//Class declaration
	class TransferRegionsMO : public DutyPeerTaskMO
	{
	// Public methods
	public:
		TransferRegionsMO( DutyPeerTask& task, const ACE_Future<bool>& future,
			DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions );

	protected:

		virtual void execute();

	private:

		TransferRegionsMO& operator=( const TransferRegionsMO& );
		TransferRegionsMO( const TransferRegionsMO& );

		DutyPeerTask& m_task;
		DutyState& m_state;
		const TA_Base_Bus::SessionInfo& m_session;
		const DASType::RegionSet& m_regions;
	}; 
} // TA_Base_Bus

#endif //TRANSFERREGIONSMO_H_INCLUDED
