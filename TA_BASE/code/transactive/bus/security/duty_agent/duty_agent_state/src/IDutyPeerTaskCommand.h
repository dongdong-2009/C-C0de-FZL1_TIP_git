/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/IDutyPeerTaskCommand.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Base class for the command pattern for the DutyPeerTask class.
 *
 */

#ifndef IDUTYPEERTASKCOMMAND_H_INCLUDED
#define IDUTYPEERTASKCOMMAND_H_INCLUDED

namespace TA_Base_Bus
{
    class IDutyPeerTaskCommand
    {
    public:

		virtual std::auto_ptr<IDutyPeerTaskCommand> clone() const = 0;

		virtual ~IDutyPeerTaskCommand(){}

 		ACE_Future<bool>  dispatchExecute(DutyPeerTask& dpt, DutyState& state,bool directCall)
 		{
 			if(directCall)
 			{
				ACE_Future<bool> future; 

				try
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"IDutyPeerTaskCommand::dispatchExecute before directExecute");
					this->directExecute(dpt,state);  //call don't go through queue	
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"IDutyPeerTaskCommand::dispatchExecute after directExecute");
					future.set(true);
				}
				catch (...)
				{
				    future.set(false);
				}

 				return future;
 			}
 			else
 			{
 				return this->execute(dpt,state);
 			}
 		}
		virtual ACE_Future<bool> execute( DutyPeerTask& dpt, DutyState& state) = 0;
		virtual void directExecute(DutyPeerTask& dpt, DutyState& state) = 0;
    };
}

#endif // IDUTYPEERTASKCOMMAND_H_INCLUDED
