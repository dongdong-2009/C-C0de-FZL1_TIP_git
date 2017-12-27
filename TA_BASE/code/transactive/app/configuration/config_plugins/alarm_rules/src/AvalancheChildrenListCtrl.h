/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_rules/src/AvalancheChildrenListCtrl.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the list display used to show the
  * child parameters of an Avalanche Suppression Rule.
  */

#ifndef AVALANCHE_CHILDREN_LIST_CTRL_H
#define AVALANCHE_CHILDREN_LIST_CTRL_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"
#include "app/configuration/config_plugins/alarm_rules/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_rules/src/AlarmRulesDatabaseAccessor.h"
#include "bus\mfc_extensions\src\mfc_extensions.h"


namespace TA_Base_App
{
    class AvalancheChildrenListCtrl : public CEditableListCtrl, public ICellManager
    {
	    public:
            /**
             * Constructor
             */
		    AvalancheChildrenListCtrl();


            /**
             * Destructor
             */
		    virtual ~AvalancheChildrenListCtrl();


            /**
             * populateControl
             *
             * This will populate the list control with the parameters of the
             * AvalancheSuppressionRule passed in
             *
             * @param currentRule  the current rule the user has selected.
             * @param accessor     the AlarmRules database accessor.
             */
            virtual void populateControl(
                TA_Base_Core::AvalancheSuppressionRuleData* currentRule,
                AlarmRulesDatabaseAccessor* accessor);



			// TD 11796 "Add" child is not implemented as all entities should be loaded from SCADA
            /** 
              * addChild
              *
              * Adds a child to the end of the list of avalanche suppression
              * rule children.
              *
              */
            virtual void addChild();


            /** 
              * removeChild
              *
              * Removes the selected child from the list of avalanche
              * suppression rule children.
              *
              */
            virtual void removeChild();

            /** 
              * addDataNode
              *
              * Shows a dialog allowing the user to pick a data node,
              * and adds all children of that node to this list.
              *
              */
            virtual void addDataNode();


            //
		    // ICellManager methods
            //

            /**
             * GetCellType
             *
             * This determines the type of cell the list control should display.
             *
             * @param CEditableListCtrl*  the list control
             * @param int                 the currently selected item.
             * @param int                 the currently selected column.
             *
             * @return  the string type if this is the first column,
             *          the combo type if this is the second column.
             */
            virtual CellType GetCellType(
                CEditableListCtrl* listCtrl, int item, int subItem);


            /**
             * GetCellMaximumCharacters
             *
             * Return the type of the specified table cell. For unlimited number of characters this method
             * should return 0. This method is only relevant to CT_TEXT and CT_NUMBER field types.
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             parameter name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              value column. Otherwise the user cannot edit it.
             */
            virtual unsigned long GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem);

            
            /**
             * GetCellData
             *
             * This will determine the values to display in the combo box
             * and return them.
             *
             * @param CEditableListCtrl*  the list control
             * @param int                 the currently selected item.
             * @param int                 the currently selected column.
             *
             * @return  the data for the selected item.
             */
		    virtual CellData GetCellData(
                CEditableListCtrl* listCtrl, int item, int subItem);


            /**
             * GetCellMask
             *
             * This will determine and return the mask to be used for the specified list control cell.
             *
             * @param CEditableListCtrl* - This is the list control.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column.
             *
             * @return CString - The mask for the selected item.
             */
            virtual CString GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem);


            /**
             * GetCellMaskPromptChar
             *
             * This will determine and return the mask prompt character to be used for the specified list control cell.
             *
             * @param CEditableListCtrl* - This is the list control.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column.
             *
             * @return char - The prompt character.
             */
            virtual char GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem);


            /**
             * UpdateCellValue
             *
             * Once the user has changed a value then this is called.
             * This must set the value in the AvalancheSuppressionRuleData object.
             *
             * @param CEditableListCtrl*  the list control
             * @param int                 the currently selected item.
             * @param int                 the currently selected column.
             * @param CString             the new value for the item
             *
             * @return bool - This indicates whether the update worked or not.
             *                True indicates everything went ok and the control
             *                can update. False indicates an error occurred and
             *                the update cannot take place.
             */
            virtual bool UpdateCellValue(
                CEditableListCtrl* listCtrl, int item, int subItem, CString value);


	    // Overrides
		    // ClassWizard generated virtual function overrides
		    //{{AFX_VIRTUAL(AvalancheChildrenListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL


            // Generated message map functions
	    protected:
		    //{{AFX_MSG(AvalancheChildrenListCtrl)
	//}}AFX_MSG

		    DECLARE_MESSAGE_MAP()



            /** 
              * setupList
              *
              * Sets up this list control.
              *
              */
		    void setupList();

            /** 
              * getAllAlarmTypeDisplayNames
              *
              * Gets the names of all alarm types.
              *
              * @return  the names of all alarm types as a vector of strings
              *
              */
            std::vector<std::string> getAllAlarmTypeDisplayNames();



        private:
            // These are the set up details for the attribute list control

            static const int ENTITY_COLUMN;

            static const int ALARM_TYPE_COLUMN;

           	// TD 11796 No "New Entity" is to be added, as all entities should be taken from data nodes/pts
			// The name used for the entity when it is first created
            // static const std::string NEW_ENTITY_DEFAULT_NAME;

            // The avalanche suppression rule whose parameters are being
            // displayed.
            TA_Base_Core::AvalancheSuppressionRuleData* m_currentRule;

            // The database accessor used to retrieve values from the database.
            AlarmRulesDatabaseAccessor* m_databaseAccessor;


            // The map of child parameters displayed in the list
            typedef
                std::map<int, TA_Base_Core::AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>
                ChildParametersMap;

            ChildParametersMap m_parameters;

    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // AVALANCHE_CHILDREN_LIST_CTRL_H
