#if !defined(PRINTJOBSPOOLER_H_INCLUDED)
#define PRINTJOBSPOOLER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintJobSpooler.h $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Submits print jobs to the printing system. Written with LPRng in mind,
  * however, should work with any system that supports the Berkeley LPR
  * print spooler functionality.
  *
  */

namespace TA_Base_App
{
    class PrintJobSpooler : public IPrintJobSpooler
    {
    // Operations
    public:
        PrintJobSpooler();
        virtual ~PrintJobSpooler();

        /**
         * submit
         *
         * Implements IPrintJobSpooler::submit().
         */
        virtual bool submit(const PrintableData& printableData);

    private:
        PrintJobSpooler(const PrintJobSpooler&);
        PrintJobSpooler& operator=(const PrintJobSpooler&);


    // Attributes
    private:
        PrintAlarmSender m_alarmSender;
    };
}

#endif // !defined(PRINTJOBSPOOLER_H_INCLUDED)
