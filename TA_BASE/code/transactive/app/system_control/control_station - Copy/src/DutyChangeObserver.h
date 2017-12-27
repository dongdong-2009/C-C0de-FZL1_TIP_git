// DutyChangeObserver.h: interface for the DutyChangeObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUTYCHANGEOBSERVER_H__E06F53B9_D3D5_4CC9_B646_018104AEE9A6__INCLUDED_)
#define AFX_DUTYCHANGEOBSERVER_H__E06F53B9_D3D5_4CC9_B646_018104AEE9A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DutyChangeObserver  
{
public:
    virtual void processDutyChange( const std::vector< TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDuty >& subsystemDuties ) = 0;
};

#endif // !defined(AFX_DUTYCHANGEOBSERVER_H__E06F53B9_D3D5_4CC9_B646_018104AEE9A6__INCLUDED_)
