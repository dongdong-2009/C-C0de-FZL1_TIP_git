#ifndef AUTHEN_CONST_H__
#define AUTHEN_CONST_H__

#include "ace/Task.h"
#include "ace/Future.h"
#include "ace/Activation_Queue.h"
#include "ace/Method_Request.h"
#include "ace/OS.h"
#include "ace/Singleton.h"
#include "ace/Thread_Manager.h"
#include <vector>

#include "bus/security/authentication_library/src/SessionInfo.h"

// Constants
namespace TA_Base_Bus
{
	namespace AuthenLibConst
	{
		const int ACE_ERROR = -1; 
	}
}
#endif