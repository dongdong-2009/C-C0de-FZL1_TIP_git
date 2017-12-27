/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/QuickToolComboEx.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class extends the CComboBoxEx class. It is used for a combo box which contains
  * quick launch entities. Those are entities that may have a control application,
  * map display, schematic display or camera associated with them.
  * 
  * This class controls loading the entities/devices into the list. It also 
  * decides which buttons should be available once a device is selected. It will
  * perform the actual functions required when the buttons are pressed as well.
  * 
  */

#if !defined(AFX_QUICKTOOLCOMBOEX_H__5FFBD8DC_8E8A_463C_BEC2_DE5556245E86__INCLUDED_)
#define AFX_QUICKTOOLCOMBOEX_H__5FFBD8DC_8E8A_463C_BEC2_DE5556245E86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuickToolComboEx.h : header file
//

#include <map>
#include <vector>
#include <string>

class ControlStationBar;
namespace TA_Base_Core
{
    class IQuickFindEntity;
}

/////////////////////////////////////////////////////////////////////////////
// CQuickToolComboEx window

class CQuickToolComboEx : public CComboBoxEx
{
public:

    /**
    * Constructor
    *
    * @param ControlStationBar* - This is a pointer to the dialog with the device 
    *                             buttons. This class needs to be able to enable
    *                             and disable the buttons through this object.
    */
    CQuickToolComboEx(ControlStationBar* pParent);

    /**
    * Destructor
    */
	virtual ~CQuickToolComboEx();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickToolComboEx)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	/**
	 * loadEntities()
	 *
	 * Load all the entities that should be displayed in the quick launch
     * combo box
	 */
	void loadEntities();

    
    /**
    * viewSchematicDisplayPressed
    *
    * This is called when the View Schematic Display button is pressed in the
    * Control Station bar. This will perform the actual actions required to 
    * retrieve the appropriate display and then get it showing on the users
    * workstation.
    */
    void viewSchematicDisplayPressed();


    /**
    * cameraViewPressed
    *
    * This is called when the Camera View button is pressed in the
    * Control Station bar. This will perform the actual actions required to 
    * retrieve the appropriate camera and preset associations, assign
    * the camera to a monitor and then move the camera to the appropriate
    * preset (if configured).
    */
    void cameraViewPressed();


	// Generated message map functions
protected:
	//{{AFX_MSG(CQuickToolComboEx)
	afx_msg void OnSelchange();//NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

    // The key representing an invalid application type
    static const unsigned long INVALID_APPTYPE;

    // This is the width of this combo box.
    static const int COMBO_BOX_WIDTH;

    // This is the width and height of the icons to display on the associated buttons.
    static const int ICON_WIDTH;
    static const int ICON_HEIGHT;

    // This is the initial size the combo box should be (ie number of items)
    static const int NUM_INITIAL_ITEMS;
    // This is the amount the combo box should grow by each time it runs out of
    // space (number of items)
    static const int NUM_TO_GROW_BY;

    // This is the runtime parameter name for the camera entity
    static const std::string CONTROLENTITY_PARAMNAME;

    // This is the runtime parameter name for the preset
    static const std::string PRESET_PARAMNAME;

    // This map is indexed by appType and contains indexes into the image
    // list (m_imageList)
    std::map<int,int> m_apptypeToIndex;

	// Store the images that are used for the icons in the quick toolbar
	CImageList* m_imageList;

	// Pointer to the parent so we can disable and enable the buttons
    // (these are the control, display schematic, display map, camera buttons)
	ControlStationBar* m_controlStationBar;

    // This vector contains all the entities that will be inserted into the combo
    // They must be deleted correctly when this class is destructed.
    std::vector<TA_Base_Core::IQuickFindEntity*> m_entities;

    // Stores the names in the quick find list
    std::vector<std::string> m_namelist;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUICKTOOLCOMBOEX_H__5FFBD8DC_8E8A_463C_BEC2_DE5556245E86__INCLUDED_)
