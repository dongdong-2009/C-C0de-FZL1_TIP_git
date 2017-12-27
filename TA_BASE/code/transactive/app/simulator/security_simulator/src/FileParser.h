/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_4677/transactive/app/signalling/signalling_simulator/src/FileParser.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2006/10/13 17:39:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * Quick hacky file parser for a well formed ini file.
  */

#ifndef SIG_FILE_PARSER_H
#define SIG_FILE_PARSER_H

#include <string>
#include <map>
#include <vector>
#include <boost/regex.hpp>

class FileParser
{

public:
    
    FileParser(const std::string& filename)
        : m_fileName(filename)
    {}

    void parseFile( std::vector< std::string >& agentNames );

    
private:

    std::vector<std::string> tokenizeString(const std::string& theString, const std::string& separatorList );
    std::string trim(const std::string & s);

    std::string getSection(const std::string & s);

    std::string m_fileName;

};

#endif // SIG_FILE_PARSER_H
