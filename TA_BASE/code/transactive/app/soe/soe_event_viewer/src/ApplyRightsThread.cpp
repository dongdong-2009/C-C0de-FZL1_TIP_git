// ApplyRights.cpp: implementation of the CApplyRights class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "app\soe\soe_event_viewer\src\EventViewer.h"
#include "app/soe/soe_event_viewer/src/ApplyRightsThread.h"
#include "app\soe\soe_event_viewer\EventListComponent\src\EventListComponent.h"
#include "app\soe\soe_event_viewer\src\CombinedEventFilter.h"

#include "bus\generic_gui\src\IGUIAccess.h"

#include "core\data_access_interface\src\IResource.h"
#include "core\data_access_interface\src\ResourceAccessFactory.h"

#include "core\utilities\src\FunctionEntry.h"

CApplyRightsThread::CApplyRightsThread(TA_Base_Bus::IGUIAccess *pControlClass) : m_pControlClass(pControlClass)
{
}

CApplyRightsThread::~CApplyRightsThread()
{

}

void CApplyRightsThread::run()
{
	FUNCTION_ENTRY("CApplyRightsThread::run");
	
	SrtActionPermittedInfo *pSrtActionPermittedInfo = new SrtActionPermittedInfo();
	
	std::auto_ptr<TA_Base_Core::IResource> resource(
        TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(m_pControlClass->getGUIEntity()->getKey()) );
	
	TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( resource->getKey()); 
	pSrtActionPermittedInfo->locationKey = entityData->getLocation();
	pSrtActionPermittedInfo->subsystemKey = entityData->getSubsystem();
	delete entityData;
	pSrtActionPermittedInfo->sessionId = m_pControlClass->getSessionId();
	pSrtActionPermittedInfo->resourceId = resource->getKey();
	
	CWinApp* pApp = AfxGetApp();
	PostMessage(pApp->m_pMainWnd->GetSafeHwnd(), WM_USER_APPLY_RIGHTS, reinterpret_cast<unsigned long>(pSrtActionPermittedInfo), 0);
	
	FUNCTION_EXIT;
}

void CApplyRightsThread::terminate()
{
}