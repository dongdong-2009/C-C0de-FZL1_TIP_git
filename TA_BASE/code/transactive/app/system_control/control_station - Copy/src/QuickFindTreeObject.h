/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/QuickFindTreeObject.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// QuickFindTreeObject.h: interface for the QuickFindTreeObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUICKFINDTREEOBJECT_H__1233B998_5EF2_42D7_84B2_7374AC266B8B__INCLUDED_)
#define AFX_QUICKFINDTREEOBJECT_H__1233B998_5EF2_42D7_84B2_7374AC266B8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/object_tree_control/ObjectTreeItem.h"

namespace TA_Base_Core
{
    class IItem;        // foward declaration
}


class QuickFindTreeObject : public CObjectTreeItem  
{
public:

    // Define the three types of quick find object that can exist
    enum ETreeObjectType
    {
        REGION,
        SUBSYSTEM,
        ENTITY
    };

    // A struct in which the type of the quick find object is maintained, as well as the region key.
    // NOTE: The regionKey setting only needs to be defined for objects of type SUBSYSTEM
    struct ObjectTypeInfo
    {
        ETreeObjectType type;
        unsigned long regionKey;    
    };

    /**
     * constructor
     *
     * Constructs an instance of this object based around the specified entity object
     *
     * @param entity The entity on which this tree item is based.
     * @param entityName Reference to the string that contains the name of this entity
     * @param icon Handle to the icon that will be displayed for this item in the tree. May be NULL.
     */
    QuickFindTreeObject(TA_Base_Core::IItem& treeItem, std::string& name,HICON icon,ObjectTypeInfo typeInfo);

	virtual ~QuickFindTreeObject();    

	// ----
    // Functions defined by CObjectTreeItem
    // ----
    
    /**
     * createChildItems
     *
     * Creates the child items of this particular node.
     *
     * @return true if child items were succesfully created, false if there are no children, or they could not be 
     * created.
     */
	bool createChildItems();

    /**
     * getItemLabelText
     *
     * Determines the name that should be displayed for this item in the tree display.
     *
     * @return The text to use for the label.
     */
	const char * getItemLabelText();

    /**
     * hasChildren
     *
     * Determines if this item has any children.
     *
     * @return true if the item has children
     */
	bool hasChildren();

    /** 
     * getKey
     *
     * Returns a unique identifier for this tree item. This identifier is used to locate the item if necessary.
     *
     * @return The key on which the tree control will search for this item.
     */
	const std::string getKey();

    /**
     * getItemImage
     *
     * Allows this item to set its own image, and use a different image for expanded and contracted nodes.
     *
     * @param itemExpanded true if the item is in an expanded state
     *
     * @return A handle to the icon to be displayed.
     */
    HICON getItemImage(bool itemExpanded); 

    /**
     * getObjectType
     *
     * Retrieves the type of this object.
     *
     * @return The type ID representing the type of this object.
     */
    ETreeObjectType getObjectType()
    {
        return m_typeInfo.type;
    };

    /**
     * getItemData
     *
     * Retuns the database object represented by this tree object
     *
     * @return A pointer to the IItem object represented by this tree object
     */
    TA_Base_Core::IItem* getItemData()
    {
        return m_itemData;
    }

private:

    /**
     * createChildItemsOfRegion
     *
     * Creates the child tree objects for a region item
     *
     * @return true if child items were succesfully created, false if there are no children, or they could not be 
     * created. 
     */
    bool createChildItemsOfRegion();

    /**
     * createChildItemsOfSubsystem
     *
     * Creates the child tree objects for a subsystem item. Uses the region key held in the m_typeInfo object to
     * restrict the children to a particular region.
     *
     * @return true if child items were succesfully created, false if there are no children, or they could not be 
     * created. 
     */
    bool createChildItemsOfSubsystem();

    TA_Base_Core::IItem* m_itemData;

    bool m_hasChildren;

    std::string m_name;

    HICON m_icon;

    ObjectTypeInfo m_typeInfo;

};

#endif // !defined(AFX_QUICKFINDTREEOBJECT_H__1233B998_5EF2_42D7_84B2_7374AC266B8B__INCLUDED_)
