#if !defined(PRINTMO_H_INCLUDED)
#define PRINTMO_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintMO.h $
  * @author:   Bart Golab
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/18 16:11:24 $
  * Last modified by:  $Author: haijun.li $
  *
  * The ACE Method Object that executes the print method.
  * It always keeps a reference to the same future object 
  * that it returned to the client so that it can set the
  * result value in it.	
  *
  */
#include "ace/Method_Request.h"
namespace TA_Base_App
{
    class PrintMO : public ACE_Method_Request
    {
    // Operations
    public:
        PrintMO(IPrintJobSpooler& printJobSpooler, const PrintableData& printableData, const ACE_Future<bool>& result);
        virtual ~PrintMO();

        /**
         * call 
         *
         * Called by the PrintingService active object once this MO 
         * is dequeued from the activation queue. Submits its printable
         * data to print job spooler and sets the result in the future
         * object.
         */	
        virtual int call();

    private:
        PrintMO(const PrintMO&);
        PrintMO& operator=(const PrintMO&);        


    // Attributes
    private:
        IPrintJobSpooler& m_printJobSpooler;
        PrintableData m_printableData;
        ACE_Future<bool> m_result;
    };
}

#endif // !defined(PRINTMO_H_INCLUDED)
