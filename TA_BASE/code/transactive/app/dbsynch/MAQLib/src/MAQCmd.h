//////////////////////////////////////////////////////////////////////
///		@file		MAQCmd.h
///		@author		zhilin,ouyang
///		@date		2012-08-29 11:44:41
///
///		@brief	    to declare the MAQ Commands for MySQL database such 
///                 as enq/deq etc using the mcl
//////////////////////////////////////////////////////////////////////
#ifndef MAQCMD_H
#define MAQCMD_H
#include "AQCommonData.h"
#include <list>

NS_BEGIN(sql)
class Connection;
class ResultSet;
class Statement;
NS_END(sql)

NS_BEGIN(TA_AQ_Lib)

class CMAQCmd
{
private:
	typedef  std::vector<std::string>		     RowT;  // data row
	typedef  std::list<RowT*>                    LstDataBuffT;

public:
	CMAQCmd(sql::Connection* pConnection);
	~CMAQCmd();

public:
	int    mAQEnq(CMessage* pEnqMsg, PMAQ_ENQ_OPT pEnqOpt);
	int    mAQDeq(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg);
	void   mAQDeqCommit(const std::string& strQName, int nSubID, const std::string& strConsumer, const nPairT& rSeqIDPair);
	void   mAQDeqCommit(const std::string& strQName, const std::string& strMsgID);
	bool   mAQListen(const std::string& strQName, u_int  nSubID);
	bool   mAQListen(const std::string& strQName, const std::string& strSubName);

	void   setConnection(sql::Connection* pConnection);
	int    mAQGetSrvID();

private:
	int      _FetchData(sql::ResultSet* pResSet, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg);
	int      _FetchData(sql::ResultSet* pResSet, LstDataBuffT& rLstData);
	int      _FormatDBBuff2Msg(PMAQ_DEQ_OPT pDeqMsg, LstDataBuffT& crDBData, LstMsgConT& rLstMsg);
	void     _FreeDBDataBuffer(LstDataBuffT& crDBData);
	uint32   _CheckSubCnt(const std::string& strQName);
	void     _ProcessExcepData(const std::string& strQName, const std::string& strMsgID, uint32 uSubID = 0, const char* pszSubName = "");

private:
	sql::Connection*                 m_pConnection;
	std::auto_ptr<sql::Statement>    m_pStatement;
	u_long                           m_ulTimeOut;
	int                              m_nLogCount;
	uint32                           m_nErrCount;
};

NS_END(TA_AQ_Lib)

#endif //MAQCMD_H


































