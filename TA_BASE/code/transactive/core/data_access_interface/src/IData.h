/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IData.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IData is an interface to an object that provides data. The object represented by this
  * interface will contain code that is specific to a particular database system (e.g. oracle)
  * This object is responsible for interpreting the data return from the database and (if 
  * necessary) converting it to the specified data types.
  */

#if !defined(IData_6F37C686_5A1A_4f0e_85DB_C36A508A0467__INCLUDED_)
#define IData_6F37C686_5A1A_4f0e_85DB_C36A508A0467__INCLUDED_

#include <string>
#include <sys/timeb.h>

namespace TA_Base_Core
{
    class IData
    {

    public:

        /**
		 * getNumRows
		 * 
		 * This function returns the number of rows of data contained by this object.
		 *
		 * @returns the number of rows of data contained by this IData object in unsigned long format.
		 */
        virtual unsigned long getNumRows() =0;

        /**
		 * getIntegerData
		 *
		 * This function returns the data at the specified location as an integer.
		 *
		 * @param rowIndex The row number from which to retrieve the data. This MUST be less than or 
		 * equal to m_numRows. This is tested by an assert.
		 * @param fieldName The name of the column (field) from which to retrieve the data.
		 *
		 * @return The data is returned as an integer
		 *
		 * @exception DataException If the data cannot be converted to an integer, a DataException 
		 * is thrown. NB: The string retrieved by the sql statement MUST NOT contain any leading spaces, or
		 * the number will be considered a non-integer and an exception will be thrown.
		 *
		 * pre: rowIndex <= getNumRows()
		 */
        virtual int getIntegerData(const unsigned long rowIndex, const std::string& fieldName, const int defaultValue = 0) =0;

        /**
		 * getUnsignedLongData
		 * 
		 * This function returns the data at the specified location as an unsigned long
		 *
		 * @param rowIndex The row number from which to retrieve the data. This MUST be less than or 
		 * equal to m_numRows. This is tested by an assert.
		 * @param fieldName The name of the column (field) from which to retrieve the data.
		 *
		 * @return The data is retunred as an unsigned long
		 *
		 * @exception DataException If the data cannot be converted to an unsigned 
		 * long, a DataException is thrown. NB: The string retrieved by the sql statement 
		 * MUST NOT contain any leading spaces, or the number will be considered 
		 * invalid and an exception will be thrown.
		 *
		 * pre: rowIndex <= getNumRows()
		 */
        virtual unsigned long getUnsignedLongData(const unsigned long rowIndex, const std::string& fieldName, const unsigned long defaultValue = 0) =0;

        /**
		 * getRealData
		 * 
		 * This function returns the data at the specified location as a double
		 *
		 * @param rowIndex The row number from which to retrieve the data. This MUST be less than or 
		 * equal to m_numRows. This is tested by an assert.
		 * @param fieldName The name of the column (field) from which to retrieve the data.
		 *
		 * @return The data is retunred as a dobule
		 *
		 * @exception DataException If the data cannot be converted to a double 
		 * a DataException is thrown. NB: 1) The string retrieved by the sql statement 
		 * MUST NOT contain any leading spaces, or the number will be considered 
		 * invalid and an exception will be thrown. 2) The format of the real data retrived by
		 * the sql statement must be [-](0123456789)n[.(0123456789)n]. That is, a "-", if present, 
		 * must be in the first location, there must always be at least one numeric chracter, and 
		 * the decimal point, if present, must always be trailed by at least one numeric character.
		 * If the number differs from these specifications, an exception will be thrown.
		 *
		 * pre: rowIndex <= getNumRows()
		 */
        virtual double getRealData(const unsigned long rowIndex, const std::string& fieldName, const double defaultValue = 0.0)=0;

        /**
		 * getBooleanData
		 * 
		 * This function returns the data at the specified location as a boolean
		 *
		 * @param rowIndex The row number from which to retrieve the data. This MUST be less than or 
		 * equal to m_numRows. This is tested by an assert.
		 * @param fieldName The name of the column (field) from which to retrieve the data.
		 *
		 * @return The data is retunred as a boolean
		 *
		 * @exception DataException If the data cannot be converted to a boolean 
		 * a DataException is thrown. NB: 1) The string retrieved by the sql statement 
		 * MUST NOT contain any leading spaces, or the number will be considered 
		 * invalid and an exception will be thrown.
		 * If the number differs from these specifications, an exception will be thrown.
		 *
		 * pre: rowIndex <= getNumRows()
		 */
        virtual bool getBooleanData(const unsigned long rowIndex, const std::string& fieldName, const bool defaultValue = false) =0;

