//////////////////////////////////////////////////////////////////////
///		@file		Listener.h
///		@author		zhilin,ouyang
///		@date		2012-08-20 14:44:41
///
///		@brief	    the message listener for specific queue table.   
///                 currently the listener only support one subscriber
///                 vs one listener. the relationship is one to one.
///                 when there is a message enqueue in the queue table
///                 for the specific subscriber, the listener will return
///                 otherwise it will always keep listen the queue table for
///                 next coming messages.
//////////////////////////////////////////////////////////////////////
#ifndef  LISTENER_H
#define  LISTENER_H
#include "AQCommonDef.h"
#include "Agent.h"

NS_BEGIN(TA_AQ_Lib)

class IDatabase;

class CListener
{
public:
	CListener(IDatabase* pDatabase);
	CListener(IDatabase* pDatabase, const CAgent& rhs);
	CListener(IDatabase* pDatabase, const std::string& strQName);
	CListener(IDatabase* pDatabase, const std::string& strQName, const CAgent& rhs);
	~CListener(void);

public:
	bool		listen();

	void		setAgent(const CAgent& rhs);
	CAgent		getAgent() const;

	void        setQName(const std::string& strQName);
	std::string getQName()const;

	void		setSubID(u_int nSubID);
	u_int		getSubID() const;

	void		setWaitTimeForListen(int nWaitTime);
	int			getWaitTimeForListen()const;

	void		setListenTimeOut(u_long ulTimeOut);
	u_long		getListenTimeOut()const;

private:
	IDatabase*        m_pDatabase;
	CAgent            m_Agent;
	int               m_nWaitTime;
	u_long            m_ulTimeOut;
	u_int             m_nSubID;
	std::string       m_strQName;
};

NS_END(TA_AQ_Lib)

#endif  // MYSQLLISTENER_H




























