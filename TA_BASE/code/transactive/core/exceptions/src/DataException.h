/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DataException.h $
  * @author K. Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * database code. It is derived from TransactiveException.
  * This exception class is used when errors are encountered 
  * with the data selected from the database.
  */


#if !defined(AFX_DATAEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_DATAEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{
	class DataException : public TransactiveException  
	{
	public:
 
		enum EDataFail	// Used to specify how the data retrieved is invalid
		{
			WRONG_TYPE,                // The value requested was of the wrong type.
			NO_VALUE,	               // There is no value associated with the request
            NOT_UNIQUE,                // There is more than one value associated with a request for which there should only be one
            CANNOT_DELETE,             // A delete request was made and the item cannot be deleted
            MISSING_MANDATORY,         // This indicates mandatory data is missing so this entity cannot be applied
			INVALID_VALUE			   // The value for this parameter was retrieved but is not valid, e.g. out of range.
		};


	 /**
	   * DataException (Constructor)
	   * 
	   * This constructs a DatabaseException.
	   *
	   * @param char* - This contains as much information as possible about the error that occurred. This can be any
	   *                additional information about the error.
	   * @param EDataFail - This indicates what sort of failure occurred. This allows the code catching the
	   *                    exception to decide how it wants to deal with the error.
	   * @param string - This contains which data was invalid. This may be a column name or a field name.
	   *
	   */
		DataException(const char *msg,
					  const EDataFail type,
					  const std::string& data) throw();

		virtual ~DataException() throw();

	 /**
	   * getFailType
	   * 
	   * This returns the type of failure that occurred with the data. The value
	   * returned should allow the application to determine an appropriate course of
	   * action. The application should always retrieve this so it can determine
	   * what to do next.
	   *
	   * @return EDataFail This indicates the type of failure that occurred. This
	   *                   is generic no matter what implementation of the underlying
	   *                   database code exists.
	   *
	   */
		EDataFail getFailType() const { return m_type; };

	 /**
	   * getWhichData
	   * 
	   * This returns which piece of data had the failure associated with it.
	   *
	   * @return string A string description of which piece of data had the error.
	   *
	   */
	   std::string getWhichData() const { return m_data; };


	private:

	 /**
	   * logException
	   * 
	   * This class must log the exception details that are specific to it. ie Everything except the
	   * string message passed in. There could be multiple constructors and each must log so this method
	   * is used to put the logging code in a common place.
	   *
	   */
		void logException();


		// Member Variables
		EDataFail   m_type;     // This is the type of failure that occurred.
		std::string m_data;     // This describes which piece of data is invalid

	};
} // Closes TA_Base_Core

#endif // !defined(AFX_DATAEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)

