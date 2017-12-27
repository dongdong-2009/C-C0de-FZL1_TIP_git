/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/online_printing_agent/src/IConfigPrintableEvent.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface extends the IScheduleData interface to allow a row of
  * data in the Schedule table to be written as well as read.
  *
  */


#ifndef I_CONFIG_PRINTABLE_EVENT_H
#define I_CONFIG_PRINTABLE_EVENT_H


#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/online_printing_agent/src/IPrintableEventData.h"

#include <string>
namespace TA_Base_Core
{

class IConfigPrintableEvent: public IPrintableEventData
{

public:


    /**
     *  Saves this Schedule to the database.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem storing
     *  this Schedule in the database.
     *
     */
    virtual bool hasChanged() =0;

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
    virtual void applyChanges() = 0;


 
    /**
     *  Sets the type of the Schedule.
     */
    virtual void setPrinter( std::string printer) = 0;


    /**
     *  Sets the minute of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setAlarmflag( std::string alarmflag) = 0;


    /**
     *  Sets the hour of the Schedule. (Note this is a 24 hour number, not 12)
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setCreatetime( std::string createtime ) = 0;


    /**
     *  Sets the day of the month of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setSeverity( std::string severity) = 0;


    /**
     *  Sets the month of the year of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setAsset( std::string asset) = 0;

    
    /**
     *  Sets the year of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setDescription( std::string description) = 0;


    /**
     *  Sets the days of the week of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setValue( std::string value ) = 0;


    /**
     *  Sets the day on which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setMMS( std::string mms) = 0;

    /**
     *  Sets the month in which the Schedule should run for the last time.
     *  If there is no end date this field is undefined.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setDSS( std::string dss) = 0;

    /**
     *  Sets the year in which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setAvalanche( std::string avalanche) = 0;

	virtual void setOperatorname( std::string operatorname) = 0;
	virtual void setAlarm_Comment( std::string alarmcomment) = 0;
	virtual void setAlm( std::string alm) = 0;
	virtual void setLocation_Key( unsigned long locationkey) = 0;
    /**
     *  The destructor.
     */
    virtual ~IConfigPrintableEvent() {};

};


};// TA_Base_Core


#endif // I_CONFIG_PRINTABLE_EVENT_H
