/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendTreeItem.h $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// TrendTreeItem.h : header file
//

#if !defined(TRENDTREEITEM_H_INCLUDED)
#define TRENDTREEITEM_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus\mfc_extensions\src\mfc_extensions.h"


namespace TA_Base_Core
{
	class IEntityData;	
}

namespace TA_Base_App
{    
    /////////////////////////////////////////////////////////////////////////////
    // CTrendTreeItem
	class IItemStore;

    class CTrendTreeItem : public CObjectTreeItem
    {
	    public:

		    CTrendTreeItem(TA_Base_Core::IEntityData* entityData, IItemStore* itemStore, 
				 bool bItemTrendEnabled = false); //TD17812
			virtual ~CTrendTreeItem() {}
 
		    virtual bool createChildItems();  
 			virtual const char * getItemLabelText();
		    virtual bool hasChildren();
			virtual bool isTrendedDatapointType() { return m_bItemTrendEnabled; }  //TD17812
			virtual CString getDisplayName(const CString& fullName);	
			TA_Base_Core::IEntityData* getEntityData() { return m_entityData;};

		protected:
			std::string					m_entityLabel;
 			TA_Base_Core::IEntityData*  m_entityData;
			bool						m_hasChildren; 
			IItemStore*					m_itemStore;
			bool						m_bItemTrendEnabled; //TD17812
			
			
    };

} // namespace TA_Base_App

#endif // !defined(TRENDTREEITEM_H_INCLUDED)
