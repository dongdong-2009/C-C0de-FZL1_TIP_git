/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReportHelper.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * EquipReportHelper is an object that is held by all type-specific equipment report objects, as well as by
  * the ConfigLocation. It contains all data common across locations, and manipulation
  * methods for the data. It helps avoid re-writing code for each location object.
  *
  */

#if !defined(EquipReportHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
#define EquipReportHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_


#include <string>
#include <map>
#include <ctime>
#include <vector>

#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{

    class EquipReportHelper
    {
	public:

		void applyChanges();
		void deleteThisEquipReport();
		void invalidate();
		
		// set methods for various fields
		void setReportTypeFilterFlag(const bool flag);
		void setReportTypeFilter(const std::string &reportFilter);
		void setAssetFilterFlag(const bool flag);
		void setAssetFilter(const std::string& assetFilter);
		void setDescFilterFlag(const bool flag);
		void setDescFilter(const std::string& descFilter);
		void setDPNameFilterFlag(const bool flag);
		void setDPNameFilter(const std::string& dpNameFilter);
		void setDPTypeFilterFlag(const bool flag);
		void setDPTypeFilter(const std::string& dpTypeFilter);
		void setDPValueFilterFlag(const bool flag);
		void setDPValueFilter(const std::string& dpValueFilter);
		
		void setStatusFilterFlag(const bool flag);
		void setScanInhibitFlag(const bool flag);
		void setAlarmInhibitFlag(const bool flag);
		void setControlInhibitFlag(const bool flag);
		void setMmsInhibitFlag(const bool flag);
		void setTaggedFlag(const bool flag);
		void setTaggedPTWFlag(const bool flag);
		void setOverriddenFlag(const bool flag);

		void setStatusChangeByFilterFlag(const bool flag);
		void setStatusChangeByFilter(const std::string& statusFilter);
		void setTimestampFilterFlag(const bool flag);
		void setTimestampFilter(const std::string& timestampFilter);
		
		void setName(const std::string& name);
		void setLocationId(const unsigned long locationId);
		void setProfile(const unsigned long profile);
		
		// get methods for various fields
		std::string		getReportTypeFilter();
		bool			getReportTypeFilterFlag();
		std::string		getAssetFilter();
		bool			getAssetFilterFlag();
		std::string		getDescFilter();
		bool			getDescFilterFlag();
		std::string		getDPNameFilter();
		bool			getDPNameFilterFlag();
		std::string		getDPTypeFilter();
		bool			getDPTypeFilterFlag();
		std::string		getDPValueFilter();
		bool			getDPValueFilterFlag();
		
		bool			getStatusFilterFlag();
		bool			getScanInhibitFlag();
		bool			getAlarmInhibitFlag();
		bool			getControlInhibitFlag();
		bool			getMmsInhibitFlag();
		bool			getTaggedFlag();
		bool			getTaggedPTWFlag();
		bool			getOverriddenFlag();
		
		std::string		getStatusChangeByFilter();
		bool			getStatusChangeByFilterFlag();
		std::string		getTimestampFilter();
		bool			getTimestampFilterFlag();
		
		std::string		getName();
		unsigned long	getKey();
		unsigned long	getLocationId();
		unsigned long	getProfile();
	    std::string     getId();

		virtual bool isNew() const { return m_isNew; };

		// constructors
		EquipReportHelper();
		EquipReportHelper(const EquipReportHelper& theEquipReport);
        EquipReportHelper(std::string& key);
		EquipReportHelper(std::string&  key, const std::string& name, const unsigned long locationId);


	    /**
	      * EquipReportHelper
	      * 
	      * This constructor creates a new object using the input data
          * which is representative of a row returned from SQL statement
	      * 
	      * @param : const unsigned long row
          *          - the row of data in the data object that we should collect data from
          *          in order to construct ourselves
	      * @param : TA_Base_Core::IData& data
          *          - the IData interface that should have been obtained using the 
          *          getBasicQueryData function
	      * 
	      * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
	      */		
		EquipReportHelper(const unsigned long row, TA_Base_Core::IData& data);

		// destructor
		~EquipReportHelper() {};

	private:
		void modifyExistingEquipReport();	
		void addNewEquipReport();

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this action was initially loaded from the database
         *      OR     - writeProfile() has already been called
         */
        void reload();

        /**
          * reloadUsing
          *
          * Fills out the local members with data contained in the input data object
          *
          * @param : const unsigned long row - the row to query in the data object
          * @param : TA_Base_Core::IData& data - the IData interface that should 
          *          have been obtained using the getBasicQueryData function
          *         
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
          */
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data);
		
        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();
		
		std::string 	m_Key;				// primary key
		std::string		m_Name;				// name of the report
		unsigned long	m_LocationId;		// id of the location for this report
		unsigned long	m_Profile;			// profile of the location for this report

		bool			m_ReportTypeFilterFlag;
		std::string		m_ReportTypeFilter;
		bool			m_AssetFilterFlag;	// asset filter flag
		std::string		m_AssetFilter;		// asset filter string
		bool			m_DescFilterFlag;	// description filter
		std::string		m_DescFilter;
		bool			m_DPNameFilterFlag;	// data point name
		std::string		m_DPNameFilter;
		bool			m_DPTypeFilterFlag;	// data point type
		std::string		m_DPTypeFilter;		
		bool			m_DPValueFilterFlag;// data point value
		std::string		m_DPValueFilter;
		
        std::string m_localDatabase;//database name

		bool			m_StatusFilterFlag;	// status 
		bool			m_ScanInhibitFlag;
		bool			m_AlarmInhibitFlag;
		bool			m_ControlInhibitFlag;
		bool			m_MmsInhibitFlag;
		bool			m_TaggedFlag;
		bool			m_TaggedPTWFlag;
		bool			m_OverriddenFlag;

		bool			m_StatusChangeByFilterFlag;	// status 
		std::string		m_StatusChangeByFilter;
		bool			m_TimestampFilterFlag; // timestamp
		std::string		m_TimestampFilter;	

		bool			m_isNew;
		bool			m_isValidData;

	};

} // TA_Base_Core

#endif // !defined(EquipReportHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
