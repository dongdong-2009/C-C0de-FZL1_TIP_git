/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/BasicHelper.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Basic functions for validating/formatting dates, retrieving session information,
  * sending audit messages, checking rights and processing log files.  
  * All the functions within class are static.  They are used by multiple
  * classes.
  */

#if defined( WIN32 )
#pragma warning ( disable : 4018 )      // signed/unsigned mismatch when using std::isdigit().
#pragma warning ( disable : 4786 )
#endif // defined( WIN32 )

#include <locale>
#include <sstream>
#include <fstream>
#include <bitset>
#include <ctype.h>
#include <afxdisp.h>
#include <Windows.h>
#include <atlbase.h>

#include "app/data_management/archive_manager/src/BasicHelper.h"
#include "app/data_management/archive_manager/src/resource.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"    
#include "core/data_access_interface/src/IOperator.h"      
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/ArchiveAudit_MessageTypes.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"


namespace
{
    // Constants used in processFiles().  These may need to be changed if 
    // the Imp.exe and Exp.exe programs are not used for importing and exporting
    // the data.

    const std::string IMPORT_SUCC  ( "Import terminated successfully without warnings" );
    const std::string IMPORT_OKWARN( "Import terminated successfully with warnings" );
    const std::string IMPORT_FAIL  ( "Import terminated unsuccessfully" );
    const std::string EXPORT_SUCC  ( "Export terminated successfully without warnings" );
    const std::string EXPORT_OKWARN( "Export terminated successfully with warnings" );
    const std::string EXPORT_FAIL  ( "Export terminated unsuccessfully" );
}

namespace TA_ArchiveManager
{
    ///////////////////////////////////////////////////////////////////////////
    //
    // Date formatting.
    //
    ///////////////////////////////////////////////////////////////////////////

    time_t convertCOleDateTimeToTimeT( const COleDateTime& date )
    {
        // Convert the date string into a time_t.

        struct tm dateTm;
        dateTm.tm_mday = date.GetDay();
        dateTm.tm_mon = date.GetMonth() - 1;        // Months go from 0 - 11 in a tm struct.
        dateTm.tm_year = date.GetYear() - 1900;     // tm_year is from 1900.
        dateTm.tm_hour = date.GetHour();
        dateTm.tm_isdst = -1;
        dateTm.tm_min = date.GetMinute();
        dateTm.tm_sec = date.GetSecond();

        time_t time = mktime( &dateTm );
        if ( time == static_cast< time_t >( -1 ) )
        {
            TA_THROW( TA_Base_Core::ArchiveException( "Unable to convert date/time." ) );
        }
        return time;
    }

   
    std::string convertTimeTToStringDate( const time_t& date, bool includeTime /* = false */ )
    {
        COleDateTime theDate( date );
		// CL-16726 --
		// not all System default use "d/mm/yyyy" date format
// 		std::string dateStr = theDate.Format( VAR_DATEVALUEONLY, LANG_USER_DEFAULT );
// 		if('/' == dateStr.at(1))
// 		{
// 			dateStr = '0' + dateStr;
// 		}
		// -- CL-16726

		// CL-16726 ++
		// Sorting date format MM/DD/YYYY
        std::string dateStr = theDate.Format("%Y/%m/%d");
		// ++ CL-16726
        if( includeTime )
        {
            //std::string timeStr = theDate.Format( VAR_TIMEVALUEONLY, LANG_USER_DEFAULT );
			// CL-16726 ++
			// using time format HH24:MI:SS
			std::string timeStr = theDate.Format("%H:%M:%S");
			// ++ CL-16726
            if ( !timeStr.empty() )
            {
                dateStr += " " + timeStr;
            }
        }

        return dateStr;
    }


    std::string convertTimeTToFormattedString( const time_t& date )
    {
        const std::string separator( "_" );

        std::stringstream dateStr( "" );
        COleDateTime theDate( date );
        
        // Year
        int year = theDate.GetYear();
        if ( year < 1000 )
        {
            dateStr << "0";
        }
        dateStr << year;  
        dateStr << separator;
        
        // Month

        int month = theDate.GetMonth();
        if ( month < 10 )
        {
            dateStr << "0";
        }
        dateStr << month;
        dateStr << separator;
        
        // Day

        int day = theDate.GetDay();
        if ( day < 10 )
        {
            dateStr << "0";
        }
        dateStr << day;

        return dateStr.str();
    }


