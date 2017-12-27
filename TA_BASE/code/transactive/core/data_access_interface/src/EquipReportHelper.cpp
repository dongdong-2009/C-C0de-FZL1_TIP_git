/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReportHelper.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#endif

#include <vector>

#include "core/data_access_interface/src/EquipReportHelper.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IEquipReport.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/RunParams.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	// the spaces under Filters are a TEMP FIX due to an issue with the database strings
    EquipReportHelper::EquipReportHelper() 
        : m_Key(""),
		  m_Name(""),
		  m_LocationId(0),
		  m_Profile(0),
		  m_ReportTypeFilter("Equipment and DataPoints"), m_ReportTypeFilterFlag(false),
		  m_AssetFilter(""), m_AssetFilterFlag(false),
		  m_DescFilter(""), m_DescFilterFlag(false),
		  m_DPNameFilter(""), m_DPNameFilterFlag(false),
		  m_DPTypeFilter(""), m_DPTypeFilterFlag(false),
		  m_DPValueFilter(""), m_DPValueFilterFlag(false),
		  m_StatusFilterFlag(false),
		  m_ScanInhibitFlag(false),
		  m_AlarmInhibitFlag(false),
		  m_ControlInhibitFlag(false),
		  m_MmsInhibitFlag(false),
		  m_TaggedFlag(false),
		  m_TaggedPTWFlag(false),
		  m_OverriddenFlag(false),
		  m_StatusChangeByFilter(""), m_StatusChangeByFilterFlag(false),
		  m_TimestampFilter(""), m_TimestampFilterFlag(false),
          m_isValidData(false),
          m_isNew(true) 
    {
        m_Key = TA_Base_Core::TA_Uuid::generateUUID();
    }

	EquipReportHelper::EquipReportHelper(std::string& key) 
        : m_Key(key),
		  m_Name(""),
		  m_LocationId(0),
		  m_Profile(0),
		  m_ReportTypeFilter("Equipment and DataPoints"), m_ReportTypeFilterFlag(false),
		  m_AssetFilter(""), m_AssetFilterFlag(false),
		  m_DescFilter(""), m_DescFilterFlag(false),
		  m_DPNameFilter(""), m_DPNameFilterFlag(false),
		  m_DPTypeFilter(""), m_DPTypeFilterFlag(false),
		  m_DPValueFilter(""), m_DPValueFilterFlag(false),
		  m_StatusFilterFlag(false),		  
		  m_ScanInhibitFlag(false),
		  m_AlarmInhibitFlag(false),
		  m_ControlInhibitFlag(false),
		  m_MmsInhibitFlag(false),
		  m_TaggedFlag(false),
		  m_TaggedPTWFlag(false),
		  m_OverriddenFlag(false),
		  m_StatusChangeByFilter(""), m_StatusChangeByFilterFlag(false),
		  m_TimestampFilter(""), m_TimestampFilterFlag(false),
          m_isValidData(false),
          m_isNew(false) {}

	EquipReportHelper::EquipReportHelper(std::string& key, const std::string& name, const unsigned long locationId)
        : m_Key(key),          
		  m_Name(name),
          m_isValidData(true),
		  m_LocationId(locationId),
		  m_Profile(0),
		  m_ReportTypeFilter("Equipment and DataPoints"), m_ReportTypeFilterFlag(false),
		  m_AssetFilter(""), m_AssetFilterFlag(false),
		  m_DescFilter(""), m_DescFilterFlag(false),
		  m_DPNameFilter(""), m_DPNameFilterFlag(false),
		  m_DPTypeFilter(""), m_DPTypeFilterFlag(false),
		  m_DPValueFilter(""), m_DPValueFilterFlag(false),
		  m_StatusFilterFlag(false),		  
		  m_ScanInhibitFlag(false),
		  m_AlarmInhibitFlag(false),
		  m_ControlInhibitFlag(false),
		  m_MmsInhibitFlag(false),
		  m_TaggedFlag(false),
		  m_TaggedPTWFlag(false),
		  m_OverriddenFlag(false),
		  m_StatusChangeByFilter(""), m_StatusChangeByFilterFlag(false),
		  m_TimestampFilter(""), m_TimestampFilterFlag(false),
          m_isNew(false) {}

    EquipReportHelper::EquipReportHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isNew(false),
        m_isValidData(false)
    {
        FUNCTION_ENTRY("EquipReportHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }

	EquipReportHelper::EquipReportHelper(const EquipReportHelper& theEquipReportToCopy)
	{
	 	m_Key = TA_Base_Core::TA_Uuid::generateUUID();
		m_Name = theEquipReportToCopy.m_Name;
		m_LocationId = theEquipReportToCopy.m_LocationId;
		m_Profile = theEquipReportToCopy.m_Profile;
        m_isValidData = theEquipReportToCopy.m_isValidData;
		m_ReportTypeFilter = theEquipReportToCopy.m_ReportTypeFilter;
		m_ReportTypeFilterFlag = theEquipReportToCopy.m_ReportTypeFilterFlag;
		m_AssetFilter = theEquipReportToCopy.m_AssetFilter;
		m_AssetFilterFlag = theEquipReportToCopy.m_AssetFilterFlag;
		m_DescFilter = theEquipReportToCopy.m_DescFilter;
		m_DescFilterFlag = theEquipReportToCopy.m_DescFilterFlag;
		m_DPNameFilter = theEquipReportToCopy.m_DPNameFilter;
		m_DPNameFilterFlag = theEquipReportToCopy.m_DPNameFilterFlag;
		m_DPTypeFilter = theEquipReportToCopy.m_DPTypeFilter;
		m_DPTypeFilterFlag = theEquipReportToCopy.m_DPTypeFilterFlag;
		m_DPValueFilter = theEquipReportToCopy.m_DPValueFilter;
		m_DPValueFilterFlag = theEquipReportToCopy.m_DPValueFilterFlag;
		m_StatusFilterFlag = theEquipReportToCopy.m_StatusFilterFlag;
		m_ScanInhibitFlag = theEquipReportToCopy.m_ScanInhibitFlag;
		m_AlarmInhibitFlag = theEquipReportToCopy.m_AlarmInhibitFlag;
		m_ControlInhibitFlag = theEquipReportToCopy.m_ControlInhibitFlag;
		m_MmsInhibitFlag = theEquipReportToCopy.m_MmsInhibitFlag;
		m_TaggedFlag = theEquipReportToCopy.m_TaggedFlag;
		m_TaggedPTWFlag = theEquipReportToCopy.m_TaggedPTWFlag;
		m_OverriddenFlag = theEquipReportToCopy.m_OverriddenFlag;
		m_StatusChangeByFilter = theEquipReportToCopy.m_StatusChangeByFilter;
		m_StatusChangeByFilterFlag = theEquipReportToCopy.m_StatusChangeByFilterFlag;
		m_TimestampFilter = theEquipReportToCopy.m_TimestampFilter;
		m_TimestampFilterFlag = theEquipReportToCopy.m_TimestampFilterFlag;
        m_isNew = true;
    }

	void EquipReportHelper::setName(const std::string& Name)
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_Name = Name;
	}

	// set asset filter
	void EquipReportHelper::setAssetFilterFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_AssetFilterFlag = flag;
	}

	void EquipReportHelper::setAssetFilter(const std::string &assetFilter)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_AssetFilter = assetFilter;
	}

	void EquipReportHelper::setReportTypeFilterFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_ReportTypeFilterFlag = flag;
	}

	void EquipReportHelper::setReportTypeFilter(const std::string &reportFilter)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_ReportTypeFilter = reportFilter;
	}


	// set description filter
	void EquipReportHelper::setDescFilterFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_DescFilterFlag = flag;
	}

	void EquipReportHelper::setDescFilter(const std::string &descFilter)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_DescFilter = descFilter;

	}

	// set data point name filter
	void EquipReportHelper::setDPNameFilterFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_DPNameFilterFlag = flag;
	}

	void EquipReportHelper::setDPNameFilter(const std::string &dpNameFilter)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_DPNameFilter = dpNameFilter;

	}

	// set data point type filter
	void EquipReportHelper::setDPTypeFilterFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_DPTypeFilterFlag = flag;
	}

	void EquipReportHelper::setDPTypeFilter(const std::string &dpTypeFilter)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_DPTypeFilter = dpTypeFilter;

	}

	// set data point value filter
	void EquipReportHelper::setDPValueFilterFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_DPValueFilterFlag = flag;
	}

	void EquipReportHelper::setDPValueFilter(const std::string &dpValueFilter)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_DPValueFilter = dpValueFilter;

	}

	// set status filter
	void EquipReportHelper::setStatusFilterFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_StatusFilterFlag = flag;
	}

	void EquipReportHelper::setAlarmInhibitFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_AlarmInhibitFlag = flag;
	}

	void EquipReportHelper::setScanInhibitFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_ScanInhibitFlag = flag;
	}

	void EquipReportHelper::setControlInhibitFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_ControlInhibitFlag = flag;
	}

	void EquipReportHelper::setMmsInhibitFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_MmsInhibitFlag = flag;
	}

	void EquipReportHelper::setTaggedFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_TaggedFlag = flag;
	}

	void EquipReportHelper::setTaggedPTWFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_TaggedPTWFlag = flag;
	}

	void EquipReportHelper::setOverriddenFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_OverriddenFlag = flag;
	}

	// set status change by filter
	void EquipReportHelper::setStatusChangeByFilterFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_StatusChangeByFilterFlag = flag;
	}

	void EquipReportHelper::setStatusChangeByFilter(const std::string &statusFilter)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_StatusChangeByFilter = statusFilter;
	}

	// set timestamp filter
	void EquipReportHelper::setTimestampFilterFlag(const bool flag)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_TimestampFilterFlag = flag;
	}

	void EquipReportHelper::setTimestampFilter(const std::string &timestampFilter)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		m_TimestampFilter = timestampFilter;

	}

	void EquipReportHelper::setLocationId(const unsigned long locationId)
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_LocationId = locationId;
	}

	void EquipReportHelper::setProfile(const unsigned long profile)
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_Profile = profile;
	}

	void EquipReportHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

		columnNames.push_back("SCEREP_ID");
        columnNames.push_back("REPORT_NAME");
        columnNames.push_back("LOCATION_ID");
		columnNames.push_back("PROFILE");
		columnNames.push_back("REPORT_TYPE_FILTER_FLAG");
		columnNames.push_back("REPORT_TYPE_FILTER");
		columnNames.push_back("ASSET_FILTER_FLAG");
		columnNames.push_back("ASSET_FILTER");
		columnNames.push_back("DESC_FILTER_FLAG");
		columnNames.push_back("DESC_FILTER");
		columnNames.push_back("DPNAME_FILTER_FLAG");
		columnNames.push_back("DPNAME_FILTER");
		columnNames.push_back("DPTYPE_FILTER_FLAG");
		columnNames.push_back("DPTYPE_FILTER");
		columnNames.push_back("DPVALUE_FILTER_FLAG");
		columnNames.push_back("DPVALUE_FILTER");
		columnNames.push_back("STATUS_FILTER_FLAG");
		columnNames.push_back("SCAN_INHIBIT_FLAG");
		columnNames.push_back("ALARM_INHIBIT_FLAG");
		columnNames.push_back("CONTROL_INHIBIT_FLAG");
		columnNames.push_back("MMS_INHIBIT_FLAG");
		columnNames.push_back("TAGGED_FLAG");
		columnNames.push_back("TAGGED_PTW_FLAG");
		columnNames.push_back("OVERRIDDEN_FLAG");
		columnNames.push_back("STATUS_CHANGE_BY_FILTER_FLAG");
		columnNames.push_back("STATUS_CHANGE_BY_FILTER");
		columnNames.push_back("TIMESTAMP_FILTER_FLAG");
		columnNames.push_back("TIMESTAMP_FILTER");
	}

	void EquipReportHelper::reload()
	{
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);

		// set up the sql string
       /* std::ostringstream sql;
        sql << " select SCEREP_ID, REPORT_NAME, LOCATION_ID, PROFILE,";
		sql << " REPORT_TYPE_FILTER_FLAG, REPORT_TYPE_FILTER,";
		sql << " ASSET_FILTER_FLAG, ASSET_FILTER, DESC_FILTER_FLAG, DESC_FILTER,";
		sql << " DPNAME_FILTER_FLAG, DPNAME_FILTER, DPTYPE_FILTER_FLAG, DPTYPE_FILTER,";
		sql << " DPVALUE_FILTER_FLAG, DPVALUE_FILTER, STATUS_FILTER_FLAG,";
		sql << " SCAN_INHIBIT_FLAG, ALARM_INHIBIT_FLAG, CONTROL_INHIBIT_FLAG, MMS_INHIBIT_FLAG,";
		sql << " TAGGED_FLAG, TAGGED_PTW_FLAG, OVERRIDDEN_FLAG,";
		sql << " STATUS_CHANGE_BY_FILTER_FLAG, STATUS_CHANGE_BY_FILTER,";
		sql << " TIMESTAMP_FILTER_FLAG, TIMESTAMP_FILTER";
		sql << " from SC_EQUIP_REPORT";
		sql << " where SCEREP_ID = '" << m_Key << "' order by REPORT_NAME";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_STD_SELECT_14505, m_Key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_SELECT_14505, m_Key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_SELECT_14505, m_Key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data( databaseConnection->executeQuery( strSql, columnNames ) );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
            reasonMessage << "No equipment report found for pkey = " << m_Key;
			TA_THROW(DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of equipment report records."); 
        reloadUsing(0, *data);
	}

    void EquipReportHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_Key = data.getStringData( row, columnNames[0] );
        m_Name = data.getStringData( row, columnNames[1] );
        m_LocationId = data.getUnsignedLongData( row, columnNames[2] );
            
	    m_Profile = data.getUnsignedLongData( row, columnNames[3], 0);
		m_ReportTypeFilterFlag = data.getBooleanData( row, columnNames[4]);
        m_ReportTypeFilter = data.getStringData( row, columnNames[5]);
		m_AssetFilterFlag = data.getBooleanData( row, columnNames[6] );
        m_AssetFilter = data.getStringData( row, columnNames[7] );
		m_DescFilterFlag = data.getBooleanData( row, columnNames[8] );
		m_DescFilter = data.getStringData( row, columnNames[9] );
		m_DPNameFilterFlag = data.getBooleanData( row, columnNames[10] );
		m_DPNameFilter = data.getStringData( row, columnNames[11] );
		m_DPTypeFilterFlag = data.getBooleanData( row, columnNames[12] );
		m_DPTypeFilter = data.getStringData( row, columnNames[13] );
		m_DPValueFilterFlag = data.getBooleanData( row, columnNames[14] );
		m_DPValueFilter = data.getStringData( row, columnNames[15] );
		
		m_StatusFilterFlag = data.getBooleanData( row, columnNames[16] );
		m_ScanInhibitFlag = data.getBooleanData( row, columnNames[17] );
		m_AlarmInhibitFlag = data.getBooleanData( row, columnNames[18] );
		m_ControlInhibitFlag = data.getBooleanData( row, columnNames[19] );
		m_MmsInhibitFlag = data.getBooleanData( row, columnNames[20]);
		m_TaggedFlag = data.getBooleanData( row, columnNames[21] );
		m_TaggedPTWFlag = data.getBooleanData( row, columnNames[22] );
		m_OverriddenFlag = data.getBooleanData( row, columnNames[23] );

		m_StatusChangeByFilterFlag = data.getBooleanData( row, columnNames[24] );
		m_StatusChangeByFilter = data.getStringData( row, columnNames[25] );
		m_TimestampFilterFlag = data.getBooleanData( row, columnNames[26] );
		m_TimestampFilter = data.getStringData( row, columnNames[27] );

        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }	

	void EquipReportHelper::invalidate()
	{
	   FUNCTION_ENTRY("invalidate()");

		// This method cannot be called until the key has been set
		TA_ASSERT(!m_isNew,"Attempted to call Invalidate() on a new entity");

		m_isValidData = false;

		FUNCTION_EXIT;
	}

	void EquipReportHelper::deleteThisEquipReport()
	{
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);
        
		// attempt to delete all equipment report data for this report as well
		/*std::ostringstream sql2;
        sql2 << "BEGIN ";
        sql2 << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		sql2 << "delete SC_EQUIP_REPORT_ENTITIES where SCEREP_ID = ''" << m_Key;
        sql2 << "'''); ";
        sql2 << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_Oracle_DELETE_15452, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_DELETE_15452, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_DELETE_15452,
			getLocalDatabaseName(), m_Key);

		databaseConnection->executeModification(strSql);
		
		// attempt to delete the equipment report
		/*std::ostringstream sql;
        sql << "BEGIN ";
        sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		sql << "delete SC_EQUIP_REPORT where SCEREP_ID = ''" << m_Key;
        sql << "'''); ";
        sql << "END;";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_Oracle_DELETE_14951,
//		strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_DELETE_14951,

		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_DELETE_14951,
			getLocalDatabaseName(), m_Key);

		databaseConnection->executeModification(strSql);
	}

	void EquipReportHelper::applyChanges()
	{
		FUNCTION_ENTRY("ApplyChanges()");

        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.

        std::vector<std::string> fieldNames;
        if (m_Name.empty())
        {
            fieldNames.push_back("REPORT_NAME");
        }
		if (m_LocationId == 0)
		{
			fieldNames.push_back("LOCATION_ID");
		}
        
		// Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("Report data not fully specified. Report cannot be written to database" ,fieldNames) );
        }

        // Write the report to the database

        // Need to check if this is a new entity or an existing entity - 
        // as a new entity will be inserted, while an existing entity will be updated
        if (m_isNew)
        {
             // This is a new entity
            addNewEquipReport();
        }
        else 
        {
            // This is an existing entity
            modifyExistingEquipReport();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
	}



	void EquipReportHelper::addNewEquipReport()
	{		
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);
        
        // First check if an Equipment Report exists with a specified name and at this particular location
		// we can have names which are the same as long as they are at different locations
        /*std::ostringstream formatSQL;
        formatSQL << "select SCEREP_ID from SC_EQUIP_REPORT where REPORT_NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_Name) << "' ";
		formatSQL << "and LOCATION_ID = " << m_LocationId;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_STD_SELECT_14506,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_SELECT_14506,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_SELECT_14506,
			databaseConnection->escapeQueryString(m_Name), m_LocationId);

        std::string keyColumn = "SCEREP_ID";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // There should be 0 rows returned, otherwise a record exists and cannot be added
        if (0 != data->getNumRows()) 
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "An Equipment Report already exists with a name = " << m_Name.c_str() << " at location " << m_LocationId;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"ReportName") );
        }

        delete data;
        data = NULL;

        // now we can write the equipment report data to the database
		/*formatSQL.str("");
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
        formatSQL << "insert into SC_EQUIP_REPORT (SCEREP_ID, REPORT_NAME, LOCATION_ID, PROFILE, ";
		formatSQL << "REPORT_TYPE_FILTER_FLAG, REPORT_TYPE_FILTER, ";
		formatSQL << "ASSET_FILTER_FLAG, ASSET_FILTER, DESC_FILTER_FLAG, DESC_FILTER, ";
		formatSQL << "DPNAME_FILTER_FLAG, DPNAME_FILTER, DPTYPE_FILTER_FLAG, DPTYPE_FILTER, ";
		formatSQL << "DPVALUE_FILTER_FLAG, DPVALUE_FILTER, STATUS_FILTER_FLAG, ";
		formatSQL << "SCAN_INHIBIT_FLAG, ALARM_INHIBIT_FLAG, CONTROL_INHIBIT_FLAG, MMS_INHIBIT_FLAG, ";
		formatSQL << "TAGGED_FLAG, TAGGED_PTW_FLAG, OVERRIDDEN_FLAG, ";
		formatSQL << "STATUS_CHANGE_BY_FILTER_FLAG, STATUS_CHANGE_BY_FILTER, ";
		formatSQL << "TIMESTAMP_FILTER_FLAG, TIMESTAMP_FILTER) ";
        formatSQL << "values ( ''" <<m_Key << "'' ,''";
		formatSQL << databaseConnection->escapeAQSQLString(m_Name) << "'',";
		formatSQL << m_LocationId << ", ";
		formatSQL << m_Profile << ", ";
		formatSQL << m_ReportTypeFilterFlag << ",''" << databaseConnection->escapeAQSQLString(m_ReportTypeFilter) << "'', ";
		formatSQL << m_AssetFilterFlag << ",''" << databaseConnection->escapeAQSQLString(m_AssetFilter) << "'', ";
		formatSQL << m_DescFilterFlag << ",''" << databaseConnection->escapeAQSQLString(m_DescFilter) << "'', ";
		formatSQL << m_DPNameFilterFlag << ",''" << databaseConnection->escapeAQSQLString(m_DPNameFilter) << "'', ";
		formatSQL << m_DPTypeFilterFlag << ",''" << databaseConnection->escapeAQSQLString(m_DPTypeFilter) << "'', ";
		formatSQL << m_DPValueFilterFlag << ",''" << databaseConnection->escapeAQSQLString(m_DPValueFilter) << "'', ";
		formatSQL << m_StatusFilterFlag << ",";
		formatSQL << m_ScanInhibitFlag << ",";
		formatSQL << m_AlarmInhibitFlag << ",";
		formatSQL << m_ControlInhibitFlag << ",";
		formatSQL << m_MmsInhibitFlag << ",";
		formatSQL << m_TaggedFlag << ",";
		formatSQL << m_TaggedPTWFlag << ",";
		formatSQL << m_OverriddenFlag << ",";
		formatSQL << m_StatusChangeByFilterFlag << ",''" << databaseConnection->escapeAQSQLString(m_StatusChangeByFilter) << "'', ";
		formatSQL << m_TimestampFilterFlag << ",''" << databaseConnection->escapeAQSQLString(m_TimestampFilter) << "'')"; 
        formatSQL << "'); ";
        formatSQL << "END;";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_Oracle_INSERT_14851, getLocalDatabaseName(), m_Key,
//		strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_INSERT_14851, getLocalDatabaseName(), m_Key,

		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_INSERT_14851, getLocalDatabaseName(), m_Key,
			databaseConnection->escapeAQSQLString(m_Name), m_LocationId, m_Profile, m_ReportTypeFilterFlag, databaseConnection->escapeAQSQLString(m_ReportTypeFilter),
			m_AssetFilterFlag, databaseConnection->escapeAQSQLString(m_AssetFilter), m_DescFilterFlag, databaseConnection->escapeAQSQLString(m_DescFilter),
			m_DPNameFilterFlag, databaseConnection->escapeAQSQLString(m_DPNameFilter), m_DPTypeFilterFlag, databaseConnection->escapeAQSQLString(m_DPTypeFilter),
			m_DPValueFilterFlag, databaseConnection->escapeAQSQLString(m_DPValueFilter), m_StatusFilterFlag, m_ScanInhibitFlag, m_AlarmInhibitFlag,
			m_ControlInhibitFlag, m_MmsInhibitFlag, m_TaggedFlag, m_TaggedPTWFlag, m_OverriddenFlag, m_StatusChangeByFilterFlag,databaseConnection->escapeAQSQLString(m_StatusChangeByFilter),
			m_TimestampFilterFlag, databaseConnection->escapeAQSQLString(m_TimestampFilter) );
		
        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this report and update the date fields
        /*std::ostringstream sql;
		sql << "select SCEREP_ID from SC_EQUIP_REPORT where REPORT_NAME = '"
			<< databaseConnection->escapeQueryString(m_Name).c_str() << "' "
			<< "and LOCATION_ID = " << m_LocationId;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_STD_SELECT_14506,
//		strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_SELECT_14506,

		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_SELECT_14506,
			databaseConnection->escapeQueryString(m_Name), m_LocationId );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for name = " << m_Name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for name = " << m_Name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_Key = data->getStringData(0,keyColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

	}

	void EquipReportHelper::modifyExistingEquipReport()
	{
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

		// now lets get the equipment report in question
        /*std::ostringstream formatSQL;
		formatSQL.str("");
		formatSQL << "select SCEREP_ID from SC_EQUIP_REPORT where REPORT_NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_Name);
		formatSQL << "' and LOCATION_ID = " << m_LocationId;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_STD_SELECT_14506,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_SELECT_14506,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_SELECT_14506,
			databaseConnection->escapeQueryString(m_Name), m_LocationId);

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "SCEREP_ID";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        // check if a record exists in the database
        if (1 == data->getNumRows()) // the record was found
        {
            if (m_Key != data->getStringData(0,keyColumn)) // check if the keys are the same
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A report already exists with name = " << m_Name.c_str();         
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"ReportName") );
            }
            // else - it matches, so that's okay - it means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one report already exists name = " << m_Name.c_str();            
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"ReportName") );
        }

        delete data;
        data =  NULL;

		// update the record
        /*std::ostringstream sql;

        sql << "BEGIN ";
        sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
        sql << "update SC_EQUIP_REPORT set REPORT_NAME = ''" << (databaseConnection->escapeAQSQLString(m_Name)).c_str() << "'', ";
		sql << "REPORT_TYPE_FILTER_FLAG = " << m_ReportTypeFilterFlag << ", ";
		sql << "REPORT_TYPE_FILTER = ''" << databaseConnection->escapeAQSQLString(m_ReportTypeFilter) << "'', ";
		sql << "ASSET_FILTER_FLAG = " << m_AssetFilterFlag << ", ";
		sql << "ASSET_FILTER = ''" << databaseConnection->escapeAQSQLString(m_AssetFilter) << "'', ";
		sql << "DESC_FILTER_FLAG = " << m_DescFilterFlag << ", ";
		sql << "DESC_FILTER = ''" << databaseConnection->escapeAQSQLString(m_DescFilter) << "'', ";
		sql << "DPNAME_FILTER_FLAG = " << m_DPNameFilterFlag << ", ";
		sql << "DPNAME_FILTER = ''" << databaseConnection->escapeAQSQLString(m_DPNameFilter) << "'', ";
		sql << "DPTYPE_FILTER_FLAG = " << m_DPTypeFilterFlag << ", ";
		sql << "DPTYPE_FILTER = ''" << databaseConnection->escapeAQSQLString(m_DPTypeFilter) << "'', ";
		sql << "DPVALUE_FILTER_FLAG = " << m_DPValueFilterFlag << ", ";
		sql << "DPVALUE_FILTER = ''" << databaseConnection->escapeAQSQLString(m_DPValueFilter) << "'', ";
		sql << "STATUS_FILTER_FLAG = " << m_StatusFilterFlag << ", ";
		sql << "SCAN_INHIBIT_FLAG = " << m_ScanInhibitFlag << ", ";
		sql << "ALARM_INHIBIT_FLAG = " << m_AlarmInhibitFlag << ", ";
		sql << "CONTROL_INHIBIT_FLAG = " << m_ControlInhibitFlag << ", ";
		sql << "MMS_INHIBIT_FLAG = " << m_MmsInhibitFlag << ", ";
		sql << "TAGGED_FLAG = " << m_TaggedFlag << ", ";
		sql << "TAGGED_PTW_FLAG = " << m_TaggedPTWFlag << ", ";
		sql << "OVERRIDDEN_FLAG = " << m_OverriddenFlag << ", ";
		sql << "STATUS_CHANGE_BY_FILTER_FLAG = " << m_StatusChangeByFilterFlag << ", ";
		sql << "STATUS_CHANGE_BY_FILTER = ''" << databaseConnection->escapeAQSQLString(m_StatusChangeByFilter) << "'', ";
		sql << "TIMESTAMP_FILTER_FLAG = " << m_TimestampFilterFlag << ", ";
		sql << "TIMESTAMP_FILTER = ''" << databaseConnection->escapeAQSQLString(m_TimestampFilter) << "'', ";
		sql << "PROFILE = " << m_Profile << " ";
        sql << " where SCEREP_ID = ''" << m_Key << "''";
        sql << "'); ";
        sql << "END;";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_Oracle_UPDATE_14701, getLocalDatabaseName(), 
//		strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_UPDATE_14701, getLocalDatabaseName(), 

		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_UPDATE_14701, getLocalDatabaseName(),
			databaseConnection->escapeAQSQLString(m_Name), m_ReportTypeFilterFlag, databaseConnection->escapeAQSQLString(m_ReportTypeFilter),
			m_AssetFilterFlag, databaseConnection->escapeAQSQLString(m_AssetFilter), m_DescFilterFlag, databaseConnection->escapeAQSQLString(m_DescFilter),
			m_DPNameFilterFlag, databaseConnection->escapeAQSQLString(m_DPNameFilter), m_DPTypeFilterFlag, databaseConnection->escapeAQSQLString(m_DPTypeFilter),
			m_DPValueFilterFlag, databaseConnection->escapeAQSQLString(m_DPValueFilter), m_StatusFilterFlag, m_ScanInhibitFlag, m_AlarmInhibitFlag,
			m_ControlInhibitFlag, m_MmsInhibitFlag, m_TaggedFlag, m_TaggedPTWFlag, m_OverriddenFlag, m_StatusChangeByFilterFlag, databaseConnection->escapeAQSQLString(m_StatusChangeByFilter),
			m_TimestampFilterFlag, databaseConnection->escapeAQSQLString(m_TimestampFilter), m_Profile, m_Key);

        databaseConnection->executeModification(strSql);
	}

	std::string EquipReportHelper::getName()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_Name;
	}

	bool EquipReportHelper::getAssetFilterFlag()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_AssetFilterFlag;
	}

	std::string EquipReportHelper::getAssetFilter()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_AssetFilter;
	}
	
	bool EquipReportHelper::getReportTypeFilterFlag()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_ReportTypeFilterFlag;
	}

	std::string EquipReportHelper::getReportTypeFilter()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_ReportTypeFilter;
	}


	bool EquipReportHelper::getDescFilterFlag()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_DescFilterFlag;
	}

	std::string EquipReportHelper::getDescFilter()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_DescFilter;
	}

	bool EquipReportHelper::getDPNameFilterFlag()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_DPNameFilterFlag;
	}

	std::string EquipReportHelper::getDPNameFilter()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_DPNameFilter;
	}

	bool EquipReportHelper::getDPTypeFilterFlag()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_DPTypeFilterFlag;
	}

	std::string EquipReportHelper::getDPTypeFilter()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_DPTypeFilter;
	}

	bool EquipReportHelper::getDPValueFilterFlag()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_DPValueFilterFlag;
	}

	std::string EquipReportHelper::getDPValueFilter()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_DPValueFilter;
	}

	bool EquipReportHelper::getStatusFilterFlag()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_StatusFilterFlag;
	}

	bool EquipReportHelper::getScanInhibitFlag()
	{		
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_ScanInhibitFlag;
	}

	bool EquipReportHelper::getAlarmInhibitFlag()
	{		
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_AlarmInhibitFlag;
	}

	bool EquipReportHelper::getControlInhibitFlag()
	{		
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_ControlInhibitFlag;
	}

	bool EquipReportHelper::getMmsInhibitFlag()
	{		
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_MmsInhibitFlag;
	}

	bool EquipReportHelper::getTaggedFlag()
	{		
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_TaggedFlag;
	}

	bool EquipReportHelper::getTaggedPTWFlag()
	{		
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_TaggedPTWFlag;
	}

	bool EquipReportHelper::getOverriddenFlag()
	{		
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_OverriddenFlag;
	}

	bool EquipReportHelper::getStatusChangeByFilterFlag()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_StatusChangeByFilterFlag;
	}

	std::string EquipReportHelper::getStatusChangeByFilter()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_StatusChangeByFilter;
	}

	bool EquipReportHelper::getTimestampFilterFlag()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_TimestampFilterFlag;
	}

	std::string EquipReportHelper::getTimestampFilter()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_TimestampFilter;
	}

	unsigned long EquipReportHelper::getLocationId()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_LocationId;
	}

	unsigned long EquipReportHelper::getProfile()
	{
		// We don't want to call reload() if this is a new report and it has not been written to the database
		if (!m_isNew)  
		{            
			// This is an existing report and the data needs to be loaded
			reload();
		}

		return m_Profile;
	}
	
	unsigned long EquipReportHelper::getKey()
	{
		return 0;
	}

	std::string EquipReportHelper::getId()
	{
		return m_Key;
	}

	std::string EquipReportHelper::getLocalDatabaseName()
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
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Scada_Cd, Write);

                /*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Scada_Cd, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

} // TA_Base_Core

