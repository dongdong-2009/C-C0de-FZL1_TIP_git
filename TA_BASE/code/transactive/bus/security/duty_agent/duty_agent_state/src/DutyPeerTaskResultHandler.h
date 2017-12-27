/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPeerTaskResultHandler.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Knows how to collate results from a command performed on multiple DutyPeerTask objects.
  */

#ifndef DUTYPEERTASKRESULTHANDLER_H_INCLUDED
#define DUTYPEERTASKRESULTHANDLER_H_INCLUDED

namespace TA_Base_Bus
{
	//Class declaration
	class DutyPeerTaskResultHandler
	{
	// Public methods
	public:
		DutyPeerTaskResultHandler( DutyState& state );
		~DutyPeerTaskResultHandler();

		void set( const DASType::DbKey& location, DutyPeerTask& dpt,
			std::auto_ptr<IDutyPeerTaskCommand> command,bool directCall);

		void set( const DASType::DbKey& location, DutyPeerTask& dpt,
			IDutyPeerTaskCommand& command,bool directCall );
		
		void waitOnAll( DASType::LocationSet& failedPeers );

	private:
		DutyPeerTaskResultHandler( const DutyPeerTaskResultHandler& );
		DutyPeerTaskResultHandler& operator =( const DutyPeerTaskResultHandler& );
		class Result
		{
		public:

			Result( const DutyState& state );
			

			DutyState m_state;
			ACE_Future<bool> m_future;

		private:
			Result( const Result& copyThis );
			Result& operator=( const Result& );
		};

		DutyState& m_state;

		typedef std::map<DASType::DbKey, Result*> ResultMap;
		ResultMap m_resultMap;

		typedef std::set<IDutyPeerTaskCommand*> CommandSet;
		CommandSet m_commandSet;
	}; 
} // TA_Base_Bus

#endif //DUTYPEERTASKRESULTHANDLER_H_INCLUDED
