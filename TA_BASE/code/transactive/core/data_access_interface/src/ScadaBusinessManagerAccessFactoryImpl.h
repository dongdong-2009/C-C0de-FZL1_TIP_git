// ScadaBusinessManagerAccessFactoryImpl.h: interface for the ScadaBusinessManagerAccessFactoryImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCADABUSINESSMANAGERACCESSFACTORYIMPL_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_)
#define AFX_SCADABUSINESSMANAGERACCESSFACTORYIMPL_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/src/ScadaBusinessManagerAccessFactory.h"
#include "core/data_access_interface/src/ScadaBusinessItem.h"

namespace TA_Base_Core
{
	class IDatabase;
	typedef std::map<std::string, ScadaBusinessItemVector *> ScadaBusinessItemMap;

	class ScadaBusinessManagerAccessFactoryImpl  
	{
	public:
		ScadaBusinessManagerAccessFactoryImpl();
		~ScadaBusinessManagerAccessFactoryImpl();

		const ScadaBusinessItemVector & getScadaBusinessItems(const std::string& agentName);

	private:
		ScadaBusinessManagerAccessFactoryImpl& operator=(const ScadaBusinessManagerAccessFactoryImpl &);
		ScadaBusinessManagerAccessFactoryImpl(const ScadaBusinessManagerAccessFactoryImpl &);

		ScadaBusinessItemVector * loadDataFromDb(const std::string& agentName);

		void loadHeartBeat(ScadaBusinessItemVector*, IDatabase*, const std::string&);

		void loadMonitorAndWrite(ScadaBusinessItemVector*, IDatabase*, const std::string&);

		EScadaBusinessSubsystem getSubsystemEnumFromString(const std::string &);

		void splitString(const std::string &, ScadaBusinessEntitiesVector &);

		void getTokensFromString(const std::string &, std::vector<std::string> &);

		void trimString(std::string &);

		bool isNumberString(const std::string &);
			
		ScadaBusinessItemMap m_item_map;
	};
}

#endif // !defined(AFX_SCADABUSINESSMANAGERACCESSFACTORYIMPL_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_)