    std::string convertTimeTToYYMMDDString( const time_t& date )
    {
        std::stringstream dateStr( "" );
        COleDateTime theDate( date );
        
        // Year

        int year = theDate.GetYear();
        if ( year < 1000 )
        {
            dateStr << "0";
        }
        dateStr << year;  
        
        // Month

        int month = theDate.GetMonth();
        if ( month < 10 )
        {
            dateStr << "0";
        }
        dateStr << month;
        
        // Day

        int day = theDate.GetDay();
        if ( day < 10 )
        {
            dateStr << "0";
        }
        dateStr << day;

        return dateStr.str();
    }

	std::string convertTimeTToDDMMYYString( const time_t& date )
    {
        std::stringstream dateStr( "" );
        COleDateTime theDate( date );
            
        // Day

        int day = theDate.GetDay();
        if ( day < 10 )
        {
            dateStr << "0";
        }
        dateStr << day;

		 // Month

        int month = theDate.GetMonth();
        if ( month < 10 )
        {
            dateStr << "0";
        }
        dateStr << month;

		 
        // Year

        int year = theDate.GetYear();
        if ( year < 1000 )
        {
            dateStr << "0";
        }
		year -= (year/100)*100; 
		if(year<10)
		{
			dateStr<<0;
		}
        dateStr << year;  
        
        return dateStr.str();
    }



    ///////////////////////////////////////////////////////////////////////////
    //
    // File name processing.
    //
    ///////////////////////////////////////////////////////////////////////////


