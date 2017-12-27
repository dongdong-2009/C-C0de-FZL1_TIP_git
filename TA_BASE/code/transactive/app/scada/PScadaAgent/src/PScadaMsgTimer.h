/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/3002_Review_Branch/3002/transactive/app/scada/PScadaAgent/src/PScadaMsgTimer.h $
  * @author:  Liu Yong
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/09/24 11:21:41 $
  * Last modified by:  $Author: grace.koh $
  *
  * This class is a timer for pscada 104 protocol message created via the PScadaUnit. Once the message was not received
  * within the defined time, the relevant datapoint will raise a fieldchange.
  */
#ifndef PSCADAMSGTIMER_H
#define PSCADAMSGTIMER_H

#include <map>

#include "core/threads/src/Thread.h"

//#include "bus/scada/common_library/src/CommonDefs.h"
#define ACE_LACKS_NET_IF_H
#include "bus/scada/datapoint_library/src/DataPoint.h"
#undef ACE_LACKS_NET_IF_H
namespace TA_Base_Core
{
    class ReEntrantThreadLockable; // Forward declaration
}


namespace TA_IRS_App
{
	struct PScadaTimerInfo
	{
		TA_Base_Bus::EDataPointControlState			controlOperation;
		TA_Base_Bus::DataPoint*						dataPoint;
		time_t										timeExpired;
	};

	class PScadaMsgTimer : public TA_Base_Core::Thread
    {

    public:

        /**
          * Constructor
          *
          * Creates a PScadaMsgTimer object
          */
        PScadaMsgTimer();

        /**
          * Destructor
          */
        ~PScadaMsgTimer();

        /** 
          * addMessage
          *
          * This method adds a message that will be expired if the response message is not received
		  * within the required period
          *
          * @param The pkey of the datapoint and message type
          */
        void addMessage( TA_Base_Bus::DataPoint* dataPoint, TA_Base_Bus::EDataPointControlState controlOperation);

        /** 
          * removeMessage
          *
		  * This method remove a message that will be expired if the response message is not received
		  * within the required period
          *
          * @param The pkey of the new rule
          */
        void removeMessage( TA_Base_Bus::DataPoint* dataPoint, TA_Base_Bus::EDataPointControlState controlOperation);

         /**
         * run
         *
         * This method loops through the map of rules and checks whether the
         * time has expired for the rule. If so, the rule is removed.
         */
        virtual void run();

        /**
         * terminate
         *
         * Terminates the loop that checks the expiry times of the rules.
         */
        virtual void terminate();

    private:

		long m_messageTimeout;                            // Lifetime of rules in seconds
		long m_operateTimeout;                            // Lifetime of rules in seconds

        std::map<unsigned long, PScadaTimerInfo*> m_timedMessages;   // Rule Id and expiry time

        static unsigned int PSCADA_PROTOCOL_MESSAGE_TIMEOUT;
		static unsigned int PSCADA_PROTOCOL_OPERATE_TIMEOUT;

        bool m_terminated;

        static TA_Base_Core::ReEntrantThreadLockable m_lock;
    };
}

#endif // !defined(PSCADAMSGTIMER_H)
