/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/occi_database/src/OcciSqlExceptionHandler.h $
 * @author:  Robert Stagg
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 */

#if !defined(EA_8AE13E64_0673_45a9_97F5_6936B23B9E60__INCLUDED_)
#define EA_8AE13E64_0673_45a9_97F5_6936B23B9E60__INCLUDED_

using oracle::occi::SQLException;

class OcciSqlExceptionHandler
{

public:
	/**
	 * Takes an SQL Exception and turns it into a TA exception
	 * 
	 * @param ex
	 */
	static void throwOnError(SQLException& ex);

private:
    OcciSqlExceptionHandler(){};
    ~OcciSqlExceptionHandler(){};

public:

	// Various errors we are interested in.
	enum 
	{
		UNIQUE_CONSTRAINT_VIOLATED_ERROR =     1,
        SESSION_KILLED                   =    12,
        NOT_LOGGED_IN                    =  1012,
		COLUMN_NULL_ERROR                =  1405,
        END_OF_FILE_ON_COMMS_CHANNEL     =  3113,
        NO_LISTENER                      = 12541,
        NO_SUCH_DATABASE                 = 12545,
		ALREADY_CONNECTED_ERROR          = 24309,
        ALREADY_AUTHENTICATED            = 24313,
        REQUIRE_EXPLICT_ATTACH           = 24327
	};

};
#endif // !defined(EA_8AE13E64_0673_45a9_97F5_6936B23B9E60__INCLUDED_)