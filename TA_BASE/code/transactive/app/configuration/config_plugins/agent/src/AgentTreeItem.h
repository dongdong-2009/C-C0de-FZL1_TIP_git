/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This is an item to be added to the agent tree
  */


#if !defined(AGENTTREEITEM_H_INCLUDED)
#define AGENTTREEITEM_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "bus\mfc_extensions\src\mfc_extensions.h"


namespace TA_Base_Core
{
	class IItem;
}

namespace TA_Base_App
{
    class AgentDatabaseAccessor;

    class AgentTreeItem : public CObjectTreeItem
    {
	    public:

            /**
             * Constructor
             * 
             * @param AgentDatabaseAccessor& - The object to use to retrieve information for populating this item
             * @param IItem* - The object this tree item is representing
             * @param bool - This indicates if we are in charge of deleting the object passed in. If this is
             *               true then we must delete the object when we have finished. If it is false
             *               then we don't need to worry about deleting. The default is false
             */
            AgentTreeItem(AgentDatabaseAccessor& db, TA_Base_Core::IItem* item, bool shouldDelete = false);


            /**
             * Destructor
             */
            virtual ~AgentTreeItem();


            /**
             * getItemData
             *
             * @return Returns the IItem* object associated with this tree item
             */
		    TA_Base_Core::IItem* getItemData()
            {
                return m_data;
            };


            /**
             * createChildItems
             *
             * This should create all child items of this item and add them to the tree
             *
             * @return bool - true if the create was successful, false otherwise
             */
            virtual bool createChildItems();

            /**
             * getItemLabelText
             *
             * @return Returns the text to use in the tree for this item
             */
		    virtual const char * getItemLabelText();


            /**
             * hasChilren
             *
             * This must determine if this item has any children or not
             *
             * @return bool - false if this item has no children, true if it has children
             */
		    virtual bool hasChildren();


            /**
             * getKey
             *
             * This returns a key identifier for this item
             *
             * @return string - A key identifier
             */
		    virtual const std::string getKey();


            /**
             * getItemImage
             *
             * This returns the icon to use for this item
             *
             * @param bool - true if the item is expanded, false it if it not. This can be used to determine
             *               which icon to return. In our situation we always return the same icon so this 
             *               is ignored
             *
             * @return HICON - The icon to display for this item
             */
		    virtual HICON getItemImage(bool itemExpanded);

	    private:

            // We can use this to retrieve the loaded items etc
            AgentDatabaseAccessor& m_databaseAccessor;

            // We are not responsible for deleting this item
		    TA_Base_Core::IItem*  m_data;

            // This indicates if we are in charge of deleting the IItem object
            bool m_shouldDeleteItem;

            // The icon to use to represent this object
            HICON m_icon;

            std::string m_itemLabelText;
    };

} // namespace TA_Base_App

#endif // !defined(AGENTTREEITEM_H_INCLUDED)
