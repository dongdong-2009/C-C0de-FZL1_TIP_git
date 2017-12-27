#if !defined(__C_MYSQL_ENV_H__)
#define __C_MYSQL_ENV_H__

#include "AQCommonData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

NS_BEGIN(sql)
class Driver;
NS_END(sql)


NS_BEGIN(TA_AQ_Lib)

using namespace TA_Base_Core;

class CMysqlEnv 
{

public:
	static CMysqlEnv& getInstance();
	static void removeInstance();

public:
	sql::Driver* getDriver();
	void setMysqlConOpt(PMAQ_CON_OPT pConOpt);
	MAQ_CON_OPT* getConOpt();

	void connThreadInit();
	void connThreadEnd();

private:
	static CMysqlEnv*							      m_pInstance;
	static TA_Base_Core::NonReEntrantThreadLockable   m_instanceLock;

private:
	CMysqlEnv(void);	
	~CMysqlEnv(void);

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CMysqlEnv);
private:
	bool           m_bMysqlEnvInit;
	sql::Driver*   m_Driver;
	MAQ_CON_OPT*   m_conOpt;
};

NS_END(TA_AQ_Lib)

#endif//__C_MYSQL_ENV_H__
