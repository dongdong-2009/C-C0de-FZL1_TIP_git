/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_raiser/src/main.cpp $
* @author:  Darren Gilbert
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2012/06/12 13:35:44 $
* Last modified by:  $Author: builder $
*
* This class implements the AlarmRaiser class.
* 
* The commandline parameters are as follows:
*
* EntityName - The configuration entity the application will use to 
* retrieve information such as username, password, location and profile.
* Example:    --EntityName=AlarmRaiser
*
* DbConnection - Lists the database connection to use.
* Example:    --DbConnection=transact:devtra:devtra
*
* NotifyHosts - Hosts where notification agent is running.
* Example:    --NotifyHosts=itpms-2:6667,itpms-2:6668
*
* AlarmType - Alarm type name as identified in the corba messages register [4]
* Example:    --AlarmType=NetworkFailureAlarm
* 
* Unique - Whether you want an alarm to be unique or not [5].  The value to use 
* should be either FALSE or TRUE.
* Example:    --Unique=FALSE
* AlarmEntity - The entity that the alarm is raised against.
* If this is not specified then the entity specified by EntityName will be used.
* Example:    --AlarmEntity=mft-itpms-2
*
* DescriptionParameters - Parameters for the description field when submitting an alarm.
* A ";"will separate the parameters and an "=" will separate the parameter and its 
* corresponding value for example the following describes a description field with 
* parameters Type, Name and Address:
* 
*
* Example: --DescriptionParameters="Type=PC;Name=Portia;Address=192.168.70.109"
*
* The output description from the examples above was "PC Portia at 192.168.70.109 is not contactable"
*/

#ifdef WIN32
#pragma warning (disable:4786)
#endif
#include <iterator>
#include <string>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/MessagePublicationManager.h"

#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/AlarmRaiserEntityData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/threads/src/Thread.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"

#include "core/naming/src/Naming.h"
#include "core/alarm/src/AlarmPublicationManager.h"

typedef boost::char_separator<char> SeparatorType;
typedef boost::tokenizer<SeparatorType> TokenizerType;

namespace
{
    template <typename T>
        T* getPointer(T& t)
    {
        return(&t);
    }
    /**
    * dummy
    * This function instantiates the template to ensure there are no unresolved symbols
    */
    void dummy()
    {
        TA_Base_Core::NameValuePair nvp("nvp", "");
        TA_Base_Core::NameValuePair* pnvp = getPointer(nvp);
    }
    
    const std::string UE040028("UE040028-Invalid command line argument %s, will abort.");
    const std::string UE040007("UE040007-Information returned from the database invalid.");
    const std::string UE040013("UE040013-Failed to retrieve entity %s from database.");
    const std::string UE040036("UE040036-Database error occurred.");
    const std::string UE040037("UE040037-Unable to create Alarm Type Data %s, will abort.");
    const std::string UNIQUE("TRUE");
}

