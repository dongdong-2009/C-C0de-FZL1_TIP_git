/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/ConfigJobData.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Implements the IConfigJobData interface to allow manipulation
  * of a row of data in the Job table.
  *
  */

// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "core/data_access_interface/scheduling_agent/src/ConfigJobData.h"
#include "core/data_access_interface/scheduling_agent/src/ScheduleDataAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/RunParams.h"
#include "core/uuid/src/TAuuid.h"


#include <iostream>
#include <sstream>

#include <map>

// Kindly assertion without breakpoint, just throw exception
#define TA_CONFIGJOBDATA_EXCEPTION(test, info)					\
    {												\
        if (!(test))                                \
        { \
            std::ostringstream msg; \
            msg << #test << ":FALSE Msg:" << #info; \
            TA_THROW( TA_Base_Core::DataException( msg.str().c_str(), \
            TA_Base_Core::DataException::MISSING_MANDATORY, "")); \
        }											\
    }

namespace TA_Base_Core
{


const std::string ConfigJobData::SCHEDULED_JOB_TABLE_NAME = "SH_JOB";
const std::string ConfigJobData::SCHEDULED_JOB_SEQUENCE   = "SHJOB_SEQ";

const std::string ConfigJobData::ID_COLUMN                = "shjob_id";
const std::string ConfigJobData::ORDER_ID_COLUMN                = "order_id";
const std::string ConfigJobData::JOB_TYPE_COLUMN          = "job_type";
const std::string ConfigJobData::DESCRIPTION_COLUMN       = "description";
const std::string ConfigJobData::IS_INHIBITED_COLUMN      = "is_inhibited";
const std::string ConfigJobData::LOCATION_ID_COLUMN       = "location_id";
const std::string ConfigJobData::PROFILE_ID_COLUMN        = "seprof_id";
const std::string ConfigJobData::PARAMETERS_COLUMN        = "parameters";


const std::string ConfigJobData::IS_INHIBITED_TRUE  = "Y";
const std::string ConfigJobData::IS_INHIBITED_FALSE = "N";

const std::string ConfigJobData::PARAMETER_SEPARATOR = "|||";


ConfigJobData::~ConfigJobData()
{
    delete m_schedule;
    m_schedule = NULL;
};

ConfigJobData::ConfigJobData()

    :

    m_dataNeedsToBeReadFromDB(   false  ),
    m_dataNeedsToBeWrittenToDB(  true   ),
    m_id(                        ""     ),
	m_orderId(                   0      ),
    m_jobType(                   ""     ),
    m_description(               ""     ),
    m_isInhibited(               false  ),
    m_locationID(                0      ),
    m_profileID(                 0      ),
    m_isDeleted(                 false  )
{
    FUNCTION_ENTRY("ConfigJobData()");
    // get an uuid for this job
    m_id = getNewID();

	m_orderId = getNewOrderId();

    // add an empty schedule
    m_schedule = ScheduleDataAccessFactory::getInstance().createSchedule();
    m_schedule->setScheduleID( m_id );
}


