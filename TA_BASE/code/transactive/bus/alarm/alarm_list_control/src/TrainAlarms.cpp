/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/TrainAlarms.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  */
// TrainAlarms.cpp: implementation of the TrainAlarms class.
//
//////////////////////////////////////////////////////////////////////

#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "bus/alarm/alarm_list_control/src/alarmlistctrl.h"
#include "bus/alarm/alarm_list_control/src/TrainAlarms.h"
#include "bus/alarm/alarm_list_control/src/LocationSelectionDialog.h"
#include "bus/alarm/alarm_list_control/src/TrainAgentAccess.h"


#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainExceptionsCorba.h"

#include "core/data_access_interface/entity_access/src/TrainCctvCameraEntityData.h"
#include "core/data_access_interface/entity_access/src/tvssagententitydata.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"

#include "core/utilities/src/RunParams.h"
#include "core\utilities\src\DebugUtil.h"

#include <sstream>
#include <iomanip>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CCTV_ACTIVATE_CATCH_HANDLER()\
catch (const TA_Base_Core::TransactiveException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", ex.what() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.what();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Core::OperationModeException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", ex.reason.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << std::string( "Operation mode error" );\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Core::AccessDeniedException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", ex.reason.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << std::string( "Access Denied" );\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::InvalidTrainException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::InvalidParameterException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransmissionFailureException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransmissionFailureException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransactionTimeoutException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransactionTimeoutException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransactionCancelledException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransactionCancelledException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::MpuChangeoverException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::MpuChangeoverException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::BadCommandException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::BadCommandException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TrainCctvException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TrainCctvException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( ... )\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "...", "..." );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_650019);\
}


#define CCTV_DEACTIVATE_CATCH_HANDLER( TrainId )\
catch (const TA_Base_Core::TransactiveException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", ex.what() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.what();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Core::OperationModeException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", ex.reason.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << std::string( "Operation mode error" );\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Core::AccessDeniedException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", ex.reason.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << std::string( "Access Denied" );\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::InvalidTrainException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::InvalidParameterException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransmissionFailureException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransmissionFailureException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransactionTimeoutException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransactionTimeoutException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransactionCancelledException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransactionCancelledException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::MpuChangeoverException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::MpuChangeoverException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::BadCommandException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::BadCommandException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
    catch ( TA_Base_Bus::ITrainExceptionsCorba::TrainCctvException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TrainCctvException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( ... )\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "...", "..." );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\


namespace TA_Base_Bus
{
	TrainAlarms::TrainAlarms()// : m_locationSelectionDialog(0)
	{
        FUNCTION_ENTRY( "TrainAlarms" );
        FUNCTION_EXIT;
   	}


	TrainAlarms::~TrainAlarms()
	{
        FUNCTION_ENTRY( "~TrainAlarms" );
        
// 		if (NULL == m_locationSelectionDialog)
// 		{
// 			delete m_locationSelectionDialog;
// 			m_locationSelectionDialog  = NULL;
// 		}
       
        FUNCTION_EXIT;
	}

	TrainAlarms& TrainAlarms::getInstance()
	{
        FUNCTION_ENTRY( "getInstance" );
        
        FUNCTION_EXIT;
		return *( TrainAlarmsSingleton::instance() );   
	}

	void TrainAlarms::acknowledgeAlarm(TA_Base_Bus::AlarmDataItem* details)
    {
        FUNCTION_ENTRY( "acknowledgeAlarm" );
		
		//step 1 get the tvssagent corba
		std::string strTVSSAgentName = TA_Base_Core::TVSSAgentEntityData::getStaticType();
        TA_Base_Core::CorbaNameList names = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(strTVSSAgentName, details->locationKey,true );
        
        //step 2 call interface to tvssAcknowledgeAlarm
        try
        {
			if (names.size() == 0)
			{
				char szMsg[255] = {0};
				sprintf(szMsg, "Configuration error for entity name for %s at location %ld. Please contact your administrator.", strTVSSAgentName.c_str(), details->locationKey);
				TA_THROW( TA_Base_Core::TransactiveException(szMsg));
			}
			m_tvssManagerNameObj.setCorbaName(names[0]);
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "call tvssAcknowledgeAlarm,entitykey=%ul,sessionId=%s",details->assocEntityKey,sessionId.c_str());
            CORBA_CALL(m_tvssManagerNameObj,tvssAcknowledgeAlarm,(details->assocEntityKey,sessionId.c_str()));
        }
        catch (TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
        {
            // object resolution etc
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TVSSException", e.what );
            MessageBox(NULL,e.what,"TVSSException",MB_OK);
        }
        catch (TA_Base_Core::TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
            MessageBox(NULL,te.what(),"TransactiveException",MB_OK);
        }
        catch( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while trainBorneAcknowleageAlarm: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
            MessageBox(NULL,exceptionMsg.c_str(),"CORBA Exception",MB_OK);
        }



        FUNCTION_EXIT;
    }

// Note:
// Comment these two functions because we are not using this.

//     bool TrainAlarms::activateTrain( unsigned char trainId,
//                                      TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence cameraIds,
//                                      std::string sessionId )
//     {
//         FUNCTION_ENTRY( "activateTrain" );
// 
//         try
//         {
//             // Get the train information from the train information agent
//             ITrainInformationCorba::TrainDetails_var trainInfo;
//             CORBA_CALL_RETURN( trainInfo, TrainAgentAccess::getInstance().getTrainInformationAgent(), getTrainDetails, ( trainId ) );//limin
//     
//             bool delocalised = false;
//             unsigned long overrideLocation = 0;
//     
//             // if the ats information is invalid, or the localisation is invalid
//             if ( ( false == trainInfo->atsValid ) ||
//                  ( false == trainInfo->isLocalisationValid ) )
//             {
//                 // create a m_locationSelectionDialog if it is not created yet
//                 if ( NULL == m_locationSelectionDialog )
//                 {
//                     m_locationSelectionDialog = new LocationSelectionDialog();
//                 }
//                 
//                 // using the location in the information set the initial location in m_locationSelectionDialog
//                 m_locationSelectionDialog->setInitialSelection( trainInfo->currentLocation );
//             
//                 // call DoModal
//                 if ( IDOK != m_locationSelectionDialog->DoModal() )
//                 {
//                     // if the return value is not IDOK, return
//                     return false;
//                 }
//     
//                 // if the return value is IDOK, get the selected location from the dialog
//                 delocalised = true;
//                 overrideLocation = m_locationSelectionDialog->getSelectedLocation();
//     
//                 if ( 0 == overrideLocation )
//                 {
//                     return false;
//                 }
//             }
//     
//             try
//             {
// 				TA_Base_Bus::ITrainCctvCorba::CctvSwitchState SwitchState;
//                 CORBA_CALL_RETURN( SwitchState, TrainAgentAccess::getInstance().getTrainCctvAgent(), getCctvState, ( trainId ) );//limin
// 
// 				if ( TA_Base_Bus::ITrainCctvCorba::CctvQuad == SwitchState.cctvState && SwitchState.item4 != 0 )
// 				{
// 					TA_Base_Bus::TransActiveMessage userMsg;
// 
// 					std::ostringstream train;
// 					train << "Train "
// 							<< long(trainInfo->trainId);
// 
// 					std::ostringstream newCameras;
// 					std::ostringstream oldCameras;
// 
// 					switch( cameraIds.length() )
// 					{
// 					case 1:
// 						newCameras << "camera ";
// 						oldCameras << "camera "
// 								   << long(SwitchState.item4);
// 						break;
// 					case 2:
// 						newCameras << "cameras ";
// 						oldCameras << "cameras "
// 								   << long(SwitchState.item4) 
// 								   << ","
// 								   << long(SwitchState.item3);
// 					    break;
// 					default:
// 						newCameras << "cameras ";
// 						oldCameras << "the old cameras";
// 					    break;
// 					}
// 
// 					for( unsigned int i = 0; i < cameraIds.length(); i ++ ) 
// 					{
// 						newCameras << cameraIds[i] << " ";
// 					}
// 
// 					userMsg << train.str();
// 					userMsg << oldCameras.str();
// 					userMsg << newCameras.str();
// 					
// 					UINT returnVal = userMsg.showMsgBox( IDS_UW_090026 );
// 					if( IDYES != returnVal )
// 					{
// 						return true;
// 					}
// 				}
// 
//                 // activate the CCTV using the train cctv agent
//                 // depending on whether the location is to be overridden, and whether a location was selected, call either addAlarmImage or addAlarmImageDelocalisedTrain
//                 if ( delocalised )
//                 {
//                     CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), addAlarmImageDelocalisedTrain, ( trainId, cameraIds, overrideLocation, sessionId.c_str() ) );//limin
//                 }
//                 else
//                 {
//                     CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), addAlarmImage, ( trainId, cameraIds, sessionId.c_str() ) );//limin
//                 }
//     
//                 return true;
//             }
//             catch ( ITrainExceptionsCorba::TrainCctvConflictException& ex)
//             {
//                 std::string message = ex.details.in();
//                 LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TrainCctvConflictException", message.c_str() );
//             
//                 // catch any cctv conflict exceptions and prompt the user to deactivate a train
//                 TA_Base_Bus::TransActiveMessage userMsg;
//                 CString error = message.c_str();
//                 userMsg << error;
//                 UINT selectedButton = userMsg.showMsgBox( IDS_UE_650018 );
//                     
//                 if( IDOK == selectedButton )
//                 {
//                     // once the train is deactivated, try to activate the camera again
// 
//                     std::ostringstream trainIdString;
//                     trainIdString << "PV" << std::setfill('0') << std::setw(2) << ex.owningTrain;
//                     try
//                     {
//                         CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), setCarrierOff, ( ex.owningTrain, sessionId.c_str() ) );//limin
//                     }
//                     CCTV_DEACTIVATE_CATCH_HANDLER( trainIdString.str() );
// 
//                 
//                     if ( delocalised )
//                     {
//                         CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), addAlarmImageDelocalisedTrain, ( trainId, cameraIds, overrideLocation, sessionId.c_str() ) );//limin
//                     }
//                     else
//                     {
//                         CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), addAlarmImage, ( trainId, cameraIds, sessionId.c_str() ) );//limin
//                     }
//     
//                     FUNCTION_EXIT;
//                     return true;
//                 }
//             }
//         }
//         CCTV_ACTIVATE_CATCH_HANDLER();
//     
//         FUNCTION_EXIT;
//         return false;
//     }
// 
// 
//     void TrainAlarms::switchMonitor(unsigned long location, unsigned char trainId, TA_Base_Bus::SwitchManagerCorbaDef::TrainCameraSequence cameraIds, unsigned long consoleKey)
//     {
//         FUNCTION_ENTRY( "switchMonitor" );
// 
//         // Use VideoSwitchAgentFactory to activate the cameras using acknowledgeTrainCameraAlarm
//         // catch the exceptions and display the error message
//         try
//         {
//             CORBA_CALL( VideoSwitchAgentFactory::getInstance().getSwitchManager(), acknowledgeTrainCameraAlarm, (
//                     location, trainId, cameraIds, consoleKey ) );//limin
//         }
//         catch (TA_Base_Core::TransactiveException& te)
//         {
//             LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
//         
//             TA_Base_Bus::TransActiveMessage userMsg;
//             CString temp = te.what();
//             CString description = "switching trainborne image to monitor: " + temp;
//             userMsg << description;
//             UINT selectedButton = userMsg.showMsgBox(IDS_UW_632015);
//         }
//         catch (TA_Base_Bus::VideoSwitchAgentException& ex)
//         {
//             LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", ex.what.in() );
//     
//             TA_Base_Bus::TransActiveMessage userMsg;
//             CString temp = ex.what.in();
//             CString description = "switching trainborne image to monitor: " + temp;
//             userMsg << description;
//             UINT selectedButton = userMsg.showMsgBox(IDS_UW_632015);
//         }
//         catch( const CORBA::Exception& cex )
//         {
//             LOG_EXCEPTION_CATCH( SourceInfo, "CORBA", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ).c_str() );
//     
//             TA_Base_Bus::TransActiveMessage userMsg;
//             CString temp = "switching trainborne image to monitor: ";
//             std::string temp2 = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex );
//             CString description =  temp + temp2.c_str();
//             userMsg << description;
//             UINT selectedButton = userMsg.showMsgBox(IDS_UW_632015);
//         }
//         catch(...)
//         {
//             LOG_EXCEPTION_CATCH( SourceInfo, "...", "While switching monitor" );
// 
//             TA_Base_Bus::TransActiveMessage userMsg;
//             CString description = "switching trainborne image to monitor: ";
//             userMsg << description;
//             UINT selectedButton = userMsg.showMsgBox(IDS_UW_632015);
//         }
// 
//         FUNCTION_EXIT;
//     }

}
