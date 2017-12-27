// IScadaBusinessItem.h: interface for the IScadaBusinessItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISCADABUSINESSITEM_H__B22EFC47_AFBF_417A_90FE_65E8CF778789__INCLUDED_)
#define AFX_ISCADABUSINESSITEM_H__B22EFC47_AFBF_417A_90FE_65E8CF778789__INCLUDED_

/**
  * 
  * IScadaBusinessItem is an interface to a ScadaBusinessItem object. It allows the action object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
    */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

namespace TA_Base_Core
{
    enum EScadaBusinessType
    {
	    SCADA_BUSINESS_TYPE_UNKNOWN = 0,
		SCADA_BUSINESS_TYPE_HEARTBEAT,
		SCADA_BUSINESS_TYPE_MONITOR_AND_WRITE
    };

    enum EScadaBusinessSubsystem
    {
	    SCADA_BUSINESS_SUBSYSTEM_UNKNOWN = 0,
		SCADA_BUSINESS_SUBSYSTEM_PA,
		SCADA_BUSINESS_SUBSYSTEM_CCTV,
		SCADA_BUSINESS_SUBSYSTEM_PIDS,
		SCADA_BUSINESS_SUBSYSTEM_BAS,
		SCADA_BUSINESS_SUBSYSTEM_FAS
    };

	typedef std::vector<unsigned long> ScadaBusinessEntitiesVector;

	class IScadaBusinessItem  
	{
	public:
        virtual ~IScadaBusinessItem(){};

		virtual const std::string & getAgentName() = 0;

		virtual EScadaBusinessType getType() = 0;

		virtual EScadaBusinessSubsystem getSubsystem() = 0;

		virtual unsigned long getIntervalTime() = 0;

		virtual unsigned long getLocation() = 0;

		virtual const ScadaBusinessEntitiesVector * getInputEntites() = 0;

		virtual const ScadaBusinessEntitiesVector * getOutputEntities() = 0;

		virtual const ScadaBusinessEntitiesVector * getOutputEntitiesOrdering() = 0;
	};
} //close namespace TA_Base_Core

#endif // !defined(AFX_ISCADABUSINESSITEM_H__B22EFC47_AFBF_417A_90FE_65E8CF778789__INCLUDED_)
