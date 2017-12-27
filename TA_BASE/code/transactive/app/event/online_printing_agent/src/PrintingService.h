#if !defined(PRINTINGSERVICE_H_INCLUDED)
#define PRINTINGSERVICE_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintingService.h $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Queues printable data before spooling a print job.
  * The thread pool servicing the queue is currently limited to a single thread, 
  * however, this could be changed to implement a different service strategy.
  *
  */

namespace TA_Base_App
{
    class PrintingService : public ACE_Task<ACE_MT_SYNCH>
    {
    // Operations
    public:
        PrintingService(IPrintJobSpooler& printJobSpooler);
        virtual ~PrintingService();

        /**
         * run
         *
         * Activates the PrintingService task.
         */
        virtual void start();

        /**
         * terminate
         *
         * Shuts down the PrintingService task.
         */
        virtual void terminate();

        /**
         * print
         *
         * Creates and queues a PrintMO, which eventually submits the data
         * to the print job spooler and returns the result in a future object.
         */
        virtual ACE_Future<bool> print(const PrintableData& printableData);

    protected:
        /**
         * open/close
         *
         * Task activation/cleanup methods.
         */
        virtual int open(void* args);
        virtual int close(unsigned long flags = 0);

        /**
         * svc
         *
         * Entry point for all threads created by the task.
         */
        virtual int svc(void);

    private:
        PrintingService(const PrintingService&);
        PrintingService& operator=(const PrintingService&);        


    // Attributes
    private:
        IPrintJobSpooler& m_printJobSpooler;
        ACE_Activation_Queue m_activationQueue;
    };
}

#endif // !defined(PRINTINGSERVICE_H_INCLUDED)
