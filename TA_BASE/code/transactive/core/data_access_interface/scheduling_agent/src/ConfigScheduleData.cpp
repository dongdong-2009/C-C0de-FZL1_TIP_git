/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/ConfigScheduleData.cpp $
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


#include "core/data_access_interface/scheduling_agent/src/ConfigScheduleData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/RunParams.h"

#include "boost/tokenizer.hpp"

#include <iostream>
#include <sstream>
#include <list>


namespace TA_Base_Core
{


const std::string ConfigScheduleData::DAYS_OF_WEEK_SEPARATOR = ",";

const std::string ConfigScheduleData::SCHEDULING_TABLE_NAME = "sh_job";
const std::string ConfigScheduleData::ID_COLUMN             = "shjob_id";
const std::string ConfigScheduleData::SCHEDULE_TYPE_COLUMN  = "schedule_type";
const std::string ConfigScheduleData::MINUTE_COLUMN         = "minute";
const std::string ConfigScheduleData::HOUR_COLUMN           = "hour";
const std::string ConfigScheduleData::DAY_OF_MONTH_COLUMN   = "day_of_month";
const std::string ConfigScheduleData::MONTH_OF_YEAR_COLUMN  = "month_of_year";
const std::string ConfigScheduleData::YEAR_COLUMN           = "year";
const std::string ConfigScheduleData::DAYS_OF_WEEK_COLUMN   = "days_of_week";
const std::string ConfigScheduleData::END_DAY_COLUMN        = "end_day";
const std::string ConfigScheduleData::END_MONTH_COLUMN      = "end_month";
const std::string ConfigScheduleData::END_YEAR_COLUMN       = "end_year";



ConfigScheduleData::ConfigScheduleData()
    :
    m_id(""),
    m_type(0),
    m_minute(0),
    m_hour(0),
    m_dayOfMonth(0),
    m_monthOfYear(0),
    m_year(0),
    m_endDay(0),
    m_endMonth(0),
    m_endYear(0),
    m_isDeleted(false)
{
    m_daysOfWeek.clear();

    //
    // No data to be written yet.
    //
    m_dataNeedsToBeWrittenToDB = false;

    //
    // No data to be read from DB.
    //
    m_dataNeedsToBeReadFromDB = false;

}

ConfigScheduleData::ConfigScheduleData(const std::string& id)
    :
    m_id(id),
    m_type(0),
    m_minute(0),
    m_hour(0),
    m_dayOfMonth(0),
    m_monthOfYear(0),
    m_year(0),
    m_endDay(0),
    m_endMonth(0),
    m_endYear(0),
    m_isDeleted(false)
{
    m_daysOfWeek.clear();

    //
    // No data to be written yet.
    //
    m_dataNeedsToBeWrittenToDB = false;

    //
    // Data needs to be read from DB.
    //
    m_dataNeedsToBeReadFromDB = true;


    //
    // Throws an exception if there is no object with ID = m_id in the DB,
    // or if fields are invalid.
    //
    reloadDataFromDB();
}


ConfigScheduleData::ConfigScheduleData(
    IData* data, unsigned int rowNum)
    :
    m_dataNeedsToBeReadFromDB(false),
    m_dataNeedsToBeWrittenToDB(false),
    m_id(""),
    m_type(0),
    m_minute(0),
    m_hour(0),
    m_dayOfMonth(0),
    m_monthOfYear(0),
    m_year(0),
    m_endDay(0),
    m_endMonth(0),
    m_endYear(0),
    m_isDeleted(false)
{
    TA_ASSERT(data != NULL, "Data must not be NULL");
    TA_ASSERT(rowNum < data->getNumRows(), "Invalid row number");
    m_daysOfWeek.clear();

    //
    // Throws an exception if fields are invalid.
    //
    getDataFromRow(data, rowNum);
}


ScheduleDataAccessFactory::IConfigScheduleDataVector
    ConfigScheduleData::getAllSchedules()
{
    //
    // The names of columns that will be retrieved from the DB.
    //
    ColumnNamesVector columnNames;
    columnNames.push_back(ID_COLUMN);
    columnNames.push_back(SCHEDULE_TYPE_COLUMN);
    columnNames.push_back(MINUTE_COLUMN);
    columnNames.push_back(HOUR_COLUMN);
    columnNames.push_back(DAY_OF_MONTH_COLUMN);
    columnNames.push_back(MONTH_OF_YEAR_COLUMN);
    columnNames.push_back(YEAR_COLUMN);
    columnNames.push_back(DAYS_OF_WEEK_COLUMN);
    columnNames.push_back(END_DAY_COLUMN);
    columnNames.push_back(END_MONTH_COLUMN);
    columnNames.push_back(END_YEAR_COLUMN);
    
    //std::stringstream sql;
    //sql <<
    //    "select " << ID_COLUMN            << ", "
    //              << SCHEDULE_TYPE_COLUMN << ", "
    //              << MINUTE_COLUMN        << ", "
    //              << HOUR_COLUMN          << ", "
    //              << DAY_OF_MONTH_COLUMN  << ", "
    //              << MONTH_OF_YEAR_COLUMN << ", "
    //              << YEAR_COLUMN          << ", "
    //              // If the days of week column is NULL, use a space
    //              // (otherwise an exception will be thrown)
    //              << "nvl(" << DAYS_OF_WEEK_COLUMN << ", ' ')" << " as " << DAYS_OF_WEEK_COLUMN << ", "
    //              << END_DAY_COLUMN       << ", "
    //              << END_MONTH_COLUMN     << ", "
    //              << END_YEAR_COLUMN      << " "

    //    "from " << SCHEDULING_TABLE_NAME;	
    
    //
    // Get a reference to the database, and execute the query.
    //
    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Write);
//	std::string strSql  = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_Oracle_SELECT_37054);
//	std::string strSql   = database->prepareSQLStatement(SH_JOB_SELECT_37054);
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, SH_JOB_SELECT_37054);
    IData* data         = database->executeQuery(strSql, columnNames);



    //
    // The collection of Schedules that will be returned.
    //
    ScheduleDataAccessFactory::IConfigScheduleDataVector schedules;

    try
    {
        //
        // Create a new ConfigScheduleData object for each row of data.
        //
        for(unsigned int i = 0; i < data->getNumRows(); i ++)
        {
            ConfigScheduleData* schedule = new ConfigScheduleData(data, i);
            schedules.push_back(schedule);
        }

        //
        // Delete the data object.
        //
        delete data;
        data = NULL;

    }
    //
    // Ensure any memory gets cleaned up if there is an exception.
    //
    catch(...)
    {
        for(ScheduleDataAccessFactory::IConfigScheduleDataVector::iterator it =
                schedules.begin();
            it != schedules.end();
            it ++)
        {
            delete *it;
        }
        

        delete data;
        data = NULL;

        throw;
    }

    return schedules;
}



