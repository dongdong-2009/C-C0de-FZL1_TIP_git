/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VmsEnquirer.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Light-weight singleton class to communicate with the VMS Agent
  * for selected enquiry functions only :
  *     - get all signs
  *     - get scenes for a sign
  *
  */

#if defined( WIN32 )
#pragma warning ( disable : 4786 4250 )
#endif // defined( WIN32 )

#include "stdafx.h"

#if defined (STEPTYPE_VMS_SET_SCENE) || defined (STEPTYPE_VMS_BLANKOUT) || defined (STEPTYPE_VMS_RESTORE_SCHEDULE)

#include "core/exceptions/src/TransactiveException.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/data_access_interface/entity_access/src/VmsAgentEntityData.h"
#include "core/synchronisation/src/ThreadLockable.h"
#include "VmsEnquirer.h"

namespace TA_Base_App
{
    namespace TA_Signs
    {

        //////////////////////////////////////////////////////////////////////
        // Construction/Destruction
        //////////////////////////////////////////////////////////////////////

        VmsEnquirer * VmsEnquirer::m_singleton = 0;
        TA_Base_Core::ThreadLockable VmsEnquirer::m_singletonLock;


        VmsEnquirer::VmsEnquirer()
        {
            FUNCTION_ENTRY( "VmsEnquirer" );

            TA_Base_Core::CorbaName name = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity( TA_Base_Core::VmsAgentEntityData::getStaticType() );
            m_signFactory.setCorbaName( name.getAgentName(), "VMS/SignFactory" );
#if defined (STEPTYPE_VMS_SET_SCENE)
            m_sceneFactory.setCorbaName( name.getAgentName(), "VMS/SceneFactory" );
#endif

            FUNCTION_EXIT;
        }


        VmsEnquirer& VmsEnquirer::getInstance()
        {
            FUNCTION_ENTRY( "getInstance" );

            if( m_singleton == 0)
            {
                TA_Base_Core::ThreadGuard guard( m_singletonLock);
                if( m_singleton == 0)
                {
                    m_singleton = new VmsEnquirer();
                }
            }

            FUNCTION_EXIT;
            return (* m_singleton);
        }


        SignObjectSeq const VmsEnquirer::getSigns()
        {
            FUNCTION_ENTRY( "getSigns" );

            SignObjectSeq_var   signObjSeq;

            try
            {
				CORBA_CALL_RETURN( signObjSeq,
				                   m_signFactory,
				                   getAll,
				                   () );
            }
            catch ( TA_Base_Core::ObjectResolutionException& ex )
            {
                std::stringstream ss;
                ss << "VmsEnquirer::getSigns::ObjectResolutionException(" << ex.what() << ")";
                TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
            }

            catch( CORBA::TRANSIENT& ex )
            {
                // Most probable cause: the Agent is no longer available (but was OK before)
                LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::TRANSIENT", std::string( ex.NP_minorString() ) );
                std::stringstream ss;
                ss << "VmsEnquirer::getSigns::CORBA::TRANSIENT(" << ex.NP_minorString() << ")";
                TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
            }

            catch( CORBA::Exception& ex )
            {
                // Most probable cause: the Agent is no longer available (but was OK before)
                std::string exMsg = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex );
                LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", exMsg.c_str() );
                std::stringstream ss;
                ss << "VmsEnquirer::getSigns::CORBA::Exception(" << exMsg.c_str() << ")";
                TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
            }
            catch( TA_Base_Core::TransactiveException& )
            {
                throw;
            }
            catch( ... )
            {
                // Catch everything so we only throw TransactiveExceptions from here.

                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception." );
                TA_THROW( TA_Base_Core::TransactiveException( "VmsEnquirer::getSigns::UnknownException" ));
            }

            FUNCTION_EXIT;
            return( signObjSeq);
        }


#if defined (STEPTYPE_VMS_SET_SCENE)

