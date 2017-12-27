/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/IReportFile.h $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/11/16 18:23:22 $
  * Last modified by:  $Author: hui.wang $
  * 
  * IReportFile is an interface to an ReportFile object. It allows the ReportFile object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IReportFile_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IReportFile_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

namespace TA_Base_Core
{

    class IReportFile
    {

    public:
        virtual ~IReportFile() { };

        /**
         * getReportFileId
         *
         * Returns the report file ID for this REPORT_FILE entry.
         *
         * @return The File ID for this report as a unsigned long.
         */
        virtual unsigned long getReportFileId() = 0;

        /**
         * getReportFileName
         *
         * Returns the report file name for this REPORT_FILE entry.
		 * Display this file name on CTreeCtrl List
         *
         * @return The name for the report file as an string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, or if the wrong ammount
		 *			   of data is retrieved.
         */
		virtual std::string getReportFileName() =0;

        /**
         * getReportFilePhysicalPath
         *
         * Returns the physical path for the REPORT_FILE
         *
         * @return The physical path for the report file as an string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
		 * @exception DataException A DataException will be thrown if the data cannot be 
		 *            converted to the required format, or if the wrong ammount
		 *			   of data is retrieved.
         */
        virtual std::string getReportFilePhysicalPath() =0;

        /**
         * getReportFileDirectoryId
         *
         * Returns the directory ID of the report file 
         *
         * @return The ID for directory as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
		 * @exception DataException A DataException will be thrown if the data cannot be 
		 *            converted to the required format, or if the wrong ammount
		 *			   of data is retrieved.
         */
        virtual unsigned long getReportFileDirectoryId() = 0;

        /**
         * invalidate
         *
         * Make the data contained by this ReportFile as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() = 0;

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        virtual void applyChanges() = 0;		
};
} //close namespace TA_Base_Core

#endif // !defined(IReportFile_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