/**
* getParameters
*
*/
std::vector<TA_Base_Core::NameValuePair> getParameters( std::string const& p_strRawParameters )
{
    //The input Description Paramaters will be formatted like this if the Alarm Type is a NetworkFailureAlarm:
    //"Type=PC;Name=Portia;Address=192.168.70.109"

    std::vector<TA_Base_Core::NameValuePair> description;
    SeparatorType separator(";");
        
    //Seperate using the semi-colon
    TokenizerType tokens(p_strRawParameters, separator);
    TokenizerType::iterator begin(tokens.begin());
    //See how many token pairs there are
    size_t data = std::distance(begin, tokens.end());
    std::string beginToken = "";
    
    while ( data>0 )
    {
		std::string s(*begin++);
		std::string name("");
		std::string value("");
		std::string::size_type equals = s.find_first_of("=");
		if( std::string::npos == equals )
		{
			name = s;
		}
		else
		{
			name = s.substr(0, equals);
			value = s.substr(equals+1, std::string::npos);
		}
		description.push_back(TA_Base_Core::NameValuePair(name, value));
		data-=1;
    }
    
    return description;
}
/**
* checkCommandLine
*
*/
bool checkCommandLine()
{
    char  buffer[2000]="";

    std::string entityName ( TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ));
    if ( entityName.empty() )
    {
        sprintf( buffer,UE040028.c_str(),RPARAM_ENTITYNAME );  
        std::cerr<<buffer<<std::endl;
        LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,UE040028.c_str() ,RPARAM_ENTITYNAME);
        return(1);
    }
    std::string dbConnection ( TA_Base_Core::RunParams::getInstance().get( RPARAM_DBCONNECTION ));
    if ( dbConnection.empty() )
    {
        sprintf( buffer,UE040028.c_str(),RPARAM_DBCONNECTION );  
        std::cerr<<buffer<<std::endl;
        LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,UE040028.c_str() , RPARAM_DBCONNECTION);   
        return(1);
    }
    std::string notifyHosts ( TA_Base_Core::RunParams::getInstance().get( RPARAM_NOTIFYHOSTS ));
    if ( notifyHosts.empty() )
    {
        sprintf( buffer,UE040028.c_str(),RPARAM_NOTIFYHOSTS );  
        std::cerr<<buffer<<std::endl;
        LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,UE040028.c_str() , RPARAM_NOTIFYHOSTS);
        return(1);
    }
    std::string alarmTypeName ( TA_Base_Core::RunParams::getInstance().get( "AlarmType" ));
    if ( alarmTypeName.empty() )
    {
        sprintf( buffer,UE040028.c_str(),"AlarmType" );  
        std::cerr<<buffer<<std::endl;
        LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,UE040028.c_str() , "AlarmType");
        return(1);
    }
    std::string unique( TA_Base_Core::RunParams::getInstance().get( "Unique" ));
    if ( unique.empty() )
    {
        //No need to abort, Alarm will be Non-Unique.
        LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,UE040028.c_str() , "Unique");
    }
    std::string descriptionParameters( TA_Base_Core::RunParams::getInstance().get( "DescriptionParameters" ));
    if ( descriptionParameters.empty() ) 
    {
        //No need to abort, only invalid description parameters.
        LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,UE040028.c_str() , "DescriptionParameters");
    }
    std::string alarmEntity ( TA_Base_Core::RunParams::getInstance().get( "AlarmEntity" ));
    if ( alarmEntity.empty() )
    {
        //Use the EntityName instead
        TA_Base_Core::RunParams::getInstance().set("AlarmEntity", TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str());
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"AlarmEntity not specified using AlarmRaiser entity %s",entityName.c_str());
    }

    return(true);
}