        SignSpecificObjectSeq const VmsEnquirer::getScenesForSign( unsigned long signKey )
        {
            FUNCTION_ENTRY( "getScenesForSign" );

            SignSpecificObjectSeq_var   ssObjSeq;
            SignSpecificObjectSeq_var   outObjSeq = new SignSpecificObjectSeq;
            SignSpecificObject          scene;

            try
            {
                // There is no way to get just a sign from the VmsAgent ..
                // so get all of them and pick out the one we want.
                SignObjectSeq_var signObjSeq;
				CORBA_CALL_RETURN( signObjSeq,
				                   m_signFactory,
				                   getAll,
				                   () );
                
                SignTypeID lookForTypeOfSign = 0;
                for (int idx=0; idx < signObjSeq->length(); idx++)
                {
                    if ( signObjSeq[idx].entityKey == signKey)
                    {
                        lookForTypeOfSign = signObjSeq[idx].typeOfSign;
                        break;
                    }
                }

                // Throw an exception if we couldn't find the sign
                if (0 >= lookForTypeOfSign)
                {
                    std::stringstream ss;
                    ss << "VmsEnquirer::getScenesForSign: Unable to find sign with entityKey " << signKey;
                    TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
                }

                // Now get all Scenes and pick out the ones with matching sign type

				CORBA_CALL_RETURN( ssObjSeq,
				                   m_sceneFactory,
				                   getAll,
				                   () );

                unsigned int outNum = 0;
                for (idx=0;idx<ssObjSeq->length();idx++)
                {
                    scene = ssObjSeq[idx];
                    if (scene.typeOfSign == lookForTypeOfSign)
                    {
                        outObjSeq->length(outNum+1);
                        outObjSeq[outNum] = scene;
                    }
                }

            }

            catch (TA_Base_Core::ObjectResolutionException& ex)
            {
                std::stringstream ss;
                ss << "VmsEnquirer::getSigns::ObjectResolutionException(" << ex.what() << ")";
                TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
            }
            catch( CORBA::TRANSIENT& ex)
            {
                // Most probable cause: the Agent is no longer available (but was OK before)
                LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::TRANSIENT", std::string( ex.NP_minorString() ) );
                std::stringstream ss;
                ss << "VmsEnquirer::getSigns::CORBA::TRANSIENT(" << ex.NP_minorString() << ")";
                TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
            }
            catch( CORBA::Exception& ex)
            {
                // Most probable cause: the Agent is no longer available (but was OK before)
                std::string exMsg = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex );
                LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", exMsg.c_str() );
                std::stringstream ss;
                ss << "VmsEnquirer::getSigns::CORBA::Exception(" << exMsg.c_str() << ")";
                TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
            }
            catch( TA_Base_Core::TransactiveException& )
            {
                throw;
            }
            catch( ... )
            {
                // Catch everything so we only throw TransactiveExceptions from here.

                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception." );
                TA_THROW( TA_Base_Core::TransactiveException( "VmsEnquirer::getSigns::UnknownException" ));
            }

            FUNCTION_EXIT;
            return( outObjSeq);
        }


        std::string VmsEnquirer::getVmsSceneName( unsigned long vmsSceneId )
        {
            FUNCTION_ENTRY( "getVmsSceneName" );

            try
            {
                // Get all Scenes and pick out the one we're interested in.
                SignSpecificObjectSeq_var ssObjSeq;
				CORBA_CALL_RETURN( ssObjSeq,
				                   m_sceneFactory,
				                   getAll,
				                   () );
                SignSpecificObject scene;
                for (int idx=0;idx<ssObjSeq->length();idx++)
                {
                    scene = ssObjSeq[idx];
                    if (vmsSceneId == scene.id)
                    {
                        FUNCTION_EXIT;
                        return scene.name;
                    }
                }

            }

            catch (TA_Base_Core::ObjectResolutionException& ex)
            {
                std::stringstream ss;
                ss << "VmsEnquirer::getVmsSceneName::ObjectResolutionException(" << ex.what() << ")";
                TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
            }

            catch( CORBA::TRANSIENT& ex)
            {
                // Most probable cause: the Agent is no longer available (but was OK before)
                LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::TRANSIENT", std::string( ex.NP_minorString() ) );
                std::stringstream ss;
                ss << "VmsEnquirer::getVmsSceneName::CORBA::TRANSIENT(" << ex.NP_minorString() << ")";
                TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
            }

            catch( CORBA::Exception& ex)
            {
                // Most probable cause: the Agent is no longer available (but was OK before)
                std::string exMsg = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex );
                LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", exMsg.c_str() );
                std::stringstream ss;
                ss << "VmsEnquirer::getVmsSceneName::CORBA::Exception(" << exMsg.c_str() << ")";
                TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
            }

            catch( TA_Base_Core::TransactiveException& )
            {
                throw;
            }

            catch( ... )
            {
                // Catch everything so we only throw TransactiveExceptions from here.

                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception." );
                TA_THROW( TA_Base_Core::TransactiveException( "VmsEnquirer::getVmsSceneName::UnknownException" ));
            }

            std::stringstream ss;
            ss << "The VMS scene with id " << vmsSceneId << " was not found.";
            TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));

            FUNCTION_EXIT;
            return "";
        }


#endif


        std::string VmsEnquirer::getVmsName( unsigned long vmsEntityKey )
        {
            FUNCTION_ENTRY( "getVmsName" );

            // TODO: Ideally you would just ask for the one sign, but the VmsEnquirer doesn't
            // support this yet.

            TA_Signs::SignObjectSeq signs = TA_Signs::VmsEnquirer::getInstance().getSigns();

            // Loop through the signs.

            for ( unsigned int i = 0; i < signs.length(); i++ )
            {
                if ( vmsEntityKey == signs[i].entityKey )
                {
                    FUNCTION_EXIT;
                    return signs[i].name;
                }
            }
            std::stringstream ss;
            ss << "The VMS with entity key " << vmsEntityKey << " was not found.";
            TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));

            FUNCTION_EXIT;
            return "";
        }


    } // TA_Signs
} // TA_Base_App

#endif // STEPTYPE_VMS_SET_SCENE || STEPTYPE_VMS_BLANKOUT || STEPTYPE_VMS_RESTORE_SCHEDULE