	ConfigJobData::ConfigJobData( const std::string& id )
    :
    m_id(id),
    m_jobType(""),
    m_description(""),
    m_isInhibited(false),
    m_locationID(0),
    m_profileID(0),
    m_parameters(),
    m_schedule(NULL),
    m_isDeleted(false)
{
    FUNCTION_ENTRY("ConfigJobData( unsigned long id )");
    //
    // No data to be written yet.
    //
    m_dataNeedsToBeWrittenToDB = false;

    //
    // Data needs to be read from DB.
    //
    m_dataNeedsToBeReadFromDB = true;


    //
    // Get this job's schedule from the database.
    //
    m_schedule = ScheduleDataAccessFactory::getInstance().getSchedule(id);
    TA_CONFIGJOBDATA_EXCEPTION(m_schedule != NULL, "The schedule must not be NULL");
    

    //
    // Throws a JobDoesNotExistException if there is no object
    // with ID = m_id in the DB.
    //

    /** TODO should this be in the constructor?? */
    reloadDataFromDB();
}


ConfigJobData::ConfigJobData(
    IData* data, unsigned int rowNum)
    :
    m_id(""),
	m_orderId(0),
    m_jobType(""),
    m_description(""),
    m_isInhibited(false),
    m_locationID(0),
    m_profileID(0),
    m_parameters(),
    m_schedule(NULL),
    m_dataNeedsToBeWrittenToDB(false),
    m_dataNeedsToBeReadFromDB(true),
    m_isDeleted(false)
{
    FUNCTION_ENTRY("ConfigJobData(IData* data, unsigned int rowNum)");
    TA_CONFIGJOBDATA_EXCEPTION(data != NULL, "Data must not be NULL");
    TA_CONFIGJOBDATA_EXCEPTION(rowNum < data->getNumRows(), "Invalid row number");

    getDataFromRow(data, rowNum);

    m_schedule = ScheduleDataAccessFactory::getInstance().getSchedule(m_id);
    TA_CONFIGJOBDATA_EXCEPTION(m_schedule != NULL, "The schedule must not be NULL");
}





JobDataAccessFactory::IConfigJobDataVector
    ConfigJobData::getAllJobs()
{
    return getAllJobsOfAnyType("");
}



JobDataAccessFactory::IConfigJobDataVector
    ConfigJobData::getAllJobsOfType(const std::string& jobType)
{
    TA_CONFIGJOBDATA_EXCEPTION(jobType != "", "Job type must not be blank");

    return getAllJobsOfAnyType(jobType);
}


JobDataAccessFactory::IConfigJobDataVector
    ConfigJobData::getAllJobsOfAnyType(const std::string& jobType)
{
    FUNCTION_ENTRY("getAllJobsOfAnyType");
    //
    // The names of columns that will be retrieved from the DB.
    //
    ColumnNamesVector columnNames;
    columnNames.push_back(ID_COLUMN);
	columnNames.push_back(ORDER_ID_COLUMN);
    columnNames.push_back(JOB_TYPE_COLUMN);
    columnNames.push_back(DESCRIPTION_COLUMN);
    columnNames.push_back(IS_INHIBITED_COLUMN);
    columnNames.push_back(LOCATION_ID_COLUMN);
    columnNames.push_back(PROFILE_ID_COLUMN);
    columnNames.push_back(PARAMETERS_COLUMN);


    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Read);

    std::stringstream commonSql;
    //commonSql <<
    //    "select " << ID_COLUMN            << ", "
				//  << ORDER_ID_COLUMN	  << ", "
    //              << JOB_TYPE_COLUMN      << ", "
    //              << DESCRIPTION_COLUMN   << ", "
    //              << IS_INHIBITED_COLUMN  << ", "
    //              << LOCATION_ID_COLUMN   << ", "
    //              << PROFILE_ID_COLUMN    << ", "
    //              // If the parameters column is NULL, use a space
    //              // (otherwise an exception will be thrown)
    //              << "nvl(" << PARAMETERS_COLUMN << ", ' ')" << " as " << PARAMETERS_COLUMN << 
    //    " from " << SCHEDULED_JOB_TABLE_NAME <<
    //    " where ";
    if (!jobType.empty())
    {
        // only search by job type if the jobType is specified
        commonSql << JOB_TYPE_COLUMN << " = '" << database->escapeQueryString(jobType) << "'" << " and ";
    }
        

    //
    // The collection of Jobs that will be returned.
    //
    JobDataAccessFactory::IConfigJobDataVector jobs;
    IData* data;

    unsigned long lastShJobOrderId = 0;
    unsigned long numRowsFetched = 0;
    do {
   /*     std::stringstream sql;
        sql << commonSql.str() << ORDER_ID_COLUMN << " > " << lastShJobOrderId << " order by " << ORDER_ID_COLUMN; */
//			std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_Oracle_SELECT_37051, commonSql.str(), lastShJobOrderId);
//			std::string strSql  = database->prepareSQLStatement(SH_JOB_SELECT_37051, commonSql.str(), lastShJobOrderId);
	    SQLStatement strSql;
	    database->prepareSQLStatement(strSql, SH_JOB_SELECT_37051, commonSql.str(), lastShJobOrderId);


        //
        // Execute the query
        //
        data = database->executeQuery(strSql, columnNames, SimpleDb::MAX_ROWS);

        do
        {
            //
            // Create a new ConfigJobData object for each row of data.
            //
            numRowsFetched = data->getNumRows();
            if (numRowsFetched > 0)
            {
                for(unsigned int i = 0; i < numRowsFetched; i ++)
                {
                    try
                    {
                        lastShJobOrderId = data->getUnsignedLongData(i, ORDER_ID_COLUMN);
                        ConfigJobData* job = new ConfigJobData(data, i);
                        jobs.push_back(job);
                    }
                    catch(...)
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Job construction fail");
                    }
                }

                //
                // Delete the data object.
                //
                //lastShJobOrderId = dynamic_cast<ConfigJobData*>(jobs.back())->getOrderId();
            }

            delete data;
            data = NULL;

        }
        while( database->moreData(data) );
    } while(numRowsFetched == SimpleDb::MAX_ROWS);
    

    return jobs;
}