/**
* MAINLINE
*
*/
int main(int argc, char** argv)
{   
    char  buffer[2000]="";

    TA_Base_Core::RunParams::getInstance().parseCmdLine(argc,argv);
    
    if( !checkCommandLine() )
    {
        return(1);
    }

    // get command line values
    std::string entityName ( TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ));
    std::string dbConnection ( TA_Base_Core::RunParams::getInstance().get( RPARAM_DBCONNECTION ));
    std::string notifyHosts ( TA_Base_Core::RunParams::getInstance().get( RPARAM_NOTIFYHOSTS ));
    std::string alarmTypeName ( TA_Base_Core::RunParams::getInstance().get( "AlarmType" ));
    std::string unique( TA_Base_Core::RunParams::getInstance().get( "Unique" ));
    std::string descriptionParameters( TA_Base_Core::RunParams::getInstance().get( "DescriptionParameters" ));
    std::string alarmEntity ( TA_Base_Core::RunParams::getInstance().get( "AlarmEntity" ));
    
    TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugInfo);
    TA_Base_Core::CorbaUtil::getInstance().initialise();
    TA_Base_Core::CorbaUtil::getInstance().activate();
    
    try
    {
        // use AlarmRaiser entity to get the login details and set the location
        //std::auto_ptr< TA_Base_Core::AlarmRaiserEntityData > alarmRaiserEntityData(
        //dynamic_cast<TA_Base_Core::AlarmRaiserEntityData*>
		  std::auto_ptr<TA_Base_Core::IEntityData> entityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName.c_str()));
        TA_Base_Core::AlarmRaiserEntityData* alarmRaiserEntityData = dynamic_cast<TA_Base_Core::AlarmRaiserEntityData *>(entityData.get());
        
		  if( alarmRaiserEntityData == NULL )
        {
            sprintf( buffer,UE040013.c_str(),entityName.c_str() );  
            std::cerr<<buffer<<std::endl;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,UE040013.c_str(),entityName.c_str());
            TA_THROW(TA_Base_Core::TransactiveException(buffer));
        }
        
        TA_Base_Core::RunParams::getInstance().set( RPARAM_LOCATIONKEY, boost::lexical_cast<std::string>(alarmRaiserEntityData->getLocation()).c_str());

        // get session
        TA_Base_Bus::AuthenticationLibrary authLib; 
        std::string session(authLib.requestSession(alarmRaiserEntityData->getUserId(),
            alarmRaiserEntityData->getProfileId(),
            alarmRaiserEntityData->getLocation(),
            alarmRaiserEntityData->getWorkStationId(),
            alarmRaiserEntityData->getPassword()));    
        
        // get the entity to raise the alarm on
        std::auto_ptr< TA_Base_Core::IEntityData > alarmEntityData(
            TA_Base_Core::EntityAccessFactory::getInstance().getEntity(alarmEntity.c_str()));
        if( alarmEntityData.get() == NULL )
        {
            sprintf( buffer,UE040013.c_str(),alarmEntity.c_str() );  
            std::cerr<<buffer<<std::endl;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,UE040013.c_str(),alarmEntity.c_str());
            TA_THROW(TA_Base_Core::TransactiveException(buffer));
        }
        
        // get the alarm type to raise
        std::auto_ptr<TA_Base_Core::IAlarmTypeData> alarmTypeData(
            TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmType(alarmTypeName.c_str(), false));
        
        if( alarmTypeData.get() == NULL )
        {
            sprintf( buffer,UE040037.c_str(),alarmTypeName.c_str() );  
            std::cerr<<buffer<<std::endl;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,UE040037.c_str(),alarmTypeName.c_str());
            TA_THROW( TA_Base_Core::TransactiveException(buffer) );
        }
        
        // raise the alarm
        std::string contextName = alarmTypeData->getContextName();
        std::string alarmTypeDataKey = boost::lexical_cast<std::string>(alarmTypeData->getKey());
        TA_Base_Core::MessageContext context( "Alarm",contextName.c_str() , TA_Base_Core::AlarmMessage );
        TA_Base_Core::MessageType alarm( context, alarmTypeDataKey.c_str() );
        
        std::vector <TA_Base_Core::NameValuePair> parameters;
        parameters = getParameters(descriptionParameters);
        TA_Base_Core::DescriptionParameters dp(parameters.size());
        std::transform(parameters.begin(), parameters.end(), dp.begin(), getPointer<TA_Base_Core::NameValuePair>);
        
        
        if (unique == UNIQUE)
        {
            TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarm(
                alarm,
                alarmEntityData->getKey(),
                alarmEntityData->getTypeKey(),
                dp,
                alarmEntityData->getName(),
                alarmEntityData->getLocation(),
                alarmEntityData->getSubsystem());
			TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
        }
        else
        {
            TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarm(
                alarm,
                alarmEntityData->getKey(),
                alarmEntityData->getTypeKey(),
                dp,
                alarmEntityData->getName(),
                alarmEntityData->getLocation(),
                alarmEntityData->getSubsystem());
			TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
        }
        
    }
    catch( TA_Base_Core::DatabaseException& )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", UE040036.c_str() );
        std::cerr << UE040036.c_str() <<std::endl;
        return(1);
    }
    catch( TA_Base_Core::DataException& )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "DataException",UE040007.c_str() );
        std::cerr <<UE040007.c_str()<<std::endl;
        return(1);
    }
    catch(...)
    {
        return(1);
    }	

    //TD4479 - need to wait till the alarm has been sent before commencing cleanup
    TA_Base_Core::Thread::sleep( 250 );

	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

//	TA_Base_Core::AlarmPublicationManager::getInstance().cleanUpSuppliers();

	TA_Base_Core::Naming::cleanUp();

//	TA_Base_Core::MessagePublicationManager::getInstance().cleanUpSuppliers();

	TA_Base_Core::CorbaUtil::getInstance().shutdown();

	TA_Base_Core::CorbaUtil::cleanup();

    return( 0 );
}

