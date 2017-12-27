// ApplyRights.h: interface for the CApplyRights class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLYRIGHTS_H__6FE842D6_6799_4B54_9CEA_F03424267ACD__INCLUDED_)
#define AFX_APPLYRIGHTS_H__6FE842D6_6799_4B54_9CEA_F03424267ACD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
   class IGUIAccess;
}

#include "core\threads\src\Thread.h"

class CApplyRightsThread : public TA_Base_Core::Thread
{
public:
	CApplyRightsThread(TA_Base_Bus::IGUIAccess *pControlClass);
	virtual ~CApplyRightsThread();

	virtual void run();
	virtual void terminate();

private:
	 TA_Base_Bus::IGUIAccess *m_pControlClass;
};

#endif // !defined(AFX_APPLYRIGHTS_H__6FE842D6_6799_4B54_9CEA_F03424267ACD__INCLUDED_)