    time_t getDateFromFileName( const std::string& fileName )
    {            
        // KT 10 Mar 04: PW #3136 - Changed '-' to '_' for operating system compatibility.

        // The archive files have to be named in the format 
        // AYYYY_MM_DD_filepostfix.dmp or MYYYY_MM_DD_filepostfix.dmp.

        std::locale loc;
        if( ( 16 >= fileName.length()  ) ||
            ( ( 'A' !=  fileName[0] ) && ( 'M' != fileName[0] ) ) ||
            ( !std::isdigit( fileName[1], loc )  ) ||
            ( !std::isdigit( fileName[2], loc )  ) ||
            ( !std::isdigit( fileName[3], loc )  ) ||
            ( !std::isdigit( fileName[4], loc )  ) ||
            ( '_' !=         fileName[5]         ) ||
            ( !std::isdigit( fileName[6], loc )  ) ||
            ( !std::isdigit( fileName[7], loc )  ) ||
            ( '_' !=         fileName[8]         ) ||
            ( !std::isdigit( fileName[9], loc )  ) ||
            ( !std::isdigit( fileName[10], loc ) ) ||
            ( '_' !=         fileName[11] ) )
        {
            // This message doesn't get displayed.
			return getDateFromFileNameDDMMYY(fileName); // CL-16726: try old version file name
            CString error;
            error.FormatMessage( UE050110_INVALID_FILENAME_FORMAT, fileName.c_str() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        // Convert the string into a time_t.

        struct tm dateTm;
        dateTm.tm_mday = atoi( fileName.substr( 9, 2 ).c_str() );
        dateTm.tm_mon = atoi( fileName.substr( 6, 2 ).c_str() ) - 1;        // Months go from 0 - 11 in a tm struct.
        dateTm.tm_year = atoi( fileName.substr( 1, 4 ).c_str() ) - 1900;    // tm_year is from 1900.
        dateTm.tm_hour = 0;
        dateTm.tm_isdst = -1;
        dateTm.tm_min = 0;
        dateTm.tm_sec = 0;

        // KT 17 Mar 04: Added after running test rigs and finding that mktime
        // wraps the date around if the month and/or day values are invalid.

        if ( dateTm.tm_mon < 0 || dateTm.tm_mon > 11 )
        {
            TA_THROW( TA_Base_Core::ArchiveException( "The month value is invalid" ) );
        }
        if ( dateTm.tm_mday < 1 || dateTm.tm_mday > 31 )
        {
            TA_THROW( TA_Base_Core::ArchiveException( "The day value is invalid" ) );
        }

        time_t time = mktime( &dateTm );
        if ( time == static_cast< time_t >( -1 ) )
        {
            TA_THROW( TA_Base_Core::ArchiveException( "Unable to get date from file name due to conversion error." ) );
        }
        return time;
    }

	time_t getDateFromFileNameDDMMYY( const std::string& fileName )
    {     
		// TD16705

        // The archive files have to be named in the format 
        // ADDMMYYfilepostfix.dmp or MDDMMYYfilepostfix.dmp.

        std::locale loc;
        if( ( 11 >= fileName.length()  ) ||
            ( ( 'A' !=  fileName[0] ) && ( 'M' != fileName[0] ) ) ||
			( !std::isdigit( fileName[1], loc ) ) ||
			( !std::isdigit( fileName[2], loc ) ) ||
			( !std::isdigit( fileName[3], loc ) ) ||
			( !std::isdigit( fileName[4], loc ) ) ||
			( !std::isdigit( fileName[5], loc ) ) ||
			( !std::isdigit( fileName[6], loc ) ) )
        {
            // This message doesn't get displayed.

            CString error;
            error.FormatMessage( UE050110_INVALID_FILENAME_FORMAT, fileName.c_str() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        // Convert the string into a time_t.

        struct tm dateTm;
        dateTm.tm_mday = atoi( fileName.substr( 1, 2 ).c_str() );
        dateTm.tm_mon = atoi( fileName.substr( 3, 2 ).c_str() ) - 1;        // Months go from 0 - 11 in a tm struct.
        dateTm.tm_year = atoi( fileName.substr( 5, 2 ).c_str() ) + 2000 - 1900;    // tm_year is from 1900.
        dateTm.tm_hour = 0;
        dateTm.tm_isdst = -1;
        dateTm.tm_min = 0;
        dateTm.tm_sec = 0;

        // KT 17 Mar 04: Added after running test rigs and finding that mktime
        // wraps the date around if the month and/or day values are invalid.

        if ( dateTm.tm_mon < 0 || dateTm.tm_mon > 11 )
        {
            TA_THROW( TA_Base_Core::ArchiveException( "The month value is invalid" ) );
        }
        if ( dateTm.tm_mday < 1 || dateTm.tm_mday > 31 )
        {
            TA_THROW( TA_Base_Core::ArchiveException( "The day value is invalid" ) );
        }

        time_t time = mktime( &dateTm );
        if ( time == static_cast< time_t >( -1 ) )
        {
            TA_THROW( TA_Base_Core::ArchiveException( "Unable to get date from file name due to conversion error." ) );
        }
        return time;
    }

    std::string getPostfixFromFileName( const std::string& fileName )
    {   
        // KT 10 Mar 04: PW #3136 - Changed '-' to '_' for operating system compatibility.

        /*int lastDash = fileName.rfind( "_" );
        int logPos = fileName.rfind( ".dmp" );
          
		
        TA_ASSERT( lastDash != -1, "The file name is not in the format AYYYY_MM_DD_filepostfix.dmp or MYYYY_MM_DD_filepostfix.dmp." );
        TA_ASSERT( lastDash != fileName.size(), "The file name is not in the format AYYYY_MM_DD_filepostfix.dmp or MYYYY_MM_DD_filepostfix.dmp." );
        TA_ASSERT( logPos != -1, "The file name is not in the format AYYYY_MM_DD_filepostfix.dmp or MYYYY_MM_DD_filepostfix.dmp." );
        TA_ASSERT( logPos != fileName.size(), "The file name is not in the format AYYYY_MM_DD_filepostfix.dmp or MYYYY_MM_DD_filepostfix.dmp." );
        TA_ASSERT( lastDash < logPos - 1, "The file name is not in the format AYYYY_MM_DD_filepostfix.dmp or MYYYY_MM_DD_filepostfix.dmp." );

        return fileName.substr( lastDash + 1, logPos - lastDash - 1 );*/
		
		// TD16705
		return fileName.substr( fileName.rfind( '\\' ) + strlen("?DDMMYY") + 1 , fileName.length() - strlen(getVersionNumber().c_str()) - strlen(".dmp") - strlen("?DDMMYY") -fileName.rfind( '\\' ) -1 );
    }


    std::string replaceBackslashes( const std::string& fileName )
    {
        CString str( fileName.c_str() );
        str.Replace( "\\", "\\\\" );
        return std::string( str );
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    // Operator Information.
    //
    ///////////////////////////////////////////////////////////////////////////


    std::string getOperatorNameFromSessionId()
    {
        // Get the operator name from the database using the sessionId.
        
        try
        {
            std::auto_ptr< TA_Base_Core::IOperator > operatorData(
                TA_Base_Core::OperatorAccessFactory::getInstance().getOperatorFromSession( 
                TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ), false ) );
            if ( 0 == operatorData.get() )
            {
                CString reason;
                reason.LoadString( UE050111_REASON_OP_FAILED );
                CString error;
                error.FormatMessage( UE050111_OPERATOR_NAME, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
            return operatorData->getName();
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            CString message;
            message.FormatMessage( UE050111_OPERATOR_NAME, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( message ) ) );
        }
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            CString message;
            message.FormatMessage( UE050111_OPERATOR_NAME, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( message ) ) );
        }
        TA_ASSERT( false, "The name should already have been returned, or an exception thrown." );
        return "";
    }


    std::string getWorkstationNameFromSessionId()
    {
        // Get the workstation name from the database using the sessionId.
        
        try
        {
            std::auto_ptr< TA_Base_Core::IConsole > consoleData(
                TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( 
                TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ), false ) );
            if ( 0 == consoleData.get() )
            {
                TA_THROW( TA_Base_Core::ArchiveException( "Failed to determine the workstation name from the session id." ) );
            }
            return consoleData->getName();
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            CString message;
            message.FormatMessage( UE050111_OPERATOR_NAME, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( message ) ) );
        }
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            CString message;
            message.FormatMessage( UE050111_OPERATOR_NAME, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( message ) ) );
        }
        TA_ASSERT( false, "The name should already have been returned, or an exception thrown." );
        return "";
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    // Audit message sending.
    //
    ///////////////////////////////////////////////////////////////////////////

