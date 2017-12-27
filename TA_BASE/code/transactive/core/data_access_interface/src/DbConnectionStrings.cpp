/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DbConnectionStrings.cpp $
 * @author:  R.Stagg
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2015/06/04 10:09:29 $
 * Last modified by:  $Author: Ouyang $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#pragma warning(disable:4819 4996)
#endif

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/DataTypesLookup.h"
#include "core/exceptions/src/DbConnectionNoFile.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/utilities/src/TAAssert.h"
#include "core/database/src/CommonType.h"

#include <iostream>

namespace fs = boost::filesystem;

namespace TA_Base_Core
{
	NonReEntrantThreadLockable DbConnectionStrings::m_singletonLock;
	DbConnectionStrings* DbConnectionStrings::m_sDbConnStrObj = NULL;

    DbConnectionStrings::DbConnectionStrings(std::string filename)
        : m_filename(filename)
    {
    }

    DbConnectionStrings::~DbConnectionStrings()
    {
    }

    DatabaseConnectionMap& DbConnectionStrings::getConnectionMap()
    {
        FUNCTION_ENTRY("getConnectionMap");
        if (m_dbMap.size() == 0)
        {
            ParseFile();
        }

    	FUNCTION_EXIT;
        return m_dbMap;
    
	}

	DbConnectionStrings& DbConnectionStrings::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		
		if ( m_sDbConnStrObj == NULL )
		{
			m_sDbConnStrObj = new DbConnectionStrings();
		}		

		return *m_sDbConnStrObj;
	}

	DataConnections DbConnectionStrings::getConnectionList(const EDataTypes dataType, const EDataActions action)
	{
		return  DbConnectionStrings::getInstance().getConnectionListStatic(m_filename, dataType, action);
	}

    DataConnections DbConnectionStrings::getConnectionListInternal(const EDataTypes dataType, const EDataActions action)
    {
        FUNCTION_ENTRY("getConnectionListInternal");

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Looking for a database for data type %i, action %i.",
			 dataType, action );

        if (m_dbMap.size() == 0)
        {
            ParseFileEx();
        }
        DatabaseConnectionMap::iterator dbIter ( m_dbMap.find( std::make_pair( dataType, action )));
        if ( dbIter == m_dbMap.end() )
        {
            std::string msg ("No connection string defined for ");
            msg += dataType;   // should create a lookup function
            msg += "/";        // to change the enums back into
            msg += action;     // strings.
            throw DbConnectionFailed(msg); 
        }

		DataConnections retDataConn;
		DataConnections::const_iterator cIter = (dbIter->second).begin();
		while ( cIter != (dbIter->second).end())
		{
			std::string strDbName = (cIter->first).c_str();
	    	std::string strConn   = (cIter->second).c_str();
			retDataConn.push_back(std::make_pair(strDbName, strConn));
			cIter++;
		}
		FUNCTION_EXIT;
        return retDataConn;
