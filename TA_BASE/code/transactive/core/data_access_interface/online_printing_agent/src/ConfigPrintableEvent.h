/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/online_printing_agent/src/ConfigPrintableEvent.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Implements the IConfigScheduleData interface to allow manipulation
  * of a row of schedule data. Note that schedule data is stored in the
  * Job table, but is accessed using this separate class for convenience.
  *
  */


#ifndef CONFIG_PRINTABLE_EVENT_H
#define CONFIG_PRINTABLE_EVENT_H



#include "core/data_access_interface/online_printing_agent/src/IConfigPrintableEvent.h"
#include "core/data_access_interface/online_printing_agent/src/PrintableEventData.h"

#include <string>



namespace TA_Base_Core
{

class PrintableEventHelper;

class ConfigPrintableEvent: public IConfigPrintableEvent
{

public:

    /** 
     *  Gets all schedules from the database.
     */
    ConfigPrintableEvent( unsigned long pkey,std::string printer,
                              std::string alarmflag, std::string createtime,
                              std::string severity, std::string value,
                              std::string asset,std::string description ,
                              std::string mms,std::string dss,
                              std::string avalanche,std::string operatorname,
                              std::string alarmcomment,std::string alm,
                              unsigned long locationkey);

 
    /**
     *  This will write all changes made to the database.
     *
     *  @exception DatabaseException
     *  Thrown if there is a database error.
     *
     *  @exception DataException
     *  Thrown if the a parameter name cannot be found, or if there is more
     *  than one value for a parmeter.
     *
     */
    virtual void applyChanges();

    virtual bool hasChanged();

    /**
     *  Indicates that the data in the database has changed and it should be
     *  reloaded if required.
     */
    virtual void invalidate();



    /**
     *  Gets the ID of the Schedule.
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
    virtual unsigned long getPkey();


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
    virtual std::string getPrinter();


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
    virtual std::string getAlarmflag();


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
    virtual std::string getCreatetime();


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
    virtual std::string getSeverity();


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
    virtual std::string getAsset();

    
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
    virtual std::string getDescription() ;


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
    virtual std::string getValue();


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
    virtual std::string getMMS();

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
    virtual std::string getDSS();

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
    virtual std::string getAvalanche();

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
    virtual std::string getOperatorname();
	virtual std::string getAlarm_Comment();
	virtual std::string getAlm();
	virtual unsigned long getLocation_Key();


    /**
     *  Sets the type of the Schedule.
     */
    virtual void setPrinter( std::string printer);


    /**
     *  Sets the minute of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setAlarmflag( std::string alarmflag);


    /**
     *  Sets the hour of the Schedule. (Note this is a 24 hour number, not 12)
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setCreatetime( std::string createtime );


    /**
     *  Sets the day of the month of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setSeverity( std::string severity);


    /**
     *  Sets the month of the year of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setAsset( std::string asset);

    
    /**
     *  Sets the year of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setDescription( std::string description);


    /**
     *  Sets the days of the week of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setValue( std::string value );


    /**
     *  Sets the day on which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setMMS( std::string mms);

    /**
     *  Sets the month in which the Schedule should run for the last time.
     *  If there is no end date this field is undefined.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setDSS( std::string dss);

    /**
     *  Sets the year in which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setAvalanche( std::string avalanche);

	virtual void setOperatorname( std::string operatorname);
	virtual void setAlarm_Comment( std::string alarmcomment);
	virtual void setAlm( std::string alm);
	virtual void setLocation_Key( unsigned long locationkey);

    /**
     *  The destructor.
     */
    virtual ~ConfigPrintableEvent();





private:
        

	PrintableEventHelper* m_helper;


private:
	ConfigPrintableEvent& operator=(const ConfigPrintableEvent &);

        /**
         * updateChanges
         *
         * This updates the map recording the changes to the alarm plan association. Whenever a call to set
         * is made in this class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param unsigned long - The old key that was stored for this attribute
         * @param unsigned long - The new key to be stored for this attribute
         */
     void updateChanges(const std::string& name, unsigned long oldKey, unsigned long newKey);

     bool m_isValidData;

};


};// TA_Base_Core


#endif // CONFIG_PRINTABLE_EVENT_H
