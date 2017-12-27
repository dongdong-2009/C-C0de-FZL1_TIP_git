#ifndef __GETSUBSYSTEMSTATEDAC_H__
#define __GETSUBSYSTEMSTATEDAC_H__

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_TIP/TA_BASE/transactive/app/security/duty_agent/src/GetSubsystemStateDAC.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/10/31 12:12:40 $
 * Last modified by:  $Author: grace.koh $
 *
 * The "getSubsystemState" command.
 */

#include "app/security/duty_agent/src/IDutyAgentCommand.h"

namespace TA_Base_App
{
    class GetSubsystemStateDAC : public IDutyAgentCommand
    {
    public:
	
		GetSubsystemStateDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
			const TA_Base_Bus::DASType::SubsystemSet& subsystems,const TA_Base_Bus::DASType::LocationSet& locSet );

		virtual void execute();

	private:

		TA_Base_Bus::DutyState&                  m_state;
		TA_Base_Bus::DutyPeerTaskManager&        m_taskManager;
		const TA_Base_Bus::DASType::LocationSet& m_locSet;
		const TA_Base_Bus::DASType::SubsystemSet& m_subsystems;
    };
}

#endif //__GETSUBSYSTEMSTATEDAC_H__