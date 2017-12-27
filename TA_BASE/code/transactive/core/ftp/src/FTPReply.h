#ifndef FTPREPLY_H
#define FTPREPLY_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/ftp/src/FTPReply.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Represents a single line of a response from the FTP server,
 * which generally consists of a three digit code and explanatory
 * string message. This class interprets the reply code for the
 * overall 'result' type and determines whether or not this line is
 * the final line in the reply.
 *
 */
#include <string>


namespace TA_Base_Core
{
	class FTPReply
	{

	public:

       	enum FTPReplyType
        {
			MultiLine_Part,			// This is a part of a multi-line reply from the FTP server
            Positive_Preliminary,   // Action being intiated, expect another reply
            Positive_Completion,    // Action successfully completed
            Positive_Intermediate,  // Command accepted, action pending receipt of further information
            Negative_Transient,     // Command not accepted, temporary error condition
            Negative_Permanent      // Command not accepted, permanent error condition
        };


		/**
		 * Constructor
		 *
		 * Seperates the string into code and message, and determines
		 * whether or not the line is part of a multi-line reply.
		 *
		 * @param	replyData	A line from a FTP server reply, terminated by
		 * 						the Telnet EOL sequence.
		 *
		 */
		FTPReply( const char*  replyData );


		/**
		 * Destructor
		 *
		 */
		~FTPReply();


		/**
		 * isLastReply()
		 *
		 * If the line is part of a multi-line reply, or the reply type
		 * indicates further response forthcoming, then this is not the last
		 * line. Otherwise, it is.
		 *
		 * @return whether or not this is the final line of the reply.
		 *
		 */
		bool isLastReply() const;


		/**
		 * getType()
		 *
		 * Either returns the type as being part of a multi-line reply (as
		 * determined in the constructor) or interprets the first digit of
		 * the reply code.
		 *
		 * @return	the 'type' of this reply
		 */
		FTPReplyType getType() const;

			
		/**
		 * logReply()
		 *
		 * @return a formatted string containing the reply code and message
		 *
		 */
		std::string logReply() const;


	private:


		int				m_replyCode[3];
		bool			m_multiLinePart;
		std::string		m_replyString;

	};
}


#endif // FTPREPLY_H

