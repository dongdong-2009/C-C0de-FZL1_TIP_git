/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/ftp/src/FTPReply.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by:  $Author: Ouyang $
 * 
 * Represents a single line of a response from the FTP server,
 * which generally consists of a three digit code and explanatory
 * string message. This class interprets the reply code for the
 * overall 'result' type and determines whether or not this line is
 * the final line in the reply.
 *
 */

#include <cstring>
#include <cctype>

#include "core/ftp/src/FTPReply.h"
#include "core/ftp/src/FTPConstants.h"
#include "core/ftp/src/FTPException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::FTPException;

namespace TA_Base_Core
{
	FTPReply::FTPReply( const char*  replyData )
	{
		TA_ASSERT( replyData != NULL, "NULL replyData pointer" );

		// Locate the terminating Telnet EOL sequence
		char const* eolPos = strstr( replyData, TELNET_EOL_SEQUENCE );
		if ( eolPos == NULL )
		{	
			TA_ASSERT( false, "No Telnet EOL sequence detected in reply data" );
		}

		// Are the first three characters a reply code ?
		if ( isdigit( replyData[0] ) > 0 && isdigit( replyData[1] ) > 0 && isdigit( replyData[2] ) > 0 )
		{
			// Yes - seperate the code from the message
			char tempNumber[2];
			m_replyCode[0] = atoi( strncpy( tempNumber, &replyData[0], 1 ) );	
			m_replyCode[1] = atoi( strncpy( tempNumber, &replyData[1], 1 ) );	
			m_replyCode[2] = atoi( strncpy( tempNumber, &replyData[2], 1 ) );	
			m_replyString.assign( replyData + 4, eolPos - replyData - 4 );

			// if the reply code is followed by a "-" as opposed to a space, then
			// this line is part of a multi-line reply
			m_multiLinePart = ( replyData[3] == '-' );
		}
		else
		{
			// No - this is part of a multi line reply. The whole
			// string is a message, there is no reply code
			m_replyCode[0] = 0;
			m_replyCode[1] = 0;
			m_replyCode[2] = 0;
			m_multiLinePart = true;
			m_replyString.assign( replyData, eolPos - replyData );
		}
	}
		


	FTPReply::~FTPReply()
	{
		// nothing yet
	}


	bool FTPReply::isLastReply() const
	{
		if ( m_multiLinePart || getType() == Positive_Preliminary )
		{
			return false;
		}
		return true;
	}
		

	FTPReply::FTPReplyType FTPReply::getType() const
	{
		if ( m_multiLinePart )
		{
			return MultiLine_Part;
		}

		switch( m_replyCode[0] )
		{
			case 1 : return Positive_Preliminary;
			case 2 : return Positive_Completion;
			case 3 : return Positive_Intermediate;
			case 4 : return Negative_Transient;
			case 5 : return Negative_Permanent;
			default : TA_THROW( FTPException( "Invalid first digit in reply code" ) );
        }
    }

			
	std::string FTPReply::logReply() const
	{
		char reply[256] = {0};
		if ( m_replyCode[0] == 0 )
		{
			sprintf( reply, "    %s", m_replyString.c_str() );
		}
		else
		{
			sprintf( reply, "%d%d%d %s", m_replyCode[0], m_replyCode[1], m_replyCode[2], m_replyString.c_str() );
		}
		return std::string( reply );
	}
}