        /**
		 * getStringData
		 * 
		 * This function returns the data at the specified location as an unsigned long
		 *
		 * @param rowIndex The row number from which to retrieve the data. This MUST be less than or 
		 * equal to m_numRows. This is tested by an assert.
		 * @param fieldName The name of the column (field) from which to retrieve the data.
		 *
		 * @return The data is retunred as a string
		 *
		 * @exception DataException If the data cannot be converted to a string 
		 * a DataException is thrown.
		 *
		 * pre: rowIndex <= getNumRows()
		 */
        virtual std::string getStringData(const unsigned long rowIndex, const std::string& fieldName, const std::string defaultValue = "")=0;

        /**
		 * getDateData
		 * 
		 * This function returns the data at the specified location as a time_t
		 *
		 * @param rowIndex The row number from which to retrieve the data. This MUST be less than or 
		 * equal to m_numRows. This is tested by an assert.
		 * @param fieldName The name of the column (field) from which to retrieve the data.
		 *
		 * @return The data is retunred as a time_t
		 *
		 * @exception DataException If the data cannot be converted to a time_t 
		 * a DataException is thrown.
		 *
		 * pre: 1) To use this method the date MUST be selected from the database using 
         *         the sql statement select format: TO_CHAR([column],'YYYYMMDDHH24MMSS') 
         *         from [table];
		 *         An assert is used to test that the string is in the YYYYMMDDHH24MMSS 
         *         format.
		 *         NB: The day/month compatibility will not be tested by this routine, 
         *         and the mktime function will convert any invalid date to an equivalent 
         *         valid one (e.g. 29/02/03 will become 01/03/03). Note however that dates 
         *         greater than 31 will generate an exception.
		 *		2) rowIndex <= getNumRows()
		 */
        virtual time_t getDateData(const unsigned long rowIndex, const std::string& fieldName, const time_t defaultValue = 0)=0;

		/**
		 * getTimestampData
		 * 
		 * This function returns the data at the specified location as a timeb
		 *
		 * @param rowIndex The row number from which to retrieve the data. This MUST be less than or 
		 * equal to m_numRows. This is tested by an assert.
		 * @param fieldName The name of the column (field) from which to retrieve the data.
		 *
		 * @return The data is retunred as a timeb
		 *
		 * @exception DataException If the data cannot be converted to a timeb 
		 * a DataException is thrown.
		 *
		 * pre: 1) To use this method the date MUST be selected from the database using the sql
		 * statement select format: TO_CHAR([column],'YYYYMMDDHH24MMSSFF3') from [table];
		 * An assert is used to test that the string is in the YYYYMMDDHH24MMSSFF3 format.
		 * NB: The day/month compatibility will not be tested by this routine, and the mktime
		 * function will convert any invalid date to an equivalent valid one (e.g. 29/02/03 will
		 * become 01/03/03). Note however that dates greater than 31 will generate an exception.
		 *		2) rowIndex <= getNumRows()
		 */
        virtual timeb getTimestampData(const unsigned long rowIndex,const std::string& fieldName) =0;

        /**
		 * isNull
		 * 
		 * This function checks if the data at the specified location is NULL or an empty string.
		 *
		 * @param rowIndex The row number from which to retrieve the data. This MUST be less than or 
		 * equal to m_numRows. This is tested by an assert.
		 * @param fieldName The name of the column (field) from which to retrieve the data.
		 *
		 * @return true if the field is null, and false otherwise.
		 *
		 * pre: rowIndex <= getNumRows()
		 */
        virtual bool isNull(const unsigned long rowIndex, const std::string& fieldName) =0;
		
		/**
	      * getNullTime
	      * 
	      * Return the seconds from 01/01/1990 12:00:00
	      */
		virtual time_t getNullTime() = 0;

        virtual ~IData() {};
    };
} //close namespace TA_Base_Core
#endif // !defined(IData_6F37C686_5A1A_4f0e_85DB_C36A508A0467__INCLUDED_)
