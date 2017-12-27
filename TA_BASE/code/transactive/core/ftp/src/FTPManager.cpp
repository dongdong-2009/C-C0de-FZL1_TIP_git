/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/ftp/src/FTPManager.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by:  $Author: Ouyang $
 * 
 * Presents the FTP Interface user with a simple, one command
 * method of performing FTP tasks. Directs an FTPClient instance
 * through the necessary steps. 
 *
 */

#include <sstream>

#include "core/ftp/src/FTPManager.h"
#include "core/ftp/src/FTPException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::FTPException;

namespace TA_Base_Core
{
	FTPManager* FTPManager::s_singleton = NULL;
	TA_Base_Core::ReEntrantThreadLockable	FTPManager::s_singletonLock;
		

	FTPManager& FTPManager::getInstance()
	{
    	if ( s_singleton == NULL )
        {
        	// Double locking to ensure that there is only one
            ThreadGuard guard( s_singletonLock );

            if ( s_singleton == NULL )
            {
           	 	s_singleton = new FTPManager();
            }
        }
        return *s_singleton;
	}


	void FTPManager::retrieveFile( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
					   			   const std::string& fileLocation, std::vector<unsigned char>& fileBuffer, unsigned int hostPort,
								   FTPClient::FileType fileType ) 
	{
		try
		{
			// Parse fileName for directory and file name
			std::string fileName, fileDirectory;
			std::string::size_type slashPos = fileLocation.find_last_of( "/" );
			if ( slashPos != std::string::npos )
			{
				//
				// There is directory information present as well - seperate from the
				// actual file name
				//
				fileDirectory.assign( fileLocation.substr( 0, slashPos ) );
				fileName.assign( fileLocation.substr( slashPos + 1, std::string::npos ) );
			}
			else
			{
				fileName.assign( fileLocation );
			}
	
			// Instantiate FTPClient and connect,login to server
			FTPClient newSession;
			newSession.connectToServer( hostAddress, hostPort );
			newSession.userLogin( userName, userPassword );
			
			// Change server side directory if necessary
			if ( fileDirectory.length() > 0 )
			{
				newSession.changeDirectory( fileDirectory );
			}
	
			newSession.setFileType( fileType );
			newSession.retrieveFile( fileName, fileBuffer );
			newSession.closeConnection();
		}
		catch( FTPException& )
		{
			throw;
		}
		catch( TransactiveException& tex )
		{
			TA_THROW( FTPException( tex.what() ) );
		}
		catch( ... )
		{
			TA_THROW( FTPException( "Unknown exception caught during FTP operation" ) );
		}
	}


	void FTPManager::sendFile( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
				const std::string& srcFileLocation, const  std::string& dstFileLocation, 
				unsigned int hostPort, FTPClient::FileType fileType )
	{
		if (fileType != FTPClient::Ascii)
		{
			//current implementation only support TXT file
			TA_THROW( FTPException( "Send file function only support TXT file yet." ) );
		}
		
		try
		{
			// Parse fileName for directory and file name
			std::string fileName, fileDirectory;
			std::string::size_type slashPos = dstFileLocation.find_last_of( "/" );
			if ( slashPos != std::string::npos )
			{
				//
				// There is directory information present as well - seperate from the
				// actual file name
				//
				fileDirectory.assign( dstFileLocation.substr( 0, slashPos ) );
				fileName.assign( dstFileLocation.substr( slashPos + 1, std::string::npos ) );
			}
			else
			{
				fileName.assign( dstFileLocation );
			}
			
			// prepare the buffer to be sent
			std::ifstream in(srcFileLocation.c_str());
			std::stringstream fileStream;
			fileStream << in.rdbuf();
			in.close();

			if (fileStream.str().empty() == true)
			{
				TA_THROW( FTPException( "Read file error." ) );
			}

			// Instantiate FTPClient and connect,login to server
			FTPClient newSession;
			newSession.connectToServer( hostAddress, hostPort );
			newSession.userLogin( userName, userPassword );
			
			// Change server side directory if necessary
			if ( fileDirectory.length() > 0 )
			{
				newSession.changeDirectory( fileDirectory );
			}
			
			newSession.setFileType( fileType );
			newSession.sendFile( fileName, fileStream );
			newSession.closeConnection();
		}
		catch( FTPException& )
		{
			throw;
		}
		catch( TransactiveException& tex )
		{
			TA_THROW( FTPException( tex.what() ) );
		}
		catch( ... )
		{
			TA_THROW( FTPException( "Unknown exception caught during FTP operation" ) );
		}
		
	}

	FTPManager::FTPManager()
	{
		// Nothing yet
	}


	FTPManager::~FTPManager()
	{
		// Nothing yet
	}

	void FTPManager::execSimpleCommand( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
		std::string& command, std::string & directoryLocation, unsigned int hostPort, FTPClient::FileType fileType )
	{
		try
		{
			// Instantiate FTPClient and connect,login to server
			FTPClient newSession;
			newSession.connectToServer( hostAddress, hostPort );
			newSession.userLogin( userName, userPassword );
			
			// Change server side directory if necessary
			if ( directoryLocation.length() > 0 )
			{
				newSession.changeDirectory( directoryLocation );
			}
	
			newSession.setFileType( fileType );
			newSession.execSimpleCommand( command );
			newSession.closeConnection();
		}
		catch( FTPException& )
		{
			throw;
		}
		catch( TransactiveException& tex )
		{
			TA_THROW( FTPException( tex.what() ) );
		}
		catch( ... )
		{
			TA_THROW( FTPException( "Unknown exception caught during FTP operation" ) );
		}
	}


}
