/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/SingletonAnswerBackErrorUtil.cpp $
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


#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/scada/datapoint_library/src/IAnswerBackErrorNotification.h"
#include "bus/scada/datapoint_library/src/AnswerBackErrorUser.h"
#include "bus/scada/datapoint_library/src/SingletonAnswerBackErrorUtil.h"


namespace TA_Base_Bus
{

	SingletonAnswerBackErrorUtil::SingletonAnswerBackErrorUtil() 
	:
	m_answerBackErrorUserList ( 0 ),
	m_sessionID("")
	{

	};


	SingletonAnswerBackErrorUtil::~SingletonAnswerBackErrorUtil()
	{
		if ( 0 != m_answerBackErrorUserList )
		{
			delete m_answerBackErrorUserList;
			m_answerBackErrorUserList = 0;
		}
	}


	void SingletonAnswerBackErrorUtil::startABETimeoutClock ( IAnswerBackErrorNotification * user,
															  unsigned long abeTimeoutPeriodMSeconds,
															  const std::string & abeExpression,
															  const std::string & sessionID)
	{
		// Save the session Id
		m_sessionID = sessionID;

		// if there is no AnswerBackErrorUser created for the specified user
		if ( 0 == m_answerBackErrorUserList )
		{
			// create an instance of the AnswerBackErrorUser for this user
			m_answerBackErrorUserList = new AnswerBackErrorUser ( user, abeTimeoutPeriodMSeconds, abeExpression );
		}

		// if there is already an AnswerBackErrorUser
		else
		{
			// update the user's timeout clock with the specified timeout period
			m_answerBackErrorUserList->updateTimeoutPeriod ( abeTimeoutPeriodMSeconds );

			// update the user's ABE expression with the specified expression
			m_answerBackErrorUserList->updateABEExpression ( abeExpression );
		}

		// start the ABE clock now
		m_answerBackErrorUserList->startTiming();
	}


	void SingletonAnswerBackErrorUtil::stopABETimeoutClock ( IAnswerBackErrorNotification * user )
	{
		if ( 0 != m_answerBackErrorUserList )
		{
			m_answerBackErrorUserList->stopTiming();
		}
	}


	void SingletonAnswerBackErrorUtil::unregisterForABETimeoutClock ( IAnswerBackErrorNotification * user )
	{
		if ( 0 != m_answerBackErrorUserList )
		{
			delete m_answerBackErrorUserList;
			m_answerBackErrorUserList = 0;
		}
	}

	std::string SingletonAnswerBackErrorUtil::getSessionID()
	{
		return m_sessionID;
	}

}
