/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/EntityTypeException.h $
  * @author N. Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * database code. It is derived from TransactiveException.
  * The exception class is used when entity construction is requested for an
  * of a type for which there is no concrete class available.
  */


#if !defined(AFX_EntityTypeException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_EntityTypeException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{
	class EntityTypeException : public TransactiveException  
	{
	public:
 
	   /**
	    * EntityTypeException (Constructor)
	    * 
	    * This constructs an EntityTypeException.
	    *
	    * @param char* - This contains as much information as possible about the error that occurred. This can be any
	    *                additional information about the error.
	    * @param type - This is the entity type name for which a concrete implementation could not be found.
	    * @param key - This is the pkey into the entity table for the entity that could not be created. It is an optional
        *              parameter, and can be left to the default of zero if not known at the time the exception is generated.
	    */
		EntityTypeException(const char *msg,
                      const std::string& type,
					  const unsigned long key = 0) throw();

		virtual ~EntityTypeException() throw();

	   /**
	    * getType
	    * 
	    * This returns the name of the entity type that was not found as a concrete class.
	    *
	    * @return std::string This is the name of the entity type as a string.
	    *
	    */
        std::string getFailType() const { return m_type; };

	   /**
	    * getKey
	    * 
	    * This returns the entity key for the entity that could not be created.
	    *
	    * @return unsigned long The entity key for the entity that could not be created.
	    *
	    */
	   unsigned long getKey() const { return m_key; };


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
        std::string m_type;     // This is the entity type that could not be matched.
		unsigned long m_key;     // This is the key for the entity of the invalid type.

	};
} // Closes TA_Base_Core

#endif // !defined(AFX_EntityTypeException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)

