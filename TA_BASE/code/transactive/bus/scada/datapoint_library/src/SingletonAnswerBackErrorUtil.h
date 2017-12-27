/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/SingletonAnswerBackErrorUtil.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */

#ifndef SINGLETON_ANSWER_BACK_ERROR_UTIL_H
#define SINGLETON_ANSWER_BACK_ERROR_UTIL_H

#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
    // forward definition
	class IAnswerBackErrorNotification;
	class AnswerBackErrorUser;

	class SingletonAnswerBackErrorUtil
	{

	public:

		SingletonAnswerBackErrorUtil();
		virtual ~SingletonAnswerBackErrorUtil();

		/**
		*
		* getInstance
		*
		* Get an instance of this SingletonTimerUtil
		*/
		//static SingletonAnswerBackErrorUtil & getInstance();


		/**
		*
		* startABETimeoutClock
		*
		* Start the ABE timeout clock, allocated for the registered user, for notification
		* of the ABE expresion result when the specified timeout period is expired.
		*
		* @param    user						The registered user for ABE notification
		* @param    abeTimeoutPeriodMSeconds	The required timeout period in mseconds
		* @param    abeExpression				The ABE expression to be evaluated when timeout
		*										expired
		*
		*/
		void startABETimeoutClock ( IAnswerBackErrorNotification * user,
									unsigned long abeTimeoutPeriodMSeconds,
									const std::string & abeExpression,
									const std::string & sessionID);


		/**
		*
		* stopABETimeoutClock
		*
		* Stop the timeout clock, allocated for the register user.
		*
		* @param    user			The registered user for ABE notification
		*
		*/
		void stopABETimeoutClock ( IAnswerBackErrorNotification * user );


		/**
		*
		* unregisterForABETimeoutClock
		*
		* Stop the timeout clock and remove the specified user
		*
		* @param    user			The registered user for ABE notification
		*
		*/
		void unregisterForABETimeoutClock ( IAnswerBackErrorNotification * user );

		/**
		*
		* getSessionID
		*
		* Get the session ID
		*
		* @ param
		*
		*/
		std::string getSessionID ();

	private:

		//AnswerBackErrorUser * findExistingUser ( IAnswerBackErrorNotification * user );

		AnswerBackErrorUser * m_answerBackErrorUserList;
		std::string			  m_sessionID;

		SingletonAnswerBackErrorUtil ( const SingletonAnswerBackErrorUtil & theInstance );
		SingletonAnswerBackErrorUtil & operator= ( const SingletonAnswerBackErrorUtil & rhs );
	};

}

#endif
