///////////////////////////////////////////////////////////
//  DataLogAgentImpl.h
//  Implementation of the Class DataLogAgentImpl
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:05
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#if !defined(EA_BBDAE718_3421_45bd_BBF1_45EC25CFDE25__INCLUDED_)
#define EA_BBDAE718_3421_45bd_BBF1_45EC25CFDE25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/corba/src/ServantBase.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/datalog/IDL/src/DataLogAgentCorbaDef.h"
#include "core/datalog/IDL/src/DataLogMessageCorbaDef.h"
 

namespace TA_Base_Bus
{
	class DataLogAgentImpl : public virtual POA_TA_Base_Core::DataLogAgentCorbaDef,
		                     public virtual TA_Base_Core::ServantBase
												  
	{

	public:
		DataLogAgentImpl(std::string& agentName);
		 
		virtual ~DataLogAgentImpl();
		virtual TA_Base_Core::DataLogSyncSequence* getAllDataLogSync();

// 		virtual void callTest(const char * inStr)
// 		{
// 			return;
// 		}
	private:

		// Disable copy constructor and assignment operator
		DataLogAgentImpl( const DataLogAgentImpl& theDataLogAgentImpl);
		DataLogAgentImpl& operator=(const DataLogAgentImpl &);


	};
}
#endif // !defined(EA_BBDAE718_3421_45bd_BBF1_45EC25CFDE25__INCLUDED_)
