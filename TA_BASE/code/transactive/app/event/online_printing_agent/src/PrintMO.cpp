/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintMO.cpp $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * The ACE Method Object that executes the print method.
  * It always keeps a reference to the same future object 
  * that it returned to the client so that it can set the
  * result value in it.	
  *
  */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

using namespace TA_Base_App;


PrintMO::PrintMO(IPrintJobSpooler& printJobSpooler, const PrintableData& printableData, const ACE_Future<bool>& result) :
    m_printJobSpooler(printJobSpooler), m_printableData(printableData), m_result(result)
{
//    FUNCTION_ENTRY("PrintMO(IPrintJobSpooler& printJobSpooler, const PrintableData& printableData, const ACE_Future<bool>& result)");
//    FUNCTION_EXIT;
}


PrintMO::~PrintMO()
{
//    FUNCTION_ENTRY("~PrintMO()");
//    FUNCTION_EXIT;
}


int PrintMO::call()
{
//    FUNCTION_ENTRY("call()");
//    FUNCTION_EXIT

    return m_result.set(m_printJobSpooler.submit(m_printableData));
}