void ConfigJobData::deleteJob()
{
    FUNCTION_ENTRY("deleteJob");
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Deleting Job with id = %s", m_id.c_str());

    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Job has already been deleted");

    // The schedule must be deleted first so as not to violate the
    // foreign key constraint from its pkey to the job's pkey
    TA_CONFIGJOBDATA_EXCEPTION(m_schedule != NULL, "Schedule must not be NULL");

    m_schedule->deleteSchedule();


   /* std::stringstream sql;
    sql <<
    "BEGIN " <<
        "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','DELETE', '" <<
        "delete from " << SCHEDULED_JOB_TABLE_NAME <<
        " where " << ID_COLUMN << " = ''" << m_id << "''" << 
        "');" <<
    "END;";*/


    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Write);
//	std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_Oracle_DELETE_37451, m_id);
//	std::string strSql  = database->prepareSQLStatement(SH_JOB_SELECT_37451, getLocalDatabaseName(), m_id);
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, SH_JOB_SELECT_37451, getLocalDatabaseName(), m_id);
    database->executeModification(strSql);


    //
    // Mark the job as having been deleted
    //
    m_isDeleted = true;
}

void ConfigJobData::saveJob()
{
    createJobInDB();
}

void ConfigJobData::createJobInDB()
{
    FUNCTION_ENTRY("createJobInDB");
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Creating Job with id = %s", m_id.c_str());

    
    //
    // Check first that the Job doesn't already exist in the database.
    //
    ColumnNamesVector columnNames;
    columnNames.push_back(ID_COLUMN);
    
   /* std::ostringstream sql;
    sql <<
        "select " << ID_COLUMN << " " <<
        "from " << SCHEDULED_JOB_TABLE_NAME << " "
        "where " << ID_COLUMN << " = '" << m_id << "'";*/
    

    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Write);
