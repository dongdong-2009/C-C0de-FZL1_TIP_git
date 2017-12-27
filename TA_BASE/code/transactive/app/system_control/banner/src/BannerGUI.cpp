/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/src/BannerGUI.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/13 16:55:49 $
  * Last modified by:  $Author: lu.xiangmei $
  *
  */
// BannerGUI.cpp: implementation of the BannerGUI class.
//
//////////////////////////////////////////////////////////////////////

#include "app/system_control/banner/src/stdafx.h"
#include "app/system_control/banner/src/banner.h"
#include "app/system_control/banner/src/BannerGUI.h"
#include "app/system_control/banner/src/BannerDlg.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveDialog.h"

#include "core/data_access_interface/entity_access/src/Banner.h"
#include "core/data_access_interface/plans/src/AlarmPlanAssocAccessFactory.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

//DTL-Bannner-PositionWrong - start
#include "core/data_access_interface/entity_access/src/Banner.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataException.h"
#include "core\exceptions\src\EntityTypeException.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"

using TA_Base_Core::TransactiveException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::EntityTypeException;
using TA_Base_Core::IEntityData;
//DTL-Bannner-PositionWrong - end

using TA_Base_Core::Banner;
using TA_Base_Core::GenericGUIException;
using TA_Base_Core::DebugUtil;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    BannerGUI::BannerGUI()
    :
	AbstractDialogGUI( IDD_BANNER_DIALOG ), 
		m_bannerEntity(0) //DTL-Bannner-PositionWrong
    {
    }


    BannerGUI::~BannerGUI()
    {
    }


    void BannerGUI::createApplicationObject()
    {
		//TD18095, jianghp, to fix the performance of showing manager application
		BannerDlg * pDlg = new BannerDlg(*this); 
		//TD18095, jianghp, to fix the performance of showing manager application
       setApplicationWnd( *pDlg );
    }


    unsigned long BannerGUI::getApplicationType()
    {
	    return BANNER_GUI_APPTYPE;
    }


    void BannerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
	    setGuiEntity(*guiEntity);

        m_bannerEntity = dynamic_cast<Banner*>(guiEntity);
        if(m_bannerEntity == NULL)
        {
           TA_THROW(GenericGUIException("Entity configuration is not of type Banner!", GenericGUIException::ENTITY_CONFIGURATION_INVALID));
        }
    }


    void BannerGUI::checkCommandLine()
    {
    }


    void BannerGUI::entityChanged(const std::vector<std::string>& changes)
    {
	    //TODO: This will be called by GenericGUI when it receives a callback
	    //indicating that the GUI entity has been modified. GenericGUI will have
	    //invalidated the entity database object and will tell the GUI which
	    //items were modified. This method just has to retrieve the details out
	    //of it and update accordingly. If the details in it are invalid then this
	    //should print a warning message to the user and continue to use the old settings (ie do not update the GUI).
    }


    void BannerGUI::serverIsDown()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
	    //up or down. The GUI should be updated to indicate the network status.
	    //If the server has gone down it should enter some sort of read-only mode and
	    //if the server comes back up it can restore to the correct operating mode.
    }


    void BannerGUI::serverIsUp()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
	    //up or down. The GUI should be updated to indicate the network status.
	    //If the server has gone down it should enter some sort of read-only mode and
	    //if the server comes back up it can restore to the correct operating mode.
    }


    void BannerGUI::loadAllUserSettings()
    {
		// DTL-Bannner-PositionWrong - start
		// replace code to directly access the banner entity from database and position the banner window
		// to its correct position and avoid displaying the banner on the center of the screen.

		if (m_bannerEntity!=NULL) return;

		//
		// Retrieve the entity name from RunParams to ensure it was on the command line.
		//
		std::string entityName = TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME );
		if ( entityName.empty() )
		{
			TA_THROW ( GenericGUIException( "No entity specified on command line (ie --entity-name not found)", GenericGUIException::COMMAND_LINE_INVALID ) );
		}

		//
		// Load the entity and check the configuration is correct
		//
		try
		{
			TA_Base_Core::IEntityData* guiEntity;
			guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );
			TA_ASSERT ( guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception." );

			m_bannerEntity = dynamic_cast<Banner*>(guiEntity);
			if(m_bannerEntity == NULL)
			{
				TA_THROW(GenericGUIException("Entity configuration is not of type Banner!", GenericGUIException::ENTITY_CONFIGURATION_INVALID));
			}
			getApplicationWnd()->MoveWindow( m_bannerEntity->getLeftPosition(), m_bannerEntity->getTopPosition(), 
				m_bannerEntity->getWidth(), m_bannerEntity->getHeight() );
		}
		catch ( const DatabaseException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch,"DatabaseException", "Will change this exception into a GenericGUIException. We don't care what happened just that we can't connect to the database" );
			TA_THROW ( GenericGUIException( GenericGUIException::NO_DATABASE_CONNECTION ) );
		}
		catch ( const EntityTypeException& )
		{
			LOG(SourceInfo,DebugUtil::ExceptionCatch,"EntityTypeException","Will change this exception into a GenericGUIException.");
			TA_THROW ( GenericGUIException( GenericGUIException::ENTITY_CONFIGURATION_INVALID ) );
		}
		catch ( const TA_Base_Core::DataException& ex )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch,"DataException", "Will change this exception into a GenericGUIException." );
			switch ( ex.getFailType() )
			{
			case ( DataException::WRONG_TYPE ):
				TA_THROW ( GenericGUIException( GenericGUIException::ENTITY_CONFIGURATION_INVALID ) );
				break;
			case ( DataException::NO_VALUE ):
				TA_THROW ( GenericGUIException( GenericGUIException::ENTITY_DOES_NOT_EXIST ) );
				break;
			case ( DataException::NOT_UNIQUE ):
				TA_THROW ( GenericGUIException( GenericGUIException::ENTITY_NOT_UNIQUE ) );
				break;
			default:
				TA_ASSERT(false, "Do not recognise the DataException type");
			}
		}
		//setWindowPosition();
		// DTL-Bannner-PositionWrong - end
    }


    void BannerGUI::saveAllUserSettings()
    {
	    //This method does nothing.
	    //The method defintion is needed to instantiate the abstract base class.
	    //There are no user settings for the Banner.
    }


    void BannerGUI::prepareForClose()
    {
	    //TODO: This provides a standard method for each GUI to use to clean itself 
	    //up ready to exit. This will be called whenever the application exits whether 
	    //it is closed by the Control Station or by the user. This should perform 
	    //tasks such as saving user settings, stopping any threads and asking the user 
	    //if they want to save their settings. When this method is completed the GUI 
	    //should be in a safe state to be closed.

        try
        {
            saveAllUserSettings();
        }
        catch ( TA_Base_Core::UserSettingsException&)
        {
            // Handle here with appropriate errror message
        }

        TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().cleanUp();

        getApplicationWnd()->PostMessage(WM_CLOSE, 0, 0);
    }

	//TD18095, jianghp
	void BannerGUI::onInitGenericGUICompleted()
	{
		CWaitCursor cur;

		CWnd *pWnd = getApplicationWnd();
		pWnd->LockWindowUpdate();

		BannerDlg * pDlg = dynamic_cast<BannerDlg * >(pWnd);
		TA_ASSERT(pDlg != NULL, "the application window is NULL!");

		pDlg->init(m_bannerEntity->getLeftPosition(), m_bannerEntity->getTopPosition(), 
			m_bannerEntity->getWidth(), m_bannerEntity->getHeight(), m_bannerEntity->getStartTab());
		pDlg->UnlockWindowUpdate();
		pDlg->UpdateWindow();
	}
} // namespace TA_Base_App
