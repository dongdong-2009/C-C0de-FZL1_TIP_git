#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif

#include "bus/scada/business_library/src/BusinessManager.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/IScadaBusinessItem.h"
#include "core/data_access_interface/src/ScadaBusinessManagerAccessFactory.h"

namespace TA_Base_Bus
{
	BusinessManager* BusinessManager::m_singleton = NULL;
	TA_Base_Core::ReEntrantThreadLockable BusinessManager::m_singletonLock;

	BusinessManager::BusinessManager()
		: m_isControl(false)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--create BusinessManager instance");
		m_businesses.clear();
	}

	BusinessManager::~BusinessManager()
	{
		std::vector<Business*>::iterator iter;
		for (iter = m_businesses.begin(); iter != m_businesses.end(); ++iter)
		{
			if (NULL != *iter)
			{
				delete *iter;
				*iter = NULL;
			}
		}
		m_businesses.clear();
	}

	void BusinessManager::setToMonitor()
	{
		m_isControl = false;

		std::vector<Business*>::iterator iter;
		for (iter = m_businesses.begin(); iter != m_businesses.end(); ++iter)
		{
			if (NULL != *iter)
			{
				(*iter)->setToMonitor();
			}
		}
	}

	void BusinessManager::setToControl()
	{
		m_isControl = true;

		std::vector<Business*>::iterator iter;
		for (iter = m_businesses.begin(); iter != m_businesses.end(); ++iter)
		{
			if (NULL != *iter)
			{
				(*iter)->setToControl();
			}
		}
	}

	void BusinessManager::hostStart()
	{
		createBusiness();

		std::vector<Business*>::iterator iter;
		for (iter = m_businesses.begin(); iter != m_businesses.end(); ++iter)
		{
			if (NULL != *iter)
			{
				(*iter)->hostStart();
			}
		}

		if (m_isControl)
		{
			setToControl();
		}
		else
		{
			setToMonitor();
		}		
	}

	void BusinessManager::hostStop()
	{
		std::vector<Business*>::iterator iter;
		for (iter = m_businesses.begin(); iter != m_businesses.end(); ++iter)
		{
			if (NULL != *iter)
			{
				(*iter)->hostStop();
			}
		}
	}

	BusinessManager& BusinessManager::getInstance()
	{
		if (NULL == m_singleton)
		{
			TA_THREADGUARD(m_singletonLock);
			if (NULL == m_singleton)
			{
				m_singleton = new BusinessManager();
			}
		}

		return	*m_singleton;
	}

	void BusinessManager::removeInstance()
	{
		if (NULL != m_singleton)
		{
			TA_THREADGUARD(m_singletonLock);
			if (NULL != m_singleton)
			{
				delete m_singleton;
				m_singleton = NULL;
			}
		}
	}

	void BusinessManager::addBusiness(Business* business)
	{
		if (NULL != business)
		{
			m_businesses.push_back(business);
		}
	}

	void BusinessManager::createBusiness()
	{
		std::string hostEntityName = TA_Base_Core::RunParams::getInstance().get("EntityName");

		const TA_Base_Core::ScadaBusinessItemVector& businessList = TA_Base_Core::ScadaBusinessManagerAccessFactory::getInstance().getScadaBusinessItems(hostEntityName);;
		TA_Base_Core::ScadaBusinessItemVector::const_iterator iter;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--load %d BusinessEntity", businessList.size());

		int businessCount = 0;

		for (iter = businessList.begin(); iter != businessList.end(); ++iter)
		{
			Business* business = m_businessFactory.createBusiness(*iter);
			if (business)
			{
				m_businesses.push_back(business);
				businessCount++;
			}
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "BI--create Business %d failed!", (*iter)->getType());
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BI--create %d Business", businessCount);
	}

}