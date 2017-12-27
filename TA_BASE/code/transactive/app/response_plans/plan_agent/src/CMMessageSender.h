#ifndef CM_MESSAGE_SENDER_HEADER_INCLUDED
#define CM_MESSAGE_SENDER_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Mahaveer Pareek
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */
//
#include "ace/Singleton.h"

namespace TA_Base_Core
{
    class MessageType;
    class CommsMessageSender;
    struct CurrentPlanStepDetail;
}

namespace TA_Base_App
{
	class CMMessageSenderImpl
	{
	// Public methods
	public:
		friend class ACE_Singleton<CMMessageSenderImpl, ACE_Recursive_Thread_Mutex>;

        bool sendActiveStepUpdateMessage(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);

	// Private methods
	private:
		CMMessageSenderImpl();
		~CMMessageSenderImpl();

		CMMessageSenderImpl( const CMMessageSenderImpl& );
        CMMessageSenderImpl& operator=( const CMMessageSenderImpl& );
   
    // Private data
    private:

        std::auto_ptr<TA_Base_Core::CommsMessageSender> m_commsSender;

	}; // CMMessageSenderImpl

	typedef ACE_Singleton<CMMessageSenderImpl, ACE_Recursive_Thread_Mutex> CMMessageSender;

} // TA_Base_App

#endif //CM_MESSAGE_SENDER_HEADER_INCLUDED
