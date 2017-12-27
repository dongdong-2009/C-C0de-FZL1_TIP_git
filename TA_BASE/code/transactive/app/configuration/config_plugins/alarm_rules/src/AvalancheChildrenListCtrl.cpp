/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_rules/src/AvalancheChildrenListCtrl.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the list display used to show the
  * child parameters of an Avalanche Suppression Rule.
  */

#pragma warning(disable:4786)


#include "app/configuration/config_plugins/alarm_rules/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_rules/src/AvalancheChildrenListCtrl.h"
#include "app/configuration/config_plugins/alarm_rules/src/resource.h"
#include "app/configuration/config_plugins/alarm_rules/src/SelectDataNodeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/alarm_rule/src/AlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "app/configuration/config_plugins/alarm_rules/src/ChineseSupport.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using namespace alarmrule_chinese;
//using TA_Base_Core::AvalancheSuppressionRuleData::EntityKeyAlarmTypePair;
typedef TA_Base_Core::AvalancheSuppressionRuleData::EntityKeyAlarmTypePair EntityKeyAlarmTypePair;
using TA_Base_Core::RunParams;
using TA_Base_Core::AlarmRuleData;

namespace TA_Base_App
{
    // These are the set up details for the attribute list control

    const int     AvalancheChildrenListCtrl::ENTITY_COLUMN        = 0;

    const int     AvalancheChildrenListCtrl::ALARM_TYPE_COLUMN       = 1;

    // TD 11796 No "New Entity" is to be added, as all entities should be taken from data nodes/pts
    // const std::string AvalancheChildrenListCtrl::NEW_ENTITY_DEFAULT_NAME = "[New Entity]";

    AvalancheChildrenListCtrl::AvalancheChildrenListCtrl()
        :
        m_currentRule(NULL),
        CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    AvalancheChildrenListCtrl::~AvalancheChildrenListCtrl()
    {
    }


BEGIN_MESSAGE_MAP(AvalancheChildrenListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(AvalancheChildrenListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AvalancheChildrenListCtrl::PreSubclassWindow() 
    {
	    setupList();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void AvalancheChildrenListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		                                      LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES);

        CRect rect;
        GetWindowRect(&rect);
        int width = (rect.Width() - 5) / 2;

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(ENTITY_COLUMN,     ENTITY_COLUMN_NAME,     LVCFMT_LEFT, width);
        InsertColumn(ALARM_TYPE_COLUMN, ALARMTYPE_COLUMN_NAME, LVCFMT_LEFT, width);

        FUNCTION_EXIT;
    }


    void AvalancheChildrenListCtrl::populateControl(
        TA_Base_Core::AvalancheSuppressionRuleData* currentRule,
        AlarmRulesDatabaseAccessor* accessor)
    {
        FUNCTION_ENTRY("populateControl");

        TA_ASSERT(accessor != NULL, "The database accessor must not be NULL");

        // Remove all old attributes and add the new ones
        DeleteAllItems();
        m_parameters.clear();

        if (currentRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        m_currentRule = currentRule;

        std::vector<EntityKeyAlarmTypePair> children = m_currentRule->getChildren();
        
        // Now add all the items into the list and into the internal data
        // structure this object maintains
        for (unsigned int i = 0; i < children.size(); i ++)
        {
            unsigned long entityKey  = children[i].first;
            unsigned long alarmTypeKey = children[i].second;
			
			// TD 11796 entityName is loaded from data nodes/pts, no "new entity" is created
            std::string entityName = AlarmRuleData::getEntityName(entityKey);
            // If the entity key is 0, the entity is new and hasn't been
            // given a name yet, so use the default name.
            /*if(entityKey == 0)
            {
                entityName = NEW_ENTITY_DEFAULT_NAME;
            }
            else
            {
                entityName = AlarmRuleData::getEntityName(entityKey);
            }*/

            std::string alarmTypeName =
                AlarmRuleData::getAlarmTypeDisplayName(alarmTypeKey);


            // Insert the entity name in the first column of the list,
            // and the alarm type into the second
            int position = InsertItem(i, entityName.c_str());
            SetItemText(position, ALARM_TYPE_COLUMN, alarmTypeName.c_str());

            // Insert the pair of values into the internal data structure.
            m_parameters[position] = children[i];
        }

        FUNCTION_EXIT;
    }


    ICellManager::CellType AvalancheChildrenListCtrl::GetCellType(
        CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    // Check if item is valid (We can get -1 sometimes but not sure why,
        // should check editable list control)
	    if (item < 0)
        {
            return CT_NOTEDITABLE;
        }

        // The entity column is just text
        if(subItem == ENTITY_COLUMN)
        {
            // TD 11796 disallow editing of cell but re-select from the SCADA tree using Add button
			// as listing all data points will take a long time and list is too long to be useful
			return CT_NOTEDITABLE;
        }

        // The alarm type column is any alarm type
        else if(subItem == ALARM_TYPE_COLUMN)
        {
            return CT_SELECTION;
        }

        TA_ASSERT(false, "Invalid column number requested");

        return CT_NOTEDITABLE;
    }


    unsigned long AvalancheChildrenListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }


    ICellManager::CellData AvalancheChildrenListCtrl::GetCellData(
        CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    ICellManager::CellData data;

        try
        {
            // Return all alarm types as the allowable values in this column
            if(subItem == ALARM_TYPE_COLUMN)
            {
                data = getAllAlarmTypeDisplayNames();
            }
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException",
                 "Could not get the possible enumeration values for this "
                 "avalanche suppression rule parameter");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException",
                 "Could not get the possible enumeration values for this "
                 "avalanche suppression rule parameter");
        }

