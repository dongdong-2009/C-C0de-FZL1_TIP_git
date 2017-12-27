/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/online_printing_agent/src/PrintableEventData.h $
 * @author:  Cameron Rochester
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * MmSchedulingData is an implementation of IMmSchedulingData. It holds the data specific to 
 * alarm plan associations.
 */

#ifndef PRINTABLE_EVENT_DATA_H
#define PRINTABLE_EVENT_DATA_H

#include <string>

#include "core/data_access_interface/online_printing_agent/src/IPrintableEventData.h"
#include "core/data_access_interface/online_printing_agent/src/PrintableEventHelper.h"

namespace TA_Base_Core
{
	class PrintableEventHelper;	
}

namespace TA_Base_Core
{
	class PrintableEventData: public IPrintableEventData
	{
	public:

		/**
		 * MmSchedulingData 
		 *
		 * Construct a MmSchedulingData class for the entry represented by the specified data.
		 * 
		 *
		 * @param key The key of record in the database
		 * @param subsystemKey: the key of the associated physical subsystem
         * @param time_t scheduledTime: the time the subsystem generates MMS messages
         * @param time_t lastRunDate: the last time/date that the subsystem was reported
		 * @param subName: the name of the physical subsystem
		 */
		PrintableEventData( unsigned long pkey,std::string printer,
                            std::string alarmflag, std::string createtime,
                            std::string severity, std::string value,
                            std::string asset,std::string description ,
                            std::string mms,std::string dss,
                            std::string avalanche,std::string operatorname,
                            std::string alarmcomment,std::string alm,
                            unsigned long locationkey);


		virtual ~PrintableEventData();
	


		/**
         * invalidate
         *
         * Mark the data contained by this Alarm type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

	 // end IItem methods
		
        virtual unsigned long getPkey();


 
        virtual std::string getPrinter();


    
        virtual std::string getAlarmflag();


        virtual std::string getCreatetime();


 
        virtual std::string getSeverity();


 
        virtual std::string getAsset();

    
 
        virtual std::string getDescription();


  
        virtual std::string getValue();


 
        virtual std::string getMMS();

  
        virtual std::string getDSS();

 
        virtual std::string getAvalanche();

  
        virtual std::string getOperatorname();


	    virtual std::string getAlarm_Comment();


	    virtual std::string getAlm();


	    virtual unsigned long getLocation_Key();


	private: //data

		PrintableEventHelper* m_helper;

	private: //methods:

		PrintableEventData& operator=(const PrintableEventData &);
		PrintableEventData( const PrintableEventData& thePrintableEvent);
	};


} // closes TA_IRS_Core

#endif // !PRINTABLE_EVENT_DATA_H
