/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/AnswerBackErrorUser.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2015/01/21 14:56:06 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  */

#ifndef ANSWER_BACK_ERROR_USER_H
#define ANSWER_BACK_ERROR_USER_H

#include <string>


#include "bus/scada/datapoint_library/src/IExpressionNodeNotification.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"

namespace TA_Base_Bus
{
	class ExpressionEvaluator;
	class IAnswerBackErrorNotification;
	class CommonContext;
};

namespace TA_Base_Bus
{
	class AnswerBackErrorUser : public TA_Base_Core::AbstractThreadedTimeoutUser,
								public IExpressionNodeNotification
	{
	public:

		/**
		*
		* AnswerBackErrorUser
		*
		* Parameterised contructor
		*
		* @param    user						The registered user for ABE notification
		* @param    abeTimeoutPeriodMSeconds	The required timeout period in mseconds
		* @param    abeExpression				The ABE expression to be evaluated when timeout
		*										expired
		*/
		AnswerBackErrorUser ( IAnswerBackErrorNotification * user,
							  unsigned long abeTimeoutPeriodMSeconds,
							  const std::string & abeExpression );


		/**
		*
		* ~TimeoutUser
		*
		* Destructor
		*
		*/
		virtual ~AnswerBackErrorUser();


		//
		// operations required by the AbstractThreadedTimeoutUser class
		//


		/**
		*
		* threadedTimerExpired
		*
		* To be implemented by users of TimerUtil.  This operation will be called
		* when the registered timeout period has expired.
		*
		*/
		virtual void threadedTimerExpired(long timerId, void* userData);


		//
		// operations specific to IExpressionNodeNotification
		//


		/**
		 * processExpressionChanges
		 *
		 * Call back when the expression node has changed.
		 *
		 */
		virtual void processExpressionChanges();


		// 
		// operations specific to AnswerBackErrorUser
		//


		/**
		*
		* updateTimeoutPeriod
		*
		* update the current timeout period with specified new period
		*
		* @param    newTimeoutPeriod	The new timeout period
		*
		*/
		void updateTimeoutPeriod ( unsigned long newTimeoutPeriod );


		/**
		*
		* updateABEExpression
		*
		* update the current ABE expression with specified expression
		*
		* @param    newExpression	The new ABE expression to be evaluated
		*
		*/
		void updateABEExpression ( const std::string & newExpression );


		/**
		 * startTiming()
		 *
		 * Start timeout clock
		 *
		 */
		void startTiming();


		/**
		 * stopTiming()
		 *
		 * Stops timeout clock
		 *
		 */
		void stopTiming();


		/**
		*
		* getIsCallbackUserMatched
		*
		* Returns a boolean true to indicate that the specified callback user is the
		* same as the ABE user associated with this instance, false otherwise
		*
		* @param    user	The callback user to compare
		*
		*/
		bool getIsCallbackUserMatched ( IAnswerBackErrorNotification * user );

	private:

		void setTimeoutStartedFlag ( bool started );

		bool m_clockHasStarted;
		unsigned long m_abeTimeoutPeriodMSeconds;
		std::string m_abeExpression;
		IAnswerBackErrorNotification * m_abeUser;
		TA_Base_Bus::ExpressionEvaluator * m_BusExpressionEvaluator;
		CommonContext *		m_context;

		AnswerBackErrorUser(){};
		AnswerBackErrorUser ( const AnswerBackErrorUser & obj );
		AnswerBackErrorUser & operator= ( const AnswerBackErrorUser & rhs );
	};

} // close namespace TA_Base_Bus


#endif