	    return data;
    }


    CString AvalancheChildrenListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char AvalancheChildrenListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    std::vector<std::string> AvalancheChildrenListCtrl::getAllAlarmTypeDisplayNames()
    {
        // The vector of strings that will be returned.
        // This is static because it only needs to be retrieved once.
        static std::vector<std::string> alarmTypes;

        // If this method hasn't been called before.
        if(alarmTypes.empty())
        {
            TA_ASSERT(m_databaseAccessor != NULL,
                      "Database accessor must not be NULL");

            // The alarm types from the database accessor are in the form
            // of a [key => alarm type name] map.
            // We only need the names.

            std::map<unsigned long, std::string> alarmTypesMap =
                m_databaseAccessor->getAllAlarmTypeDisplays();

            alarmTypes.reserve(alarmTypesMap.size());
            
            // Copy the names into a new vector
            for(std::map<unsigned long, std::string>::iterator it =
                    alarmTypesMap.begin();
                it != alarmTypesMap.end();
                it ++)
            {
                alarmTypes.push_back( (*it).second );
            }
        }

        return alarmTypes;
    }



    bool AvalancheChildrenListCtrl::UpdateCellValue(
        CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
	    TA_ASSERT(m_currentRule != NULL,
                  "We do not yet have an avalanche suppression item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        try
        {  

// 			//add here to show the updated value:
// 			SetItemText(item,ALARM_TYPE_COLUMN , value);

            // Get the keys for this row from the map maintained by
            // this class
            ChildParametersMap::iterator oldKeysIt = m_parameters.find(item);
            
            if(oldKeysIt == m_parameters.end())
            {
                // List has probably been cleared, so just return
                return false;
            }

            // Get the existing entity and alarm type
            std::string entity = listCtrl->GetItemText(item, ENTITY_COLUMN);
            std::string alarmType = listCtrl->GetItemText(item, ALARM_TYPE_COLUMN);

            // Get the old and new entity key and alarm type keys for the child
            EntityKeyAlarmTypePair keys = (*oldKeysIt).second;
            unsigned long oldEntityKey  = keys.first;
            unsigned long oldAlarmTypeKey = keys.second;

            // If the entity column was updated
            if(subItem == ENTITY_COLUMN)
            {
                // If the entity hasn't changed, return now
                if(entity == std::string(value))
                {
                    return false;
                }
                entity = value;

				// TD 11796 No "New Entity" is to be added, as all entities should be taken from data nodes/pts
                // If the entity has the default name, its key must be 0
                // if(entity == NEW_ENTITY_DEFAULT_NAME)
                // {
                //    keys.first = 0;
                // }
                // else
                // {
                    keys.first = AlarmRuleData::getEntityKeyFromName(entity);
                // }
            }
            // If the alarm type column was updated
            else if(subItem == ALARM_TYPE_COLUMN)
            {
                // If the alarm type hasn't changed, return now
                if(alarmType == std::string(value))
                {
                    return false;
                }
                alarmType = value;
//                 keys.second =
//                     AlarmRuleData::getAlarmTypeKeyFromName(alarmType);
				keys.second =
					AlarmRuleData::getAlarmTypeKeyFromDisplayName(alarmType);
            }
            else
            {
                TA_ASSERT(false, "Invalid column specified");
            }


            // Replace the existing parameter in the rule
            EntityKeyAlarmTypePair oldPair(oldEntityKey, oldAlarmTypeKey);
            
            m_currentRule->replaceChild(oldPair, keys);

            // Replace the parameter in the map maintained by this class.
            m_parameters[item] = keys;
            
            //update button in detail view and status in listView. 
            AbstractDetailView* detailView = dynamic_cast<AbstractDetailView*>(GetParent());
            if (detailView != NULL)
            {
				detailView->refreshButtonAvailability();
				AbstractListView* listView = dynamic_cast<AbstractListView*>(detailView->getListView());
				if(listView != NULL){
					listView->itemDetailsChanged(m_currentRule->getUniqueIdentifier(),"");
				}	
            }

            return true;
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,
                 "DatabaseException",
                 "Could not set the parameter");
        }
        catch( const TA_Base_Core::DataException& exception )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,
                 "DataException",
                 "Parameter entered was of the wrong type");

            unsigned int errorId;
            switch(exception.getFailType())
            {
                case (TA_Base_Core::DataException::NOT_UNIQUE):
                {
                    errorId = IDS_FIELD_NOT_UNIQUE;
                    break;
                } 

                case (TA_Base_Core::DataException::INVALID_VALUE):
                case (TA_Base_Core::DataException::WRONG_TYPE):
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

        return false;
    }


	// TD 11796 "Add" child is not implemented as all entities should be loaded from SCADA
    void AvalancheChildrenListCtrl::addChild()
    {
	/*    TA_ASSERT(m_currentRule != NULL,
                  "We do not yet have an avalanche suppression item set");

        // Add a new child to the list of children and repopulate the list
        try
        {
            m_currentRule->addChild( EntityKeyAlarmTypePair(0, "0") );
        }
        catch(const TA_Base_Core::DataException& exception)
        {
            unsigned int errorId;
            switch(exception.getFailType())
            {
                case (TA_Base_Core::DataException::NOT_UNIQUE):
                {
                    errorId = IDS_FIELD_NOT_UNIQUE;
                    break;
                } 

                case (TA_Base_Core::DataException::INVALID_VALUE):
                case (TA_Base_Core::DataException::WRONG_TYPE):
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
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(errorId);
            reason += exception.getWhichData().c_str();
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            userMsg.showMsgBox(IDS_UE_030060);

        }
        catch(const TA_Base_Core::DatabaseException&)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            CString action, reason;
            action.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATABASE_ERROR);
            userMsg << action << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        populateControl(m_currentRule, m_databaseAccessor);

        // Select the child that has just been added.
        SetItemState(m_parameters.size() - 1, LVIS_SELECTED, LVIS_SELECTED);
	*/
    }
    

    void AvalancheChildrenListCtrl::addDataNode()
    {
	    TA_ASSERT(m_currentRule != NULL,
                  "We do not yet have an avalanche suppression item set");

        // Construct the dialog and then wait for the user to finish with it
        SelectDataNodeDialog dlg(m_databaseAccessor, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                         "User cancelled the selection of all children "
                         "under the data node");
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "User selected all children under the datanode which "
                     "is %s",
                     dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentRule !=  NULL, "current rule is NULL.");

            // Get the children of the datapoint and add them
            // to the list.
            std::vector<unsigned long> keys =
                m_databaseAccessor->getEntityDescendants( dlg.getSelectedKey() );

            unsigned long alarmType = dlg.getAlarmTypeKey();
            for(std::vector<unsigned long>::iterator it = keys.begin();
                it != keys.end();
                it ++)
            {
                m_currentRule->addChild( EntityKeyAlarmTypePair(*it, alarmType) );
            }
        }
        catch ( const TA_Base_Core::DataException& exception)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,
                 "DataException",
                 "Could not retrieve child entities of datapoint");

            unsigned int errorId;
            switch(exception.getFailType())
            {
                case (TA_Base_Core::DataException::NOT_UNIQUE):
                {
                    errorId = IDS_FIELD_NOT_UNIQUE;
                    break;
                } 

                case (TA_Base_Core::DataException::INVALID_VALUE):
                case (TA_Base_Core::DataException::WRONG_TYPE):
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
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(errorId);
            reason += exception.getWhichData().c_str();
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            userMsg.showMsgBox(IDS_UE_030060);

        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,
                 "DatabaseException",
                 "Could not retrieve child entities of datapoint");

            TA_Base_Bus::TransActiveMessage userMsg;
            CString action, reason;
            action.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATABASE_ERROR);
            userMsg << action << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }

        populateControl(m_currentRule, m_databaseAccessor);
    }


    void AvalancheChildrenListCtrl::removeChild()
    {
        int item = GetSelectionMark();

        // If no item is selected, give an error message
        if(item == -1)
        {
            CString actionName;
            actionName.LoadString(IDS_DELETE);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            return;
        }


	    TA_ASSERT(m_currentRule != NULL,
                  "We do not yet have an avalanche suppression item set");

        try
        {
            // Remove the selected child parameters
            int selectionCount = GetSelectedCount();
            int item = -1;
            for(int i = 0; i < selectionCount; i ++)
            {
                item = GetNextItem(item, LVNI_SELECTED);
                ChildParametersMap::iterator selectedIt =
                    m_parameters.find(item);

                TA_ASSERT(selectedIt != m_parameters.end(),
                          "The selected item must be in the list");

                EntityKeyAlarmTypePair pairToRemove = (*selectedIt).second;
        
                m_currentRule->deleteChild( pairToRemove );
            }
        }
        catch(const TA_Base_Core::DataException& exception)
        {
            unsigned int errorId;
            switch(exception.getFailType())
            {
                case (TA_Base_Core::DataException::NOT_UNIQUE):
                {
                    errorId = IDS_FIELD_NOT_UNIQUE;
                    break;
                } 

                case (TA_Base_Core::DataException::INVALID_VALUE):
                case (TA_Base_Core::DataException::WRONG_TYPE):
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
            actionName.LoadString(IDS_DELETE);
            reason.LoadString(errorId);
            reason += exception.getWhichData().c_str();
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            userMsg.showMsgBox(IDS_UE_030060);
        }
        catch(const TA_Base_Core::DatabaseException&)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            CString action, reason;
            action.LoadString(IDS_DELETE);
            reason.LoadString(IDS_DATABASE_ERROR);
            userMsg << action << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        
        // Repopulate the control
        populateControl(m_currentRule, m_databaseAccessor);
    }


}