//	std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_STD_SELECT_37001, m_id);
//	std::string strSql  = database->prepareSQLStatement(SH_JOB_SELECT_37001, m_id);
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, SH_JOB_SELECT_37001, m_id);
    IData* data         = database->executeQuery(strSql, columnNames);

    try
    {
        if(data->getNumRows() != 0)
        {
            std::stringstream message;
            message <<
                "An attempt was made to create a Job that already exists. "
                << ID_COLUMN << " = " << m_id;

            TA_THROW( DataException(message.str().c_str(),
                                    DataException::NOT_UNIQUE,
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
        throw;
    }

    delete data;


    //
    // Escape single quotes from description and job type
    //
    std::string description = database->escapeAQSQLString(m_description);
    std::string jobType     = database->escapeAQSQLString(m_jobType);


    //
    // Get m_isInhibited as a string
    //
    std::string isInhibited = IS_INHIBITED_TRUE;
    if(! m_isInhibited)
    {
        isInhibited = IS_INHIBITED_FALSE;
    }

    //
    // Get m_parameters as a single string
    //
    std::string parameters =
        database->escapeAQSQLString(getParametersAsString());


    //
    // It doesn't already exist, so create it.
    //
   /* std::ostringstream insertSql;
    insertSql <<
    "BEGIN " <<
        "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','INSERT', '" <<
        "insert into " << SCHEDULED_JOB_TABLE_NAME <<
        "("
            << ID_COLUMN            << ", "
			<< ORDER_ID_COLUMN		<< ", "
            << JOB_TYPE_COLUMN      << ", "
            << DESCRIPTION_COLUMN   << ", "
            << IS_INHIBITED_COLUMN  << ", "
            << LOCATION_ID_COLUMN   << ", "
            << PROFILE_ID_COLUMN    << ", "
            << PARAMETERS_COLUMN    <<
        ") " <<
        "values " <<
        "(" <<
            "''" << m_id          << "'', " <<
					m_orderId	  << ", "	<<
            "''" << jobType       << "'', " <<
            "''" << description   << "'', " <<
            "''" << isInhibited   << "'', " <<
                    m_locationID  << ", "   <<
                    m_profileID   << ", "   <<
            "''" << parameters    << "''"   <<
        ")"
        "');" <<
    "END;";*/
//	strSql = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_Oracle_INSERT_37351, getLocalDatabaseName(),
//	strSql  = database->prepareSQLStatement(SH_JOB_INSERT_37351, getLocalDatabaseName(),

		database->prepareSQLStatement(strSql, SH_JOB_INSERT_37351, getLocalDatabaseName(),
		m_id, m_orderId, jobType, description, isInhibited, m_locationID, m_profileID, parameters);


    database->executeModification(strSql);


    //
    // The data is now up to date so it does not need to be read from the DB
    // and does not need to be written to the DB.
    //
    m_dataNeedsToBeReadFromDB  = false;
    m_dataNeedsToBeWrittenToDB = false;


}


void ConfigJobData::applyChanges()
{
    FUNCTION_ENTRY("applyChanges");
    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Mutator called on deleted object");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Updating Job with id = %s", m_id.c_str());

    if( ! m_dataNeedsToBeWrittenToDB )
    {
        return;
    }


    //
    // Get a reference to the database
    //
    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Write);


    //
    // Escape single quotes from description and job type
    //
    std::string description = database->escapeAQSQLString(m_description);
    std::string jobType     = database->escapeAQSQLString(m_jobType);

    //
    // Get m_isInhibited as a string
    //
    std::string isInhibited = IS_INHIBITED_TRUE;
    if(! m_isInhibited)
    {
        isInhibited = IS_INHIBITED_FALSE;
    }

    //
    // Get m_parameters as a single string
    //
    std::string parameters =
        database->escapeAQSQLString(getParametersAsString());



    //
    // Create the sql
    //
  /*  std::stringstream sql;
    sql <<
        
    "BEGIN " <<
        "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','UPDATE', '" <<
        "update " << SCHEDULED_JOB_TABLE_NAME << " " <<

        "set " <<
            ID_COLUMN            << " = ''" <<  m_id          << "'', " <<
			ORDER_ID_COLUMN		 << " = "	<<  m_orderId	  << ", "	<<
            JOB_TYPE_COLUMN      << " = ''" <<  jobType       << "'', " <<
            DESCRIPTION_COLUMN   << " = ''" <<  description   << "'', " <<
            IS_INHIBITED_COLUMN  << " = ''" <<  isInhibited   << "'', " <<
            LOCATION_ID_COLUMN   << " = "   <<  m_locationID  << ", "   <<
            PROFILE_ID_COLUMN    << " = "   <<  m_profileID   << ", "   <<
            PARAMETERS_COLUMN    << " = ''" <<  parameters    << "'' "  <<
            
        "where " << ID_COLUMN << " = ''" << m_id << "''" <<
        "');" <<
    "END;";*/
//	std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_Oracle_UPDATE_37201, getLocalDatabaseName(),
//	std::string strSql  = database->prepareSQLStatement(SH_JOB_UPDATE_37201, getLocalDatabaseName(),
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, SH_JOB_UPDATE_37201, getLocalDatabaseName(),
		m_id, m_orderId, jobType, description, isInhibited, m_locationID, m_profileID, parameters, m_id );


    //
    // Execute the query.
    //
    database->executeModification(strSql);



    // Write changes to the schedule as well
    TA_CONFIGJOBDATA_EXCEPTION(m_schedule != NULL, "Schedule should never be NULL");
    m_schedule->applyChanges();


    //
    // Data is now in synch with DB
    //
    if(m_dataNeedsToBeReadFromDB)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "Job with id = %s written to DB when data "
                    "needed to be read from DB.",
                    m_id.c_str());

        m_dataNeedsToBeReadFromDB = false;
    }

    m_dataNeedsToBeWrittenToDB = false;
}


