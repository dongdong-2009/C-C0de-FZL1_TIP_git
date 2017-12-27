/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AbstractPrinter.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
#if !defined(ABSTRACTPRINTER)
#define ABSTRACTPRINTER

#include <string>

namespace TA_Base_App
{
    class AbstractPrinter
    {
    public:

        /**
          * Constructer
          *
          * @param Printer name
          * @param Printer port
          * @param Printer host name
          */
        AbstractPrinter( const std::string& p_name, 
                         const std::string& p_port, 
                         const std::string& p_host )
        : m_printerName( p_name ), m_printerPort( p_port ), m_printerHost( p_host ){}

        /**
          * Destructer
          */
        virtual ~AbstractPrinter(){}

        /**
          * print
          *
          * @param The string to print
          */
        virtual void print( const std::string& p_stringToPrint ) = 0;

    private:

        std::string m_printerName;
        std::string m_printerPort;
        std::string m_printerHost;

    protected:

        /**
          * getPrinterName
          *
          * @return The printer name
          */
        const std::string getPrinterName() { return m_printerName; }

        /**
          * getPrinterPort
          *
          * @return The printer port
          */
        const std::string getPrinterPort() { return m_printerPort; }

        /**
          * getPrinterHost
          *
          * @return The name of the host that the printer is connected to
          */
        const std::string getPrinterHost() { return m_printerHost; }

    };
}

#endif // !defined(ABSTRACTPRINTER)
