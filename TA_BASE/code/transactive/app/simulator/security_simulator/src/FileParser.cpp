/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_4677/transactive/app/signalling/signalling_simulator/src/FileParser.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2006/11/20 15:12:21 $
  * Last modified by:  $Author: trainteam $
  *
  */
#include "StdAfx.h"
#include "FileParser.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "boost/regex.hpp"
#include "boost/tokenizer.hpp"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace
{
	const std::string AGENT_NAME_FLAG = "Agent";
	const std::string TOKEN = ",";
}

void FileParser::parseFile( std::vector< std::string >& agentNames )
{
    std::ifstream infile (m_fileName.c_str());

    if (!infile.is_open())
    {
        std::stringstream error;
        error << "Error opening " << m_fileName << std::endl;
#ifdef WIN32
		throw exception (error.str().c_str());
#endif
    }
    
    std::stringstream fileStream;

    while (infile.good())
    {
        fileStream << (char) infile.get();
    }

    infile.close();


    std::vector<std::string> lines = tokenizeString(fileStream.str(), "\n" );

    int caseNo = 0;

    // while file not empty
    for (std::vector<std::string>::iterator iter = lines.begin(); iter != lines.end(); ++iter)
    {
        // trim the line of leading and trailing whitespace
        std::string line = trim(*iter);

        // if whitespace skip line
        if (line == "" || line.at(0) == ';' || line.at(0) == '*')
        {
            continue;
        }
        else if(line.at(0) == '-')
        {
			long posLocatonFlag = line.find(AGENT_NAME_FLAG);
            long pos = line.find('=', 0);
            if( pos != std::string::npos && posLocatonFlag != std::string::npos )
            {
				std::string temp = line.substr(pos+1, line.size() - pos - 2);
				int lineSize = line.size();
				agentNames = tokenizeString(line.substr(pos+1, line.size() - pos - 2), TOKEN);
                
            }
        }
    }

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully loaded: ");
}


std::vector<std::string> FileParser::tokenizeString(const std::string& theString, const std::string& separatorList )
{
    std::vector<std::string> parts;
    typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

    boost::char_separator<char> sep( separatorList.c_str() );
    tokenizer tokens(theString, sep);
    
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
        parts.push_back(*tok_iter);
    }

    // if parts is empty, then this should return the entire string
    if (parts.size() == 0)
    {
        parts.push_back(theString);
    }

    return parts;
}

std::string FileParser::trim(const std::string & s)
{
    static boost::regex e("\\A\\s+|\\s+\\z");

    return boost::regex_replace(s, e, "");
}

std::string FileParser::getSection(const std::string & s)
{
    static boost::regex e("\\[|\\]");

    return boost::regex_replace(s, e, "");
}