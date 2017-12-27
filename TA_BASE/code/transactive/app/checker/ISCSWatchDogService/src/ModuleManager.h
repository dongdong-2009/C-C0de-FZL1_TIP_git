// ModuleManager.h: interface for the ModuleManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODULEMANAGER_H__DD8A9194_CB1F_4E83_AE82_7AD831A96DDC__INCLUDED_)
#define AFX_MODULEMANAGER_H__DD8A9194_CB1F_4E83_AE82_7AD831A96DDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

class WatchedModule;

const unsigned int MODULEID_ALARMSTORE = 10;
const unsigned int MODULEID_OPCBRIDGE  = 2;
const unsigned int MODULEID_ALARMSTORE_CMD  = 3;

class ModuleManager //: public CDialog
{
public:
	ModuleManager();
	virtual ~ModuleManager();
public:
	void createModule(const unsigned int moduleID);
	void stopAll();
private:
//	WatchedModule* m_pWatchedModule;
	std::map<unsigned int, WatchedModule*>		m_ModuleMap;

};

#endif // !defined(AFX_MODULEMANAGER_H__DD8A9194_CB1F_4E83_AE82_7AD831A96DDC__INCLUDED_)