ConfigScheduleData::~ConfigScheduleData() {};


void ConfigScheduleData::deleteSchedule()
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Deleting schedule with id = %s", m_id.c_str());

    TA_ASSERT(!m_isDeleted, "Schedule has already been deleted");

    //
    // The schedule information is stored in the job database,
    // so when the corresponding job is deleted, the schedule will
    // be also.
    //

    m_isDeleted = true;
}


void ConfigScheduleData::createScheduleInDB()
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Creating schedule in database, with id = %s", m_id.c_str());

    
    //
    // Check that an entry in the job table exists in the database.
    //
    ColumnNamesVector columnNames;
    columnNames.push_back(ID_COLUMN);
    
   /* std::ostringstream sql;
    sql <<
        "select " << ID_COLUMN << " " <<
        "from " << SCHEDULING_TABLE_NAME << " "
        "where " << ID_COLUMN << " = '" << m_id << "'";*/
    

    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Write);
//	std::string strSql  = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_STD_SELECT_37001, m_id);
//	std::string strSql   = database->prepareSQLStatement(SH_JOB_SELECT_37001, m_id);
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, SH_JOB_SELECT_37001, m_id);
    IData* data         = database->executeQuery(strSql, columnNames);

    try
    {
        if(data->getNumRows() == 0)
        {
            std::stringstream message;
            message <<
                "An attempt was made to create a schedule when the " <<
                "corresponding job does not exist. " <<
                ID_COLUMN << " = " << m_id;

            TA_THROW( DataException(message.str().c_str(),
                                    DataException::MISSING_MANDATORY,
                                    ID_COLUMN)
                    );
        }
    }

    //
    // Make sure the memory for the IData object gets cleaned up
    //
    catch(...)
    {
        delete data;
        data = NULL;

        throw;
    }

    delete data;
    data = NULL;



    //
    // Add the schedule information to the job.
    //
    applyChanges();
}

void ConfigScheduleData::saveSchedule()
{
    applyChanges();
}

