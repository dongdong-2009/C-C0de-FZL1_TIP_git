// This is a singleton class to manage the ats system and subsystem
#pragma once
#include "CommonDefine.h"

struct SystemSubSystem
{
	std::string strSystemName;
	STRING_LIST subSystems;
};

typedef std::vector<SystemSubSystem*> SystemSubSystemList;

class CSystemSubSystem
{
public: // functions
	static CSystemSubSystem& getInstance();
	static void freeInstance();
	
	~CSystemSubSystem(void);

private: //functions
	CSystemSubSystem(void);

private: // variables
	static CSystemSubSystem* m_pInstance;

	SystemSubSystemList m_objSystemList;
public:
	void getSystemList(STRING_LIST& systemList);
	void getAllSubSystemList(STRING_LIST& subSystemList);
	void getSubSystemList(std::string& strSystem, STRING_LIST& subSystemList);
};
