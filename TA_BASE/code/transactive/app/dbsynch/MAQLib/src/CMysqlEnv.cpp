#include "CMysqlEnv.h"	
#include "driver/mysql_public_iface.h"
#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_AQ_Lib)

CMysqlEnv* CMysqlEnv::m_pInstance = 0;
TA_Base_Core::NonReEntrantThreadLockable CMysqlEnv::m_instanceLock;

CMysqlEnv& CMysqlEnv::getInstance()
{
	FUNCTION_ENTRY("COracleEnv::getInstance()");
	ThreadGuard guard( m_instanceLock );
	if (NULL == m_pInstance)
	{
		m_pInstance = new CMysqlEnv();
	}

	FUNCTION_EXIT;
	return (*m_pInstance);
}

void CMysqlEnv::removeInstance()
{
	DEF_DELETE(m_pInstance);
}


CMysqlEnv::CMysqlEnv()
: m_bMysqlEnvInit(false)
, m_Driver(NULL)
, m_conOpt(NULL)
{	
	m_Driver = (sql::Driver*)get_driver_instance();
	m_conOpt = new MAQ_CON_OPT();
	if (!m_Driver || !m_conOpt)
	{
		LOG0(SourceInfo, DebugUtil::DebugFatal, "error ! initialize mysql driver or connection options failed!");
		exit(EXIT_FAILURE);
	}
	else
	{
		m_bMysqlEnvInit = true;
	}
	return ;
}

CMysqlEnv::~CMysqlEnv()
{
	
	if (!m_bMysqlEnvInit)
	{
		return;
	}
	DEF_DELETE(m_conOpt);
	
}

sql::Driver* CMysqlEnv::getDriver()
{
	return m_Driver;
}

void CMysqlEnv::setMysqlConOpt(PMAQ_CON_OPT pConOpt)
{
	if (pConOpt)
	{
		m_conOpt->m_nConSlashSwitch = pConOpt->m_nConSlashSwitch;
		m_conOpt->m_strCharSet      = pConOpt->m_strCharSet;
	}
}

MAQ_CON_OPT* CMysqlEnv::getConOpt()
{
	return m_conOpt;
}

void CMysqlEnv::connThreadInit()
{
	if(m_Driver)
		m_Driver->threadInit();
}

void CMysqlEnv::connThreadEnd()
{
	if(m_Driver)
		m_Driver->threadEnd();
}

NS_END(TA_AQ_Lib)