//		return dbIter->second;
	
    }

    void DbConnectionStrings::ParseFile()
    {
        FUNCTION_ENTRY("ParseFile");
        // open file and slurp data into map
        TA_ASSERT(!m_filename.empty(),"Filename can not be null");

        fs::path filepath = fs::system_complete(  fs::path( m_filename) );
        fs::ifstream dbConnectStringFile( filepath );
        if ( !dbConnectStringFile )
        {
            std::string msg ("Unable to open file:" + filepath.string());
            LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, msg.c_str());
			throw DbConnectionNoFile( msg.c_str() ); 

        }

        char line[2048]; // 2048 - a number PFTA that should by larger than any single line in the file
        do
        {
            // read a line of values ( a record in db terms )
            dbConnectStringFile.getline(line, sizeof(line));
			//if is empty line
			if (line[0] == 0x0)
			{
				continue;
			}

            // break into values and stuff into a vector for ease of manipulation
            boost::escaped_list_separator<char> sep("\\",",","\"");
            std::string strLine(line);
            boost::tokenizer<boost::escaped_list_separator<char> > tokens ( strLine ,  sep);
            boost::tokenizer<boost::escaped_list_separator<char> >::iterator token = tokens.begin();
            std::vector<std::string> values;
            while ( token != tokens.end() ) 
            {
                values.push_back( *token );
                token++;
            }

            // run some checks to ensure we have the right number
            if ( values.size() && values[0].compare("Data Type") == 0)
            {
                continue; // Skip header row
            }
            if ( values.size() < CONN_STR_PARAM_COUNT ) // min required is datatype,dataaction,dbtype,dbname,dbuser,dbpass,dbhostname
            {
                //throw; //something OR just log it ??
                LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                    "Not enough parameters in file: %s ", filepath.string().c_str());
                LOGMORE ( SourceInfo, "line is: %s :SKIPPING", line);
                continue;
            }
            if ( (values.size() - CONN_STR_PARAM_COUNT ) % CONN_STR_PARAM_MIN_COUNT != 0 ) // any remaing values must be connection triples
            {
                //throw; // something OR just log it ??
                LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                    "Incorrect number of parameters in file: %s ", filepath.string().c_str());
                LOGMORE ( SourceInfo, "line is: %s :SKIPPING", line);
                continue;
            }

            // we have the correct number of parameters, store them
            DataKey key (std::make_pair(getDataTypesEnum(values[0]),getDataActionsEnum(values[1])));
            values.erase(values.begin());
            values.erase(values.begin());

            std::vector<DataConnection> connections;
			std::string strFirst;
            while (values.size())
            {
                if( !values[0].empty() )
				{
					if (!values[CON_STR_Hostname].empty())					
						strFirst = values[CON_STR_SchemaName] + "@" + values[CON_STR_Hostname];					
					else
						strFirst = values[CON_STR_SchemaName];
					connections.push_back( std::make_pair(strFirst, values[CON_STR_DBType] + ":" + values[CON_STR_SchemaName] + ":" + values[CON_STR_UserName] + ":" + values[CON_STR_Passwd] + ":" + values[CON_STR_Hostname] ));
				}
                values.erase(values.begin());
                values.erase(values.begin());
                values.erase(values.begin());
				values.erase(values.begin());
				values.erase(values.begin());
            }            
            m_dbMap.insert(std::make_pair(key,connections) );
            
        }while (!dbConnectStringFile.eof());

		FUNCTION_EXIT;
	}

	void DbConnectionStrings::ParseFileEx()
	{
		FUNCTION_ENTRY("ParseFileEx");
		// open file and slurp data into map
		TA_ASSERT(!m_filename.empty(),"Filename can not be null");

		fs::path filepath = fs::system_complete(  fs::path( m_filename) );
		fs::ifstream dbConnectStringFile( filepath );
		if ( !dbConnectStringFile )
		{
			std::string msg ("Unable to open file:" + filepath.string());
			LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, msg.c_str());
			throw DbConnectionNoFile( msg.c_str() ); 

		}

		char line[2048]; // 2048 - a number PFTA that should by larger than any single line in the file
		do
		{
			// read a line of values ( a record in db terms )
			dbConnectStringFile.getline(line, sizeof(line));
			//if is empty line
			if (line[0] == 0x0)
			{
				continue;
			}

			// break into values and stuff into a vector for ease of manipulation
			boost::escaped_list_separator<char> sep("\\",",","\"");
			std::string strLine(line);
			boost::tokenizer<boost::escaped_list_separator<char> > tokens ( strLine ,  sep);
			boost::tokenizer<boost::escaped_list_separator<char> >::iterator token = tokens.begin();
			std::vector<std::string> values;
			while ( token != tokens.end() ) 
			{
				values.push_back( *token );
				token++;
			}

			// run some checks to ensure we have the right number
			if ( values.size() && values[0].compare("Data Type") == 0)
			{
				continue; // Skip header row
			}
			if ( values.size() < CONN_STR_PARAM_COUNT ) // min required is datatype,dataaction,dbtype,dbname,dbuser,dbpass,dbhostname
			{
				//throw; //something OR just log it ??
				LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					"Not enough parameters in file: %s ", filepath.string().c_str());
				LOGMORE ( SourceInfo, "line is: %s :SKIPPING", line);
				continue;
			}
			if ( (values.size() - CONN_STR_PARAM_COUNT ) % CONN_STR_PARAM_MIN_COUNT != 0 ) // any remaing values must be connection triples
			{
				//throw; // something OR just log it ??
				LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					"Incorrect number of parameters in file: %s ", filepath.string().c_str());
				LOGMORE ( SourceInfo, "line is: %s :SKIPPING", line);
				continue;
			}

			// we have the correct number of parameters, store them
			DataKey key (std::make_pair(getDataTypesEnum(values[0]),getDataActionsEnum(values[1])));
			values.erase(values.begin());
			values.erase(values.begin());

			std::vector<DataConnection> connections;
			while (values.size())
			{
				if( !values[0].empty() )
				{				
					connections.push_back( std::make_pair(values[CON_STR_SchemaName], values[CON_STR_DBType] + ":" + values[CON_STR_SchemaName] + ":" + values[CON_STR_UserName] + ":" + values[CON_STR_Passwd] + ":" + values[CON_STR_Hostname] ));
				}
				values.erase(values.begin());
				values.erase(values.begin());
				values.erase(values.begin());
				values.erase(values.begin());
				values.erase(values.begin());
			}            
			m_dbMap.insert(std::make_pair(key,connections) );

		}while (!dbConnectStringFile.eof());

		FUNCTION_EXIT;
	}

	DataConnections DbConnectionStrings::getConnectionListStatic(const std::string& strFileName, const EDataTypes dataType, const EDataActions action)
	{
		 FUNCTION_ENTRY("getConnectionListStatic");

   		 ThreadGuard guard( m_getDbNameLock );
		 DbConnStrMap::const_iterator cIter = m_sMapDbConnStrs.find(strFileName);
		 if ( cIter != m_sMapDbConnStrs.end() )
		 {
			 DbConnectionStrings* pDbConnStr = cIter->second;
			 if (pDbConnStr != NULL)
			 {
				return pDbConnStr->getConnectionListInternal(dataType, action);
			 }
			 //should never reach here;
			 throw DbConnectionFailed("getConnectionListStatic(): the DbConnectionStrings pointer is null");
		 }
		 else
		 {
			 LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "Database file changed, fileName: %s ", strFileName.c_str());
			 DbConnectionStrings* pDbConnStr = new DbConnectionStrings(strFileName);
			 m_sMapDbConnStrs.insert(DbConnStrMap::value_type(strFileName, pDbConnStr));
	
			 return pDbConnStr->getConnectionListInternal(dataType, action);
		 }      
	}

    std::string DbConnectionStrings::getDatabaseName(const std::string& connectionString, const std::string& prefix)
    {
        static std::map<std::string, std::string> cache;
        std::string& name = cache[connectionString];
        
        if (name.size())
        {
            return name;
        }

        std::vector<std::string> vs;
        boost::split(vs, connectionString, boost::is_any_of(",:/;|"), boost::token_compress_off);
        TA_ASSERT(CON_SIZE == vs.size(), str(boost::format("Bad db connection string format :%s") % connectionString).c_str());
        return name = prefix + (vs[CON_STR_Hostname].empty() ? vs[CON_STR_SchemaName] : (vs[CON_STR_SchemaName] + "@" + vs[CON_STR_Hostname]));
    }

} // namespace
