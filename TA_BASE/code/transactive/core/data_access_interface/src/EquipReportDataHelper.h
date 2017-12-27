/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReportDataHelper.h $
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

#if !defined(AFX_EQUIPREPORTDATAHELPER_H__747F7AB1_C408_4194_849D_F6C3C97DA210__INCLUDED_)
#define AFX_EQUIPREPORTDATAHELPER_H__747F7AB1_C408_4194_849D_F6C3C97DA210__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
// database access related
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

// exception and debug related
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	class EquipReportDataHelper  
	{
	public:
		// get methods
		unsigned long getKey();
		unsigned long getDataPointKey();
		unsigned long getDataNodeKey();
        std::string   getId();
        std::string   getReportKey();
		
		// set methods
		void setDataPointKey(const unsigned long aDataPointKey);
		void setDataNodeKey(const unsigned long aDataNodeKey);
        void setReportKey(std::string& aReportKey);
		
		// support methods
		virtual bool isNew() const { return m_isNew; };
		void invalidate();
		void deleteThisEquipReportData();
		void applyChanges();

		// constructors
		EquipReportDataHelper();
		EquipReportDataHelper(const EquipReportDataHelper& theEquipReportData);
		EquipReportDataHelper(std::string& key);
		EquipReportDataHelper(std::string& key,  std::string& reportId);

	    /**
	      * EquipReportDataHelper
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
		EquipReportDataHelper(const unsigned long row, TA_Base_Core::IData& data);
		
		// destructor
		~EquipReportDataHelper() {};
	private:
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

		void addNewEquipReportData();
		void modifyExistingEquipReportData();
        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();
		
		unsigned long m_Key;
        std::string   m_stringId;
        std::string   m_ReportKey;
		unsigned long m_DataNodeKey;
		unsigned long m_DataPointKey;

		bool	m_isNew;
		bool	m_isValidData;
        
        std::string m_localDatabase;
	};

}

#endif // !defined(AFX_EQUIPREPORTDATAHELPER_H__747F7AB1_C408_4194_849D_F6C3C97DA210__INCLUDED_)
