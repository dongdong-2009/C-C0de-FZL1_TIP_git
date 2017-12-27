/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DataConfigurationException.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This file outlines a simple exception class for use with
  * database code. It is derived from TransActiveException.
  * This exception class is used when errors are encountered 
  * in the data that is to be written to the database.
  */


#if !defined(AFX_DataConfigurationException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_DataConfigurationException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

#include "TransactiveException.h"

namespace TA_Base_Core
{
	class DataConfigurationException : public TransactiveException  
	{
	public:
 
	  /**
	   * DataConfigurationException (Constructor)
	   * 
	   * This constructs a DataConfigurationException.
	   *
	   * @param msg - This contains as much information as possible about the error that occurred. This can be any
	   *                additional information about the error.
       * @param fieldNames - This is a vector containing the missing and/or invalid field name
       *                that are preventing the Data object being commited to the database.
	   *
	   */
        DataConfigurationException(const char *msg,std::vector<std::string> fieldsNames) throw();

		virtual ~DataConfigurationException() throw();

	   		   /**
		* getMissingFields
		*
		* Returns the fields that have not been specified.
		*
		* @return The vector of fields that have yet to be specified for a database write to succeed.
		* Field names will be in allcaps (e.g. "NAME", "DESCRIPTION", "ACCESSLEVEL", etc).
		*/
	   const std::vector<std::string>& getMissingFields() const {return m_fields;};


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
		std::vector<std::string> m_fields;     // This describes which piece of data is invalid
	};
} // Closes TA_Base_Core

#endif // !defined(AFX_DataConfigurationException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)


