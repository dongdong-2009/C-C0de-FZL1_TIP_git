/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/HelpConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for help settings
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/HelpConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"


#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
using TA_Base_Core::DebugUtil;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    // The names of the entity parameters we will use
    const std::string HelpConfiguration::HELP_MESSAGE_ENABLED = "EnableHelpURL";
    const std::string HelpConfiguration::HELP_URL             = "HelpURL";
    const std::string HelpConfiguration::ENG_NOTE_ENABLED     = "EnableEngineeringNote";
    const std::string HelpConfiguration::ENG_NOTE             = "EngineeringNote";

    // The limit in length for the text fields.
    const int HelpConfiguration::HELP_URL_FIELD_LENGTH = 128;
    const int HelpConfiguration::ENG_NOTE_FIELD_LENGTH = 128;


    
    IMPLEMENT_DYNCREATE(HelpConfiguration, CPropertyPage)

    HelpConfiguration::HelpConfiguration() : m_parent(NULL), CPropertyPage(HelpConfiguration::IDD, IDS_PROPPAGE_HELP)
    {
	    //{{AFX_DATA_INIT(HelpConfiguration)
	    m_engNoteCheck = FALSE;
	    m_helpUrlCheck = FALSE;
	    m_engNote = _T("");
	    m_helpURL = _T("");
	    //}}AFX_DATA_INIT
    }

    HelpConfiguration::~HelpConfiguration()
    {
    }

    void HelpConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(HelpConfiguration)
	DDX_Control(pDX, IDC_HELP_URL_BITMAP, m_bitmapHelpUrl);
	DDX_Control(pDX, IDC_ENG_NOTE_BITMAP, m_bitmapEngNote);
	    DDX_Control(pDX, IDC_EDIT_HELP_URL, m_helpURLControl);
	    DDX_Control(pDX, IDC_EDIT_ENG_NOTE, m_engNoteControl);
	    DDX_Check(pDX, IDC_CHECK_ENG_NOTE, m_engNoteCheck);
	    DDX_Check(pDX, IDC_CHECK_HELP_MESSAGE, m_helpUrlCheck);
	    DDX_Text(pDX, IDC_EDIT_ENG_NOTE, m_engNote);
	    DDX_Text(pDX, IDC_EDIT_HELP_URL, m_helpURL);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(HelpConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(HelpConfiguration)
	ON_BN_CLICKED(IDC_CHECK_ENG_NOTE, onCheckEngNote)
	ON_BN_CLICKED(IDC_CHECK_HELP_MESSAGE, onCheckHelpMessage)
	ON_EN_KILLFOCUS(IDC_EDIT_ENG_NOTE, onKillfocusEditEngNote)
	ON_EN_KILLFOCUS(IDC_EDIT_HELP_URL, onKillfocusEditHelpUrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void HelpConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                         ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;

        loadEntityIntoPage();

        FUNCTION_EXIT;
    }

    
    void HelpConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");

        parametersToIgnore.insert(HELP_MESSAGE_ENABLED);
        parametersToIgnore.insert(HELP_URL);
        parametersToIgnore.insert(ENG_NOTE_ENABLED);
        parametersToIgnore.insert(ENG_NOTE);
/**/
        FUNCTION_EXIT;
    }


    void HelpConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");

        if (m_currentEntity->getParameterValue(HELP_MESSAGE_ENABLED) == ScadaDetailView::TA_TRUE)
        {
            checkMandatory(HELP_URL,IDS_LABEL_HELP_URL,params);
        }

        if (m_currentEntity->getParameterValue(ENG_NOTE_ENABLED) == ScadaDetailView::TA_TRUE)
        {
            checkMandatory(ENG_NOTE,IDS_LABEL_ENG_NOTE,params);
        }

        FUNCTION_EXIT;
    }

    
    void HelpConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");
        FUNCTION_EXIT;
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // HelpConfiguration message handlers

    BOOL HelpConfiguration::OnInitDialog() 
    {
	    CPropertyPage::OnInitDialog();
	    
        m_engNoteControl.SetLimitText(ENG_NOTE_FIELD_LENGTH);
        m_helpURLControl.SetLimitText(HELP_URL_FIELD_LENGTH);

        loadEntityIntoPage();
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void HelpConfiguration::onCheckEngNote() 
    {
        FUNCTION_ENTRY("onCheckBoolFalseAlarm");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        UpdateData(TRUE);

        if (m_engNoteCheck)
        {
            m_currentEntity->setParameterValue(ScadaDetailView::TA_TRUE,ENG_NOTE_ENABLED);
        }
        else
        {
            m_currentEntity->setParameterValue(ScadaDetailView::TA_FALSE,ENG_NOTE_ENABLED);
        }
	    
        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void HelpConfiguration::onCheckHelpMessage() 
    {
        FUNCTION_ENTRY("onCheckBoolFalseAlarm");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        UpdateData(TRUE);

        if (m_helpUrlCheck)
        {
            m_currentEntity->setParameterValue(ScadaDetailView::TA_TRUE,HELP_MESSAGE_ENABLED);
        }
        else
        {
            m_currentEntity->setParameterValue(ScadaDetailView::TA_FALSE,HELP_MESSAGE_ENABLED);
        }
	    
        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void HelpConfiguration::onKillfocusEditEngNote() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_engNote),ENG_NOTE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void HelpConfiguration::onKillfocusEditHelpUrl() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_helpURL),HELP_URL);
    
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods

    void HelpConfiguration::loadEntityIntoPage()
    {
        FUNCTION_ENTRY("loadEntityIntoPage");

        if (m_hWnd == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Could not load the entity details yet as OnInitDialog has not been called");
            FUNCTION_EXIT;
            return;
        }

        if (m_currentEntity == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Could not load the entity details yet as no current entity has been set yet");
            FUNCTION_EXIT;
            return;
        }


        try
        {
            std::string value = m_currentEntity->getParameterValue(HELP_MESSAGE_ENABLED);
            m_helpUrlCheck = (0 == value.compare(ScadaDetailView::TA_TRUE));
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if the true alarm was enabled or not");
        }

        try
        {
            std::string value = m_currentEntity->getParameterValue(ENG_NOTE_ENABLED);
            m_engNoteCheck = (0 == value.compare(ScadaDetailView::TA_TRUE) );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if the false alarm was enabled or not");
        }

        try
        {
	        m_engNote = m_currentEntity->getParameterValue(ENG_NOTE).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the true alarm message");
        }

        try
        {
	        m_helpURL = m_currentEntity->getParameterValue(HELP_URL).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the false alarm message");
        }

        UpdateData(FALSE);

        refresh();

        // First GetParent() gives us the property sheet. The next one gives us the view
        m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void HelpConfiguration::refresh()
    {
        FUNCTION_ENTRY("refresh");

        // If checked then enable
        GetDlgItem(IDC_EDIT_HELP_URL)->EnableWindow(m_helpUrlCheck);

        GetDlgItem(IDC_EDIT_ENG_NOTE)->EnableWindow(m_engNoteCheck);

        UpdateData(true);

        FUNCTION_EXIT;
    }


    void HelpConfiguration::checkMandatory(const std::string& parameterName,
                                                       int displayNameStringId,
                                                       std::vector<std::string>& params )
    {
        if (m_currentEntity->getParameterValue(parameterName).empty())
        {
            
			//set the Default value if the value for the mandatory parameter is not entered.
			std::string entityName(m_currentEntity->getName());

			TA_Base_Core::IEntityData* iEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
            TA_Base_Core::EntityData* entityData = dynamic_cast <TA_Base_Core::EntityData*> (iEntityData);	
			TA_Base_Core::IData& data = entityData->getHelper()->getConfigParameters();
			std::string parameterDefaultValue("");
			for (unsigned long i = 0;i < data.getNumRows();i++)
			{
					
				if( 0 == data.getStringData(i,entityData->getHelper()->PARAMETERNAME).compare(parameterName) )
				{
				 parameterDefaultValue = data.getStringData(i,entityData->getHelper()->PARAMETERDEFAULTVALUE);
				 m_currentEntity->setParameterValue(parameterDefaultValue, parameterName);
				 break;
				}
			}	
			
			
			//if the default value for the mandator parameter is not set in the database.
			if (m_currentEntity->getParameterValue(parameterName).empty())
			{
						
			CString dialogTitle;
            dialogTitle.LoadString(IDS_PROPPAGE_HELP);

            CString labelTitle;
            labelTitle.LoadString(displayNameStringId);

            std::string parameter(dialogTitle);
            parameter += " - ";
            parameter += labelTitle;

            params.push_back(parameter);
			}
        }
    }

}
