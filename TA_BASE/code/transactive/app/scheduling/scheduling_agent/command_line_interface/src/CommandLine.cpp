/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/command_line_interface/src/CommandLine.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This application provides a command line interface to the Scheduling Agent
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4786) // identifier was truncated in the browser information
#endif // _MSC_VER

#include "app/scheduling/scheduling_agent/command_line_interface/src/InvalidSyntaxException.h"
#include "app/scheduling/scheduling_agent/command_line_interface/src/CommandLine.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_App
{



CommandLine::CommandLine(const std::string& theLine)
{
    m_tokens = getTokensFromString(theLine);

    m_parameters = getParametersFromTokens(m_tokens);
}


std::vector<std::string> CommandLine::getTokensFromString(
    const std::string& theString) const
{
    std::vector<std::string> tokens;

    if( theString.find("\n") != std::string::npos )
    {
        TA_THROW(InvalidSyntaxException("Line contains a new line character"));
    }


    //
    // Go through each character in the string
    //
    std::string::size_type pos = 0;
    
    std::string token;
    while( pos < theString.length() )
    {
        //
        // If the character is a space
        //
        if( theString[pos] == ' ' )
        {
            //
            // If the previous character was not a space,
            // a token has just ended, so add it to the string
            //
            if( pos > 0 && theString[pos - 1] != ' ' )
            {
                tokens.push_back(token);
                token = "";
            }

            pos ++;
        }
        //
        // If the character is a quote
        //
        else if( theString[pos] == '"' )
        {
            //
            // If the previous character was not a space, throw an exception
            //
            if( pos > 0 && theString[pos - 1] != ' ' )
            {
                TA_THROW(InvalidSyntaxException("Quotes must be preceded by a space"));
            }

            //
            // Clear the token and move to the character after the quote
            //
            token = "";

            pos ++;

            //
            // Keep incrementing and recording characters till the next
            // quote or the end of the string.
            //
            std::string::size_type quoteStartPos = pos;
            while( theString[pos] != '"' && pos < theString.length() )
            {
                token += theString[pos];
                pos ++;
            }

            //
            // If there was not matching quote, throw an exception
            //
            if(theString[pos] != '"')
            {
                TA_THROW(InvalidSyntaxException("Unmatched quote found in string"));
            }

            //
            // If the next character is not a space, throw an exception
            //
            if( pos < theString.length() && theString[pos] != ' ' )
            {
                TA_THROW(InvalidSyntaxException("Quotes must be followed by a space"));
            }
        }
        //
        // If the current character is just a normal character
        // (not a space or quote) add it to the end of the token
        //
        else
        {
            token += theString[pos];
            pos ++;
        }

    }

    //
    // The last token in the string has not been added yet
    //
    tokens.push_back(token);

    return tokens;
}


std::map<std::string, std::string> CommandLine::getParametersFromTokens(
    const std::vector<std::string>& tokens) const
{
    std::map<std::string, std::string> parameters;

    for(unsigned int i = 0; i < tokens.size(); i ++)
    {
        //
        // The parameter name is the bit after the -,
        // eg  "-paramName"  =>  "paramName"
        //
        if( tokens[i][0] == '-' )
        {
            if( tokens[i].length() < 2 )
            {
                TA_THROW(InvalidSyntaxException("Parameter name not specified"));
            }
            std::string paramName(tokens[i], 1, tokens[i].length() - 1);

            if( i == tokens.size() - 1 )
            {
                std::string message = "No value specified for parameter ";
                message            += paramName;
                TA_THROW(InvalidSyntaxException(message));
            }

            parameters[paramName] = tokens[i + 1];
            i ++;
        }
    }

    return parameters;
}


std::string CommandLine::getParameter(unsigned int index) const
{
    if( index > m_tokens.size() - 1 )
    {
        return "";
    }
    else
    {
        return m_tokens[index];
    }
}


std::string CommandLine::getParameter(const std::string& parameterName) const
{
    std::map<std::string, std::string>::const_iterator it =
        m_parameters.find(parameterName);

    if( it == m_parameters.end() )
    {
        return "";
    }

    return (*it).second;
}


std::string CommandLine::getCommand() const
{
    //
    // The command is the first token
    //
    return getParameter(0);
}


} // TA_Base_App
