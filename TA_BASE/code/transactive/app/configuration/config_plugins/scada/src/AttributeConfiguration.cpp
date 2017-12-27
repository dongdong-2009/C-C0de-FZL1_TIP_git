/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/AttributeConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the attribute list used for all entities. In this case it is put in a property
  * page so the general attributes can be configured with the other attributes
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/AttributeConfiguration.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(AttributeConfiguration, CPropertyPage)

    AttributeConfiguration::AttributeConfiguration()
    : m_currentEntity(NULL), 
      CPropertyPage(AttributeConfiguration::IDD, IDS_PROPPAGE_ATTRIBUTES)
    {
	    //{{AFX_DATA_INIT(AttributeConfiguration)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    AttributeConfiguration::~AttributeConfiguration()
    {
    }


    void AttributeConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AttributeConfiguration)
	    DDX_Control(pDX, IDC_LIST, m_attributeList);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AttributeConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(AttributeConfiguration)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AttributeConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                              ScadaDatabaseAccessor& databaseAccessor)
    {
        m_currentEntity = &currentEntity;

        m_attributeList.populateControl( currentEntity, m_parametersToIgnore );

        // Clear the parameters to ignore ready for next time.
        m_parametersToIgnore.clear();
    }


    void AttributeConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");

        TA_ASSERT(m_currentEntity != NULL, "The entity is NULL so we cannot determine if the mandatory parameters are all set");

        std::vector<std::string> parameterNames = m_currentEntity->getParameterNames();
        for( std::vector<std::string>::iterator iter = parameterNames.begin(); iter != parameterNames.end(); ++iter)
        {
            if (m_parametersToIgnore.end() != m_parametersToIgnore.find( *iter ) )
            {
                // We are ignoring this parameter so move onto the next one
                continue;
            }

            // If a parameter is mandatory and it is blank then we add it to our vector
            if( m_currentEntity->isParameterMandatory(*iter) && m_currentEntity->getParameterValue(*iter).empty() )
            {
                std::string parameter("Attribute - ");
                parameter += *iter;
                params.push_back( parameter );
            }
        }

        FUNCTION_EXIT;
    }


    void AttributeConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");
        
        //TODO: See Issue 2168

        FUNCTION_EXIT;
    }

    
    void AttributeConfiguration::setWatchParameters(std::set<std::string> paramsToWatch, CWnd* callbackWindow, UINT messageToPost)
    {
        m_attributeList.setWatchParameters(paramsToWatch, callbackWindow, messageToPost);
    }


}
