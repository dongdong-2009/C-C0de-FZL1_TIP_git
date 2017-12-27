// ommonInterface.h: interface for the CommonInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OMMONINTERFACE_H__AD92474C_33F0_4174_B83F_9A802A83D292__INCLUDED_)
#define AFX_OMMONINTERFACE_H__AD92474C_33F0_4174_B83F_9A802A83D292__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

class IRightsChecker
{
public:
	virtual bool checkRights(const std::string& location, const std::string& sessionId, 
					 unsigned long locationKey, unsigned long subsystem, unsigned long profile, 
					 unsigned long resourceKey, unsigned long actionKey, std::vector< unsigned long >& actionGroups) = 0;
	virtual void getReslt(bool& rightLibResult, bool& rightAgentResult, bool& dutyResult) = 0;
};

class IResultObserver
{
public:
	virtual void notify( bool result, std::string& resultString ) = 0;
};

#endif // !defined(AFX_OMMONINTERFACE_H__AD92474C_33F0_4174_B83F_9A802A83D292__INCLUDED_)