void ConfigScheduleData::applyChanges()
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Updating schedule with id = %s", m_id.c_str());

    TA_ASSERT(!m_isDeleted, "Schedule has already been deleted");

    if( ! m_dataNeedsToBeWrittenToDB )
    {
        return;
    }

    std::string daysOfWeekString = getDaysOfWeekAsString(m_daysOfWeek);


    //std::stringstream sql;
    //sql <<
    //    
    //"BEGIN " <<
    //    "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','INSERT', '" <<
    //    "update " << SCHEDULING_TABLE_NAME << " " <<

    //    "set " <<
    //        SCHEDULE_TYPE_COLUMN << " = "  << m_type           << ", "  <<
    //        MINUTE_COLUMN        << " = "  << m_minute         << ", "  <<
    //        HOUR_COLUMN          << " = "  << m_hour           << ", "  <<
    //        DAY_OF_MONTH_COLUMN  << " = "  << m_dayOfMonth     << ", "  <<
    //        MONTH_OF_YEAR_COLUMN << " = "  << m_monthOfYear    << ", "  <<
    //        YEAR_COLUMN          << " = "  << m_year           << ", "  <<
    //        DAYS_OF_WEEK_COLUMN  << " = ''" << daysOfWeekString << "'', " <<
    //        END_DAY_COLUMN       << " = "  << m_endDay         << ", "  <<
    //        END_MONTH_COLUMN     << " = "  << m_endMonth       << ", "  <<
    //        END_YEAR_COLUMN      << " = "  << m_endYear        << " "   <<
    //    
    //    "where " << ID_COLUMN << " = ''" << m_id << "''" <<
    //    "');" <<
    //"END;";


    //
    // Get a reference to the database, and execute the query.
    //
    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Write);
//	std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_Oracle_UPDATE_37202, getLocalDatabaseName(), m_type,m_minute,
//	std::string strSql  = database->prepareSQLStatement(SH_JOB_INSERT_37202, getLocalDatabaseName(), m_type,m_minute,
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, SH_JOB_INSERT_37202, getLocalDatabaseName(), m_type,m_minute,
		m_hour, m_dayOfMonth, m_monthOfYear, m_year, daysOfWeekString, m_endDay, m_endMonth, m_endYear, m_id);
    database->executeModification(strSql);


    //
    // Data is now in synch with DB
    //
    if(m_dataNeedsToBeReadFromDB)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "Schedule with id = %s written to DB when data "
                    "needed to be read from DB.",
                    m_id.c_str());

        m_dataNeedsToBeReadFromDB = false;
    }

    m_dataNeedsToBeWrittenToDB = false;
}



void ConfigScheduleData::reloadDataFromDB(bool forceReload /*= false*/)
{
    if( ! forceReload   &&   ! m_dataNeedsToBeReadFromDB )
    {
        return;
    }


    TA_ASSERT(!m_isDeleted, "Schedule has already been deleted");


    //
    // The names of columns that will be retrieved from the DB.
    //
    ColumnNamesVector columnNames;
    columnNames.push_back(ID_COLUMN);
    columnNames.push_back(SCHEDULE_TYPE_COLUMN);
    columnNames.push_back(MINUTE_COLUMN);
    columnNames.push_back(HOUR_COLUMN);
    columnNames.push_back(DAY_OF_MONTH_COLUMN);
    columnNames.push_back(MONTH_OF_YEAR_COLUMN);
    columnNames.push_back(YEAR_COLUMN);
    columnNames.push_back(DAYS_OF_WEEK_COLUMN);
    columnNames.push_back(END_DAY_COLUMN);
    columnNames.push_back(END_MONTH_COLUMN);
    columnNames.push_back(END_YEAR_COLUMN);
    //
    //std::stringstream sql;
    //sql <<
    //    "select " << ID_COLUMN            << ", "
    //              << SCHEDULE_TYPE_COLUMN << ", "
    //              << MINUTE_COLUMN        << ", "
    //              << HOUR_COLUMN          << ", "
    //              << DAY_OF_MONTH_COLUMN  << ", "
    //              << MONTH_OF_YEAR_COLUMN << ", "
    //              << YEAR_COLUMN          << ", "
    //              // If the days of week column is NULL, use a space
    //              // (otherwise an exception will be thrown)
    //              << "nvl(" << DAYS_OF_WEEK_COLUMN << ", ' ')" << " as " << DAYS_OF_WEEK_COLUMN << ", "
    //              << END_DAY_COLUMN       << ", "
    //              << END_MONTH_COLUMN     << ", "
    //              << END_YEAR_COLUMN      << " " <<

    //    "from " << SCHEDULING_TABLE_NAME << " " <<
    //    "where " << ID_COLUMN << " = '" << m_id << "'";
    //
    //
    // Get a reference to the database, and execute the query.
    //
    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Read);