void ConfigJobData::reloadDataFromDB(bool forceReload /*= false*/)
{
    FUNCTION_ENTRY("reloadDataFromDB");
    // Reload the schedule from the DB
    TA_CONFIGJOBDATA_EXCEPTION(m_schedule != NULL, "Schedule should never be NULL");
    m_schedule->reloadDataFromDB(forceReload);
    
    
    if( ! forceReload   &&   ! m_dataNeedsToBeReadFromDB )
    {
        return;
    }

    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Mutator called on deleted object");


    //
    // The names of columns that will be retrieved from the DB.
    //
    ColumnNamesVector columnNames;
    columnNames.push_back(ID_COLUMN);
	columnNames.push_back(ORDER_ID_COLUMN);
    columnNames.push_back(JOB_TYPE_COLUMN);
    columnNames.push_back(DESCRIPTION_COLUMN);
    columnNames.push_back(IS_INHIBITED_COLUMN);
    columnNames.push_back(LOCATION_ID_COLUMN);
    columnNames.push_back(PROFILE_ID_COLUMN);
    columnNames.push_back(PARAMETERS_COLUMN);
    

    //std::stringstream sql;
    //sql <<
    //    "select " << ID_COLUMN            << ", "
				//  << ORDER_ID_COLUMN	  << ", "
    //              << JOB_TYPE_COLUMN      << ", "
    //              << DESCRIPTION_COLUMN   << ", "
    //              << IS_INHIBITED_COLUMN  << ", "
    //              << LOCATION_ID_COLUMN   << ", "
    //              << PROFILE_ID_COLUMN    << ", "
    //              // If the parameters column is NULL, use a space
    //              // (otherwise an exception will be thrown)
    //              << "nvl(" << PARAMETERS_COLUMN << ", ' ') " << " as " << PARAMETERS_COLUMN <<
    //    " from " << SCHEDULED_JOB_TABLE_NAME << " " <<
    //    " where " << ID_COLUMN << " = '" << m_id << "'";
    
    //
    // Get a reference to the database, and execute the query.
    //
    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Read);
//	std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_Oracle_SELECT_37052, m_id);
//	std::string strSql  = database->prepareSQLStatement(SH_JOB_SELECT_37052, m_id);
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, SH_JOB_SELECT_37052, m_id);
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
                "No Job found with " << ID_COLUMN << " = " << m_id;

            TA_THROW( DataException(message.str().c_str(),
                                    DataException::MISSING_MANDATORY,
                                    ID_COLUMN)
                    );
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
                        "Job with id = %s reloaded from DB when data in "
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


void ConfigJobData::getDataFromRow(IData* data, unsigned int rowNum)
{
    FUNCTION_ENTRY("getDataFromRow");
    TA_CONFIGJOBDATA_EXCEPTION(data != NULL,                "Data must not be NULL");
    TA_CONFIGJOBDATA_EXCEPTION(rowNum < data->getNumRows(), "Invalid row number");

    //
    // Assign the data as appropriate to the member variables.
    //

    //
    // ID
    //
	std::string id = data->getStringData(rowNum, ID_COLUMN);

	//
    // ORDER_ID
    //
	unsigned long orderId = data->getUnsignedLongData(rowNum, ORDER_ID_COLUMN);


    //
    // Job Type
    //
    std::string jobType = data->getStringData(rowNum, JOB_TYPE_COLUMN);


    //
    // Description
    //
    std::string description = data->getStringData(rowNum, DESCRIPTION_COLUMN);
    


    //
    // isInhibited
    //
    std::string isInhibitedString =
        data->getStringData(rowNum, IS_INHIBITED_COLUMN);
    

    if(isInhibitedString != IS_INHIBITED_TRUE &&
       isInhibitedString != IS_INHIBITED_FALSE)
    {
        std::string message = IS_INHIBITED_COLUMN;
        message            += " field is not a valid character.";
        
        TA_THROW(DataException(message.c_str(),
                               DataException::WRONG_TYPE,
                               IS_INHIBITED_COLUMN)
                );
    }


    bool isInhibited = true;
    
    if(isInhibitedString == IS_INHIBITED_FALSE)
    {
        isInhibited = false;
    }

    
    
    //
    // Location ID
    //
    unsigned long locationID = data->getUnsignedLongData(rowNum, LOCATION_ID_COLUMN);

    //
    // Profile ID
    //
    unsigned long profileID = data->getUnsignedLongData(rowNum, PROFILE_ID_COLUMN);

    //
    // Parameters
    //
    std::string parametersString =
        data->getStringData(rowNum, PARAMETERS_COLUMN);

    //
    // If the string is a ' ', this means the column was NULL.
    // (See the reloadDataFromDB() method above)
    //
    if(parametersString == " ")
    {
        parametersString = "";
    }

    try
    {
        m_parameters = getParametersAsMap(parametersString);
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[DataError]Wrong parameter string: %s, id: %s", parametersString.c_str(), id.c_str());

        throw;
    }

    //
    // No exceptions have been thrown, so all data fields are ok.
    // Assign them to the member variables.
    //
    m_id          = id;
	m_orderId	  = orderId;	
    m_jobType     = jobType;
    m_description = description;
    m_isInhibited = isInhibited;
    m_locationID  = locationID;
    m_profileID   = profileID;
}


