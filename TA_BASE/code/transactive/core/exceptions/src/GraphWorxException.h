/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/GraphWorxException.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * the GraphWorxCommunicator in the Control Station. It is
  * derived from TransactiveException.
  * This exception class is used when errors are encountered 
  * when attempting to communicate with GraphWorX.
  */


#if !defined(AFX_GRAPHWORXEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_GRAPHWORXEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class GraphWorxException : public TransactiveException  
	{
	public:
 
		enum EFail	// Used to specify how the data retrieved is invalid
		{
            INVALID_FILE,          // The file specified was invalid. ie blank
			NOT_RUN,               // This indicates GraphWorX could not be run at all
			FILE_OPEN_ERROR        // This indicates a schematic could not be launched.
		};


	 /**
	   * GraphWorxException (Constructor)
	   * 
	   * This constructs a GraphWorxException.
	   *
	   * @param char* - This contains as much information as possible about the error that occurred. This can be any
	   *                additional information about the error.
	   * @param EFail - This indicates what sort of failure occurred. This allows the code catching the
	   *                exception to decide how it wants to deal with the error.
	   *
	   */
		GraphWorxException(const char *msg,
					  const EFail type) throw();

		virtual ~GraphWorxException() throw();

	 /**
	   * getFailType
	   * 
	   * This returns the type of failure that occurred. The value
	   * returned should allow the application to determine an appropriate course of
	   * action. The application should always retrieve this so it can determine
	   * what to do next.
	   *
	   * @return EFail This indicates the type of failure that occurred.
	   *
	   */
		EFail getFailType() const { return m_type; };

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
		EFail   m_type;     // This is the type of failure that occurred.

	};
} // Closes TA_Base_Core

#endif // !defined(AFX_GRAPHWORXEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)

