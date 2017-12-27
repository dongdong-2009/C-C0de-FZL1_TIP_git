#if !defined(__C_ORACLE_ENV_H__)
#define __C_ORACLE_ENV_H__

#include "AQCommonDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

using namespace TA_Base_Core;

NS_BEGIN(TA_AQ_Lib)

class COracleEnv 
{

public:
	static COracleEnv& getInstance();
	static void removeInstance();

private:
	static COracleEnv*							   m_pInstance;
	static TA_Base_Core::NonReEntrantThreadLockable   m_instanceLock;

private:
	COracleEnv(void);	
	~COracleEnv(void);

	DEF_DISABLE_COPY_AND_ASSIGNMENT(COracleEnv);
private:
	bool  m_bOracleEnvInit;
};

NS_END(TA_AQ_Lib)

#endif//__C_OCI_LIB_ENV_H__