    void sendAuditMessage( const TA_Base_Core::MessageType& messageType, const std::string& reason,
        bool isManualArchive )
    {
        // Get the entity key for the Archive Manager.

        unsigned long entityKey = 0;
        try
        {
            std::auto_ptr< TA_Base_Core::IEntityData > data(
                TA_Base_Core::EntityAccessFactory::getInstance().getEntity( 
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ) ) );
            if ( 0 == data.get() )
            {
                CString reason;
                reason.LoadString( UE050113_REASON_ENT_FAILED );
                CString message;
                message.FormatMessage( UE050113_AUDIT_MSG, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( message ) ) );
            }
            entityKey = data->getKey();
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            CString message;
            message.FormatMessage( UE050113_AUDIT_MSG, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( message ) ) );
        }
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            CString message;
            message.FormatMessage( UE050113_AUDIT_MSG, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( message ) ) );
        }
        
        // Set up the description parameters for the message based on the type.
        // Note: TA_Base_Core::ArchiveAudit::ArchiveCopied have an extra parameter.

        TA_Base_Core::DescriptionParameters desc;
        std::auto_ptr<TA_Base_Core::NameValuePair> param1( new TA_Base_Core::NameValuePair("ConsoleName", TA_ArchiveManager::getWorkstationNameFromSessionId()) );
        std::auto_ptr<TA_Base_Core::NameValuePair> param2( NULL ); // Don't know if we need it yet.
        desc.push_back(param1.get());
        
        if( messageType.getTypeKey() == TA_Base_Core::ArchiveAudit::ArchiveCopied.getTypeKey() )
        {
            if ( isManualArchive )
            {
                param2 = std::auto_ptr<TA_Base_Core::NameValuePair>( new TA_Base_Core::NameValuePair("AutoOrManual", MANUALLY) );
            }
            else
            {                
                param2 = std::auto_ptr<TA_Base_Core::NameValuePair>( new TA_Base_Core::NameValuePair("AutoOrManual", AUTOMATICALLY) );
            }
            desc.push_back(param2.get());
        }
        else if ( ( messageType.getTypeKey() == TA_Base_Core::ArchiveAudit::CopyArchiveFailed.getTypeKey()    ) ||
                  ( messageType.getTypeKey() == TA_Base_Core::ArchiveAudit::RestoreArchiveFailed.getTypeKey() ) ||
                  ( messageType.getTypeKey() == TA_Base_Core::ArchiveAudit::DeleteArchiveFailed.getTypeKey()  ) ||
                  ( messageType.getTypeKey() == TA_Base_Core::ArchiveAudit::ChangeDeletionDateFailed.getTypeKey()  ) )
        {
            param2 = std::auto_ptr<TA_Base_Core::NameValuePair>( new TA_Base_Core::NameValuePair("Reason", reason.c_str()) );
            desc.push_back(param2.get());
        }
		else if ( messageType.getTypeKey() == TA_Base_Core::ArchiveAudit::RestoreArchiveRequest.getTypeKey()) 
		{
			param2 = std::auto_ptr<TA_Base_Core::NameValuePair>( new TA_Base_Core::NameValuePair("Details", reason.c_str()) );
            desc.push_back(param2.get());
		}
		//TD16711++
		//Hanhao
		else if ( messageType.getTypeKey() == TA_Base_Core::ArchiveAudit::CopyArchiveRequest.getTypeKey()) 
		{
			param2 = std::auto_ptr<TA_Base_Core::NameValuePair>( new TA_Base_Core::NameValuePair("Details", reason.c_str()) );
            desc.push_back(param2.get());
		}


        // Create an audit message sender.

        std::auto_ptr< TA_Base_Core::AuditMessageSender > auditSender(
            TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::ArchiveAudit::Context ) );
        if ( 0 == auditSender.get() )
        {
            CString error;
            error.LoadString( UE050114_MSG_SENDER );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        // Send the message.

        try
        {
            auditSender->sendAuditMessage(
                messageType,
                entityKey,
                desc,
                "",
                TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ),
                "", "", "" );
        }
        catch( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOGMORE( SourceInfo, "Converting to ArchiveException" );
            CString error;
            error.FormatMessage( UE050115_AUDIT_MSG_FAILED, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        catch( ... )
        {
            CString reason;
            reason.LoadString( UE050115_REASON_UNKNOWN );
            CString error;
            error.FormatMessage( UE050115_AUDIT_MSG_FAILED, reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
    }


    void sendAuditMessageIgnoreErrors( const TA_Base_Core::MessageType& messageType, 
        const std::string& reason, bool isManualArchive )
    {
        // Send an audit message.

        try
        {
            TA_ArchiveManager::sendAuditMessage( messageType, reason, isManualArchive ); 
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            // If the creation of the audit message failed, there isn't anything we can do about it.
        }
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    // Determining rights.
    //
    ///////////////////////////////////////////////////////////////////////////


    TA_Base_Bus::RightsLibrary* getRightsLibrary()
    {
        static TA_Base_Bus::RightsLibrary* library;

        // Create a new rights library if we haven't already.
        if (library == NULL)
        {
            TA_Base_Bus::RightsLibraryFactory factory;
            library = factory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
        }

        return library;
    }

    bool isActionPermitted( unsigned long actionKey )
    {
        // Get the Archive Manager entity key and use it to determine the resource key.

        unsigned long resourceKey = 0;
        try
        {
            std::auto_ptr< TA_Base_Core::IEntityData > data(
                TA_Base_Core::EntityAccessFactory::getInstance().getEntity( 
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ) ) );
            if ( 0 == data.get() )
            {
                CString reason;
                reason.LoadString( UE050087_REASON_UNKNOWN_RESOURCE );
                CString error;
                error.FormatMessage( UE050087_UNKNOWN_RIGHTS, reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }

            std::auto_ptr< TA_Base_Core::IResource > resource( 
                    TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( data->getKey() ) );
            resourceKey = resource->getKey();
        }
        catch( TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            CString error;
            error.FormatMessage( UE050087_UNKNOWN_RIGHTS, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        catch( TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            CString error;
            error.FormatMessage( UE050087_UNKNOWN_RIGHTS, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        TA_Base_Bus::RightsLibrary* library = getRightsLibrary();
        if ( 0 == library )
        {
            CString reason;
            reason.LoadString( UE050087_REASON_RL_ACCESS_FAILURE );
            CString error;
            error.FormatMessage( UE050087_UNKNOWN_RIGHTS, reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        try
        {
            std::string reasonString( "" );
            TA_Base_Bus::EDecisionModule decisionModule;
			//TD17146++
			// modified call to isActionPermittedOnResourceAndLocSub from isActionPermittedOnResource
			TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(resourceKey);
			unsigned long locationKey = entityData->getLocation();
			unsigned long subsystemKey = entityData->getSubsystem();
			delete entityData;
			
            return library->isActionPermittedOnResourceAndLocSub( 
                TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ),
                resourceKey, 
				locationKey,
				subsystemKey,
                actionKey,
                reasonString,
                decisionModule );
			//++TD17146
        }
        catch( TA_Base_Core::RightsException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "RightsException", e.what() );
            CString error;
            error.FormatMessage( UE050087_UNKNOWN_RIGHTS, e.what() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught an unknown exception during rights checking." );
            CString error;
            error.FormatMessage( UE050087_UNKNOWN_RIGHTS, "Caught an unknown exception during rights checking." );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        return false;
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    // Checking file/directory validity.
    //
    ///////////////////////////////////////////////////////////////////////////


    bool isValidDirectory( const std::string& directory )
    {
        // Check that this directory actually exists.

        CFileFind finder;

        // Look for the file.

        BOOL wasFound = finder.FindFile( directory.c_str() );
        if ( wasFound )
        {
            finder.FindNextFile();
            if ( finder.IsDots() || finder.IsDirectory() )
            {
                finder.Close();
                return true;
            }
        }

        finder.Close();

        // KT 25 Mar 04: TD #2240 & TD #2241
        // If we didn't find the directory, it may be a drive, so try that instead. 
        // GetDriveType requires a trailing backslash to be present on the directory path.

        std::string drivePath = directory;
        std::string::reverse_iterator rit = drivePath.rbegin();
        if ( *rit != '\\' )
        {
            drivePath += "\\";
        }

        switch ( GetDriveType( drivePath.c_str() ) )
        {
            case DRIVE_REMOVABLE:   // The disk can be removed from the drive. 
            case DRIVE_FIXED:       // The disk cannot be removed from the drive. 
            case DRIVE_REMOTE:      // The drive is a remote (network) drive. 
            case DRIVE_CDROM:       // The drive is a CD-ROM drive. 
            case DRIVE_RAMDISK:     // The drive is a RAM disk. 
                
                return true;

            case DRIVE_UNKNOWN:     // The drive type cannot be determined. 
            case DRIVE_NO_ROOT_DIR: // The root path is invalid. For example, no volume is mounted at the path. 
            default:

                return false;
        };

        return false;
    }


    bool isValidFile( const std::string& file )
    {
        // Check that this directory actually exists.

        CFileFind finder;

        // Look for the file.

        BOOL wasFound = finder.FindFile( file.c_str() );
        if ( wasFound )
        {
            finder.FindNextFile();
            if ( !finder.IsDots() && !finder.IsDirectory() )
            {
                finder.Close();
                return true;
            }
        }

        finder.Close();
        return false;
    }


    std::vector<std::string> getMediaLocations()
    {
        const int MAX_DRIVES = 26; 
        std::ostringstream drive;
        std::bitset<MAX_DRIVES> drives = GetLogicalDrives(); 
        std::vector<std::string> archiveDrives;
        
        for(int index = 0; index < MAX_DRIVES; index++)
        { 
            // If bit is set, then the drive exists.
            if(drives.test(index))
            { 
                drive.str("");
                drive << static_cast<char>('A' + index) << ":";
                archiveDrives.push_back(drive.str());
            } 
        } 
        
        return archiveDrives;
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    // Processing Log Files
    //
    ///////////////////////////////////////////////////////////////////////////


    TA_ArchiveManager::EErrorStatus processFile( const std::string& file, bool isImporting )
    {
       // Set up the tokens to parse for.

       typedef std::map< std::string, TA_ArchiveManager::EErrorStatus > TokenMap;

        TokenMap tokenMap;
        if( isImporting )
        {
            tokenMap[ IMPORT_SUCC ]   = TA_ArchiveManager::EX_SUCC;
            tokenMap[ IMPORT_OKWARN ] = TA_ArchiveManager::EX_OKWARN;
            tokenMap[ IMPORT_FAIL ]   = TA_ArchiveManager::EX_FAIL;
        }
        else
        {
            tokenMap[ EXPORT_SUCC ]   = TA_ArchiveManager::EX_SUCC;
            tokenMap[ EXPORT_OKWARN ] = TA_ArchiveManager::EX_OKWARN;
            tokenMap[ EXPORT_FAIL ]   = TA_ArchiveManager::EX_FAIL;
        }

        if ( !TA_ArchiveManager::isValidFile( file ) )
        {
           CString reason;
            reason.LoadString( UE050116_REASON_INVALID_FILE );
            CString error;
            error.FormatMessage( UE050116_CHECK_LOG_FAILED, file.c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
       }

        // Open for reading, don't create the file if it doesn't already exist.

        std::ifstream in( file.c_str() );  
        if ( !in.is_open() )
        {
            CString reason;
            reason.LoadString( UE050116_REASON_OPEN_FAILED  );
            CString error;
            error.FormatMessage( UE050116_CHECK_LOG_FAILED, file.c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        std::string line;
        while( std::getline( in, line ) )
        {
            for( TokenMap::iterator it = tokenMap.begin(); it != tokenMap.end(); ++it )
            {
                if ( line.find( it->first, 0 ) != std::string::npos )
                {
                    return it->second;
                }
            }
        }

        return TA_ArchiveManager::UNKNOWN;
    }

     
    void deleteLogFiles( const TA_ArchiveManager::FileStatusMap& files )
    {
        std::stringstream error;
        for( TA_ArchiveManager::FileStatusMap::const_iterator it = files.begin(); it != files.end(); ++it )  
        {
            if( !::DeleteFile( it->second.c_str() ) )
            {
                LPVOID lpMsgBuf;
                FormatMessage( 
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                    FORMAT_MESSAGE_FROM_SYSTEM | 
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPTSTR) &lpMsgBuf,
                    0,
                    NULL 
                );
                
                error << "\n" << it->second << " : " << std::string( (LPCTSTR)lpMsgBuf );
                LocalFree( lpMsgBuf );
            }
        }

        if( !error.str().empty() )
        {
            CString errorStr;
            errorStr.FormatMessage( UW050015_DELETE_LOG_FILES_FAILED, error.str().c_str() );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( errorStr ) ) );
        }
    }

    
    std::string getOracleHomeDirectory()
    {
        const int LENGTH = 100;
        CRegKey registryKey;
        LPTSTR buffer = new TCHAR[LENGTH]; 
        DWORD size = LENGTH * sizeof(TCHAR); 

        registryKey.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Oracle");
        registryKey.QueryValue(buffer, _T("ORACLE_HOME"), &size);

        std::string homeDir(buffer);
        delete[] buffer;

        return homeDir;
    }

	std::string getVersionNumber()
	{
		return "_V2";
	}
	
	std::string Trim(std::string inputString)
	{	
		//remove trailing whitespace
		size_t endpos = inputString.find_last_not_of(" \t");
		if( std::string::npos != endpos )
		{
			inputString = inputString.substr( 0, endpos+1 );
		}
		else
			inputString = "";

		//remove leading whitespace
		size_t startpos = inputString.find_first_not_of(" \t");
		if( std::string::npos != startpos )
		{
			inputString = inputString.substr( startpos );
		}
		else
			inputString = "";

		return inputString;

	}


	CString getStringFromMediaStatus(TA_Base_App::MediaStatus mediaStatus)
	{
		CString mediaStatusText;
		switch(mediaStatus)
		{
		case (TA_Base_App::NoValidDevice):
			mediaStatusText.LoadString(UE050090_REASON_NO_DRIVES);
			break;
		//case (DriveBusy):
		case (TA_Base_App::InvalidDrive):
			mediaStatusText.LoadString(UE050090_REASON_INVALID_DRIVE);
			break;
		case (TA_Base_App::NoMedia):
			mediaStatusText.LoadString(UE050090_REASON_NO_MEDIA);
			break;
		case (TA_Base_App::MediaNotEmpty):
			mediaStatusText.LoadString(UE050090_REASON_MEDIA_NOT_BLANK);
			break;
		case (TA_Base_App::MediaWriteProtected):
			mediaStatusText.LoadString(UE050090_REASON_MEDIA_WRITE_PROTECTED);
			break;
		case (TA_Base_App::MediaOutOfMemory):
			mediaStatusText.LoadString(UE050090_REASON_MEDIA_OUT_OF_MEMORY);
			break;
		case (TA_Base_App::WriteFailed):
			mediaStatusText.LoadString(UE050090_REASON_WRITE_FAILED);
			break;
		case (TA_Base_App::WriteCancelled):
			mediaStatusText.LoadString(UE050090_REASON_WRITE_CANCELLED);
			break;
		case (TA_Base_App::WriteSuccessful):
			mediaStatusText.LoadString(UE050090_REASON_WRITE_SUCCESSFUL);
			break;
		case (TA_Base_App::MediaIncompatible):
			mediaStatusText.LoadString(UE050090_REASON_MEDIA_INCOMPATIBLE);
			break;
		case (TA_Base_App::Unknown):
			mediaStatusText.LoadString(UE050090_REASON_UNKNOWN_ERROR);
			break;

		default:
			mediaStatusText.LoadString(UE050090_REASON_WRITE_FAILED);
			break;
		}

		return mediaStatusText;
	}
} // TA_ArchiveManager