std::string ConfigJobData::getParametersAsString()
{
    FUNCTION_ENTRY("getParametersAsString");
    std::string parametersString = "";

    for(IJobData::ParameterMap::iterator it = m_parameters.begin();
        it != m_parameters.end();
        it ++)
    {
        parametersString += (*it).first;
        parametersString += PARAMETER_SEPARATOR;
        parametersString += (*it).second;
        parametersString += PARAMETER_SEPARATOR;
    }

    return parametersString;
}


IJobData::ParameterMap ConfigJobData::getParametersAsMap(
    const std::string& parametersString)
{
    FUNCTION_ENTRY("getParametersAsMap");

    //
    // Tokenise the string, splitting it on PARAMETER_SEPARATOR.
    //
    // The string looks like this:
    //
    // nameSEPvalueSEPnameSEPvalueSEP
    //
    // where SEP is PARAMETER_SEPARATOR. Note that it ends with a separator.
    //
    std::vector< std::string > nameValues;

    const std::string::size_type SEPARATOR_LENGTH =
        PARAMETER_SEPARATOR.length();

    std::string::size_type nvBegin = 0;
    std::string::size_type nvEnd   =
        parametersString.find(PARAMETER_SEPARATOR);

    while(nvEnd != std::string::npos)
    {
        std::string::size_type nvLength = nvEnd - nvBegin;
        std::string nameValue(parametersString, nvBegin, nvLength);

        nameValues.push_back(nameValue);

        nvBegin = nvEnd + SEPARATOR_LENGTH;
        nvEnd   =
            parametersString.find(PARAMETER_SEPARATOR, nvEnd + SEPARATOR_LENGTH);
    }


    //
    // There should be an even number of tokens.
    //
    if( (nameValues.size() % 2)   !=   0 )
    {
        std::string message = "The parameters field does not contain a "
                              "value for every name" + parametersString;

        TA_THROW( DataException(message.c_str(),
                                DataException::WRONG_TYPE,
                                PARAMETERS_COLUMN)
                );
    }


    //
    // The tokens can now be paired and put into the map.
    //
    IJobData::ParameterMap parameters;

    for(std::vector< std::string >::iterator it = nameValues.begin();
        it != nameValues.end();
        )
    {
        std::string name = *it;
        it ++;

        std::string value = *it;
        it ++;

        parameters[name] = value;
    }

    return parameters;
}


void ConfigJobData::invalidate()
{
    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Mutator called on deleted object");

    TA_CONFIGJOBDATA_EXCEPTION(m_schedule != NULL, "Schedule should never be NULL");
    m_schedule->invalidate();

    m_dataNeedsToBeReadFromDB = true;
}

std::string ConfigJobData::getNewID()
{
    FUNCTION_ENTRY("getNewID");

	std::string id = TA_Base_Core::TA_Uuid::generateUUID();

    return id;
}

unsigned long ConfigJobData::getNewOrderId()
{
    std::string keyColumn = "DUAL";
    std::vector<std::string> columnNames;
    columnNames.push_back(keyColumn);

    // Get the pkey
  /*  std::stringstream sql;
    sql << "select " << SCHEDULED_JOB_SEQUENCE << ".nextval "
        << "from Dual";	*/

    IDatabase* database = DatabaseFactory::getInstance().getDatabase(Scheduling_Cd, Read);
//	std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), SH_JOB_Oracle_SELECT_37053);
//	std::string strSql  = database->prepareSQLStatement(SH_JOB_SELECT_37053);
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, SH_JOB_SELECT_37053);
    IData* data = database->executeQuery(strSql, columnNames);

    unsigned long orderId;
    try
    {
        orderId = data->getUnsignedLongData(0, keyColumn);
    }
    catch(...)
    {
        // Clean up the pointer
        delete data;
        data = NULL;

        throw;
    }

    // Clean up the pointer
    delete data;
    data = NULL;

	return orderId;
}

