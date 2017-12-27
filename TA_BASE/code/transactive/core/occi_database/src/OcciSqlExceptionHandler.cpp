/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/occi_database/src/OcciSqlExceptionHandler.cpp $
 * @author:  Robert Stagg
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <occi.h>

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "OcciSqlExceptionHandler.h"

using namespace TA_Base_Core;
/*
 * we must reduce the thousands of Oracle errors
 * to just the following few:
 *
 * DatabaseException  the default
 * DataException      used when there is a problem with the data
 * DbConnectionFailed used when there has been a fatal db error
 *
 * If there are SQL errors these should be asserted as they are coding errors
 */
void OcciSqlExceptionHandler::throwOnError(SQLException& ex)
{
    FUNCTION_ENTRY("throwOnError");

	bool throwDbException = false;
	bool throwDataException = false;
	bool throwDbFailedException = false;

    TA_Base_Core::DataException::EDataFail dataFail = TA_Base_Core::DataException::WRONG_TYPE;

    std::string errorMsgStr;

    switch( ex.getErrorCode() )
	{
		case 0:
		{
			errorMsgStr = "No further information available\n";
			throwDbException = true;
		}
		break;

		case COLUMN_NULL_ERROR: // getting a null column in a query in not an error
		{
		}
		break;

        case ALREADY_AUTHENTICATED: // trying to reconnect
        {
        }
        break;

        // Following is a list of errors that all mean the same thing...
        // *** the db is dead ***
        // As we want to do the exect same thing in every case, I have broken
        // with the coding guidelines and lumped them together
        case END_OF_FILE_ON_COMMS_CHANNEL:  // db has gone away
        case NO_SUCH_DATABASE:              // bad connect string or comms error
        case NO_LISTENER:                   // db exists but not currently available
        case SESSION_KILLED:                // our session has been terminated by some sysadmin 
        case NOT_LOGGED_IN:                 // got this in testing. problably should never happen in real life
        case REQUIRE_EXPLICT_ATTACH:        // I think this is a secondary error - no idea what the primary error is
        {
            throwDbFailedException = true;
        }
        break;

        default:
		{
			throwDbException = true;
		}
		break;
	}

    // errors are checked in decending order of priority
    if( throwDbFailedException )
	{
		// log the comment error
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "%s", ex.what() );

		DbConnectionFailed e( ex.what() );
		throw e;
	}
	if( throwDataException )
	{
		// log error
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "%s", ex.what() );

        // leave the data field blank
        // hopefully who every called us will fill it in before rethrowing it
        DataException e( ex.what(), dataFail, "" );
		throw e;
	}
	if( throwDbException )
	{
		// log the error
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "%s", ex.what() );

        DatabaseException e( ex.what() );
		throw e;
	}

    FUNCTION_EXIT;
}