 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrinter.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * OnlinePrinter is an implementation of IOnlinePrinter. It holds the data specific to an OnlinePrinter entry
  * in the database, and allows read-only access to that data.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/src/OnlinePrinter.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    OnlinePrinter::OnlinePrinter(const unsigned long key)
    {
        m_OnlinePrinterHelper = new OnlinePrinterHelper(key);
    }


	OnlinePrinter::OnlinePrinter(unsigned long row, TA_Base_Core::IData& data)
		: m_OnlinePrinterHelper( new OnlinePrinterHelper(row, data) )
	{
	}


    OnlinePrinter::~OnlinePrinter() 
	{
        delete m_OnlinePrinterHelper;
        m_OnlinePrinterHelper=NULL;
    }


	std::string OnlinePrinter::getPrinter()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getPrinter();
    }


    unsigned long OnlinePrinter::getLocation()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getLocation();
    }

    unsigned long OnlinePrinter::getSubsystem()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getSubsystem();
    }

    bool OnlinePrinter::getIsAlarm()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getIsAlarm();
    }

	unsigned long OnlinePrinter::getKey()
	{
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getKey();
    }

	std::string OnlinePrinter::getName()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");

        std::ostringstream name;
		name << m_OnlinePrinterHelper->getPrinter();
        name << " - ";
		name << m_OnlinePrinterHelper->getSubsystem();

        return name.str();
    }


    void OnlinePrinter::invalidate()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        m_OnlinePrinterHelper->invalidate();
    }

} // closes TA_Core