std::string ConfigJobData::getID()
{
    return m_id;
}

unsigned long ConfigJobData::getOrderId()
{
	return m_orderId;
}

std::string ConfigJobData::getJobType()
{
    reloadDataFromDB();

    return m_jobType;
}

std::string ConfigJobData::getDescription()
{
    reloadDataFromDB();

    return m_description;
}

bool ConfigJobData::getInhibitionStatus()
{
    reloadDataFromDB();

    return m_isInhibited;
}


unsigned long ConfigJobData::getLocationID()
{
    reloadDataFromDB();

    return m_locationID;
}


unsigned long ConfigJobData::getProfileID()
{
    reloadDataFromDB();

    return m_profileID;
}


IConfigScheduleData* ConfigJobData::getSchedule()
{

    return m_schedule;
}


std::string ConfigJobData::getParameter(const std::string& parameterName)
{
    FUNCTION_ENTRY("getParameter");
    reloadDataFromDB();

    //
    // Find the name/value pair.
    //
    IJobData::ParameterMap::iterator it = m_parameters.find(parameterName);
    if( it == m_parameters.end() )
    {
        std::string message = "Could not find parameter ";
        message            += parameterName;
        message            += " in database.";

        TA_THROW( DataException(message.c_str(),
                                DataException::NO_VALUE,
                                PARAMETERS_COLUMN)
                );
    }

    //
    // Return the value.
    //
    return (*it).second;
}


IJobData::ParameterMap ConfigJobData::getParameters()
{
    reloadDataFromDB();

    return m_parameters;
}


void ConfigJobData::setId(const std::string& id)
{
    m_id = id;
    reloadDataFromDB( true );
}

void ConfigJobData::setOrderId(unsigned long orderId)
{
    m_orderId = orderId;
    reloadDataFromDB( true );
}

void ConfigJobData::setJobType( const std::string& jobType)
{
    m_jobType = jobType;
    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigJobData::setDescription(const std::string& description)
{
    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Set operation called on deleted object");
    
    m_description = description;

    m_dataNeedsToBeWrittenToDB = true;
}


void ConfigJobData::setInhibitionStatus(bool isInhibited)
{
    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Set operation called on deleted object");
    
    m_isInhibited = isInhibited;

    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigJobData::setLocationID(unsigned long locationID)
{
    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Set operation called on deleted object");
    
    m_locationID = locationID;

    m_dataNeedsToBeWrittenToDB = true;
}


void ConfigJobData::setProfileID(unsigned long profileID)
{
    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Set operation called on deleted object");
    
    m_profileID = profileID;

    m_dataNeedsToBeWrittenToDB = true;
}


void ConfigJobData::setParameter(
    const std::string& parameterName,
    const std::string& parameterValue)
{
    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Set operation called on deleted object");

    TA_CONFIGJOBDATA_EXCEPTION( parameterName.find(PARAMETER_SEPARATOR) == std::string::npos,
               "Parameter name must not contain parameter separator");

    TA_CONFIGJOBDATA_EXCEPTION( parameterValue.find(PARAMETER_SEPARATOR) == std::string::npos,
               "Parameter value must not contain parameter separator");

    m_parameters[parameterName] = parameterValue;

    m_dataNeedsToBeWrittenToDB = true;
}

void ConfigJobData::deleteParameter(
    const std::string& parameterName)
{
    TA_CONFIGJOBDATA_EXCEPTION(!m_isDeleted, "Set operation called on deleted object");

    TA_CONFIGJOBDATA_EXCEPTION( parameterName.find(PARAMETER_SEPARATOR) == std::string::npos,
               "Parameter name must not contain parameter separator");

    m_parameters.erase(parameterName);

    m_dataNeedsToBeWrittenToDB = true;
}

std::string ConfigJobData::getLocalDatabaseName()
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

           /* DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
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

