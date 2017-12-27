#ifndef GETTRANSFERABLEREGIONSDAC_H_INCLUDED
#define GETTRANSFERABLEREGIONSDAC_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/GetTransferableRegionsDAC.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * The "getTransferableRegions" command.
 */

#include "app/security/duty_agent/src/IDutyAgentCommand.h"

namespace TA_Base_App
{
    class GetTransferableRegionsDAC : public IDutyAgentCommand
    {
    public:
	
		GetTransferableRegionsDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
			const TA_Base_Bus::SessionInfo& session, const TA_Base_Bus::DASType::RegionSet& locSet, bool checkSubs );

		virtual void execute();

	private:

		TA_Base_Bus::DutyState&                  m_state;
		TA_Base_Bus::DutyPeerTaskManager&        m_taskManager;
		const TA_Base_Bus::SessionInfo&                  m_session;
		const TA_Base_Bus::DASType::LocationSet& m_locSet;
		bool                                m_checkSubs;
    };
}

#endif // GETTRANSFERABLEREGIONSDAC_H_INCLUDED