//	std::string strSql  = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_Oracle_SELECT_37055, m_id);
//	std::string strSql   = database->prepareSQLStatement(SH_JOB_SELECT_37055, m_id);
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, SH_JOB_SELECT_37055, m_id);
    IData* data         = database->executeQuery(strSql, columnNames);

    try
    {
        //
        // No entry was found with the specified ID.
        // 
        if (data->getNumRows() == 0)
        {
            std::stringstream message;
            message <<
                "No schedule found with " << ID_COLUMN << " = " << m_id;

            TA_THROW( DataException(message.str().c_str(),
                      DataException::NO_VALUE, ID_COLUMN) );
        }


        getDataFromRow(data, 0);
        

        //
        // Delete the data object.
        //
        delete data;
        data = NULL;
        

        //
        // We now have valid data
        //
        m_dataNeedsToBeReadFromDB = false;

        //
        // If there was data that needed to be written to the DB,
        // it was just overwritten, so log a warning.
        // It's the same now anyway, so set m_dataNeedsToBeWrittenToDB
        // to false.
        //
        if(m_dataNeedsToBeWrittenToDB)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                        "Schedule with id = %s reloaded from DB when data in "
                        "memory had not yet been written to DB.",
                        m_id.c_str());

            m_dataNeedsToBeWrittenToDB = false;
        }
    }

    //
    // Ensure that the memory for the data object gets cleaned up.
    //
    catch(...)
    {
        delete data;
        data = NULL;

        throw;
    }
}


void ConfigScheduleData::getDataFromRow(IData* data, unsigned int rowNum)
{
    TA_ASSERT(data != NULL,                "Data must not be NULL");
    TA_ASSERT(rowNum < data->getNumRows(), "Invalid row number");

    //
    // Assign the data as appropriate to the member variables.
    // If a value is out of range or cannot be converted to the
    // correct type, a DataException will be thrown.
    //

    
    //
    // ID
    //
	std::string id = data->getStringData(rowNum, ID_COLUMN);


    //
    // Schedule Type
    //
    unsigned short scheduleType =
        (unsigned short) data->getUnsignedLongData(0, SCHEDULE_TYPE_COLUMN);
    
    //
    // Minute
    //
    unsigned short minute =
        (unsigned short) data->getUnsignedLongData(rowNum, MINUTE_COLUMN);
    
    
    //
    // Hour
    //
    unsigned short hour =
        (unsigned short) data->getUnsignedLongData(rowNum, HOUR_COLUMN);
    
    
    //
    // Day of month
    //
    unsigned short dayOfMonth = (unsigned short)
            data->getUnsignedLongData(rowNum, DAY_OF_MONTH_COLUMN);

    
    //
    // Month of year
    //
    unsigned short monthOfYear = 
        (unsigned short)
        data->getUnsignedLongData(rowNum, MONTH_OF_YEAR_COLUMN);;
    

    //
    // Year
    //
    unsigned short year = (unsigned short)
            data->getUnsignedLongData(rowNum, YEAR_COLUMN);
    
    
    //
    // Days of week
    //
    std::string daysOfWeekDataField = data->getStringData(rowNum, DAYS_OF_WEEK_COLUMN);

    //
    // If the daysOfWeek field is a space, this means it was NULL in the
    // database. (See getAllSchedules() and reloadDataFromDB())
    //
    if(daysOfWeekDataField == " ")
    {
        daysOfWeekDataField = "";
    }
    std::list<unsigned short> daysOfWeek = getDaysOfWeekFromString(daysOfWeekDataField);


    //
    // End day
    //
    unsigned short endDay = (unsigned short)
            data->getUnsignedLongData(rowNum, END_DAY_COLUMN);


    //
    // End month
    //
    unsigned short endMonth = 
        (unsigned short) data->getUnsignedLongData(rowNum, END_MONTH_COLUMN);;
    

    //
    // End year
    //
    unsigned short endYear = (unsigned short)
            data->getUnsignedLongData(rowNum, END_YEAR_COLUMN);
    

    //
    // No exceptions have been thrown, so all data fields are ok.
    // Assign them to the member variables.
    //
    m_type        = scheduleType;
    m_id          = id;
    m_minute      = minute;
    m_hour        = hour;
    m_dayOfMonth  = dayOfMonth;
    m_monthOfYear = monthOfYear;
    m_year        = year;
    m_daysOfWeek  = daysOfWeek;
    m_endDay      = endDay;
    m_endMonth    = endMonth;
    m_endYear     = endYear;
}


