/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/online_printing_agent/src/IPrintableEventData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface represents a row of data in the printable_event table.
  *
  */


#ifndef I_PRINTABLE_EVENT_DATA_H
#define I_PRINTABLE_EVENT_DATA_H

#include <string>

namespace TA_Base_Core
{

class IPrintableEventData
{

public:

   
    virtual unsigned long getPkey() = 0;


    /**
     *  Gets the type of the Printable_Event.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadPrintEventException
     *             if any of the fields are out of range.
     */
    virtual std::string getPrinter() = 0;


    /**
     *  Gets the minute of the Schedule.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getAlarmflag() = 0;


    /**
     *  Gets the hour of the Schedule. (Note this is a 24 hour number, not 12)
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getCreatetime() = 0;


    /**
     *  Gets the day of the month of the Schedule.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getSeverity() = 0;


    /**
     *  Gets the month of the year of the Schedule.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getAsset() = 0;

    
    /**
     *  Gets the year of the Schedule.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. YEAR should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getDescription() = 0;


    /**
     *  Gets the days of the week of the Schedule.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getValue() = 0;


    /**
     *  Gets the day on which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getMMS() = 0;

    /**
     *  Gets the month in which the Schedule should run for the last time.
     *  If there is no end date this field is undefined.
     *  This is only applicable for recurring schedules.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getDSS() = 0;

    /**
     *  Gets the year in which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getAvalanche() = 0;

    /**
     *  Indicates if this schedule has an end date, or if it recurs
     *  indefinitely. Once off schedules do not have an end date.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual std::string getOperatorname() = 0;
	virtual std::string getAlarm_Comment() = 0;
	virtual std::string getAlm() = 0;
	virtual unsigned long getLocation_Key() = 0;

    virtual void invalidate() = 0;

    /**
     *  The destructor.
     */
    virtual ~IPrintableEventData() {};

};


};// TA_Base_Core


#endif  //I_PRINTABLE_EVENT_DATA_H
