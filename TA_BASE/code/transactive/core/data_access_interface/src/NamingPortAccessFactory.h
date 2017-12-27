// NamingPortAccessFactory.h: interface for the NamingPortAccessFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAMINGPORTACCESSFACTORY_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_)
#define AFX_NAMINGPORTACCESSFACTORY_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/src/INamingPort.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/exceptions/src/DataException.h"
#include <map>
#include <boost/shared_ptr.hpp>

namespace TA_Base_Core
{
/**
  * This factory class is used for create NamingPort object.
  * This class is added for CL-21230
  */

	class NamingPortAccessFactory  
	{
	public:
		static NamingPortAccessFactory& getInstance();
		static void removeInstance();
		
		INamingPort& getNamingPort(const std::string& agentName);

	private:
		NamingPortAccessFactory();
		~NamingPortAccessFactory();

		static NamingPortAccessFactory* s_instance;

		typedef std::map<std::string, INamingPort*> NamingPortMap;
		NamingPortMap m_namingPorts;

		static ReEntrantThreadLockable s_lock;
	};
}

#endif // !defined(AFX_NAMINGPORTACCESSFACTORY_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_)
