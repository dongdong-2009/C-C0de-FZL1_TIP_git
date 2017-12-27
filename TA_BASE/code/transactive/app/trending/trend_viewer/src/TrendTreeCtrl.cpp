/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendTreeCtrl.cpp $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// TrendTreeCtrl.cpp : header file
//


#include "stdafx.h"
 
#include "app/trending/trend_viewer/src/TrendTreeCtrl.h"
#include "app/trending/trend_viewer/src/TrendTreeItem.h" 
#include "app/trending/trend_viewer/src/IItemStore.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h" 
#include "core/exceptions/src/TrendViewerException.h"
#include "core/utilities/src/DebugUtil.h"

#include <vector> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Bus;
using namespace TA_Base_Core;


/////////////////////////////////////////////////////////////////////////////
// CTrendTreeCtrl

namespace TA_Base_App
{
 
	// This is the entity type that will be at the root of our tree
	const std::string CTrendTreeCtrl::ROOT_ENTITY_TYPE = "SCADAROOT";	
	
	CTrendTreeCtrl::CTrendTreeCtrl(IItemStore* itemStore)
		: m_itemStore(itemStore),
		  m_canInitialiseInPreSubclassWindow(true)
	{
		m_bMulti = FALSE; // do not allow multiple selection in the tree items
		m_bAllowDragSelection = FALSE;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Lizette: trendtree created");
	}
	
	CTrendTreeCtrl::~CTrendTreeCtrl()
	{
	}
	
	
	BEGIN_MESSAGE_MAP(CTrendTreeCtrl, CObjectTreeCtrl)
		//{{AFX_MSG_MAP(CTrendTreeCtrl)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
		
	/////////////////////////////////////////////////////////////////////////////
	// CTrendTreeCtrl message handlers

	BOOL CTrendTreeCtrl::PreCreateWindow(CREATESTRUCT& cs) 
	{
		// If this is called then we were created dynamically so ensure that the tree does not get populated
		// in the PreSubclassWindow() method.
		m_canInitialiseInPreSubclassWindow = false;
		
		return CObjectTreeCtrl::PreCreateWindow(cs);
	}

	void CTrendTreeCtrl::PreSubclassWindow() 
	{
		// This method is called when creating this control statically (DDX Control) or dynamically (Create()).
		// It is ok to call loadTreeTrendItems() in the static created version but not in one created dynamically.
		
		CObjectTreeCtrl::PreSubclassWindow();
		
		if (m_canInitialiseInPreSubclassWindow)
		{
			loadTreeTrendItems();
		}
	}
	
	int CTrendTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		// This method is only called when creating this control dynamically. It is here that we will load all
		// the tree items and populate
		
		if (CObjectTreeCtrl::OnCreate(lpCreateStruct) == -1)
			return -1;
		
		loadTreeTrendItems();	
		return 0;
	}
	
	
	void CTrendTreeCtrl::loadTreeTrendItems()
	{
		if (NULL == m_itemStore)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Item store should not be NULL");
			TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, "An unexpected error occurred."));
		}
		//Start retrieving entities from the scada Root   
		std::vector<TA_Base_Core::IEntityData*>  rootEntities;
		rootEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(ROOT_ENTITY_TYPE);
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "number of root items: %d ", rootEntities.size());

		std::vector<TA_Base_Core::IEntityData*>::const_iterator   it;	
		for (it=rootEntities.begin();it!=rootEntities.end();it++)
		{
			this->AddRootItem(*(new CTrendTreeItem(*it, m_itemStore)),true);
		}
	}
	
}