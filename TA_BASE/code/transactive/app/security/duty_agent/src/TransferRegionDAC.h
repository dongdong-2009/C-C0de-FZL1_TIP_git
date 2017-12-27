#ifndef TRANSFERREGIONDAC_H_INCLUDED
#define TRANSFERREGIONDAC_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/TransferRegionDAC.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * The "take(locations)" command.
 */

#include "app/security/duty_agent/src/IDutyAgentCommand.h"

namespace TA_Base_App
{
    class TransferRegionDAC : public IDutyAgentCommand
    {
    public:
	
		TransferRegionDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
			const TA_Base_Bus::SessionInfo& session, const TA_Base_Bus::DASType::RegionSet& regions );		

		virtual void execute();

	protected:
		virtual unsigned long getActionKey() const{ return(TA_Base_Bus::aca_DUTY_TRANSFER_REGION); }

	private:

		TA_Base_Bus::DutyState& m_state;
		TA_Base_Bus::DutyPeerTaskManager& m_taskManager;
		const TA_Base_Bus::SessionInfo& m_session;
		const TA_Base_Bus::DASType::RegionSet& m_regions;
    };
}

#endif // TRANSFERREGIONDAC_H_INCLUDED
