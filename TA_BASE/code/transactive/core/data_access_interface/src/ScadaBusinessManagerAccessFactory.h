// ScadaBusinessManagerAccessFactory.h: interface for the ScadaBusinessManagerAccessFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCADABUSINESSMANAGERACCESSFACTORY_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_)
#define AFX_SCADABUSINESSMANAGERACCESSFACTORY_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <vector>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class IScadaBusinessItem;
	class ScadaBusinessManagerAccessFactoryImpl;
	typedef std::vector<IScadaBusinessItem *> ScadaBusinessItemVector;

	class ScadaBusinessManagerAccessFactory  
	{
	public:
		static ScadaBusinessManagerAccessFactory& getInstance();
		static void removeInstance();
		
		const ScadaBusinessItemVector & getScadaBusinessItems(const std::string& agentName);

	private:
		ScadaBusinessManagerAccessFactory();
		~ScadaBusinessManagerAccessFactory();

		static ScadaBusinessManagerAccessFactory* s_instance;
		static ReEntrantThreadLockable s_lock;

		ScadaBusinessManagerAccessFactoryImpl * m_impl;
	};
}

#endif // !defined(AFX_SCADABUSINESSMANAGERACCESSFACTORY_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_)
