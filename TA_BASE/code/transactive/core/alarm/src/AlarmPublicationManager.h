#if !defined(AlarmPublicationManager_6266A092_9EBF_44e2_A5D1_F0EB4A6F83D2__INCLUDED_)
#define AlarmPublicationManager_6266A092_9EBF_44e2_A5D1_F0EB4A6F83D2__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmPublicationManager.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class is used to publish Message Type information relevant to Alarms
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/PublicationManager.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

    // TODO LPP: This class is a little pointless.  See MessagePublicationManager for more information.

	class MessageContext;

	class AlarmSender;

	class AlarmPublicationManager
	{
	public:
		/**
		* destructor
		*/
		virtual ~AlarmPublicationManager();

		/**
		* getInstance
		*
		* Public accessor to the singleton
		*
		* @return   AlarmPublicationManager    One and only publication manager
		*/
		static AlarmPublicationManager& getInstance();

		/**
		* getAlarmSender
		*
		* Retrieve a new AlarmSender object using the MessageContext
		* The caller is responsible for deleting the message sender instance.
		*
		* @param	messageContext	the context of the alarm to be sending
		* @return	AlarmSender	the alarm sender to use
		*/
		/*gives*/ AlarmSender* getAlarmSender(const TA_Base_Core::MessageContext& messageContext);

	private:
		/**
		* Private constructor as the AlarmPublicationManager is a singleton
		*/
		AlarmPublicationManager();

		/**
		* disable copy constructor
		*/
		AlarmPublicationManager( const AlarmPublicationManager& theAlarmPublicationManager);

		/**
		* the one and only classInstance
		*/
		static AlarmPublicationManager* m_theClassInstance;
		
		/**
		* protect singleton creation
		*/
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	};


}; // namespace TA_Base_Core

#endif // !defined(AlarmPublicationManager_6266A092_9EBF_44e2_A5D1_F0EB4A6F83D2__INCLUDED_)
