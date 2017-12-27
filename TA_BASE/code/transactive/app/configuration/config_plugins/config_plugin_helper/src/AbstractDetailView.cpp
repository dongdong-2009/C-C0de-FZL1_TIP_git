/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an item's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/config_plugin_helper/src/StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h" //CL15079
#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IItem.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DatabaseKey.h"

// CL-15079 SoundFile
#include "core/data_access_interface/sound_manager/src/IConfigSoundFile.h"
// CL-15079
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_App
{
    // The label used to indicate that this is a new item being displayed
	//TD18206 marc_bugfix changed from " (New)" to  "-(New)"
    //const CString AbstractDetailView::NEW_LABEL = "-(New)";
	const CString AbstractDetailView::NEW_LABEL = "ÐÂ½¨";

    // The string to set in the controls where there is no value.
    const CString AbstractDetailView::NO_VALUE = "";


    AbstractDetailView::AbstractDetailView(int dialogResource) : 
        m_listView(NULL),
        m_databaseAccessor(NULL),
        m_messageSender(NULL),
        CFormView(dialogResource)
    {
	//{{AFX_DATA_INIT(AbstractDetailView)
	//}}AFX_DATA_INIT
    }


    AbstractDetailView::~AbstractDetailView()
    {
    }

    void AbstractDetailView::OnActivateView( BOOL bActivate, CView* pActivateView,CView* pDeactiveView )
    {

    }

BEGIN_MESSAGE_MAP(AbstractDetailView, CFormView)
	//{{AFX_MSG_MAP(AbstractDetailView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AbstractDetailView::resetButtonPressed(TA_Base_Core::IConfigItem* currentItem) 
    {
        FUNCTION_ENTRY("onButtonReset");

	    if (currentItem != NULL)
        {
            if (currentItem->hasChanged())
            {
                CString actionName;
                actionName.LoadString(IDS_RESET);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName;
                UINT response = userMsg.showMsgBox(IDS_UW_010003);
                if (response == IDNO)
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "User cancelled the reset");
                    FUNCTION_EXIT;
                    return;
                }
            }

            // Invalidate and re-populate the details. This will ensure all data is reloaded from the database.
            dynamic_cast<TA_Base_Core::IItem*>(currentItem)->invalidate();

            // Alert the list view that this has been updated so it can update the item's name in
            // the list if necessary.
            if (m_listView != NULL)
            {
                try
                {
                    CString name = dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getName().c_str();

                    m_listView->itemDetailsChanged( currentItem->getUniqueIdentifier(), name );
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg.showMsgBox(IDS_UE_010001);
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg.showMsgBox(IDS_UE_030001);
                }
            }

            populateItemDetails(currentItem);
        }
        FUNCTION_EXIT;
    }


    void AbstractDetailView::applyButtonPressed(TA_Base_Core::IConfigItem* currentItem) 
    {
	    FUNCTION_ENTRY("onButtonApply");

        try
        {
	        if (currentItem != NULL)
            {
                AfxGetApp()->DoWaitCursor(1);

                MessageSender::UpdateDetails updateDetails;
                updateDetails.isNewItem = currentItem->isNew();
                updateDetails.isDelete = false;
                updateDetails.configUpdateMessage = getConfigMessageType();
                updateDetails.nameOfChangedItem = dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getName();
                
                unsigned long oldKey = TA_Base_Core::DatabaseKey::getInvalidKey();
                
                if (!currentItem->isNew())
                {
                    // If item already exists, store various parameters before applying the changes
                    oldKey = dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getKey();
                 
                    updateDetails.changes = currentItem->getAllItemChanges();
                }

                // Apply the changes to the database
                currentItem->applyChanges();

                // Update the view
                populateItemDetails(currentItem);

                // Retrieve the key of the item after we have applied in case this is a new item.
                // If it is a new item it doesn't get given a key until Apply is called.
                if (TA_Base_Core::DatabaseKey::getInvalidKey() != oldKey &&
                        oldKey != dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getKey())
                {
                    // TES #782: Primary key of existing item changed, cannot simply send out 
                    // an update notification.  
                    // Only certain config items have the ability to change their primary key
                    // post-creation..
                    
                    // If the primary key is changed, clients listening out for this update
                    // need to receive a Delete notification followed by a Create notification
                    // in order to maintain a valid state (because they can't know what the
                    // key changed to, so won't know how to refresh upon a primary key change if
                    // we simply send an Update notification)

                    // Send out notification using the old key
                    sendDeleteMessageNotification(dynamic_cast<TA_Base_Core::IItem*>(currentItem), oldKey);
                                     
                    // Force change of the message type from Update to Create (with the new key)..
                    updateDetails.keyOfChangedItem = dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getKey();
                    updateDetails.isNewItem = true;
                }
                else
                {
                    // Otherwise item just created or key didn't change, use the key allocated to it..
                    updateDetails.keyOfChangedItem = dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getKey();
                }

				// CL15079++ 2008.6.17
				// modify the event description to 
				// <SoundCategory> <Priority> <Description> <SoundFile>

				// add <SoundCategory> <Priority> <Description> <SoundFile> info 
				// in updateDetail and handle it here and remove it
				{
					std::ostringstream additionInfo;
					TA_Base_Core::IConfigSoundFile* soundFileTemp = 
						dynamic_cast<TA_Base_Core::IConfigSoundFile*>(currentItem);

					if (NULL != soundFileTemp)
					{
						// The info is new
						additionInfo << "'<" << soundFileTemp->getCategoryName() << "> "
							<< "<" << soundFileTemp->getPriority() << "> "
							<< "<" << soundFileTemp->getDescription() << "> "
							<< "<" << soundFileTemp->getSoundFile() << ">'";
						TA_Base_Core::Values valTemp;
						valTemp.newValue = additionInfo.str();
						valTemp.oldValue = "";
						updateDetails.changes[RunParams::getInstance().get(TA_Base_App::TA_Configuration::RPARAM_COMPONENT.c_str())] =
							valTemp;
					}
				}
				// CL15079++

                sendMessagesForTheChange( updateDetails );
            }
        }
        catch ( const TA_Base_Core::DataConfigurationException& exception )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataConfigurationException","");

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_FIELD_MISSING);

            std::vector<std::string> missing = exception.getMissingFields();
            for ( size_t i = 0; i < missing.size(); ++i )
            {
                reason += missing[i].c_str();
                reason += "\n";
            }

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not apply the item changes");

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DataException& exception)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not apply the item changes");

            unsigned int errorId;
            switch(exception.getFailType())
            {
                case (TA_Base_Core::DataException::NOT_UNIQUE):
                {
                    errorId = IDS_FIELD_NOT_UNIQUE;
                    break;
                } 

                case (TA_Base_Core::DataException::WRONG_TYPE):
                case (TA_Base_Core::DataException::INVALID_VALUE):
                {
                    errorId = IDS_FIELD_INVALID;
                    break;
                }
            
                case (TA_Base_Core::DataException::MISSING_MANDATORY):
                {
                    errorId = IDS_FIELD_MISSING;
                    break;
                }

                default:
                {
                    errorId = IDS_UNKNOWN_ERROR;
                    break;
                }
            }

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(errorId);
            reason += exception.getWhichData().c_str();
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            userMsg.showMsgBox(IDS_UE_030060);
        }

        AfxGetApp()->DoWaitCursor(-1);
        FUNCTION_EXIT;
    }
    

    void AbstractDetailView::sendDeleteMessageNotification(TA_Base_Core::IItem* item, unsigned long keyOfItem)
    {
        MessageSender::UpdateDetails updateDetails;
        
        updateDetails.isNewItem = false;
        updateDetails.isDelete  = true;        
        updateDetails.keyOfChangedItem = keyOfItem;
        updateDetails.nameOfChangedItem = item->getName();
        updateDetails.configUpdateMessage = getConfigMessageType();
        
        getMessageSender()->sendMessagesForTheChange( updateDetails );
    }


    void AbstractDetailView::sendMessagesForTheChange(MessageSender::UpdateDetails updateDetails)
    {
        TA_ASSERT( m_messageSender != NULL, "The MessageSender is NULL");
        m_messageSender->sendMessagesForTheChange( updateDetails );
    }

    /////////////////////////////////////////////////////////////////////////////
    // AbstractDetailView diagnostics

    #ifdef _DEBUG
    void AbstractDetailView::AssertValid() const
    {
	    CFormView::AssertValid();
    }

    void AbstractDetailView::Dump(CDumpContext& dc) const
    {
	    CFormView::Dump(dc);
    }
    #endif //_DEBUG
}

