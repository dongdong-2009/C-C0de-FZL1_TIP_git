/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/command_line_interface/src/CommandLine.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This application provides a command line interface to the Scheduling Agent
  *
  */


#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H


#include <vector>
#include <string>
#include <map>


namespace TA_Base_App
{


    
class CommandLine
{

public:

    /**
     *  Constructs a new CommandLine object from the given line.
     *
     *  @exception InvalidSyntaxException
     *  if the line contains a new line, or if there are syntactic
     *  errors list ending with -paramName but having no value
     *  eg "hey there -a"
     *
     */
    CommandLine(const std::string& theLine);


    /**
     *  Gets the parameter at the given index, where the first item
     *  is at index 0.
     *
     *  eg
     *  "Hi there sonny boy -name bob"
     *  [0] [ 1 ] [ 2 ] [3] [ 4 ] [5]
     *
     *  Returns "" if there is not a parameter at that index
     */
    std::string getParameter(unsigned int index) const;


    /**
     *  Gets the parameter with the given name.
     *
     *  Returns "" if there is not a parameter with that name.
     *
     *  eg
     *  "run some command -a blah hello there -b second"
     *
     *  getParameter("a") => "blah"
     *  getParameter("b") => "second"
     */
    std::string getParameter(const std::string& parameterName) const;


    /**
     *  Returns the first token, which should be the command
     */
    std::string getCommand() const;


protected:

    /**
     *  Separate the given string into tokens.
     *  Tokens are separated by spaces, except that quoted parts of the string
     *  are one token.
     *  eg
     *  I like short shorts, "dah na na na dah na na na", who likes short shorts?
     *  1  2     3     4                  5                6    7     8     9
     */
    std::vector<std::string> getTokensFromString(
        const std::string& theString) const;

    /**
     *  Given a vector of tokens, parses for [-name / value] pairs and puts
     *  them into a map. Ignores other tokens.
     *
     *  eg
     *  {"-location", "OCC", "blah", "-size", "19"}
     *  =>
     *  {["location" => "OCC"], ["size", "19"]}
     */
    std::map<std::string, std::string> CommandLine::getParametersFromTokens(
        const std::vector<std::string>& tokens) const;


private:

    std::vector<std::string> m_tokens;
    std::map<std::string, std::string> m_parameters;
};


} // TA_Base_App

#endif // COMMAND_LINE_H
