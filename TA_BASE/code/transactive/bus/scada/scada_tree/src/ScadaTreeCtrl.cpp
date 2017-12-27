/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/scada_tree/src/ScadaTreeCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ScadaTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include <vector>
#include <stack>

#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"
#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "bus/scada/scada_tree/src/IEntityFactory.h"

#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\DataNodeEntityData.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Bus;


/////////////////////////////////////////////////////////////////////////////
// CScadaTreeCtrl

// This is the entity type that will be at the root of our tree
const std::string CScadaTreeCtrl::ROOT_ENTITY_TYPE = "SCADAROOT";


CScadaTreeCtrl::CScadaTreeCtrl(IEntityFactory* entityRetrievalFactory /*= NULL*/,
                               bool shouldDeleteFactory /*= false*/,
							   bool bExpandAllDataNodes /*= true*/)
 : m_entityRetrievalFactory(entityRetrievalFactory),
   m_shouldDeleteEntityFactory(shouldDeleteFactory),
   m_canInitialiseInPreSubclassWindow(true),
   m_bExpandAllDataNodes(bExpandAllDataNodes)
{
}

CScadaTreeCtrl::~CScadaTreeCtrl()
{
    if (m_shouldDeleteEntityFactory && m_entityRetrievalFactory != NULL)
    {
        delete m_entityRetrievalFactory;
        m_entityRetrievalFactory = NULL;
    }
}


BEGIN_MESSAGE_MAP(CScadaTreeCtrl, CObjectTreeCtrl)
	//{{AFX_MSG_MAP(CScadaTreeCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScadaTreeCtrl message handlers


BOOL CScadaTreeCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
    // If this is called then we were created dynamically so ensure that the tree does not get populated
    // in the PreSubclassWindow() method.
	m_canInitialiseInPreSubclassWindow = false;
	
	return CObjectTreeCtrl::PreCreateWindow(cs);
}


void CScadaTreeCtrl::PreSubclassWindow() 
{
    // This method is called when creating this control statically (DDX Control) or dynamically (Create()).
    // It is ok to call loadTreeRootItems() in the static created version but not in one created dynamically.

    CObjectTreeCtrl::PreSubclassWindow();

    if (m_canInitialiseInPreSubclassWindow)
    {
	    loadTreeRootItems();
    }
}


int CScadaTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    // This method is only called when creating this control dynamically. It is here that we will load all
    // the tree items and populate

	if (CObjectTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	loadTreeRootItems();
	
	return 0;
}


void CScadaTreeCtrl::loadTreeRootItems()
{
	// Get the list of the locations (e.g. stations, OCC and DCC)
    std::vector<TA_Base_Core::IEntityData*>     locations;
    bool canItemDeleteEntityData = false;

    if (m_entityRetrievalFactory != NULL)
    {
        // Use the factory we were passed to retrieve the entity details
        locations = m_entityRetrievalFactory->getEntitiesOfType(ROOT_ENTITY_TYPE);
    }
    else
    {
        // Use the EntityAccessFactory cos we're in charge of everything ourselves
	    locations = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(ROOT_ENTITY_TYPE);
    }

	std::vector<TA_Base_Core::IEntityData*>::const_iterator   it;
	for (it=locations.begin();it!=locations.end();it++)
	{
		this->AddRootItem(*(new CEntityTreeItem(*it,m_entityRetrievalFactory,m_bExpandAllDataNodes)),true);
	}

}


void CScadaTreeCtrl::setInitialDataPoint(std::string dataPointName)
{
	std::stack<std::string>   dataPointPath;
	std::string               key;
	CObjectTreeItem*          treeItem;
	TA_Base_Core::IEntityData*     entityData;
	unsigned long             parent;


	// Get the path to the data point
	try
	{
        if (m_entityRetrievalFactory != NULL)
        {
    		entityData = m_entityRetrievalFactory->getEntity(dataPointName);
        }
        else
        {
    		entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataPointName);
        }

		dataPointPath.push(dataPointName);
		parent = entityData->getParent();
		while (parent!=0)
		{
            if (m_entityRetrievalFactory != NULL)
            {
			    entityData = m_entityRetrievalFactory->getEntity(parent);
            }
            else
            {
			    entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(parent);
            }

			dataPointPath.push(entityData->getName());
			if (entityData->getType()==ROOT_ENTITY_TYPE)
			{
				// Stop
				parent=0;
			}
			else
				parent = entityData->getParent();
		}

		// Load the path
		if (!dataPointPath.empty())
		{
			// Find the root item
			key = dataPointPath.top();
			dataPointPath.pop();	
			treeItem = findSelectedRootItem(key);

			// Cycle down through the tree nodes
			while ((treeItem!=NULL)&&(!dataPointPath.empty()))
			{
				key = dataPointPath.top();
				dataPointPath.pop();	
				treeItem->expandItem();
				treeItem = treeItem->findSelectedItem(key);
			}

			// Select the data point
			if (treeItem!=NULL) treeItem->selectItem();
		}
	}
	catch(...)
	{

	}
}


int CScadaTreeCtrl::countNodes(HTREEITEM item)
{
    // First expand the node to ensure that all children are loaded and then set it back. This ensures
    // the view does not change but the items are loaded
    Expand(item, TVE_TOGGLE);
    Expand(item, TVE_TOGGLE);

    HTREEITEM child = GetChildItem(item);
    if ( child == NULL )
    {
        // This child has no children so return 0
        return 0;
    }

    // else we must determine how many children we have
    int numberOfNodes = 0;
    while(child != NULL)
    {
        // Add this node to the number of nodes plus the count of all the child nodes
        numberOfNodes = numberOfNodes + 1 + countNodes(child);

        child = GetNextItem(child, TVGN_NEXT);
    }

    return numberOfNodes;
}

