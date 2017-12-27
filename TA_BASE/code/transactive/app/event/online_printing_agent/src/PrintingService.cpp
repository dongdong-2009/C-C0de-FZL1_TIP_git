/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintingService.cpp $
  * @author:   Bart Golab
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/18 16:11:24 $
  * Last modified by:  $Author: haijun.li $
  *
  * Queues printable data before spooling a print job.
  * The thread pool servicing the queue is currently limited to a single thread, 
  * however, this could be changed to implement a different service strategy.
  *
  */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

namespace
{
    const long THREAD_PROPERTIES(THR_SCOPE_PROCESS | THR_JOINABLE);
    const int THREAD_COUNT(1);
	const ACE_Time_Value MSG_Q_TIMEOUT(0, 1000);   // 1 ms
	const ACE_Time_Value ACT_Q_TIMEOUT(0, 100000); // 100 ms
    const int GENERAL_ACE_ERROR(-1);
}

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;


PrintingService::PrintingService(IPrintJobSpooler& printJobSpooler) :
    m_printJobSpooler(printJobSpooler)
{
    FUNCTION_ENTRY("PrintingService(IPrintJobSpooler& printJobSpooler)");
    FUNCTION_EXIT;
}


PrintingService::~PrintingService()
{
    FUNCTION_ENTRY("~PrintingService()");
    FUNCTION_EXIT;
}


void PrintingService::start()
{
    FUNCTION_ENTRY("start()");

    if (open(0) == GENERAL_ACE_ERROR)
    {
        TA_THROW(OnlinePrintingAgentException("Failed to activate the printing service task."));
    }

    FUNCTION_EXIT;
}


void PrintingService::terminate()
{
    FUNCTION_ENTRY("terminate()");

    ACE_Message_Block* msg = new ACE_Message_Block;
    msg->msg_type(ACE_Message_Block::MB_HANGUP);

    // Send the threads a terminate message and wait for all to exit.
    if (putq(msg) != GENERAL_ACE_ERROR)
    {
        wait();
    }

    FUNCTION_EXIT;
}


ACE_Future<bool> PrintingService::print(const PrintableData& printableData)
{
//    FUNCTION_ENTRY("print(const PrintableData& printableData)");

    ACE_Future<bool> result;

    m_activationQueue.enqueue(new PrintMO(m_printJobSpooler, printableData, result));

//    FUNCTION_EXIT;
    return result;
}


int PrintingService::open(void* /*args*/)
{
    FUNCTION_ENTRY("open(void* args)");
    FUNCTION_EXIT;

    return activate(THREAD_PROPERTIES, THREAD_COUNT);
}


int PrintingService::close(unsigned long /*flags*/)
{
    FUNCTION_ENTRY("close(unsigned long flags)");
    FUNCTION_EXIT;

    return 0;
}


int PrintingService::svc(void)
{
    FUNCTION_ENTRY("svc()");

	int result(0);

	try
	{
		while (true)
		{
			// Check the activation queue.
            ACE_Time_Value aqTimeout(ACE_OS::gettimeofday() + ACT_Q_TIMEOUT);
			std::auto_ptr<ACE_Method_Request> mo(m_activationQueue.dequeue(&aqTimeout));

			// Ensure we have a method object to execute.
			if (mo.get())
			{
				if (mo->call() != GENERAL_ACE_ERROR)
				{
                    // Service all activation queue requests before checking the message queue.
                    continue;
				}

                TA_THROW(OnlinePrintingAgentException("Failed to execute a method call."));
            }
            else
            { 
				// Check the activation queue is OK.
				if (ACE_OS::last_error() != EWOULDBLOCK)
				{
					std::ostringstream msg;
					msg << "Failed to dequeue from activation queue (errno " << ACE_OS::last_error() << ").";

					TA_THROW(OnlinePrintingAgentException(msg.str()));
				}
            }

			// Check the message queue for terminate request.
            ACE_Time_Value mqTimeout(ACE_OS::gettimeofday() + MSG_Q_TIMEOUT);
			ACE_Message_Block* aceMsg = NULL;

			if (getq(aceMsg, &mqTimeout) != GENERAL_ACE_ERROR)
			{
				if (aceMsg->msg_type() == ACE_Message_Block::MB_HANGUP)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received MB_HANGUP message.");

                    // Let other threads see the message and bail out.
					putq( aceMsg );
					break;
				}
			}
			else
			{
                // Check the message queue is OK.
				if (ACE_OS::last_error() != EWOULDBLOCK)
				{
					std::ostringstream msg;
					msg << "Failed to dequeue from message queue (errno " << ACE_OS::last_error() << ").";

					TA_THROW(OnlinePrintingAgentException(msg.str()));
				}
			}
		}
	}
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, std::string("Unhandled ") + typeid(ex).name(), ex.what());
        result = GENERAL_ACE_ERROR;
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unhandled CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex));
        result = GENERAL_ACE_ERROR;
    }
    catch (...)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unhandled exception.", "");
        result = GENERAL_ACE_ERROR;
    }

    FUNCTION_EXIT;
	return result;
}