std::list<unsigned short> ConfigScheduleData::getDaysOfWeekFromString(
    const std::string& days)
{
    std::list< unsigned short > daysOfWeek;

    if(days.length() == 0)
    {
        return daysOfWeek;
    }

    //
    // Look for occurences of the separator string and extract the words
    // in between.
    //
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(",");
    tokenizer tokens(days, sep);
    for (tokenizer::iterator tokIter = tokens.begin(); tokIter != tokens.end(); ++tokIter)
    {
        daysOfWeek.push_back(atoi((*tokIter).c_str()));
    }

    return daysOfWeek;
}



std::string ConfigScheduleData::getDaysOfWeekAsString(
                                const std::list<unsigned short>& daysOfWeek)
{
    if(daysOfWeek.size() == 0)
    {
        return "";
    }

    std::list<unsigned short>::const_iterator day = daysOfWeek.begin();

    std::stringstream daysOfWeekString;
    daysOfWeekString << *day;
    day++;

    while(day != daysOfWeek.end())
    {
        daysOfWeekString << DAYS_OF_WEEK_SEPARATOR;
        daysOfWeekString << *day;
        day++;
    }

    return daysOfWeekString.str();
}


void ConfigScheduleData::invalidate()
{
    TA_ASSERT(!m_isDeleted, "Schedule has already been deleted");

    m_dataNeedsToBeReadFromDB = true;
}



std::string ConfigScheduleData::getID()
{
    reloadDataFromDB();

    return m_id;
}


unsigned short ConfigScheduleData::getScheduleType()
{
    reloadDataFromDB();

    return m_type;
}


unsigned short ConfigScheduleData::getMinute()
{
    reloadDataFromDB();

    return m_minute;
}



unsigned short ConfigScheduleData::getHour()
{
    reloadDataFromDB();

    return m_hour;
}



unsigned short ConfigScheduleData::getDayOfMonth()
{
    reloadDataFromDB();

    return m_dayOfMonth;
}



unsigned short ConfigScheduleData::getMonthOfYear()
{
    reloadDataFromDB();

    return m_monthOfYear;
}

unsigned short ConfigScheduleData::getYear()
{
    reloadDataFromDB();

    return m_year;
}

std::list<unsigned short> ConfigScheduleData::getDaysOfWeek()
{
    reloadDataFromDB();

    return m_daysOfWeek;
}

unsigned short ConfigScheduleData::getEndDay()
{
    reloadDataFromDB();

    return m_endDay;
}

unsigned short ConfigScheduleData::getEndMonth()
{
    reloadDataFromDB();

    return m_endMonth;
}

unsigned short ConfigScheduleData::getEndYear()
{
    reloadDataFromDB();

    return m_endYear;
}

void ConfigScheduleData::setScheduleID( const std::string& id)
{
    m_id = id;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setScheduleType( unsigned short scheduleType)
{
    m_type = scheduleType;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setMinute( unsigned short minute)
{
    m_minute = minute;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setHour( unsigned short hour )
{
    m_hour = hour;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setDayOfMonth( unsigned short dayOfMonth)
{
    m_dayOfMonth = dayOfMonth;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setMonthOfYear( unsigned short monthOfYear)
{
    m_monthOfYear = monthOfYear;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setYear( unsigned short year)
{
    m_year = year;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setDaysOfWeek( const std::list<unsigned short>& daysOfWeek )
{
    m_daysOfWeek = daysOfWeek;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setEndDay( unsigned short endDay)
{
    m_endDay = endDay;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setEndMonth( unsigned short endMonth)
{
    m_endMonth = endMonth;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigScheduleData::setEndYear( unsigned short endYear)
{
    m_endYear = endYear;
    m_dataNeedsToBeWrittenToDB = true;
}

bool ConfigScheduleData::hasEndDate()
{
    reloadDataFromDB();

    return (m_endDay != 0);
}

std::string ConfigScheduleData::getLocalDatabaseName()
{
    if (m_localDatabase.empty())
    {
        // have to find out what is the local db name
        // this should be the first in the connection list so grab it
        if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
        {
            throw DatabaseException("db-connection-file not set");
        }
        try
        {
			std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Scheduling_Cd, Write);

            /*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
            DataConnections connections = connectionStrings.getConnectionList(Scheduling_Cd, Write);
            m_localDatabase = connections[0].first;  */
        }
        catch(...) // most likely a file not found error
        {
            throw DatabaseException("Unable to find database connection infomation");
        }

    }
    return m_localDatabase;
}

};// TA_Base_Core

