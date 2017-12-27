#include "COracleEnv.h"	
#include "ocilib.h"  
#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_AQ_Lib)

COracleEnv* COracleEnv::m_pInstance = 0;
TA_Base_Core::NonReEntrantThreadLockable COracleEnv::m_instanceLock;

COracleEnv& COracleEnv::getInstance()
{
	FUNCTION_ENTRY("COracleEnv::getInstance()");
	ThreadGuard guard( m_instanceLock );
	if (NULL == m_pInstance)
	{
		m_pInstance = new COracleEnv();
	}

	FUNCTION_EXIT;
	return (*m_pInstance);
}

void COracleEnv::removeInstance()
{
	DEF_DELETE(m_pInstance);
}


COracleEnv::COracleEnv()
{	
	int nOCIFunRes = 0;
	m_bOracleEnvInit = false;
	nOCIFunRes = OCI_Initialize(NULL, NULL, OCI_ENV_THREADED);
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "error ! OCI_Initialize(), please make sure the Oracle env variables correct!");
		nOCIFunRes = -1;
		exit(EXIT_FAILURE);
	}
	else
	{
		nOCIFunRes = 0;
		m_bOracleEnvInit = true;
	}
	return ;
}

COracleEnv::~COracleEnv()
{
	int nOCIFunRes = 0;
	
	if (!m_bOracleEnvInit)
	{
		return;
	}
	
	/* cleanup OCILIB internal stuff */
	nOCIFunRes = OCI_Cleanup();

	if (1 != nOCIFunRes )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error ! OCI_Cleanup() ");
		nOCIFunRes = -1;
	}
	else
	{
		nOCIFunRes = 0;
		m_bOracleEnvInit = false;
	}
}

NS_END(TA_AQ_Lib)








