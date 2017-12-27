//////////////////////////////////////////////////////////////////////
///		@file		AQCommonData.h
///		@author		zhilin,ouyang
///		@date		2012-08-25 11:44:41
///
///		@brief	    to define some common use data or structure which 
///                 used in the AQ Library.
//////////////////////////////////////////////////////////////////////
#ifndef AQCOMMONDATA_H
#define AQCOMMONDATA_H
#include "AQCommonDef.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <utility>

NS_BEGIN(TA_AQ_Lib)

class CMessage;

// the structure of the enq_option for MAQ library
typedef struct st_maq_enq_opt
{
	u_int           m_nEnqMode;             // the mode of enqueue, use Enq AQ APIs to Q table or SQL package to target table (project specified)

	st_maq_enq_opt()
	: m_nEnqMode(ENQ_AQ_API)
	{
	}
}MAQ_ENQ_OPT, *PMAQ_ENQ_OPT;

// the structure of the database connection options for MAQ library
typedef struct st_maq_con_opt
{
	u_int           m_nConSlashSwitch;     // the switch for disable or enable back slash of the current session (MySQL only)
	std::string     m_strCharSet;          // the CharSet for this connection which will be created.

	st_maq_con_opt()
	: m_nConSlashSwitch(CON_BACKSLASH_OFF)
	{
	}
}MAQ_CON_OPT, *PMAQ_CON_OPT;

// the structure of deq option for MAQ library
typedef struct st_maq_deq_opt
{
	u_int			m_nDeqMode;            
	u_int			m_nDeqFetchSize;       // the row of message for MySQL, the size of message for Oracle 200K for instance
	u_int			m_nDeqNav;
	u_int           m_nDeqVis; 
	u_int			m_nSubID;              // 0 means local message; otherwise using built-in propagation mechanism
	u_int           m_nPayloadType;        // the type of the payload
	std::string		m_strConsumerName;
	std::string     m_strMsgID;

	st_maq_deq_opt()
	:m_nDeqMode(DEQ_REMOVE)
	,m_nDeqFetchSize(0)
	,m_nDeqNav(DEQ_FIRST_MSG)
	,m_nSubID(0)
	{
	}

}MAQ_DEQ_OPT, *PMAQ_DEQ_OPT;

typedef  std::list<CMessage*>                LstMsgConT;
typedef  std::pair<ulong64, ulong64>         nPairT;
typedef  std::map<std::string, std::string>  mapPayloadT;

typedef struct st_maq_dbmsg_tag
{
	int              m_nMsgPriority;                         // the message priority
	int              m_nPayloadType;                         // the type of the payload. 1: RAW      2: OBJ
	int              m_nState;                               // the state of the message
	int              m_nSubCnt;                              // indicate the message belong to.  0: local  non-zero(!0): remote 
	int              m_nOServerID;                           // the original message DB server ID.
	ulong64          m_ulSeqID;                              // the sequence ID of the message. 64bit.
	u_int            m_uDataLen;                             // the size of the payload user data.
	std::string      m_strMsgID;                             // the message UUID
	std::string      m_strQName;                             // the queue name in which case the message belong to.
	std::string      m_strSender;                            // the message sender
	std::string      m_strRecipientLst;                      // the recipient list for the message will be sent to.
	mapPayloadT      m_objPayload;                           // the payload of the message. it's keeps the payload in a key-value pair manner
                                                             // to support object message like Oracle does.
	st_maq_dbmsg_tag()
	: m_nMsgPriority(1)
	, m_nPayloadType(PAYLOAD_TYPE_OBJ)
	, m_nState(MSG_READY)
	, m_nSubCnt(0)
	, m_ulSeqID(0)
	, m_uDataLen(0)
	{
		m_nOServerID = 0;
		m_objPayload.clear();
	}
}DBMSG, *PDBMSG;


NS_END(TA_AQ_Lib)

#endif // AQCOMMONDATA_H


















